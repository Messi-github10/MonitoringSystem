#pragma once

#include "MySQLClient.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <unordered_set>

struct DBconfig {
    std::string ip = "127.0.0.1";
    std::string user = "root";
    std::string passwd = "";
    std::string dbName = "test";
    unsigned short port = 3306;
    size_t minSize = 4;
    size_t maxSize = 16;
    int timeout = 1000;              // 获取连接超时时间(ms)
    int maxIdleTime = 1000 * 60 * 5; // 最大空闲时间(ms)
};

class MySQLConnectionPool {
public:
    static MySQLConnectionPool* getMySQLConnectionPool();
    
    // 初始化连接池
    void initialize(const std::string& ip, unsigned short port, 
                   const std::string& user, const std::string& passwd, 
                   const std::string& dbName, size_t minSize = 4, 
                   size_t maxSize = 16, int timeout = 1000, 
                   int maxIdleTime = 1000 * 60 * 5);
    
    // 获取连接
    std::shared_ptr<MySQLClient> acquireConnection();
    
    // 释放连接(通过shared_ptr的自定义删除器调用)
    void releaseConnection(MySQLClient* conn);
    
    // 关闭连接池
    void shutdown();
    
    // 获取连接池状态
    void getPoolStatus(size_t& free, size_t& total);
    
    ~MySQLConnectionPool();

    MySQLConnectionPool(const MySQLConnectionPool&) = delete;
    MySQLConnectionPool& operator=(const MySQLConnectionPool&) = delete;

private:
    MySQLConnectionPool();
    void produceConnection();    // 生产连接线程函数
    void recycleConnection();    // 回收连接线程函数
    void addConnection();        // 添加一个新连接

private:
    DBconfig _config;
    std::queue<MySQLClient*> _connection_que;
    std::unordered_set<MySQLClient*> _busy_connections;
    std::mutex _mutex_que;
    std::condition_variable _cond;
    bool _running;
};