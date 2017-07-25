/*************************************************************************
    > File Name: m.cpp
    > Author: xxx
    > Mail: XXX.com
    > Created Time: 2017年07月22日 星期六 14时22分14秒
 ************************************************************************/

#include <iostream>
#include <sys/stat.h>  
  
using namespace std;
unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}  
int main(void)
{
    cout<<get_file_size("2017_07_22_mylog.log");
    
    return 0;
}
