/*************************************************************************
    > File Name: TCP_UDP_s.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月17日 星期一 17时42分54秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_SIZE 1024
#define LISTENQ 20

void setNonBlocking(int sockfd);
void setAddr(struct sockaddr_in *addr, int port);
void Bind(int fd, struct sockaddr *addr, socklen_t len);
void Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
void Work(int ufd_listener, int tfd_listener);


int main(int argc, char *argv[])
{
    int tfd_listener,ufd_listener;
    int sockfd; 

    struct sockaddr_in  serveraddr, myaddr;
    
    if((tfd_listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("TCP fd socket error()\n");
        exit(1);
    }

    if((ufd_listener = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("UDP fd socket error()\n");
        exit(1);
    }

    int opt = SO_REUSEADDR; //reuse immediately after used
    setsockopt(ufd_listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  //SOL_SOCKET basic socket

    setNonBlocking(tfd_listener);//set the socket nonblocking
    setNonBlocking(ufd_listener);

    setAddr(&myaddr, 4000);     //inital sockaddr_in , use port 4000 process the UDP request
    setAddr(&serveraddr, 1235); //use port 1235 process the TCP connection

    Bind(ufd_listener, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
    Bind(tfd_listener, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr));
    
    Work(ufd_listener, tfd_listener); //loop listening and processing 
    return 0;
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

void setAddr(struct sockaddr_in *addr, int port)
{
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = PF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

void Bind(int fd, struct sockaddr * addr, socklen_t len)
{
    if(bind(fd, addr, len) == -1)
    {
        printf("Bind error()\n");
        exit(1);
    }
}

void Epoll_ctl(int epfd, int op,int fd, struct epoll_event *event) //set the listen events
{
    event->data.fd = fd;
    event->events = EPOLLIN|EPOLLET;
    if(epoll_ctl(epfd, op, fd, event) == -1)
    {
        printf("Epoll_ctl error()\n");
        exit(1);
    }
}


void Work(int ufd_listener, int tfd_listener)
{ 
    struct epoll_event ev, events[20];
    int epfd = epoll_create(256);  //the max number could be listened
    struct sockaddr_in clientaddr;
    int nfds,i,n,sockfd;
    int connfd, count = 0;
    socklen_t clilen;
    char line[1000] = "Hello";

    Epoll_ctl(epfd, EPOLL_CTL_ADD, tfd_listener, &ev); //set the listen events TCP
    Epoll_ctl(epfd, EPOLL_CTL_ADD, ufd_listener, &ev); //UDP

    if(listen(tfd_listener, LISTENQ) == -1)
    {
        printf("Listen error()\n");
        exit(1);
    }
    

    while(1)
    {
        nfds = epoll_wait(epfd, events, 20, 3000);//maxevents, timeout
        printf("Wait end nfds = %d\n",nfds);

        for(i = 0; i < nfds; i++)
        {
            if(events[i].data.fd == tfd_listener) //if TCP connection
            {
               if(( connfd = accept(tfd_listener, (struct sockaddr *)& clientaddr, &clilen)) == -1)
               {
                   printf("Accept error()\n");
                   exit(1);
               }
               setNonBlocking(connfd);
               char *str = inet_ntoa(clientaddr.sin_addr);//translate the IP address
               printf("Accept a connection from %s\n",str);

               ev.data.fd = connfd;
               ev.events = EPOLLIN | EPOLLET;  //IN and ET(Edge Triggered)
               epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if(events[i].data.fd == ufd_listener) //if UDP request
            {
                printf("Udp\n");
                if(events[i].events && EPOLLIN)
                {
                    printf("Udp receive begin!\n");
                }
                if((n = recvfrom(ufd_listener, line, MAX_SIZE,0,(struct sockaddr *)&clientaddr, &clilen)) == -1)
                {
                    printf("Udp receive error\n");
                    exit(1);
                }
                printf("Udp receive success\n");
            }
            else if(events[i].events & EPOLLIN) //if connected TCP connection
            {
                printf("count = %d\n",count);
                count ++;
                if((sockfd = events[i].data.fd) == -1)
                {
                    printf("event && EPOLLIN error()\n");
                    exit(1);
                }

                if((n = read(sockfd, line, MAX_SIZE)) == -1)
                {
                    if(errno == ECONNRESET) //software caused connection abort, tcp reset
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                        printf("read error()\n");
                }
                else if( n == 0 ) //none data to receive, close  
                {
                    printf("close tcp port\n");
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                else
                {
                    write(sockfd, line, n);
                    printf("read ok %s\n",line);
                    line[n] = 0;
                }

                ev.data.fd = sockfd;
                ev.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); //add new fd to the event
            }
           /** else if(events[i].events & EPOLLOUT)  //if data out
            {
                printf("write\n");
                sockfd = events[i].data.fd;
                write(sockfd, line, n);
                ev.data.fd = sockfd;

                ev.events = EPOLLIN | EPOLLET;

                epoll_ctl(epfd, EPOLL_CTL_MOD,sockfd,&ev);
            }**/
        }
    }

}

