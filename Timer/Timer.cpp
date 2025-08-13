#include "Timer.hpp"
#include <time.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

sort_timer_list::sort_timer_list(){
    head = nullptr;
    tail = nullptr;
}

sort_timer_list::~sort_timer_list(){
    util_timer *temp = head;
    while(temp){
        head = temp->next;
        delete temp;
        temp = head;
    }
}

void sort_timer_list::add_timer(util_timer *timer){
    if(!timer){
        return;
    }

    if(!head){
        head = tail = timer;
        return;
    }

    if(timer->expire < head->expire){
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }

    add_timer(timer, head);
}

// 从list_head开始往后遍历, 但是不仅仅只是能传list_head.
void sort_timer_list::add_timer(util_timer *timer, util_timer *list_head){
    util_timer *prev = list_head;
    util_timer *temp = prev->next;

    while(temp){
        if(timer->expire < temp->expire){
            prev->next = timer;
            timer->next = temp;
            temp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = temp;
        temp = temp->next;
    }
    
    if(!temp){
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
    }
}

void sort_timer_list::adjust_timer(util_timer *timer){
    if(!timer){
        return;
    }

    util_timer *temp = timer->next;

    if(!temp || (timer->expire < temp->expire)){
        return;
    }

    // 当定时器节点是链表头结点，并且它的超时时间被延后，需要将其重新插入到合适的位置
    if(timer == head){
        head = head->next;
        head->prev = nullptr;
        timer->next = nullptr;
        add_timer(timer, head);
    }else{
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, timer->next);
    }
}

void sort_timer_list::del_timer(util_timer *timer){
    if(!timer){
        return;
    }
    
    if((timer == head) && (timer == tail)){
        delete timer;
        head = nullptr;
        tail = nullptr;
        return;
    }

    if(timer == head){
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return;
    }

    if(timer == tail){
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
        return;
    }

    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

// 定时处理链表中所有已经超时的定时器节点
void sort_timer_list::tick(){
    if(!head){
        return;
    }

    time_t cur = time(nullptr);
    util_timer *temp = head;
    while(temp){
        if(cur < temp->expire){
            break;
        }
        temp->callback_func(temp->user_data);
        head = temp->next;
        if(head){                   // 如果新的头节点存在, 把它的prev指针置空
            head->prev = nullptr;
        }
        delete temp;
        temp = head;                // 继续处理下一个节点
    }
}

void Utils::init(int timeslot){
    _TIMESLOT = timeslot;
}

int Utils::setnonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;              // 返回旧操作是为了让调用者知晓 fd 原来的状态
}

// 在内核事件表注册读事件、ET模式，选择开启EPOLLONESHOT
void Utils::add_fd(int epoll_fd, int fd, bool one_shot, int TRIGMode){
    epoll_event event;
    event.data.fd = fd;

    if(TRIGMode == 1){
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    }else{
        event.events = EPOLLIN | EPOLLRDHUP;
    }

    // 当某个文件描述符上的事件被 epoll 检测到并处理后
    // 内核会自动将该 fd 对应的事件从 epoll 事件表中移除
    if(one_shot){
        event.events |= EPOLLONESHOT;
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// 信号处理函数（将信号事件通知主循环）
void Utils::signal_handle(int signal){
    int save_errno = errno;
    int message = signal;
    send(_pipefd[1], (char *)&message, 1, 0);
    errno = save_errno;
}

// 设置信号函数
void Utils::add_signal(int signal, void(*handler)(int), bool restart){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);                        // 屏蔽所有其他信号
    assert(sigaction(signal, &sa, NULL) != -1);     // 注册信号处理函数
}

// _TIMESLOT 秒后会再次触发 SIGALRM 信号
// 从而周期性地调用 timer_handler()
void Utils::timer_handler(){
    _time_list.tick();
    alarm(_TIMESLOT);
}

void Utils::show_Error(int conn_fd, const char *info){
    send(conn_fd, info, strlen(info), 0);
    close(conn_fd);
}

int *Utils::_pipefd = 0;
int Utils::_epoll_fd = 0;

class Utils;
void callback_func(client_data *user_data)
{
    epoll_ctl(Utils::_epoll_fd, EPOLL_CTL_DEL, user_data->sock_fd, 0);
    assert(user_data);
    close(user_data->sock_fd);
    // ------------------------------------
    // http_conn::m_user_count--;
    // ------------------------------------
}