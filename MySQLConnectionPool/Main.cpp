#include "MySQLClient.hpp"
#include <mysql/mysql.h>
#include <iostream>
using namespace std;

int main()
{
    MySQLClient myclient;
    bool ret = myclient.connection("192.168.13.128", 3306, "root", "1234", "MonitoringSystem");
    if (!ret)
    {
        return EXIT_FAILURE;
    }

    string name = "Linus";
    string sql = "SELECT * FROM tb_username where name = '" + name + "'";
    cout << sql << endl;
    auto res = myclient.readOperation(sql);
    for (size_t i = 1; i < res.size(); ++i)
    {
        for (size_t j = 0; j < res[0].size(); ++j){
            cout << res[i][j] << endl;
        }
    }
    return 0;
}