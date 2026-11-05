cmake_minimum_required(VERSION 3.16)

add_library(serial STATIC)

target_compile_features(serial PUBLIC c_std_99)

if(WIN32)
    set(SERIAL_PLATFORM windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SERIAL_PLATFORM linux)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(SERIAL_PLATFORM_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/${SERIAL_PLATFORM}"
)

target_sources(serial PRIVATE
    "${SERIAL_PLATFORM_DIR}/serial.c"
)

target_include_directories(serial PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SERIAL_PLATFORM_DIR}"
)