/*************************************************************************
    > File Name: modify_config.c
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月21日 星期五 14时45分01秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>


struct Log
{
    int LogLevel;
    char LogPosition[200];
}LOG;


int main(int argc, char *argv[])
{
    FILE *pFile;
    pFile = fopen("config.ini","wb");
    if(pFile == NULL)
    {
        printf("Can not open the file.\n");
    }
    printf("Plese input the LogLevel and LogPosition\n");
    scanf("%d %s",&LOG.LogLevel,LOG.LogPosition);
    fprintf(pFile,"LogLevel:\n%d\nLogPosition:\n%s\n",LOG.LogLevel,LOG.LogPosition);
    fclose(pFile);
    return 0;
}
