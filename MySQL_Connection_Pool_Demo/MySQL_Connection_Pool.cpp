#include "MySQL_Connection_Pool.hpp"

MYSQL *Connection_Pool::GetConnection()
{
    MYSQL *con = NULL;

    if (connList.size() == 0)
    {
        return NULL;
    }

    reserve.wait();

    lock.lock();

    con = connList.front();
    connList.pop_front();

    --_free_conn;
    ++_cur_conn;

    lock.unlock();

    return con;
}

bool Connection_Pool::ReleaseConnection(MYSQL *conn)
{
    if (conn == NULL)
    {
        return false;
    }

    lock.lock();

    connList.push_back(conn);

    ++_free_conn;
    --_cur_conn;

    lock.unlock();

    reserve.post();

    return true;
}

int Connection_Pool::GetFreeConnNum()
{
    return this->_free_conn;
}

void Connection_Pool::DestroyPool()
{
    lock.lock();
    if (connList.size() > 0)
    {
        list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); ++it)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        _cur_conn = 0;
        _free_conn = 0;
        connList.clear();
    }
    lock.unlock();
}

Connection_Pool *Connection_Pool::GetInstance()
{
    static Connection_Pool conn_pool;
    return &conn_pool;
}

void Connection_Pool::init(string url, string user, string password, string database_name, unsigned short port, int max_conn, int close_log)
{
    Config config;
    config._url = url;
    config._port = port;
    config._user = user;
    config._password = password;
    config._database_name = database_name;
    config._close_log = close_log;

    _max_conn = max_conn;
    _free_conn = 0;
    _cur_conn = 0;

    for (int i = 0; i < _max_conn; i++)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);

        if (con == NULL)
        {
            perror("MySQL Error");
            exit(1);
        }

        con = mysql_real_connect(con, config._url.c_str(), config._user.c_str(), config._password.c_str(), config._database_name.c_str(), config._port, NULL, 0);

        if (con == NULL)
        {
            perror("MySQL Error");
            exit(1);
        }

        connList.push_back(con);
        ++_free_conn;
    }

    reserve = Sem(_free_conn);
    _max_conn = _free_conn;
}

Connection_Pool::Connection_Pool()
{
    _cur_conn = 0;
    _free_conn = 0;
}

Connection_Pool::~Connection_Pool()
{
    DestroyPool();
}

ConnectionRAII::ConnectionRAII(MYSQL **con, Connection_Pool *connPool)
{
    *con = connPool->GetConnection();
    conRAII = *con;
    poolRAII = connPool;
}

ConnectionRAII::~ConnectionRAII()
{
    poolRAII->ReleaseConnection(conRAII);
}