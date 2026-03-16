#include "../lock/lock.h"
#include <queue>
#include <vector>
template <typename T>
class Thread_pool
{
private:
    int max_thread_num;             // 线程数量
    int max_request_num;            // 最大request处理数量
    sem sem0;                       // 表示当前能处理的request数目
    locker locker0;                 // 锁
    std::queue<T *> request_queue;  // 请求队列，存放request
    pthread_t *pthreads;            // 线程池中的线程
    void run();                     // 真正处理request的函数，worker函数是对run的包装，目的是对其类型转换,因为pthread_create的线程动作参数要求其必须为静态返回值类型为void*函数，arg即为this，用this再调用run处理request
    static void *worker(void *arg); // Alan Worker

public:
    Thread_pool(int mthread_num, int mrequest_num);
    ~Thread_pool();
    bool append(T *request); // 向线程池中添加新request
};

template <class T>
Thread_pool<T>::Thread_pool(int mthread_num, int mrequest_num) : max_thread_num(mthread_num), max_request_num(mrequest_num)
{
    if (max_thread_num <= 0)
    {
        throw std::exception();
    }
    if (max_request_num <= 0)
    {
        throw std::exception();
    }

    pthreads = new pthread_t[max_thread_num];

    for (int i = 0; i < max_thread_num; i += 1) // 启动每一个线程
    {
        if (pthread_create(pthreads + i, NULL, worker, this) != 0) // 注意传进去的是this与worker
        {
            delete[] pthreads;
            throw std::exception();
        }
        if (pthread_detach(pthreads[i]) != 0)
        {
            delete[] pthreads;
            throw std::exception();
        }
    }
}

template <class T>
Thread_pool<T>::~Thread_pool()
{
    delete[] pthreads;
}

template <class T>
bool Thread_pool<T>::append(T *request)
{
    locker0.lock();
    if (request_queue.size() >= max_request_num) // 此处体现max_request_num的意义，如果当前request队列满了就不能再加入新的request，返回flase
    {
        locker0.unlock();
        return false;
    }
    request_queue.push(request);
    locker0.unlock();
    sem0.post();
    return true;
}

template <class T>
void *Thread_pool<T>::worker(void *arg)
{
    Thread_pool<T> *pool = static_cast<Thread_pool<T> *>(arg); // 类型转化得到this
    pool->run();
    return nullptr;
}

template <class T>
void Thread_pool<T>::run()
{
    while (true) // 此处while true的意义在于让线程不退出，如果没有的话就会导致线程只进行一次request处理就消失了，因此要不断while true，不断试图得到新的request并处理
    {
        sem0.wait();
        locker0.lock();
        T *request = request_queue.front();
        request_queue.pop();
        (*request)();
        locker0.unlock();
    }
}
