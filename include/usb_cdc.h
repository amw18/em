#ifndef USB_CDC_H
#define USB_CDC_H

// Monitors the given USB CDC ACM port for incoming data (read-only).
// Returns 0 on success, non-zero on error.
int usb_cdc_monitor(const char *port_path);

// Opens the USB CDC ACM port for read/write. Returns file descriptor or <0 on error.
int usb_cdc_open(const char *port_path);

// Closes the USB CDC ACM port.
void usb_cdc_close(int fd);

// Writes data to the USB CDC ACM port. Returns number of bytes written or <0 on error.
int usb_cdc_write(int fd, const void *buf, int count);

#endif // USB_CDC_H
