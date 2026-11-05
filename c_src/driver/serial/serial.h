#ifndef DRIVER_SERIAL_H
#define DRIVER_SERIAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "serial_platform.h"
#include "base.h"

// #define MB_SERIAL_RESPONSE_TIMEOUT_US 500000
// #define MB_SERIAL_BYTE_TIMEOUT_US 500000

typedef enum {
    RTS_MODE_NONE = 0,
    RTS_MODE_UP = 1,
    RTS_MODE_DOWN = 2
} RtsMode;

typedef enum {
    SERIAL_MODE_NONE = 0,
    SERIAL_MODE_RS232 = 1,
    SERIAL_MODE_RS485 = 2
} SerialMode;

typedef enum {
    OK,
    ERROR,
    ERROR1
} SerialResult;

typedef struct {
    u32 baudrate;
    u8 data_bits;
    u8 stop_bits;
    char parity;
    RtsMode rts_mode;
    SerialMode serial_mode;
} SerialConfig;

typedef struct {
    SerialPlatform platform;
    bool is_open;
} SerialPort;


SerialResult serialOpen(
    SerialPort *serial_port;
    bool is_open;
);

void serialInit(SerialPort *port);

SerialResult serialOpen(
    SerialPort *port,
    const char *device,
    const SerialConfig *config
);

void serialClose(Serial *serial);

SerialResult serialRead(
    SerialPort *port,
    u8 *data,
    size_t capacity,
    size_t *received,
    u32 timeout_ms
);

SerialResult serialWrite(
    SerialPort *port,
    const u8 *data,
    size_t size,
    size_t *written,
    u32 timeout_ms
);

#endif // DRIVER_SERIAL_H