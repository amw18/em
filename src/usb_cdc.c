#include "usb_cdc.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int usb_cdc_monitor(const char *port_path) {
    int fd = open(port_path, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    char buf[256];
    ssize_t n;
    printf("Listening for data on %s...\n", port_path);
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, n, stdout);
        fflush(stdout);
    }
    if (n < 0) {
        perror("read");
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}

int usb_cdc_open(const char *port_path) {
    int fd = open(port_path, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
    }
    return fd;
}

void usb_cdc_close(int fd) {
    if (fd >= 0) close(fd);
}

int usb_cdc_write(int fd, const void *buf, int count) {
    if (fd < 0) return -1;
    int n = write(fd, buf, count);
    if (n < 0) {
        perror("write");
        return -1;
    }
    return (int)n;
}
