#ifndef DRIVER_SERIAL_WINDOWS_H
#define DRIVER_SERIAL_WINDOWS_H

#include <windows.h>

typedef struct {
    HANDLE handle;
    DCB old_dcb;
    COMMTIMEOUTS old_timeouts;
    bool has_old_settings;
} SerialPort;

#endif // DRIVER_SERIAL_WINDOWS_H