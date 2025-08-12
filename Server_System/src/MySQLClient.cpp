#include "MySQLClient.hpp"
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

MySQLClient::MySQLClient(){
    MYSQL *pconn = mysql_init(&_conn);
    if(!pconn){
        cerr << "mysql_init failed" << endl;
    }
}

MySQLClient::~MySQLClient(){
    mysql_close(&_conn);
}

bool MySQLClient::connection(const string &host, unsigned short port, const string &user, const string &password, const string &database_name){
    MYSQL *pconn = mysql_real_connect(&_conn, host.c_str(), user.c_str(), password.c_str(), database_name.c_str(), port, nullptr, 0);
    if(!pconn){
        printf("Error code: %d, Error message: %s)\n", mysql_errno(&_conn), mysql_error(&_conn));
        return false;
    }else{
        return true;
    }
}

bool MySQLClient::writeOperation(const string &sql){
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0){
        printf("Error code: %d, Error message: %s)\n", mysql_errno(&_conn), mysql_error(&_conn));
        return false;
    }else{
        printf("Query OK, %ld row(s) affected.\n", mysql_affected_rows(&_conn));
        return true;
    }
}

vector<vector<string>> MySQLClient::readOperation(const string &sql){
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0){
        printf("Error code: %d, Error message: %s)\n", mysql_errno(&_conn), mysql_error(&_conn));
        return vector<vector<string>>();
    }

    vector<vector<string>> queryResult;
    MYSQL_RES *result = mysql_store_result(&_conn);
    if(!result){
        printf("Error code: %d, Error message: %s)\n", mysql_errno(&_conn), mysql_error(&_conn));
        return vector<vector<string>>();
    }

    // 返回 SELECT 查询结果的总行数
    int rows = mysql_num_rows(result);
    if(rows == 0){
        return vector<vector<string>>();
    }
    queryResult.reserve(rows + 1);

    int cols = mysql_num_fields(result);
    printf("rows: %d, cols: %d\n", rows, cols);

    // 获取列名（字段名）、数据类型、长度等元信息
    MYSQL_FIELD *filed = nullptr;
    vector<string> fileds;
    while((filed = mysql_fetch_field(result)) != nullptr){
        fileds.push_back(filed->name);
    }
    queryResult.push_back(std::move(fileds));

    // 获取每一行的数据信息
    MYSQL_ROW row;
    while((row = mysql_fetch_row(result)) != nullptr){
        vector<string> temp;
        temp.reserve(cols);
        for (int i = 0; i < cols; ++i){
            if(row[i] != nullptr){
                temp.push_back(string(row[i]));
            }else{
                temp.push_back(string());
            }
        }
        queryResult.push_back(temp);
    }

    mysql_free_result(result);
    return queryResult;
}

/*

效果如下：
1       Alice   25
2       Bob     30
3       Charlie 28

*/
void MySQLClient::format(const vector<vector<string>> &result){
    for (auto & vec : result){
        for(auto & filed : vec){
            cout << filed << "\t";
        }
        cout << endl;
    }
}
