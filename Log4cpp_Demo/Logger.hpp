#pragma once

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <string>
#include <iostream>
#include <mutex>
using std::cout;
using std::mutex;
using std::string;

class Logger{
public:
    static Logger &getInstance();

    // 记录日志方法
    void log(const string &message, log4cpp::Priority::Value priority = log4cpp::Priority::INFO);

    // 记录不同级别的日志
    void debug(const string &message);
    void info(const string &message);
    void warn(const string &message);
    void error(const string &message);

    // 设置日志级别
    void setPriority(log4cpp::Priority::Value priority);

private:
    Logger();
    ~Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;

private:
    log4cpp::Category &root;
    mutex logMutex;
};