cmake_minimum_required(VERSION 3.10)
project(Server_System VERSION 1.0.0 LANGUAGES CXX)

# [1] 基础配置 ==============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# [2] 依赖查找 ==============================================================
# 扩展搜索路径（覆盖所有常见安装位置）
list(APPEND CMAKE_PREFIX_PATH 
    "/usr" 
    "/usr/local" 
    "/opt/homebrew"  # macOS
    "C:/libconfig"   # Windows示例路径
)

# libconfig++ (增强版查找逻辑)
find_package(libconfig++ QUIET)
if(NOT libconfig++_FOUND)
    # 手动查找模式
    find_path(LIBCONFIGPP_INCLUDE_DIR libconfig.h++
        PATHS /usr/include /usr/local/include /opt/homebrew/include
        NO_DEFAULT_PATH
    )
    find_library(LIBCONFIGPP_LIBRARY
        NAMES config++ libconfig++
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /opt/homebrew/lib
        NO_DEFAULT_PATH
    )
    
    if(LIBCONFIGPP_INCLUDE_DIR AND LIBCONFIGPP_LIBRARY)
        message(STATUS "Found libconfig++ manually")
        add_library(libconfig++::libconfig++ UNKNOWN IMPORTED)
        set_target_properties(libconfig++::libconfig++ PROPERTIES
            IMPORTED_LOCATION "${LIBCONFIGPP_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBCONFIGPP_INCLUDE_DIR}"
        )
        set(libconfig++_FOUND TRUE)
    endif()
endif()

# spdlog (自带fmt)
find_package(spdlog REQUIRED)

# [3] 源文件处理 ============================================================
# 显式列出源文件（推荐）
# 收集源文件（保持您的GLOB风格但添加校验）
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 或者使用GLOB（需确保文件存在）
# file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 检查源文件是否存在
foreach(src ${SRCS})
    if(NOT EXISTS ${src})
        message(FATAL_ERROR "Missing source file: ${src}")
    endif()
endforeach()

# [4] 构建目标 ==============================================================
add_executable(Server_System ${SRCS})

# 包含目录（现代语法）
target_include_directories(Server_System PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# 链接库
target_link_libraries(Server_System PRIVATE
    pthread
    libconfig++::libconfig++
    spdlog::spdlog
)

# [5] 安装规则 ==============================================================
install(TARGETS Server_System DESTINATION bin)
install(DIRECTORY conf/ DESTINATION etc/Server_System)
install(DIRECTORY Log/ DESTINATION var/log/Server_System)

# [6] 友好错误提示 ==========================================================
if(NOT libconfig++_FOUND)
    message(FATAL_ERROR "\n
    ===================================================================\n
    libconfig++ NOT FOUND!\n\n
    Required for configuration file parsing.\n
    Install with:\n
      Ubuntu/Debian : sudo apt-get install libconfig++-dev\n
      CentOS/RHEL   : sudo yum install libconfig-devel\n
      macOS         : brew install libconfig\n
      Windows       : vcpkg install libconfig\n\n
    If already installed but not found, specify path with:\n
      cmake .. -DCMAKE_PREFIX_PATH=/path/to/libconfig++\n
    ===================================================================\n
    ")
endif()

# [7] 跨平台支持 ===========================================================
if(WIN32)
    target_link_libraries(Server_System PRIVATE ws2_32)  # Windows需要Winsock库
endif()