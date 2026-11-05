
#include "serial.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

void serialInit(SerialPort *port)
{
    if (!port)
        return;

    memset(port, 0, sizeof(*port));

    port->platform.handle = INVALID_HANDLE_VALUE;
}

SerialResult
serialOpen(SerialPort *port,
           const char *device,
           const SerialConfig *config)
{
    if (!port || !device || !config || port->is_open)
    {
        return SERIAL_ERROR_ARGUMENT;
    }

    char path[64];

    int n = snprintf(path, sizeof(path),
                     "\\\\.\\%s", device);

    if (n < 0 || (size_t)n >= sizeof(path))
    {
        return SERIAL_ERROR_ARGUMENT;
    }

    HANDLE handle = CreateFileA(
        path,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        return SERIAL_ERROR_OPEN;
    }

    DCB old_dcb = {0};
    old_dcb.DCBlength = sizeof(old_dcb);

    COMMTIMEOUTS old_timeouts;

    if (!GetCommState(handle, &old_dcb) ||
        !GetCommTimeouts(handle, &old_timeouts))
    {
        CloseHandle(handle);
        return SERIAL_ERROR_CONFIG;
    }

    DCB dcb = old_dcb;

    dcb.BaudRate = config->baudrate;
    dcb.ByteSize = config->data_bits;

    dcb.StopBits = (config->stop_bits == 2)
                       ? TWOSTOPBITS
                       : ONESTOPBIT;

    dcb.Parity = (config->parity == 'E')
                     ? EVENPARITY
                 : (config->parity == 'O')
                     ? ODDPARITY
                     : NOPARITY;

    dcb.fBinary = TRUE;
    dcb.fParity = (config->parity != 'N');

    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(handle, &dcb))
    {
        CloseHandle(handle);
        return SERIAL_ERROR_CONFIG;
    }

    COMMTIMEOUTS timeouts = old_timeouts;

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;

    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 1000;

    if (!SetCommTimeouts(handle, &timeouts))
    {
        SetCommState(handle, &old_dcb);
        CloseHandle(handle);
        return SERIAL_ERROR_CONFIG;
    }

    port->platform.handle = handle;
    port->platform.old_dcb = old_dcb;
    port->platform.old_timeouts = old_timeouts;
    port->platform.has_old_settings = true;
    port->is_open = true;

    return SERIAL_OK;
}
