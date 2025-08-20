add_library(data_manager OBJECT
    DataManager.cpp
    range/RangeManager.cpp
    reg/RegManager.cpp
    request/RequestManager.cpp
)

target_include_directories(data_manager PUBLIC .)

target_include_directories(data_manager PRIVATE reg   
                                                range
                                                request)      

target_link_libraries(data_manager PUBLIC mem_manager
                                          helpers 
                                          types
                                          config
                                          log)

