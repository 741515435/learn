/*************************************************************************
    > File Name: ping.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月14日 星期五 15时49分02秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>

#define ICMP_SIZE (sizeof(struct icmp))
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define BUF_SIZE 1024
#define NUM 6

#define UCHAR unsigned char
#define USHORT unsigned short
#define UINT unsigned int

struct icmp
{
    UCHAR type;
    UCHAR code;
    USHORT checksum;
    USHORT id;
    USHORT sequence;
    struct timeval timestamp;
};

struct ip
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    UCHAR hlen:4;
    UCHAR version:4;
#endif
#if __BYTE_ORDER == __BIG__ENDIAN
    UCHAR versin:4;
    UCHAR hlen:4;
#endif
    UCHAR tos;
    USHORT len;
    USHORT id;
    USHORT offsset;
    UCHAR ttl;
    UCHAR protocol;
    USHORT checksum;
    struct in_addr ipsrc;
    struct in_addr ipdst;
};

char buf[BUF_SIZE] = {0};

USHORT checkSum(USHORT *, int);
float timediff(struct timeval *, struct timeval *);
void pack(struct icmp *, int);
int unpack(char *,int , char *);


int main(int argc, char *argv[])
{
    struct hostent *host;
    struct icmp sendicmp;
    struct sockaddr_in from;
    struct sockaddr_in to;
    int fromlen = 0;
    int sockfd;
    int nsend = 0;
    int nreceived = 0;
    int i,n;
    in_addr_t inaddr;

    memset(&from, 0, sizeof(struct sockaddr_in));
    memset(&to, 0, sizeof(struct sockaddr_in));

    if(argc < 2)
    {
        printf("use : %s hostname/IP address\n",argv[0]);
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }

    to.sin_family = AF_INET;

    if((inaddr = inet_addr(argv[1])) == 
INADDR_NONE)
    {
        if((host = gethostbyname(argv[1])) == NULL)
        {
            printf("gethostbyname() erroe\n");
            exit(1);
        }
        to.sin_addr = *(struct in_addr *) host->h_addr_list[0];
    }
    else
        to.sin_addr.s_addr = inaddr;
    
    printf("ping %s (%s) : %d bytes of data.\n",argv[1],inet_ntoa(to.sin_addr),(int)ICMP_SIZE);

    for(int i = 0; i < NUM; i++)
    {
        nsend++;
        memset(&sendicmp, 0 , ICMP_SIZE);
        pack(&sendicmp, nsend);

        if(sendto(sockfd, &sendicmp, ICMP_SIZE,0,(struct sockaddr *)&to,sizeof(to))==-1)
        {
            printf("send() error!\n");
            continue;
        }
        if((n = recvfrom(sockfd, buf,BUF_SIZE,0,(struct sockaddr *)&from,&fromlen)) < 0)
        {
            printf("recvfrom() error!\n");
            continue;
        }
        nreceived ++;
        if(unpack(buf, n, inet_ntoa(from.sin_addr))==-1)
        {
            printf("unpack() error \n");
        }
        sleep(1);
    }

    printf("%s pint statistics \n",argv[1]);
    printf("%d packets transmitted, %d received, %%%d packets loss\n",nsend,nreceived,(nsend-nreceived)/nsend*100);

    return 0;
}

USHORT checkSum(USHORT *addr, int len)
{
    UINT sum = 0;
    while(len > 1)
    {
        sum += *addr++;
        len -= 2;
    }

    if(len == 1)sum += *(UCHAR *)addr;

    sum = (sum >> 16)+(sum & 0xffff);
    sum += (sum >> 16);

    return (USHORT) ~sum;
}

float timediff(struct timeval *begin, struct timeval *end)
{
    int n;
    n = (end->tv_sec - begin->tv_sec)*1000000+(end->tv_usec - begin->tv_usec);
    return (float)(n/1000);
}

void pack(struct icmp *icmp, int sequence)
{
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->id = getpid();
    icmp->sequence = sequence;
    gettimeofday(&icmp->timestamp,0);
    icmp->checksum = checkSum((USHORT *)icmp, ICMP_SIZE);
}


int unpack(char *buf, int len, char *addr)
{
    int  ipheadlen;
    struct ip *ip;
    struct icmp *icmp;
    float rtt;
    struct timeval end;

    ip =  (struct ip *)buf;
    ipheadlen = ip->hlen << 2;

    icmp = (struct icmp *)(buf + ipheadlen);

    len -= ipheadlen;

    if(len < 8)
    {
        printf("ICMP packets\'s length is less than 8\n");
        return -1;
    }

    if(icmp->type != ICMP_ECHOREPLY || icmp->id != getpid())
    {
        printf("ERROE!\n");
        return -1;
    }

    gettimeofday(&end,0);
    rtt = timediff(&icmp->timestamp, &end);

    printf("%d bytes from %s : icmp_seq = %u ttl = %d rttl = %f ms\n",len, addr,icmp->sequence, ip->ttl, rtt);

    return 0;
}
