# 创建静态库
add_library(CMake_Lib STATIC
    HelloWorld.cpp
    HelloWorld.hpp
)

# 设置库的包含路径
target_include_directories(CMake_Lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)