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

struct iphdr ip;
//struct tcphdr tcp;

char network[14];
char Data[3000];



int main(void)
{
    FILE *pFile;
    int n = 0;

    pFile = fopen("tcpsyn.pcap", "rb");

    if(pFile == NULL)
    {
        printf("Can not open the file\n");
        exit(1); 
    }

    fread(&pcap_head, 24, 1, pFile);
    printf("%x\n",pcap_head.magic);
    printf("%x\n",pcap_head.version_major);
    n = fread(&packet_header, 16,1, pFile);
    {
        printf("%d\n",n);
        if(n != 16)
        {
            printf("data error !\n");
            return 0;
        }

        fread(network, 14, 1, pFile);
        fread(&ip, 20, 1, pFile);
       // fread(&tcp,20, 1, pFile);
        fread(Data, packet_header.caplen - 54, 1, pFile);
        printf("%s\n",Data);
    }
    fclose(pFile);
    return 0;
}

