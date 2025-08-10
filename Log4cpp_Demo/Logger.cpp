#include "Logger.hpp"

Logger& Logger::getInstance() {
    static Logger instance;  // 线程安全的单例（C++11 保证）
    return instance;
}

Logger::Logger() : root(log4cpp::Category::getRoot()) {
    // 初始化 FileAppender 和 BasicLayout
    log4cpp::Appender* appender = new log4cpp::FileAppender("default", "server.log");
    
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S} [%p] %c : %m%n");

    appender->setLayout(layout);
    root.setAppender(appender);
    root.setPriority(log4cpp::Priority::DEBUG);  // 默认日志级别
}

Logger::~Logger() {
    // 关闭 log4cpp 并释放资源
    log4cpp::Category::shutdown();
}

void Logger::debug(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.debug(message);
}

void Logger::info(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.info(message);
}

void Logger::warn(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.warn(message);
}

void Logger::error(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.error(message);
}

void Logger::log(const std::string& message, log4cpp::Priority::Value priority) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.log(priority, message);
}

void Logger::setPriority(log4cpp::Priority::Value priority) {
    std::lock_guard<std::mutex> lock(logMutex);
    root.setPriority(priority);
}