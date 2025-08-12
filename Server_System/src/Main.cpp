#include "MonitorServer.hpp"
#include <iostream>
using std::cout;
using std::endl;

// MonitorServer入口函数
int main(){
    cout << "Hello, Welcome to the Monitor Server System." << endl;
    MonitorServer server(4, 10, 8000, "0.0.0.0");
    server.start();
    return 0;
}