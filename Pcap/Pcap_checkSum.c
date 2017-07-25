/*************************************************************************
    > File Name: Pcap_checkSum.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月20日 星期四 11时06分22秒
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

struct pacp_file_header
{
    uint32_t magic;
    uint16_t version_major;
    uint16_t version_minor;
    uint32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t linktype;
} pcap_head;
struct packet_file_header
{
    uint32_t time_stamp_h;
    uint32_t time_stamp_l;
    uint32_t caplen;
    uint32_t len;
} packet_header;

struct psd_tcp_file_header
{
    uint32_t saddr;
    uint32_t daddr;
    char     mbz;
    char     protocal;
    uint16_t tcpl;
} psd_header;


struct iphdr ip;
struct tcphdr tcp;

char network[14];
char Data[3000];
char buffer[5000];


uint16_t checkSum(uint16_t *addr, int len);

int main(void)
{
    FILE *pFile;
    int n = 0;
    uint16_t pre;
    uint16_t cal;

    pFile = fopen("tcpsyn.pcap", "rb");

    if(pFile == NULL)
    {
        printf("Can not open the file\n");
        exit(1); 
    }

    fread(&pcap_head, 24, 1, pFile);
    while( fread(&packet_header, 16,1, pFile) == 1 )
     {
        fread(network, 14, 1, pFile);
        fread(&ip, 20, 1, pFile);
        fread(&tcp,20, 1, pFile);
        fread(Data, packet_header.caplen - 54, 1, pFile);
        //printf("%s\n",Data);
        
        //Check IP
        pre = ip.check;
        ip.check = 0;
        cal = checkSum((uint16_t *)&ip, sizeof(ip));
        printf("IP checksum: pre: %x  cal: %x\n",pre, cal);

        //Check TCP
        psd_header.saddr = ip.saddr;
        psd_header.daddr = ip.daddr;
        psd_header.mbz   = 0;
        psd_header.protocal =ip.protocol;
        psd_header.tcpl = htons( ntohs(ip.tot_len) - 20 );
        pre = tcp.th_sum;
        tcp.th_sum = 0;
        memcpy(buffer,&psd_header,sizeof(psd_header));
        memcpy(buffer+sizeof(psd_header),&tcp, sizeof(tcp));
        printf("%d\n",ip.tot_len);
        memcpy(buffer+sizeof(psd_header)+sizeof(tcp), Data, ntohs(psd_header.tcpl) - 20);
        cal = checkSum((uint16_t *)&buffer, sizeof(psd_header) + ntohs(psd_header.tcpl));
        printf("TCP checksum: pre: %x cal: %x\n",pre, cal);

        //printf("%s\n",Data);
    }
    fclose(pFile);
    return 0;
}


uint16_t checkSum(uint16_t *addr, int len)
{
    uint32_t sum = 0;
    while(len > 1)
    {
        sum += *addr++;
        len -= 2;
    }

    if(len == 1)sum += *(char *)addr;

    sum = (sum >> 16)+(sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t) ~sum;
}
