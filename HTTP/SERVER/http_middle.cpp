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
void setNonBlocking(int fd);


class Mainthread
{
public:
    Mainthread()
    {
        //
        port = PORT;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&server_addr, 0, sizeof(server_addr));
        

      // setNonBlocking(server_fd);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

        bind(server_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));

        listen(server_fd, QUEUE_MAX_COUNT);

        for(int i = 0; i < threadnum; i++)
            epfd[i] = epoll_create(256);
        Pool = new threadpool<int>(threadnum, 1000, epfd);

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
            
            for(int i = 0; i < threadnum; i ++)
		{
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
            setNonBlocking(client_fd);
            ev.data.fd = client_fd;
            ev.events = EPOLLIN;
            epoll_ctl(epfd[i], EPOLL_CTL_ADD, client_fd, &ev);
            
            }
        }
    }

public:
    int server_fd;
    int client_fd;
    int epfd[15];


    ushort port;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int threadnum = 8;
    struct epoll_event ev;
    threadpool<int> *Pool;
};

int main(void)
{
    work();
    return 0;
}


void work()
{
    Mainthread Sln;
    Sln.start();
}


void setNonBlocking(int sockfd)
{
    if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0 ) | O_NONBLOCK) == -1) //F_SETFl :set the file describe flag, F_GETFD:get the FD_CLOEXEC value
    {
        printf("set NonBlocking error()\n");
        exit(1);
    }
    //set file marks
}

