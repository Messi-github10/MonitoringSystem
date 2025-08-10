#include "Configuration.hpp"
#include <iostream>

int main() {
    // 获取配置单例
    auto& config = Configuration::getInstance();

    // 1. 加载配置文件
    try {
        config.loadConfig("server.conf");
        std::cout << "=== 配置文件加载成功 ===" << std::endl;
    } catch (...) {
        std::cerr << "配置文件加载失败！请检查 test.cfg 是否存在。" << std::endl;
        return 1;
    }

    // 2. 读取并打印配置项
    std::cout << "\n=== 读取配置项 ===" << std::endl;
    std::cout << "server.ip: " << config.getString("server.ip", "未找到") << std::endl;
    std::cout << "server.port: " << config.getInt("server.port", -1) << std::endl;
    std::cout << "server.thread_num: " << config.getInt("server.thread_num", -1) << std::endl;
    std::cout << "server.task_num: " << config.getInt("server.task_num", -1) << std::endl;
    std::cout << "server.vedio_path: " << config.getString("server.vedio_path", "未找到") << std::endl;
    std::cout << "server.log_file: " << config.getString("server.log_file", "未找到") << std::endl;

    // 3. 测试读取不存在的键
    std::cout << "\n=== 测试不存在的键 ===" << std::endl;
    std::cout << "server.missing_key (int): " << config.getInt("server.missing_key", -1) << std::endl;
    std::cout << "server.missing_key (string): " << config.getString("server.missing_key", "未找到") << std::endl;

    // 4. 验证必需字段
    std::cout << "\n=== 验证必需字段 ===" << std::endl;
    bool isValid = config.validate({"server.ip", "server.port", "server.missing_key"});
    std::cout << "配置验证结果: " << (isValid ? "通过" : "失败") << std::endl;

    return 0;
}