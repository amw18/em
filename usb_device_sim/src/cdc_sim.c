#include "cdc_sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>

static int master_fd = -1;
static int slave_fd = -1;
static char slave_name[128];
static pthread_t sim_thread;
static int running = 0;

static void* sim_thread_func(void* arg) {
    unsigned char buf[1024];
    while (running) {
        ssize_t n = read(master_fd, buf, sizeof(buf));
        if (n > 0) {
            printf("[CDC_SIM] Received %zd bytes: ", n);
            for (ssize_t i = 0; i < n; ++i) printf("%02X ", buf[i]);
            printf("\n");
            // Optionally, respond to 0x0a or file protocol here
        } else if (n == 0) {
            usleep(10000);
        } else if (errno != EINTR && errno != EAGAIN) {
            perror("[CDC_SIM] read");
            break;
        }
    }
    return NULL;
}

const char* cdc_sim_start() {
    if (openpty(&master_fd, &slave_fd, slave_name, NULL, NULL) == -1) {
        perror("openpty");
        return NULL;
    }
    // Set slave pty to raw mode
    struct termios tio;
    if (tcgetattr(slave_fd, &tio) == 0) {
        cfmakeraw(&tio);
        tcsetattr(slave_fd, TCSANOW, &tio);
    }
    running = 1;
    pthread_create(&sim_thread, NULL, sim_thread_func, NULL);
    printf("[CDC_SIM] PTY slave: %s\n", slave_name);
    return slave_name;
}

void cdc_sim_stop() {
    running = 0;
    pthread_cancel(sim_thread);
    pthread_join(sim_thread, NULL);
    if (master_fd >= 0) close(master_fd);
    if (slave_fd >= 0) close(slave_fd);
    master_fd = -1;
    slave_fd = -1;
}
