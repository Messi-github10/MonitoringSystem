#include "MySQL_Connection_Pool.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>


using namespace std;

// g++ -std=c++11 *.cpp -o main  -lmysqlclient -lpthread

// 测试连接池初始化
void test_initialization() {
    cout << "=== 测试连接池初始化 ===" << endl;
    
    Connection_Pool* pool = Connection_Pool::GetInstance();
    pool->init("192.168.13.128", "root", "1234", "MonitoringSystem", 3306, 5, 0);
    
    if (pool->GetFreeConnNum() == 5) {
        cout << "✓ 连接池初始化成功，初始连接数正确" << endl;
    } else {
        cout << "✗ 连接池初始化失败，初始连接数不正确" << endl;
    }
    
    cout << endl;
}

// 测试单线程下连接获取和释放
void test_single_thread_operations() {
    cout << "=== 测试单线程操作 ===" << endl;
    
    Connection_Pool* pool = Connection_Pool::GetInstance();
    
    // 获取连接
    MYSQL* conn1 = pool->GetConnection();
    if (conn1 != nullptr) {
        cout << "✓ 成功获取第一个连接" << endl;
    } else {
        cout << "✗ 获取第一个连接失败" << endl;
    }
    
    // 检查空闲连接数
    if (pool->GetFreeConnNum() == 4) {
        cout << "✓ 空闲连接数正确减少" << endl;
    } else {
        cout << "✗ 空闲连接数不正确" << endl;
    }
    
    // 释放连接
    if (pool->ReleaseConnection(conn1)) {
        cout << "✓ 成功释放第一个连接" << endl;
    } else {
        cout << "✗ 释放第一个连接失败" << endl;
    }
    
    // 检查空闲连接数是否恢复
    if (pool->GetFreeConnNum() == 5) {
        cout << "✓ 空闲连接数正确恢复" << endl;
    } else {
        cout << "✗ 空闲连接数未正确恢复" << endl;
    }
    
    cout << endl;
}

// 测试连接池耗尽情况
void test_pool_exhaustion() {
    cout << "=== 测试连接池耗尽 ===" << endl;
    
    Connection_Pool* pool = Connection_Pool::GetInstance();
    vector<MYSQL*> connections;
    
    // 获取所有连接
    for (int i = 0; i < 5; ++i) {
        MYSQL* conn = pool->GetConnection();
        if (conn != nullptr) {
            connections.push_back(conn);
        }
    }
    
    // 尝试获取第6个连接
    MYSQL* conn6 = pool->GetConnection();
    if (conn6 == nullptr) {
        cout << "✓ 连接池耗尽时返回NULL" << endl;
    } else {
        cout << "✗ 连接池耗尽时未返回NULL" << endl;
    }
    
    // 释放一个连接后再次尝试获取
    pool->ReleaseConnection(connections.back());
    connections.pop_back();
    
    conn6 = pool->GetConnection();
    if (conn6 != nullptr) {
        cout << "✓ 释放连接后可再次获取" << endl;
    } else {
        cout << "✗ 释放连接后无法获取" << endl;
    }
    connections.push_back(conn6);
    
    // 清理
    for (auto conn : connections) {
        pool->ReleaseConnection(conn);
    }
    
    cout << endl;
}

// 线程函数：模拟并发获取和释放连接
void thread_task(int id, Connection_Pool* pool) {
    MYSQL* conn = pool->GetConnection();
    if (conn != nullptr) {
        cout << "线程 " << id << " 获取连接成功" << endl;
        
        // 模拟数据库操作
        this_thread::sleep_for(chrono::milliseconds(100));
        
        pool->ReleaseConnection(conn);
        cout << "线程 " << id << " 释放连接成功" << endl;
    } else {
        cout << "线程 " << id << " 获取连接失败" << endl;
    }
}

// 测试多线程并发访问
void test_concurrent_access() {
    cout << "=== 测试多线程并发访问 ===" << endl;
    
    Connection_Pool* pool = Connection_Pool::GetInstance();
    vector<thread> threads;
    
    // 创建10个线程，超过连接池容量
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(thread_task, i, pool);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "✓ 多线程并发测试完成" << endl;
    cout << endl;
}

// 测试RAII包装器
void test_raii_wrapper() {
    cout << "=== 测试RAII包装器 ===" << endl;
    
    Connection_Pool* pool = Connection_Pool::GetInstance();
    MYSQL* conn = nullptr;
    
    {
        ConnectionRAII raii(&conn, pool);
        if (conn != nullptr) {
            cout << "✓ 通过RAII获取连接成功" << endl;
            
            // 检查空闲连接数
            if (pool->GetFreeConnNum() == 4) {
                cout << "✓ RAII获取后空闲连接数正确" << endl;
            } else {
                cout << "✗ RAII获取后空闲连接数不正确" << endl;
            }
        } else {
            cout << "✗ 通过RAII获取连接失败" << endl;
        }
        
        // 作用域内连接应保持有效
        if (mysql_ping(conn) == 0) {
            cout << "✓ 连接在作用域内保持有效" << endl;
        } else {
            cout << "✗ 连接在作用域内失效" << endl;
        }
    }
    
    // 检查作用域结束后连接是否自动释放
    if (pool->GetFreeConnNum() == 5) {
        cout << "✓ RAII作用域结束后连接自动释放" << endl;
    } else {
        cout << "✗ RAII作用域结束后连接未自动释放" << endl;
    }
    
    cout << endl;
}

int main() {
    // 初始化连接池
    Connection_Pool* pool = Connection_Pool::GetInstance();
    pool->init("192.168.13.128", "root", "1234", "MonitoringSystem", 3306, 5, 0);
    
    // 运行测试
    // test_initialization();
    test_single_thread_operations();
    test_pool_exhaustion();
    test_concurrent_access();
    test_raii_wrapper();
    
    // 销毁连接池
    pool->DestroyPool();
    
    return 0;
}