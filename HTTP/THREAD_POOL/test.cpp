/*************************************************************************
    > File Name: test.cpp
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月24日 星期一 17时34分04秒
 ************************************************************************/

#include "thread_pool.h"
#include <iostream>
#include <unistd.h>
using namespace std;

struct nodes
{
    int i;
    void process()
    {
        printf("%d\n",i);
    }
}P[1500];

void work()
{
    threadpool<nodes> *Pool;
    Pool = new threadpool<nodes>(4,1000);
    printf("FOR\n");
    //while(1)
    for(int i = 0 ; i < 100; i++)
    {
       // sleep(1);
        P[i].i = i;
        Pool->append(&P[i]);
    }

    sleep(10);
    return ;
}

int main(void)
{
    work();
    return 0;
}
