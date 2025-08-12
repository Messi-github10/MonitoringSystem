#include "MySQLConnectionPool.hpp"
#include <chrono>

using namespace std;
using namespace std::chrono;

MySQLConnectionPool* MySQLConnectionPool::getMySQLConnectionPool() {
    static MySQLConnectionPool pool;
    return &pool;
}

void MySQLConnectionPool::initialize(const string& ip, unsigned short port, 
                                   const string& user, const string& passwd, 
                                   const string& dbName, size_t minSize, 
                                   size_t maxSize, int timeout, int maxIdleTime) {
    _config.ip = ip;
    _config.port = port;
    _config.user = user;
    _config.passwd = passwd;
    _config.dbName = dbName;
    _config.minSize = minSize;
    _config.maxSize = maxSize;
    _config.timeout = timeout;
    _config.maxIdleTime = maxIdleTime;

    // 创建初始连接
    for (size_t i = 0; i < _config.minSize; ++i) {
        addConnection();
    }

    // 启动生产者线程
    thread producer(&MySQLConnectionPool::produceConnection, this);
    producer.detach();

    // 启动回收线程
    thread recycler(&MySQLConnectionPool::recycleConnection, this);
    recycler.detach();
}

shared_ptr<MySQLClient> MySQLConnectionPool::acquireConnection() {
    unique_lock<mutex> lock(_mutex_que);
    
    // 等待直到有可用连接或超时
    if (!_cond.wait_for(lock, milliseconds(_config.timeout), [this]() {
        return !_connection_que.empty();
    })) {
        return nullptr; // 超时返回空指针
    }

    // 获取连接并设置自定义删除器
    MySQLClient* conn = _connection_que.front();
    _connection_que.pop();
    
    return shared_ptr<MySQLClient>(conn, [this](MySQLClient* client) {
        releaseConnection(client);
    });
}

void MySQLConnectionPool::releaseConnection(MySQLClient* conn) {
    unique_lock<mutex> lock(_mutex_que);
    _connection_que.push(conn);
    _cond.notify_all(); // 通知等待的线程有新连接可用
}

void MySQLConnectionPool::shutdown() {
    _running = false;
    _cond.notify_all(); // 唤醒所有等待线程
    
    unique_lock<mutex> lock(_mutex_que);
    while (!_connection_que.empty()) {
        MySQLClient* conn = _connection_que.front();
        _connection_que.pop();
        delete conn;
    }
}

void MySQLConnectionPool::getPoolStatus(size_t& free, size_t& total) {
    unique_lock<mutex> lock(_mutex_que);
    free = _connection_que.size();
    total = _connection_que.size() + _busy_connections.size();
}

MySQLConnectionPool::~MySQLConnectionPool() {
    shutdown();
}

MySQLConnectionPool::MySQLConnectionPool() : _running(true) {}

void MySQLConnectionPool::produceConnection() {
    while (_running) {
        unique_lock<mutex> lock(_mutex_que);
        if (_connection_que.size() < _config.minSize) {
            lock.unlock();
            addConnection();
        } else {
            // 每隔1秒检查一次是否需要创建新连接
            _cond.wait_for(lock, seconds(1), [this]() {
                return !_running || _connection_que.size() < _config.minSize;
            });
        }
    }
}

void MySQLConnectionPool::recycleConnection() {
    while (_running) {
        this_thread::sleep_for(seconds(1)); // 每秒检查一次
        
        unique_lock<mutex> lock(_mutex_que);
        if (_connection_que.size() > _config.minSize) {
            MySQLClient* conn = _connection_que.front();
            _connection_que.pop();
            delete conn;
        }
    }
}

void MySQLConnectionPool::addConnection() {
    MySQLClient* conn = new MySQLClient();
    if (conn->connection(_config.ip, _config.port, _config.user, 
                        _config.passwd, _config.dbName)) {
        unique_lock<mutex> lock(_mutex_que);
        _connection_que.push(conn);
        _cond.notify_all(); // 通知等待的线程有新连接可用
    } else {
        delete conn;
    }
}