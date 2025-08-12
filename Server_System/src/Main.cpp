#include "MonitorServer.hpp"
#include "Logger.hpp"
#include "Configuration.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <stdlib.h>
using std::cerr;
using std::cout;
using std::endl;
using std::string;

// MonitorServer入口函数
int main()
{
    cout << "Hello, Welcome to the Monitor Server System." << endl;

    Configuration *pconf = Configuration::getInstance();

    MonitorServer server(
            atoi(pconf->getConfigMap()["thread_num"].c_str()), 
            atoi(pconf->getConfigMap()["task_num"].c_str()),
            atoi(pconf->getConfigMap()["port"].c_str())
        );

    // 启动服务器
    server.start();


    return 0;
}