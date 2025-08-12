#ifndef MONITORSERVER_HPP
#define MONITORSERVER_HPP

#include "Threadpool.hpp"
#include "TcpServer.hpp"

class MonitorServer{
public:
    MonitorServer(int threadNum, int taskNum, unsigned short port, const string &ip = "127.0.0.1");
    ~MonitorServer();
    void start();

private:
    void onConnection(TcpConnectionPtr conn);
    void onMessage(TcpConnectionPtr conn);
    void onClose(TcpConnectionPtr conn);

private:
    Threadpool _threadpool;
    TcpServer _server;
};

#endif