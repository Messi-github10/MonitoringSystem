#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "TcpConnection.hpp"
#include "Message.hpp"

class RegisterSection1{
public:
    RegisterSection1(TcpConnectionPtr conn, const Packet &packet)
    :_conn(conn)
    ,_packet(packet)
    {}

    void process();

private:
    TcpConnectionPtr _conn;
    Packet _packet;
};

class RegisterSection2{
public:
    RegisterSection2(TcpConnectionPtr conn, const Packet &packet)
    :_conn(conn)
    ,_packet(packet)
    {}

    void process();
private:
    TcpConnectionPtr _conn;
    Packet _packet;
};



#endif