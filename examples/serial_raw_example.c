// Example: How to open and configure a CDC ACM (serial) device in raw mode on macOS
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serial_device_path>\n", argv[0]);
        return 1;
    }
    const char *dev = argv[1];
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return 2;
    }
    struct termios tio;
    if (tcgetattr(fd, &tio) != 0) {
        perror("tcgetattr");
        close(fd);
        return 3;
    }
    cfmakeraw(&tio); // Set raw mode
    cfsetspeed(&tio, B115200); // Set baud rate (change as needed)
    tio.c_cflag |= CLOCAL | CREAD; // Enable receiver, set local mode
    tio.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    tio.c_cflag &= ~PARENB; // No parity
    tio.c_cflag &= ~CSTOPB; // 1 stop bit
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8; // 8 data bits
    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        perror("tcsetattr");
        close(fd);
        return 4;
    }
    // Write a test byte
    unsigned char b = 0x0a;
    if (write(fd, &b, 1) != 1) {
        perror("write");
    } else {
        printf("Sent 0x0a to %s\n", dev);
    }
    close(fd);
    return 0;
}
