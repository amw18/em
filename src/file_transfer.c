#include "file_transfer.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int file_transfer_send_file(int fd, const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }
    // Get file size
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (filesize < 0) {
        fclose(fp);
        fprintf(stderr, "ftell failed\n");
        return -3;
    }
    // Prepare header
    unsigned char magic[4] = {0x41, 'D', 'N', 'W'};
    unsigned int total_size = (unsigned int)(filesize + 4 + 4 + 2);
    unsigned char size_bytes[4];
    size_bytes[0] = (total_size >> 0) & 0xFF;
    size_bytes[1] = (total_size >> 8) & 0xFF;
    size_bytes[2] = (total_size >> 16) & 0xFF;
    size_bytes[3] = (total_size >> 24) & 0xFF;
    unsigned char tail[2] = {0xFF, 0xFF};
    // Write header
    if (write(fd, magic, 4) != 4) { fclose(fp); return -4; }
    if (write(fd, size_bytes, 4) != 4) { fclose(fp); return -5; }
    // Write file content
    char buf[512];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        ssize_t written = write(fd, buf, n);
        if (written < 0) {
            perror("write");
            fclose(fp);
            return -2;
        }
    }
    // Write tail
    if (write(fd, tail, 2) != 2) { fclose(fp); return -6; }
    fclose(fp);
    return 0;
}
