#ifndef LOGIN_HPP
#define LOGIN_HPP

#include "TcpConnection.hpp"
#include "Message.hpp"

class UserLoginSection1
{
public:
    UserLoginSection1(TcpConnectionPtr conn, const Packet &packet)
        : _conn(conn), _packet(packet)
    {
    }

    void process();

private:
    string get_salt(string &setting);

private:
    TcpConnectionPtr _conn;
    Packet _packet;
};

class UserLoginSection2
{
public:
    UserLoginSection2(TcpConnectionPtr conn, const Packet &packet)
        : _conn(conn), _packet(packet)
    {
    }

    void process();

private:
    TcpConnectionPtr _conn;
    Packet _packet;
};

#endif