/*************************************************************************
    > File Name: TDP_s.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月17日 星期一 14时30分41秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_SIZE 1024
#define MY_PORT 5566

void my_echo(int sockfd, pid_t childpid);

int main(int argc, char *argv[])
{
    int sockfd;

    pid_t childpid;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MY_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Socker error()\n");
        exit(1);
    }

    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == -1)
    {
        printf("Bind error()\n");
        exit(1);
    }

    if (listen(sockfd, 5) < 0)
    {
        printf("Listen error()\n");
        exit(1);
    }

    my_echo(sockfd, childpid);

    return 0;
}

void my_echo(int sockfd, pid_t childpid )
{
    ssize_t n;
    int connectfd;
    char buff[MAX_SIZE];

    while (1)
    {
        printf("Running...\n");
        
        connectfd = accept(sockfd, NULL, NULL);
        if ((childpid = fork()) == 0)
        {
            close(sockfd);
            while ((n = recv(connectfd, buff, MAX_SIZE, 0)) == -1)
            {
                printf("Receive error()\n");
                exit(1);
            }
            send(connectfd, buff, n, 0);
            return ;
        }
        close(connectfd);
    }
    return ;
}
