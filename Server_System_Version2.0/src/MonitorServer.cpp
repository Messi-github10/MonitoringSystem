#include "MonitorServer.hpp"
#include "Message.hpp"
#include "Login.hpp"
#include "Register.hpp"
#include <iostream>
using std::cout;
using std::endl;

MonitorServer::MonitorServer(int threadNum, int taskNum, unsigned short port, const string &ip)
:_threadpool(threadNum, taskNum)
,_server(port, ip)
{
    using namespace std::placeholders;
    _server.setAllCallBacks(
        std::bind(&MonitorServer::onConnection, this, _1),
        std::bind(&MonitorServer::onMessage, this, _1),
        std::bind(&MonitorServer::onClose, this, _1));
}

MonitorServer::~MonitorServer(){
    _threadpool.stopThreadPool();
}

void MonitorServer::start(){
    _threadpool.startThreadPool();
    _server.start();
}

void MonitorServer::onConnection(TcpConnectionPtr conn){
    cout << "----------------------------------" << endl;
    cout << conn->toString() << "has connected." << endl;
    cout << "----------------------------------" << endl;
}

void MonitorServer::onMessage(TcpConnectionPtr conn){
    cout << "onMessage..." << endl;
    Packet packet;
    int ret = conn->readPacket(packet);
    cout << "read: [" << ret << "] Bytes" << endl;
    cout << "packet.type: [" << packet.type << "]" << endl;
    cout << "packet.length: [" << packet.length << "]" << endl;
    cout << "packet.message: [" << packet.message << "]" << endl;

    switch(packet.type){
    case TASK_TYPE_REGISTER_SECTION_1:
    {
        RegisterSection1 registerSection1(conn, packet);
        _threadpool.addTask(std::bind(&RegisterSection1::process, registerSection1));
        break;
    }
    case TASK_TYPE_REGISTER_SECTION_2:
    {
        RegisterSection2 registerSection2(conn, packet);
        _threadpool.addTask(std::bind(&RegisterSection2::process, registerSection2));
        break;
    }
    case TASK_TYPE_LOGIN_SECTION_1:
    {
        UserLoginSection1 userLoginSection1(conn, packet);
        _threadpool.addTask(std::bind(&UserLoginSection1::process, userLoginSection1));
        break;
    }
    case TASK_TYPE_LOGIN_SECTION_2:
    {
        UserLoginSection2 userLoginSection2(conn, packet);
        _threadpool.addTask(std::bind(&UserLoginSection2::process, userLoginSection2));
        break;
    }
    }
}

void MonitorServer::onClose(TcpConnectionPtr conn){
    cout << "----------------------------------" << endl;
    cout << conn->toString() << "has closed." << endl;
    cout << "----------------------------------" << endl;
}