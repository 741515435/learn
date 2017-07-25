/*************************************************************************
    > File Name: TCP_c.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月17日 星期一 14时06分38秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MY_PORT 1235 //5566
#define MAX_SIZE 1024

void client(int sockfd);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));

    if(argc != 2)
    {
        printf("Useage: tcp <IP address.\n");
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port =htons( MY_PORT);
    inet_aton(argv[1],&servaddr.sin_addr);
    
    if((sockfd = socket(AF_INET, SOCK_STREAM,0)) == -1)
    {
        printf("Socket error()\n");
        exit(1);
    }
    if((connect(sockfd,(struct sockaddr *) &servaddr, sizeof(struct sockaddr))) == -1)
    {
        printf("Connect error()\n");
        if(errno)
        printf("%s\n",strerror(errno));
        exit(1);
    }
    
    client(sockfd);

    return 0;
}


void client(int sockfd)
{
    char sendbuff[MAX_SIZE];
    char recvbuff[MAX_SIZE+1];

    while(fgets(sendbuff, MAX_SIZE, stdin) != NULL)
    {
        int n;
       send(sockfd, sendbuff, strlen(sendbuff), 0);


        if((n = recv(sockfd, recvbuff, MAX_SIZE, 0)) == -1)
        {
            printf("Receive error()\n");
            exit(1);
        }
        recvbuff[n] = 0;
        fputs(recvbuff, stdout);
       if((n = write(sockfd, sendbuff, strlen(sendbuff))) == -1)
        {
            printf("write error()\n");
            exit(1);
        }
        printf("send success: %s\n",sendbuff);
    }
    return ;
}

