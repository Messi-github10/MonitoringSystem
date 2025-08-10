#include "Logger.hpp"

int main() {
    // 初始化日志系统
    LOG_INIT();
    
    // 记录日志
    LOG_INFO("Application started");
    LOG_DEBUG("Debug message: {}", 42);
    LOG_ERROR("Error occurred: {}", "File not found");
    
    // 关闭日志系统
    LOG_SHUTDOWN();
    return 0;
}