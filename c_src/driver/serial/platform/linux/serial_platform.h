#ifndef DRIVER_SERIAL_LINUX_H
#define DRIVER_SERIAL_LINUX_H

#include <termios.h>

typedef struct {
    int fd;
    struct termios old_tios;
    // bool has_old_tios;
} SerialPlatform;

#endif // DRIVER_SERIAL_LINUX_H
