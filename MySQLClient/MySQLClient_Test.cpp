#include "MySQLClient.hpp"
#include <mysql/mysql.h>

int main(){
    MySQLClient myclient;
    bool ret = myclient.connection("192.168.13.128", 3306, "root", "1234", "MyDateBases");
    if(!ret){
        return EXIT_FAILURE;
    }

    // string sql = "INSERT INTO user VALUES(1, '朱元璋')";
    // myclient.writeOperation(sql);

    string sql = "SELECT * FROM user";
    vector<vector<string>> res = myclient.readOperation(sql);
    myclient.format(res);
    return 0;
}