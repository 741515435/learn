/*************************************************************************
  > File Name: thread_create.h
  > Author: xxx
  > Mail: XXX.com
  > Created Time: 2017年07月26日 星期三 09时10分56秒
 ************************************************************************/

#ifndef _THREAD_CREATE_H
#define _THREAD_CREATE_H


#include <cstdio>
#include <pthread.h>
#include <list>
#include <semaphore.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <map>
#include "write_log.h"

#define PORT 9002
#define QUEUE_MAX_COUNT 100
#define BUFF_SIZE 1024

#define SERVER_STRING_200 "HTTP/1.0 200 OK\r\nServer: hoohackhttpd/0.1.0\r\ncontent-Type: text/html\r\n\r\n"

#define SERVER_STRING_404 "HTTP/1.0 404 ERROR\r\nServer: hoohackhttpd/0.1.0\r\ncontent-Type: text/html\r\n\r\n404 NOT FOUND\r\n"

using namespace std;

struct replace
{
    int fd;
    char *request;
};



void process(struct epoll_event ev, int epoll_fd);
void hander_request(struct epoll_event ev, int epoll_fd);
void ERROR_404(int fd,char buf[]);
void OK_200(int fd, char buf[], char content[]);
void DEFAULT(int fd, char buf[]);

void process(struct epoll_event ev, int epoll_fd)
{
    char recv_buf[BUFF_SIZE],buf[BUFF_SIZE];
    char ss[200];
    int len;
    replace *res = (replace*)(ev.data.ptr);
    int sockfd = res->fd;

    len = recv(sockfd, recv_buf, BUFF_SIZE,0);
    if(len == -1) 
        printf("error63\n");
        /* Log::get_instance()->write_log(1, "c=%c,s=%s,f=%f", 'a',"log:Recv error()", 1.000); */
    /* printf("%s\n",recv_buf); */
    for(int i = 5; recv_buf[i] != ' '; i++)
    {
        if(recv_buf[i] == '.' && recv_buf[i-1] == '.')
        {
            printf("ERROR_404\n");
            ERROR_404(sockfd,buf);
            return;
        }
        ss[i-5] = recv_buf[i],ss[i-4] = 0;
    }

    res->request = new char[strlen(ss)]; 
    strcpy(res->request,ss);
    ev.events = EPOLLOUT;

    epoll_ctl(epoll_fd,EPOLL_CTL_MOD,sockfd,&ev);
    //hander_request(recv_buf,client_fd);

    // close(client_fd);

}

void hander_request(struct epoll_event ev, int epoll_fd)
{
    char content[1024],buf[2000], ss[2000];
    replace *res = (replace*)(ev.data.ptr);
    /* int sockfd = res->fd; */
    int sockfd = res->fd;
    strcpy(ss,res->request);
    // ERROR_404(sockfd,buf);
    //return;
    printf("GET \%s\n",ss);

    if(strlen(ss) == 0)
    {
        DEFAULT(sockfd,buf);
    }
    else
    {
        FILE *pFILE;
        pFILE = fopen(ss,"r");
        if(pFILE == NULL)
        {
            ERROR_404(sockfd,buf);
        }
        else
        {
            printf("OK_200\n");
            fscanf(pFILE,"%s",content);
            fclose(pFILE);
            OK_200(sockfd,buf,content);
        }
    }
    printf("%d send over\n", sockfd);

    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,sockfd,&ev);
    /* delete res; */
    close(sockfd);

}

void ERROR_404(int fd,char buf[])
{
    strcpy(buf, SERVER_STRING_404);
    if(send(fd, buf, strlen(buf), MSG_NOSIGNAL) == -1) 
        printf("error 132\n");
        /* Log::get_instance()->write_log(1, "c=%c,s=%s,f=%f",'a',"log:Send error 404()", 1.000); */
    // close(fd);
    return;
}

void OK_200(int fd, char buf[], char content[])
{
    strcpy(buf, SERVER_STRING_200);
    if( send(fd, buf, strlen(buf), MSG_NOSIGNAL) == -1) 
        printf("error142\n");
        /* Log::get_instance()->write_log(1, "c=%c,s=%s,f=%f", 'a',"log:Send error 200", 1.000); */
    sprintf(buf, "%s\r\n",content);
    if(send(fd, buf, strlen(buf), MSG_NOSIGNAL) == -1) printf("Send error() 200: %s\n", strerror(errno));
    // close(fd);
}
void DEFAULT(int fd, char buf[])
{
    strcpy(buf, SERVER_STRING_200);
    if(send(fd, buf, strlen(buf), MSG_NOSIGNAL) == -1) 
        printf("error 152\n");
        /* Log::get_instance()->write_log(1, "c=%c,s=%s,f=%f", 'a',"log:Send error default1", 1.000); */
    sprintf(buf, "Hello world! This is a default page.\r\n");
    if(send(fd, buf, strlen(buf), MSG_NOSIGNAL) == -1) 
        printf("error 156\n");
        /* Log::get_instance()->write_log(1, "c=%c,s=%s,f=%f", 'a',"log:Send error default2", 1.000); */
    // close(fd);
}


template <typename T>
class threadpool
{
public:
    threadpool(int thread_number, int *epfd);
    ~threadpool();
private:
    static void * worker(void *arg);
    void run();
private:
    int m_thread_number;
    pthread_t* m_threads;
};
template< typename T >
threadpool<T>::threadpool(int thread_number, int* epfd):m_thread_number(thread_number), m_threads(NULL)
{
    if((thread_number<=0))
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


        if(pthread_create(m_threads+i, NULL, worker, (void*)&epfd[i]) != 0)
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

    Log::get_instance()->init("./mylog.log", 100, 5000, 10);
    // Mfd.clear();
}
template< typename T > 
threadpool<T>::~threadpool()
{
    delete [] m_threads;
}

template< typename T > 
void *threadpool<T>::worker(void *arg)
{
    int epollfd = *(int*)(arg);
    struct epoll_event events[1024];
    int nfds;
    while(1)
    {
        nfds = epoll_wait(epollfd,events,sizeof(events), -1);

        //sleep(1);

        for(int i = 0; i < nfds; i++)
        {
            if(events[i].events & EPOLLIN)
            {
                process(events[i],epollfd);
                //close(events[i].data.fd);
            }
            else if(events[i].events & EPOLLOUT)
            {
                hander_request(events[i],epollfd);
            }
        }
    }
}
#endif
