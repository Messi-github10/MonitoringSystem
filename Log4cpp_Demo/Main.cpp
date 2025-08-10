#include "Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testLogger() {
    Logger& logger = Logger::getInstance();
    
    // 测试不同级别的日志输出
    logger.debug("这是一条调试信息");
    logger.info("这是一条普通信息");
    logger.warn("这是一条警告信息");
    logger.error("这是一条错误信息");
    
    // 测试带优先级的 log 方法
    logger.log("这是一条使用 log 方法记录的调试信息", log4cpp::Priority::DEBUG);
    logger.log("这是一条使用 log 方法记录的错误信息", log4cpp::Priority::ERROR);
    
    // 测试日志级别过滤
    std::cout << "\n设置日志级别为 WARN，只显示 WARN 及以上级别的日志\n";
    logger.setPriority(log4cpp::Priority::WARN);
    logger.debug("这条调试信息不应该显示");
    logger.info("这条普通信息不应该显示");
    logger.warn("这条警告信息应该显示");
    logger.error("这条错误信息应该显示");
    
    // 恢复日志级别
    std::cout << "\n恢复日志级别为 DEBUG\n";
    logger.setPriority(log4cpp::Priority::DEBUG);
    
    // 测试多线程日志记录
    std::cout << "\n测试多线程日志记录\n";
    auto logTask = [](int id) {
        Logger& log = Logger::getInstance();
        for (int i = 0; i < 3; ++i) {
            log.info("线程 " + std::to_string(id) + " 日志消息 " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    
    std::thread t1(logTask, 1);
    std::thread t2(logTask, 2);
    std::thread t3(logTask, 3);
    
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << "日志测试完成，请检查 server.log 文件和终端输出\n";
}

int main() {
    testLogger();
    // 确保所有日志写入完成后再退出
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}