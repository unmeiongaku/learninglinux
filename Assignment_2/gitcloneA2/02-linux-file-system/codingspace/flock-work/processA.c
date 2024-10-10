#include <sys/stat.h>
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h> 


int main(void){
    int fd;
    char buf[16] = {0};
    sprintf(buf,"hello world\n");
    fd = open("text.txt",O_RDWR|O_CREAT,0666);
    if(fd==-1){
        printf("Open file text.txt faild\n");
        return 0;
    }
    else{
        printf("open successfully\n");
    }

    if(write(fd,buf,strlen(buf)) == -1){
        printf("can not write file \n");
		return 0;
    }
    else{
        printf("file had been written \n");
    }

    if(flock(fd,LOCK_SH)==-1){
        printf("Can not set read lock\n");
    }
    else{
        printf("Set Read Lock\n");
    }
    
	while(1) {
		sleep(1);
	}
	close(fd);
    
    return 0;
}