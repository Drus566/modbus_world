/* Code style */
// Типы	                PascalCase	            MbSerial
// Конфигурации	        PascalCase	            MbSerialConfig
// Перечисления	        PascalCase	            MbRtsMode
// Функции	            camelCase	            mbSerialOpen()
// Поля структур	    snake_case	            data_bits
// Константы enum	    UPPER_SNAKE_CASE	    MB_RTS_MODE_UP
// Макросы	            UPPER_SNAKE_CASE	    MB_SERIAL_MAX_BUFFER_SIZE
// Приватные функции	camelCase + internal	serialConfigure()

#ifndef MB_HELPER_BASE_H
#define MB_HELPER_BASE_H

#include <stdint.h>
#include <stddef.h>
#include <bool.h>

/* Signed integers */
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* Unsigned integers */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Floating point */
typedef float f32;
typedef double f64;

/* Boolean */
typedef i32 b32;

/* Boolean values */
#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

/* Internal linkage */
#define internal static
#define local_persist static
#define global_variable static

/* String */
#define STR8_LIT(s) \
    ((str8){(const u8 *)(s), sizeof(s) - 1})

typedef struct {
    const u8 *data;
    size_t size;
} str8;

#endif /* MB_HELPER_BASE_H */
