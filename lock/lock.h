#ifndef LOCKER_H
#define LOCKER_H
#include <pthread.h>
#include <exception>
#include <semaphore.h>
class locker
{
private:
    pthread_mutex_t mutex;

public:
    locker()
    {
        if (pthread_mutex_init(&mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&mutex);
    }
    bool lock()
    {
        return pthread_mutex_lock(&mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&mutex) == 0;
    }

    pthread_mutex_t *getlock()
    {
        return &mutex;
    }
};
class sem
{
private:
    sem_t sem0;

public:
    sem()
    {
        sem_init(&sem0, 0, 0);
    }
    sem(int val)
    {
        sem_init(&sem0, 0, val);
    }
    ~sem()
    {
        sem_destroy(&sem0);
    }
    bool wait()
    {
        return sem_wait(&sem0) == 0;
    }
    bool post()
    {
        return sem_post(&sem0) == 0;
    }
};

class cond
{
private:
    pthread_cond_t cond0;

public:
    cond()
    {
        pthread_cond_init(&cond0, NULL);
    }
    ~cond()
    {
        pthread_cond_destroy(&cond0);
    }
    bool wait(pthread_mutex_t &mutex)
    {
        return pthread_cond_wait(&cond0, &mutex) == 0;
    }
    bool signal()
    {
        return pthread_cond_signal(&cond0) == 0;
    }
    bool broadcast()
    {
        return pthread_cond_broadcast(&cond0) == 0;
    }
    bool time_wait(struct timespec time, pthread_mutex_t &mutex)
    {
        return pthread_cond_timedwait(&cond0, &mutex, &time);
    }
};
#endif