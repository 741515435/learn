/*************************************************************************
    > File Name: UDP_TCP_1.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月15日 星期六 11时25分37秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define MY_PORT 4000 
#define MAXLINE 1024
#define SA struct sockaddr



void dg_cli( int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE],recvline[MAXLINE+1];

    while(gets(sendline) != NULL)
    {
        if((sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen)) == -1)
        {
            printf("Send error!\n");
            exit(1);
        }
       if((n = recvfrom(sockfd, recvline, MAXLINE, 0 , NULL, NULL)) == -1)
        {
            printf("Receive error!\n");
            exit(1);
        }
        //printf("recv:::");
        recvline[n] = 0;
        puts(recvline);
    }
}


int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2)
    {
        printf("usage: udpcli <IPaddress>\n");
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Socket error!\n");
        exit(1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MY_PORT);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == -1)
    {
        printf("inet_pton error!\n");
        exit(1);
    }
    dg_cli( sockfd, (SA*)&servaddr, sizeof(servaddr));

    return 0;
}
