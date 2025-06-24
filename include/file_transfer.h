#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

// Sends the content of the file at filepath to the given USB port (fd).
// Returns 0 on success, non-zero on error.
int file_transfer_send_file(int fd, const char *filepath);

#endif // FILE_TRANSFER_H
