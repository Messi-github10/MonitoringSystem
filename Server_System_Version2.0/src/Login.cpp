#include "Login.hpp"
#include "MySQLClient.hpp"
#include "OpenSSLRandom.hpp"
#include "Configuration.hpp"
#include <iostream>
#include <string.h>
using std::cerr;
using std::cout;
using std::endl;

void UserLoginSection1::process()
{
    cout << "UserLoginSection1 is processing..." << endl;
    if (_packet.type == TASK_TYPE_LOGIN_SECTION_1)
    {
        // 获取客户端发来的用户名
        _conn->current_username = _packet.message;

        // 从数据库中查找是否存在用户名
        // 创建MySQL客户端并连接数据库
        Configuration *pconf = Configuration::getInstance();
        MySQLClient mysql_client;
        bool ret = mysql_client.connection(
            pconf->getConfigMap()["host"],                                               
            static_cast<unsigned short>(std::stoi(pconf->getConfigMap()["mysql_port"])), 
            pconf->getConfigMap()["user"],                                               
            pconf->getConfigMap()["password"],                                           
            pconf->getConfigMap()["database_name"]                                       
        );
        if (!ret)
        {
            cerr << "Failed to connect to database" << endl;
        }
        string sql = "SELECT name FROM tb_username";
        auto result = mysql_client.readOperation(sql);

        // 检查查询结果是否匹配
        bool isExit = false;
        string salt = "";

        // 第一行是列名，需要从第二行开始
        for (size_t i = 1; i < result.size(); ++i)
        {
            if (result[i][0] == _conn->current_username)
            {
                isExit = true;
                string sql = "SELECT setting FROM tb_username WHERE name = '" + _conn->current_username + "'";
                auto setting_result = mysql_client.readOperation(sql);
                salt = get_salt(setting_result[1][0]);
                break;
            }
        }

        if (!isExit)
        {
            // 构造TLV并发送给对端
            // 主机字节序转换为网络字节序
            TLV tlv;
            memset(&tlv, 0, sizeof(tlv));
            tlv.type = TASK_TYPE_LOGIN_SECTION_1_RESPONSE_ERROR;
            tlv.length = 0;
            _conn->sendInLoop(tlv);
            return;
        }

        // 构造TLV并发送给对端
        // 主机字节序转换为网络字节序
        TLV tlv;
        memset(&tlv, 0, sizeof(tlv));
        tlv.type = TASK_TYPE_LOGIN_SECTION_1_RESPONSE_OK;
        tlv.length = salt.length();
        strncpy(tlv.data, salt.c_str(), tlv.length);
        _conn->sendInLoop(tlv);
    }
}

string UserLoginSection1::get_salt(string &setting)
{
    size_t first_dollar = setting.find('$');
    if (first_dollar == string::npos)
    {
        return "";
    }
    size_t second_dollar = setting.find('$', first_dollar + 1);
    if (second_dollar == string::npos)
    {
        return "";
    }
    string salt = setting.substr(second_dollar + 1);
    return salt;
}

void UserLoginSection2::process()
{
    cout << "UserLoginSection2 is processing..." << endl;
    if (_packet.type == TASK_TYPE_LOGIN_SECTION_2)
    {
        // 检查是否已经通过第一阶段登录（有用户名）
        if (_conn->current_username.empty())
        {
            cerr << "Error: Login phase 1 not completed" << endl;
            // 可以发送错误响应给客户端（TODO）
            return;
        }

        // packet：加密密文
        string encrypt = _packet.message;

        // 从数据库中查找是否存在用户名
        // 创建MySQL客户端并连接数据库
        MySQLClient mysql_client;
        bool ret = mysql_client.connection("192.168.13.128", 3306, "root", "1234", "MonitoringSystem"); // TODO：使用配置文件
        if (!ret)
        {
            cerr << "Failed to connect to database" << endl;
        }

        string sql = "SELECT encrypt FROM tb_username WHERE name = '" + _conn->current_username + "'";
        auto result = mysql_client.readOperation(sql);

        // 检查查询结果
        if (result.size() < 2)
        { // 至少应该有列名行和数据行
            cerr << "Error: User not found or query failed" << endl;
            // 发送用户不存在错误响应（TODO）
            return;
        }

        bool isMatch = (result[1][0] == encrypt);

        if (isMatch)
        {
            // 密码匹配成功
            cout << "Login successful..." << endl;
            TLV success_tlv;
            memset(&success_tlv, 0, sizeof(success_tlv));
            success_tlv.type = TASK_TYPE_LOGIN_SECTION_2_RESPONSE_OK;
            success_tlv.length = 0;

            _conn->sendInLoop(success_tlv);
        }
        else
        {
            // 密码匹配失败
            cout << "Login failed..." << endl;
            TLV fail_tlv;
            memset(&fail_tlv, 0, sizeof(fail_tlv));
            fail_tlv.type = TASK_TYPE_LOGIN_SECTION_2_RESPONSE_ERROR;
            fail_tlv.length = 0;
            _conn->sendInLoop(fail_tlv);
        }
    }
}