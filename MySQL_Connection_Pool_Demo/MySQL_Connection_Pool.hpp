#pragma once

#include "Lock.hpp"
#include <mysql/mysql.h>
#include <string>
#include <list>
using namespace std;

class Connection_Pool{
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *conn);
    int GetFreeConnNum();
    void DestroyPool();

    // 单例模式
    static Connection_Pool *GetInstance();

    void init(string url, string user, string password, string database_name, unsigned short port, int max_conn, int close_log);

private:
    Connection_Pool();
    ~Connection_Pool();

    int _max_conn;
    int _cur_conn;
    int _free_conn;
    Lock lock;
    list<MYSQL *> connList; // 连接池
    Sem reserve;            // 连接池中"可被预留（即当前可用）"的连接数量

public:

    struct Config{
        string _url;
        unsigned short _port;
        string _user;
        string _password;
        string _database_name;
        int _close_log;
    };

};

class ConnectionRAII{
public:
    ConnectionRAII(MYSQL **con, Connection_Pool *connPool);
    ~ConnectionRAII();

private:
    MYSQL *conRAII;
    Connection_Pool *poolRAII;
};