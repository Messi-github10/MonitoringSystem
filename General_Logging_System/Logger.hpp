#pragma once

#include "Noncopyable.hpp"
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class AsyncLogger : public Noncopyable{
public:

    // Magic Static（单例模式）
    // 获取的是AsyncLogger, AsyncLogger是日志系统的包装器
    static AsyncLogger & getInstance(){
        static AsyncLogger instance;
        return instance;
    }

    // 初始化日志系统
    void Init(const std::string & name = "Logger",                      // 指定日志器的名称
            const std::string & log_dir = ".logs",                      // 指定日志文件的存储目录
            size_t queue_size = 8192,                                   // 异步队列的大小
            size_t thread_count = 1,                                    // 后台线程数
            spdlog::level::level_enum level = spdlog::level::debug){    // 设置日志记录的最低级别
        std::call_once(_init_flag, [&]()                                // call_once 确保日志系统的初始化代码仅执行一次
                       {

            // 初始化异步线程池
            spdlog::init_thread_pool(queue_size, thread_count);
            
            // 创建日志输出目标（Sinks）
            // 创建一个控制台输出的Sink
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(level);
                        
            // 创建一个滚动文件输出的Sink
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_dir + "/" + name + ".log", 1024*1024*5, 3);
            file_sink->set_level(level);

            // 将控制台和文件 Sink 组合，创建异步日志器
            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            _logger = std::make_shared<spdlog::async_logger>(
                name, sinks.begin(), sinks.end(), spdlog::thread_pool(),
                spdlog::async_overflow_policy::block);

            // 设置日志格式
            // 效果：[2023-01-01 12:34:56.789] [INFO] [thread 1234] This is a log message
            _logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
            _logger->set_level(level);

            // 注册并设为默认日志器
            spdlog::register_logger(_logger);
            spdlog::set_default_logger(_logger);
            
            // 标记初始化完成
            _initialized = true;
        });
    }

    // 获取的是 spdlog::logger, spdlog::logger 是底层日志器
    std::shared_ptr<spdlog::logger> GetLogger() const{
        if(!_initialized){
            throw std::runtime_error("Logger not initialized!");
        }
        return _logger;
    }

    // 日志系统安全关闭的接口
    void Shutdown(){
        if(_initialized){
            _logger->flush();
            spdlog::drop_all();
            spdlog::shutdown();
            _initialized = false;
        }
    }

private:
    AsyncLogger() = default;
    std::shared_ptr<spdlog::logger> _logger;
    std::once_flag _init_flag;
    bool _initialized = false;
};

// 日志宏定义
#define LOG_INIT(...)       AsyncLogger::getInstance().Init(__VA_ARGS__)
#define LOG_TRACE(...)      SPDLOG_LOGGER_TRACE(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_DEBUG(...)      SPDLOG_LOGGER_DEBUG(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_INFO(...)       SPDLOG_LOGGER_INFO(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_WARN(...)       SPDLOG_LOGGER_WARN(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_ERROR(...)      SPDLOG_LOGGER_ERROR(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_CRITICAL(...)   SPDLOG_LOGGER_CRITICAL(AsyncLogger::getInstance().GetLogger(), __VA_ARGS__)
#define LOG_SHUTDOWN()      AsyncLogger::getInstance().Shutdown()