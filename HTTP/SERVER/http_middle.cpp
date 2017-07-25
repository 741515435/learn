/*************************************************************************
    > File Name: http_middle.cpp
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月24日 星期一 20时05分06秒
 ************************************************************************/

#include <iostream>
#include "http_simple.h"
using namespace std;

void init();
void HttpWork();

class Mainthread
{
public:
    Mainthread()
    {
        //
        port = PORT;
        server_fd = sockt(AF_INET, SOCK_STREAM, 0);
        memset(&server_addr, 0, soizeof(server_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        int opt = SO_REUSEADDR;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        bind(server_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));
        
        listen(server_fd, QUEUE_MAX_COUNT);

        while(1)
        start();
    }

    ~Mainthread()
    {
        //
    }

    void *start()
    {
        sonThread(client_fd);
    }

    void sonThread(int client_fd)
    {
        /////
    }

public:
    int server_fd;
    int client_fd;

    ushort port;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf[BUFF_SIZE];
    char recv_buff[BUFF_SIZE];
    char hello_str[] = "Hello world!";
}

int main(void)
{
    init();
    while(1)
    {
        HttpWork();
    }
    return 0;
}


void init()
{
    
}


