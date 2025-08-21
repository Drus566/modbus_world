add_library(modbus_core OBJECT
   rtu/ModbusCoreRtu.cpp
)

target_include_directories(modbus_core PUBLIC .
															 rtu
															 eth)

target_link_libraries(modbus_core PUBLIC helpers
													  serial
													  types)
