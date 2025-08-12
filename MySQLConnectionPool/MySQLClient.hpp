#ifndef MYSQLCLIENT_HPP
#define MYSQLCLIENT_HPP

#include <mysql/mysql.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

class MySQLClient
{
public:
    MySQLClient();
    ~MySQLClient();

    bool connection(const string &host, unsigned short port, const string &user, const string &password, const string &database_name);                                  // 连接数据库服务器
    bool writeOperation(const string &sql);             // insert、update、delete
    vector<vector<string>> readOperation(const string &sql);              // select
    void format(const vector<vector<string>> &result);  // 格式化打印结果

private:
    MYSQL _conn;
};

#endif