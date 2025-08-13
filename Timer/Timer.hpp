#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

// 升序链表定时器

class util_timer;

struct client_data{
    sockaddr_in address;
    int sock_fd;
    util_timer *timer;
};

class util_timer {
public:
    util_timer()
    :prev(nullptr)
    ,next(nullptr)
    {}

public:
    time_t expire;                  // 任务超时时间
    void (*callback_func)(client_data*);  // 定时器回调函数
    client_data *user_data;         // 定时器关联的用户数据
    util_timer *next;               // 指向下一个定时器
    util_timer *prev;               // 指向前一个定时器
};

class sort_timer_list{
public:
    sort_timer_list();
    ~sort_timer_list();

    void add_timer(util_timer *timer);
    void adjust_timer(util_timer *timer);
    void del_timer(util_timer *timer);
    void tick();

private:
    void add_timer(util_timer *timer, util_timer *list_head);

private:
    util_timer *head;
    util_timer *tail;
};

class Utils{
public:
    Utils(){}
    ~Utils(){}

    void init(int timeslot);
    int setnonblocking(int fd);
    void add_fd(int epoll_fd, int fd, bool one_shot, int TRIGMode);
    static void signal_handle(int signal);
    void add_signal(int signal, void(*handle)(int), bool restart = true);
    void timer_handler();
    void show_Error(int conn_fd, const char *info);

public:
    static int _epoll_fd;

private:
    static int *_pipefd;
    sort_timer_list _time_list;
    int _TIMESLOT;
};

void callback_func(client_data *user_data);