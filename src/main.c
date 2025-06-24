#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "usb_cdc.h"
#include "file_transfer.h"
#include <libusb-1.0/libusb.h>

static int g_usb_fd = -1;
static pthread_t monitor_thread;
static int monitor_running = 0;
static pthread_t current_thread;
static int current_running = 0;

void *monitor_thread_func(void *arg) {
    const char *usb_port_path = (const char *)arg;
    monitor_running = 1;
    usb_cdc_monitor(usb_port_path);
    monitor_running = 0;
    return NULL;
}

// Returns current in mA, or -1 on error
int usb_cdc_read_current_ma(int fd) {
    // This implementation assumes the device is already opened by libusb elsewhere.
    // We'll enumerate devices and try to find the one matching the file descriptor (fd is not used directly here).
    libusb_context *ctx = NULL;
    libusb_device **devs;
    ssize_t cnt;
    int current_ma = -1;
    if (libusb_init(&ctx) < 0) return -1;
    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        libusb_exit(ctx);
        return -1;
    }
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *dev = devs[i];
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(dev, &desc) == 0) {
            // You may want to match by VID/PID or bus/port here for your device
            // For demo, just get the first device with bDeviceClass == 2 (CDC)
            if (desc.bDeviceClass == 2 || desc.bDeviceClass == 0) {
                libusb_device_handle *handle;
                if (libusb_open(dev, &handle) == 0) {
                    unsigned char buf[256];
                    int ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf));
                    // Try to read the MaxPower from the configuration descriptor
                    struct libusb_config_descriptor *config;
                    if (libusb_get_config_descriptor(dev, 0, &config) == 0) {
                        // MaxPower is in 2mA units
                        current_ma = config->MaxPower * 2;
                        libusb_free_config_descriptor(config);
                    }
                    libusb_close(handle);
                    break;
                }
            }
        }
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return current_ma;
}

void *current_thread_func(void *arg) {
    int fd = *(int *)arg;
    current_running = 1;
    while (current_running) {
        int current_ma = usb_cdc_read_current_ma(fd);
        printf("Current: %d mA\n", current_ma);
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <usb_port_path>\n", argv[0]);
        return 1;
    }
    const char *usb_port_path = argv[1];
    printf("Monitoring USB port: %s\n", usb_port_path);

    // Open USB port for read/write
    g_usb_fd = usb_cdc_open(usb_port_path);
    if (g_usb_fd < 0) {
        fprintf(stderr, "Failed to open USB port %s\n", usb_port_path);
        return 2;
    }

    // Start monitor thread
    if (pthread_create(&monitor_thread, NULL, monitor_thread_func, (void *)usb_port_path) != 0) {
        fprintf(stderr, "Failed to create monitor thread\n");
        usb_cdc_close(g_usb_fd);
        return 3;
    }
    // Start current monitor thread
    if (pthread_create(&current_thread, NULL, current_thread_func, (void *)&g_usb_fd) != 0) {
        fprintf(stderr, "Failed to create current monitor thread\n");
        pthread_cancel(monitor_thread);
        pthread_join(monitor_thread, NULL);
        usb_cdc_close(g_usb_fd);
        return 4;
    }

    char cmd[256];
    while (1) {
        printf("Enter command (s: send 0x0a, f: send file, q: quit): ");
        fflush(stdout);
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        if (cmd[0] == 'q') break;
        if (cmd[0] == 's') {
            unsigned char b = 0x0a;
            if (usb_cdc_write(g_usb_fd, &b, 1) != 1) {
                fprintf(stderr, "Failed to send 0x0a to USB port\n");
            } else {
                printf("Sent 0x0a to USB port\n");
            }
        } else if (cmd[0] == 'f') {
            char filepath[512];
            printf("Enter file path: ");
            fflush(stdout);
            if (!fgets(filepath, sizeof(filepath), stdin)) continue;
            // Remove newline
            filepath[strcspn(filepath, "\n")] = 0;
            if (file_transfer_send_file(g_usb_fd, filepath) != 0) {
                fprintf(stderr, "Failed to send file: %s\n", filepath);
            } else {
                printf("File sent: %s\n", filepath);
            }
        }
    }

    // Cleanup
    pthread_cancel(monitor_thread);
    pthread_join(monitor_thread, NULL);
    current_running = 0;
    pthread_cancel(current_thread);
    pthread_join(current_thread, NULL);
    usb_cdc_close(g_usb_fd);
    printf("Exiting.\n");
    return 0;
}
