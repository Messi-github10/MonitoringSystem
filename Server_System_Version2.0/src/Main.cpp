#include "MonitorServer.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"
#include <iostream>
using std::cout;
using std::endl;

// MonitorServer入口函数
int main()
{
    cout << "Hello, Welcome to the Monitor Server System." << endl;

    Configuration *pconf = Configuration::getInstance();

    MonitorServer server(
        atoi(pconf->getConfigMap()["thread_num"].c_str()),
        atoi(pconf->getConfigMap()["task_num"].c_str()),
        atoi(pconf->getConfigMap()["port"].c_str()));

    server.start();
    return 0;
}