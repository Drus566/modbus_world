add_library(serial OBJECT
    Serial.cpp
)

target_include_directories(serial PUBLIC .)

target_link_libraries(serial PUBLIC helpers)
