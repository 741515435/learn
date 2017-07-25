/*************************************************************************
    > File Name: UDP_TCP_s.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月15日 星期六 11时34分04秒
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
#include <unistd.h>


#define SA struct sockaddr
#define MY_PORT 4000
#define MAXLINE 1024

void dg_echo(int sockfd, SA *pcliaddr, socklen_t client )
{
    int n;
    socklen_t len;
    char mesg[MAXLINE],newmsg[MAXLINE];

    while(1)
    {
        memset(mesg, 0, sizeof(mesg));
        len = client;
        if(( n = recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len)) == -1)
        {
            printf("Receive error!\n");
            exit(1);
        }
        //strcpy(newmsg,mesg);
            
            printf(" From %s connection: ",inet_ntoa(((struct sockaddr_in *)pcliaddr)->sin_addr));
        puts("New message!");
       //fputs(newmsg, stdout);
        
       if( sendto(sockfd, mesg, n, 0, pcliaddr, len) == -1)
       {
           printf("Send error!\n");
           exit(1);
       }
       sleep(1);
    }
}
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

   if((sockfd = socket(AF_INET, SOCK_DGRAM,0)) == -1)
   {
       printf("Socket error!\n");
       exit(1);
   }

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(MY_PORT);

   if(bind(sockfd,(SA *) &servaddr,sizeof(servaddr)) < 0)
   {
       printf("bind error!\n");
       exit(1);
   }

   dg_echo(sockfd, (SA*) &cliaddr,sizeof(cliaddr));
    return 0;
}
