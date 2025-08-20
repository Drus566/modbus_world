add_library(helpers OBJECT 
    IniFile.cpp
    String.cpp
    Time.cpp
)
target_include_directories(helpers PUBLIC .)
