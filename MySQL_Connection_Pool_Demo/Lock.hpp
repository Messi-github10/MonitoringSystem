#pragma once

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class Sem{
public:
    Sem(){
        int ret = sem_init(&_sem, 0, 0);
        if(ret != 0){
            throw std::exception();
        }
    }

    Sem(int num){
        int ret = sem_init(&_sem, 0, num);
        if (ret != 0){
            throw std::exception();
        }
    }

    ~Sem(){
        sem_destroy(&_sem);
    }

    bool wait(){
        return sem_wait(&_sem) == 0;
    }

    bool post(){
        return sem_post(&_sem) == 0;
    }

private:
    sem_t _sem;
};

class Lock{
public:
    Lock(){
        int ret = pthread_mutex_init(&_mutex, NULL);
        if (ret != 0){
            throw std::exception();
        }
    }

    ~Lock(){
        pthread_mutex_destroy(&_mutex);
    }

    bool lock(){
        return pthread_mutex_lock(&_mutex) == 0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&_mutex) == 0;
    }

    pthread_mutex_t *get(){
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;
};

class Cond{
public:
    Cond(){
        int ret = pthread_cond_init(&_cond, NULL);
        if(ret != 0){
            throw std::exception();
        }
    }

    ~Cond(){
        pthread_cond_destroy(&_cond);
    }

    bool wait(pthread_mutex_t *_mutex){
        int ret = pthread_cond_wait(&_cond, _mutex);
        return ret == 0;    // true则阻塞
    }

    bool time_wait(pthread_mutex_t *_mutex, struct timespec t){
        int ret = pthread_cond_timedwait(&_cond, _mutex, &t);
        return ret == 0;
    }

    bool signal(){
        return pthread_cond_signal(&_cond) == 0;
    }

    bool broadcast(){
        return pthread_cond_broadcast(&_cond) == 0;
    }

private:
    pthread_cond_t _cond;
};