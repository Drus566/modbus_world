add_library(modbus_interaction OBJECT
   rtu/ModbusRtu.cpp
)

target_include_directories(modbus_interaction PUBLIC .
															 		  rtu
															 		  eth)

target_link_libraries(modbus_interaction PUBLIC helpers
													  			serial
													  			types)
