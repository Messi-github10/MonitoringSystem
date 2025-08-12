#include "MySQLConnectionPool.hpp"
#include <iostream>

int main()
{
    try
    {
        // 初始化连接池
        MySQLConnectionPool::getMySQLConnectionPool()->initialize(
            "192.168.13.128", 3306, "root", "1234", "MyDateBases");

        // 获取连接
        auto conn = MySQLConnectionPool::getMySQLConnectionPool()->acquireConnection();
        if (!conn)
        {
            std::cerr << "Failed to acquire connection" << std::endl;
            return -1;
        }

        // 执行查询
        auto result = conn->readOperation("SELECT * FROM tb_user");
        conn->format(result);

        // 自动释放连接 (通过shared_ptr的析构)

        // 查看连接池状态
        size_t free, total;
        MySQLConnectionPool::getMySQLConnectionPool()->getPoolStatus(free, total);
        std::cout << "Free connections: " << free << "/" << total << std::endl;

        // 关闭连接池
        MySQLConnectionPool::getMySQLConnectionPool()->shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}