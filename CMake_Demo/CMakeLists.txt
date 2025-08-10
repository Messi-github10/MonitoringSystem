cmake_minimum_required(VERSION 3.10)
project(CMake_Demo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加主程序（Main.cpp在根目录）
add_executable(CMake_Demo
    Main.cpp
    src/HelloWorld.cpp
    src/HelloWorld.hpp  # 可选，帮助IDE识别
)

# 包含目录
target_include_directories(CMake_Demo
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 添加子目录（如果需要单独编译src中的内容）
add_subdirectory(src)