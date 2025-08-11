cmake_minimum_required(VERSION 3.10)

project(MODBUS_WORLD DESCRIPTION "MODBUS_WORLD Source C++ Modbus Tool" VERSION 1.0.0)

# Definitions for Version.h file
add_definitions(-DLIBMB_MAJOR="${PROJECT_VERSION_MAJOR}")
add_definitions(-DLIBMB_MINOR="${PROJECT_VERSION_MINOR}")
add_definitions(-DLIBMB_FIX="${PROJECT_VERSION_PATCH}")

set(CMAKE_C_STANDARD 11)               
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 

# Установка параметров для сборки статических библиотек
# set(BUILD_SHARED_LIBS OFF)  # Отключаем сборку общих библиотек

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Enable position independant code for dynamic library generation
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# add_subdirectory(3rdparty)

# message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
# message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
# message(STATUS "Current binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(src)

# add_executable(use_new_ini src/example/use_ini/new_ini.cpp)
add_executable(use_mb example/use_data_map.cpp)

#add_library(mb-static STATIC src/version.cpp)
#add_library(mb-shared SHARED src/version.cpp)

# add_executable(use_mb1 use_mb/main.cpp)

# Линковка с библиотекой modbus
target_link_libraries(use_mb map) 
# target_link_libraries(use_new_ini mb helpers data_manager)

# target_link_libraries(mb-static 
# 	mb 
# 	action_manager
# 	data_manager  
# 	mem_manager 
# 	helpers 
# 	version
# 	modbus-static
# 	log
# )

# target_link_libraries(mb-shared 
# 	mb 
# 	action_manager 
# 	data_manager
# 	mem_manager 
# 	helpers 
# 	version
# 	modbus-static
# 	log
# )


# file(GLOB_RECURSE PUBLIC_HEADERS
#      LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/src/*.h")
# install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mb)

# file(GLOB MODBUSPP_HEADERS
#      LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/3rdparty/libmodbuspp/include/*.h")
# install(FILES ${MODBUSPP_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mb/libmodbuspp)
# file(GLOB MODBUSPP_HEADERS_INTERNAL
#      LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/3rdparty/libmodbuspp/include/*.h")

# target_link_libraries(use_mb 
# 	mb 
# 	helpers
# )  # Убедитесь, что libmodbus установлена

# Рекурсивно ищем все заголовочные файлы
# file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/**/*.h")
# # Добавляем директории, содержащие заголовочные файлы
# foreach(header ${HEADER_FILES})
#     get_filename_component(DIR ${header} PATH)
#     include_directories(${DIR})
# endforeach()

#file(GLOB_RECURSE PUBLIC_HEADERS LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/src/*.h")
#sinstall(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openocpp)

