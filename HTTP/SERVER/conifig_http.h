/*************************************************************************
    > File Name: conifig_http.h
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月27日 星期四 16时22分44秒
 ************************************************************************/

#ifndef _CONIFIG_HTTP_H
#define _CONIFIG_HTTP_H

#define BUFF_SIZE 1024
#include <stdlib.h>

int  PORT; //9002
int  QUEUE_MAX_COUNT; //100
int  THREAD_NUMBER; //4
char LOG_NAME[20]; //"./my_log"
int  LOG_SPLIT_LINES;
int  LOG_MAX_QUEUE_SIZE;
int  KEEP_ALIVE;
int  USE_LOG;

void Display();

int config_init()
{
    FILE *pFile;
    pFile = fopen("config.ini","r");

    if(pFile == NULL)
        return -1;

    //PORT
    fscanf(pFile,"%*s%d",&PORT);
    if(PORT < 1024 || PORT > 65535)
    {
        PORT = 9002;
    }
    
    //THREAD_NUMBER
    fscanf(pFile,"%*s%d",&THREAD_NUMBER);
    if(THREAD_NUMBER < 1 || THREAD_NUMBER > 16)
    {
        THREAD_NUMBER = 4;
    }
        
    //QUEUE_MAX_COUNT
    fscanf(pFile,"%*s%d",&QUEUE_MAX_COUNT);
    if(QUEUE_MAX_COUNT < 10 || QUEUE_MAX_COUNT > 100000)
    {
        QUEUE_MAX_COUNT = 2048;
    }
    
    //KEEP_ALIVE
    fscanf(pFile,"%*s%d",&KEEP_ALIVE);
    if(KEEP_ALIVE < 0)
    {
        KEEP_ALIVE = 0;
    }
    
    //USE_LOG
    fscanf(pFile,"%*s%d",&USE_LOG);
    if(USE_LOG < 0)
    {
        USE_LOG = 0;
    }

    //LOG_NAME
    fscanf(pFile,"%*s%s",LOG_NAME);
    if(strlen(LOG_NAME) > 100)
    {
        strcpy(LOG_NAME,"./day_log");
    }
    //LOG_SPLIT_LINES
    fscanf(pFile,"%*s%d",&LOG_SPLIT_LINES);
    if(LOG_SPLIT_LINES < 100 || LOG_SPLIT_LINES > 1000000)
    {
        LOG_SPLIT_LINES = 20000;
    }
    //LOG_MAX_QUEUE_SIZE
    fscanf(pFile,"%*s%d",&LOG_MAX_QUEUE_SIZE);
    if(LOG_MAX_QUEUE_SIZE < 0 || LOG_MAX_QUEUE_SIZE >= 1000000)
    {
        LOG_MAX_QUEUE_SIZE = 50000;
    }
    fclose(pFile);
    Display();

    return 1;
}

void Display()
{
    printf("/******************CONFIG SETTING*************************/\n");
    printf("/***  PORT:                      %d **********************/\n",PORT);
    printf("/***  THREAD NUMBER:             %d **********************/\n",THREAD_NUMBER);
    printf("/***  QUEUE MAX COUNT:           %d **********************/\n",QUEUE_MAX_COUNT);
    printf("/***  KEEP_ALIVE:                %s **********************/\n",KEEP_ALIVE == 0?"false":"true");
    printf("/***  LOG_USE:                   %s **********************/\n",USE_LOG == 0 ? "false":"true");
    printf("/***  LOG_NAME:                  %s **********************/\n",LOG_NAME);
    printf("/***  LOG_SPLIT_LINES:           %d **********************/\n",LOG_SPLIT_LINES);
    printf("/***  LOG_MAX_QUEUE_SIZE:        %d **********************/\n",LOG_MAX_QUEUE_SIZE);
    printf("/***************CONFIG SETTING END************************/\n");
}

#endif
