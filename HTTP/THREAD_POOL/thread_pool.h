/*************************************************************************
    > File Name: thread_pool.h
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月24日 星期一 12时06分28秒
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <cstdio>
#include <pthread.h>
#include <list>
#include <cstdlib>
#include <semaphore.h>

using namespace std;

template <typename T>
class threadpool
{
public:
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    bool append(T * request);
private:
    static void * worker(void *arg);
    void run();
private:
    int m_thread_number;
    int m_max_requests;
    pthread_t* m_threads;
    std::list<T*> m_workqueue;
    pthread_mutex_t m_queuelocker;
    sem_t m_queuestat;
    bool m_stop;
};
template< typename T >
threadpool<T>::threadpool(int thread_number, int max_requests):m_thread_number(thread_number), m_max_requests(max_requests),m_stop(false),m_threads(NULL)
{
    if((thread_number<=0) || (max_requests <= 0) )
    {
        /**ERROR**/
        printf("thread_number error.\n");
        exit(1);
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
    {
        /**ERROR**/
        exit(1);
    }

    for(int i = 0; i < thread_number; i++)
    {
        printf("Create the %dth thread.\n",i);
        if(pthread_create(m_threads+i, NULL, worker, this) != 0)
        {
            delete [] m_threads;
            /**ERROR**/
            exit(1);
        }
        if(pthread_detach(m_threads[i]))
        {
            delete [] m_threads;
            /****/
            exit(1);
        }
    }
    sem_init(&m_queuestat, 0, 1);//
    pthread_mutex_init(&m_queuelocker, NULL);
}
template< typename T > 
threadpool<T>::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}

template< typename T > 
bool threadpool<T>::append(T *request)
{
    pthread_mutex_lock(&m_queuelocker);
    if(m_workqueue.size() > m_max_requests)
    {
        pthread_mutex_unlock(&m_queuelocker);
        return false;
    }
    m_workqueue.push_back(request);
    pthread_mutex_unlock(&m_queuelocker);
    sem_post(&m_queuestat);
}


template< typename T > 
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}


template< typename T > 
void threadpool<T>::run()
{
    while(!m_stop)
    {
       sem_wait(&m_queuestat);
       pthread_mutex_lock(&m_queuelocker);
       if(m_workqueue.empty())
       {
           pthread_mutex_unlock(&m_queuelocker);
           continue;
       }
       T *request = m_workqueue.front();
       m_workqueue.pop_front();
       pthread_mutex_unlock(&m_queuelocker);

       if(!request) continue;
       request->process();
    }
}

#endif
