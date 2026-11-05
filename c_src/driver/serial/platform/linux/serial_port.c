
#define _POSIX_C_SOURCE 200809L

#include "serial.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

/* Преобразование скорости в termios */
internal speed_t serialBaud(u32 baudrate) {
    switch (baudrate)
    {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return 0;
    }
}

void serialInit(SerialPort *port) {
    if (!port) return;
    memset(port, 0, sizeof(*port));
    port->platform.fd = -1;
}

SerialResult serialOpen(SerialPort *port,
                        const char *device,
                        const SerialConfig *config)
{
    if (!port || !device || !config || port->is_open)
    {
        return SERIAL_ERROR_ARGUMENT;
    }

    speed_t speed = serialBaud(config->baudrate);
    if (!speed)
    {
        return SERIAL_ERROR_CONFIG;
    }

    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        return SERIAL_ERROR_OPEN;
    }

    struct termios old_tios;

    if (tcgetattr(fd, &old_tios) != 0)
    {
        close(fd);
        return SERIAL_ERROR_CONFIG;
    }

    struct termios tios = old_tios;

    tios.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB);
    tios.c_cflag |= CLOCAL | CREAD;

    tios.c_cflag |=
        (config->data_bits == 7) ? CS7 : CS8;

    if (config->stop_bits == 2)
    {
        tios.c_cflag |= CSTOPB;
    }

    if (config->parity == 'E')
    {
        tios.c_cflag |= PARENB;
    }
    else if (config->parity == 'O')
    {
        tios.c_cflag |= PARENB | PARODD;
    }

    tios.c_iflag = 0;
    tios.c_oflag = 0;
    tios.c_lflag = 0;

    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (cfsetispeed(&tios, speed) != 0 ||
        cfsetospeed(&tios, speed) != 0 ||
        tcsetattr(fd, TCSANOW, &tios) != 0)
    {
        close(fd);
        return SERIAL_ERROR_CONFIG;
    }

    port->platform.fd = fd;
    port->platform.old_tios = old_tios;
    port->platform.has_old_tios = true;
    port->is_open = true;

    return SERIAL_OK;
}

void serialClose(SerialPort *port)
{
    if (!port || !port->is_open)
        return;

    if (port->platform.has_old_tios)
    {
        tcsetattr(port->platform.fd,
                  TCSANOW,
                  &port->platform.old_tios);
    }

    close(port->platform.fd);

    port->platform.fd = -1;
    port->platform.has_old_tios = false;
    port->is_open = false;
}
