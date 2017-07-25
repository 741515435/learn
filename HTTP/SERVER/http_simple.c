/*************************************************************************
    > File Name: http_simple.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月21日 星期五 17时09分51秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "http_simple.h"

int main()
{
    /* 定义server和client的文件描述符 */
    int server_fd = -1;
    int client_fd = -1;

    u_short port = PORT;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char buf[BUFF_SIZE];
    char recv_buf[BUFF_SIZE];
    char hello_str[] = "Hello world!";

    int hello_len = 0;

    /* 创建一个socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(-1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    /* 设置端口，IP，和TCP/IP协议族 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 绑定套接字到端口 */
    if (bind(server_fd, (struct sockaddr *)&server_addr,
         sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

    /* 启动socket监听请求，开始等待客户端发来的请求 */
    if (listen(server_fd, QUEUE_MAX_COUNT) < 0) {
        perror("listen");
        exit(-1);
    }

    printf("http server running on port %d\n", port);

    while (1) {
        /* 调用了accept函数，阻塞了程序，直到接收到客户端的请求 */
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                   &client_addr_len);
        if (client_fd < 0) {
            perror("accept");
            exit(-1);
        }
        printf("accept a client\n");

        printf("client socket fd: %d\n", client_fd);
        /* 调用recv函数接收客户端发来的请求信息 */
        hello_len = recv(client_fd, recv_buf, BUFF_SIZE, 0);

        printf("receive %d\n!!! %s\n", hello_len,recv_buf);

        /* 发送响应给客户端 */
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        printf("%s",buf);
        send(client_fd, buf, strlen(buf), 0);
        strcpy(buf, SERVER_STRING);
        printf("%s",buf);
        send(client_fd, buf, strlen(buf), 0);
        sprintf(buf, "Content-Type: text/html\r\n");
        printf("%s",buf);
        send(client_fd, buf, strlen(buf), 0);
        strcpy(buf, "\r\n");
        printf("%s",buf);
        send(client_fd, buf, strlen(buf), 0);
        sprintf(buf, "Hello World\r\n");
        printf("%s",buf);
        send(client_fd, buf, strlen(buf), 0);

        /* 关闭客户端套接字 */
        close(client_fd);
    }

    close(server_fd);

    return 0;
}
