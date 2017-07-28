/*************************************************************************
  > File Name: http_middle.cpp
  > Author: xxx
  > Mail: XXX.com
  > Created Time: 2017年07月24日 星期一 20时05分06秒
 ************************************************************************/

#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "thread_create.h"

using namespace std;

void work();


class Mainthread
{
public:
    Mainthread()
    {
        //
        
        if( config_init() == -1)
        {
            perror("init config error");
            exit(1);
        }   
        Log::get_instance()->init(LOG_NAME, 500, LOG_SPLIT_LINES, LOG_MAX_QUEUE_SIZE);
        threadnum = THREAD_NUMBER;
        port = PORT;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&server_addr, 0, sizeof(server_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


        int ret = bind(server_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));
        if (ret < 0) {
            perror("bind");
            exit(0);
        }
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));


        if(listen(server_fd, QUEUE_MAX_COUNT) == -1)
        {
            perror("listen error");
            exit(1);
        }

        for(int i = 0; i < threadnum; i++)
        { 
            epfd[i] = epoll_create(256);
            if(epfd[i] < 0)
                perror("Creat epfd error");
        }
        Pool = new threadpool<int>(threadnum, epfd);

        cout<<"Init success"<<endl;

    }

    ~Mainthread()
    {
        //
    }

    void start()
    {
        cout<<"Start"<<endl;
        while(1)
        {
            replace *rp;

            for(int i = 0; i < threadnum; i ++)
            {
                int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
                if(client_fd < 0)
                {
                    perror("client_fd error");
                    continue;
                }
                setNonBlocking(client_fd);
                rp = new replace;
                rp->fd = client_fd;
                bzero(&ev, sizeof(ev));
                ev.data.ptr = rp;
                ev.events = EPOLLIN | EPOLLET;
                if(epoll_ctl(epfd[i], EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    perror("Epoll_add error");
                    continue;
                }
                if(USE_LOG)
                {
                    LOG_INFO("login user: %d login port: %d.",client_addr.sin_addr.s_addr,client_addr.sin_port); 
                }
            }
        }
    }

    void setNonBlocking(int sockfd)
    {
        int opts;
        opts = fcntl(sockfd,F_GETFL);

        if(opts < 0)
        {
            perror("fcntl(sock,GETFL)");
            exit(1);
        }
        opts = opts | O_NONBLOCK;
        if(fcntl(sockfd,F_SETFL,opts) < 0)
        {
            perror("fcntl(sockfd,SETFL,opts)");
            exit(1);
        }
    }
public:
    int server_fd;
    int epfd[15];


    ushort port;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int threadnum;
    struct epoll_event ev;
    threadpool<int> *Pool;
};

int main(void)
{
    /* signal(SIGPIPE, SIG_IGN); */
    work();
    return 0;
}


void work()
{
    Mainthread Sln;
    Sln.start();
}


