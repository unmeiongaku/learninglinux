#include <sys/stat.h>
#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include<string.h> 

int main(void){
    int fd;
	char buf[16] = {0};

	if((fd=open("./text.txt",O_RDWR)) == -1) {
		printf("can not open file \n");
		return 0;
	} else {
		printf("open file text.txt \n");
	}

    if(flock(fd,LOCK_EX|LOCK_NB)==-1){
        /*If Do not using flag LOCK_NB, the process will be stuck at flock*/
        printf("Can not get write lock\n");
    }

    if(flock(fd,LOCK_SH|LOCK_NB)==-1){
        printf("Can not get read Lock\n");
    }
    else{
        printf("get read lock file\n");
        if(read(fd,buf,sizeof(buf))==-1){
            printf("Can not read text.txt\n");
            return 0;
        }
        else{
            printf("%s",buf);
        }
    }
    close(fd);
    return 0;
}