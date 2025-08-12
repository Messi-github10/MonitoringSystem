#include "Register.hpp"
#include "MySQLClient.hpp"
#include "OpenSSLRandom.hpp"
#include <string.h>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

void RegisterSection1::process(){
    cout << "RegisterSection1..." << endl;
    if(_packet.type == TASK_TYPE_REGISTER_SECTION_1){
        _conn->current_username = _packet.message;

        MySQLClient mysql_client;
        bool ret = mysql_client.connection("192.168.13.128", 3306, "root", "1234", "MonitoringSystem");
        if(!ret){
            cerr << "Failed to connect to database" << endl;
        }

        string sql = "SELECT name FROM tb_username";
        auto result = mysql_client.readOperation(sql);
        for (size_t i = 1; i < result.size(); i++){
            if(result[i][0] == _conn->current_username){
                // 用户名存在
                TLV tlv;
                tlv.type = TASK_TYPE_REGISTER_SECTION_1_RESPONSE_ERROR;
                tlv.length = 0;
                _conn->sendInLoop(tlv);
                return;
            }
        }

        // 用户名不存在 -> 生成 8B 的盐值
        _conn->salt = OpenSSLRandom::getInstance().GenerateSalt(8);
        TLV tlv;
        tlv.type = TASK_TYPE_REGISTER_SECTION_1_RESPONSE_OK;
        tlv.length = _conn->salt.length();
        strncpy(tlv.data, _conn->salt.c_str(), tlv.length);
        _conn->sendInLoop(tlv);
    }
}

void RegisterSection2::process(){
    cout << "RegisterSection2..." << endl;
    if(_packet.type == TASK_TYPE_REGISTER_SECTION_2){
        string encrypt = _packet.message;

        MySQLClient mysql_client;
        bool ret = mysql_client.connection("192.168.13.128", 3306, "root", "1234", "MonitoringSystem");
        if(!ret){
            cerr << "Failed to connect to database" << endl;
        }
        string sql = "INSERT INTO tb_username VALUES(null, '" + _conn->current_username + "', '" + "$1$" + _conn->salt + "', '" + encrypt + "')";
        ret = mysql_client.writeOperation(sql);
        if(!ret){
            TLV tlv;
            tlv.type = TASK_TYPE_LOGIN_SECTION_2_RESPONSE_ERROR;
            tlv.length = 0;
            _conn->sendInLoop(tlv);
            return;
        }
        TLV tlv;
        tlv.type = TASK_TYPE_REGISTER_SECTION_2_RESPONSE_OK;
        tlv.length = 0;
        _conn->sendInLoop(tlv);
    }
}