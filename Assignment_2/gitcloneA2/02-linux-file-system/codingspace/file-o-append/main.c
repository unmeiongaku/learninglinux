#include <stdio.h>
#include<string.h> 
#include<unistd.h> 
#include<fcntl.h> 
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>


int main(void){
	int fd;
	fd = open("text.txt",O_RDWR|O_APPEND,0666);
	char buf[10]="insert\n";
	if(fd==-1){
		printf("open file faild");
		return 0;
	}
	else{
        printf("File had been opened which id: %d\n",fd); 
    }
	if(lseek(fd,1,SEEK_SET)==-1){
		printf("Error seeking file\n");
		close(fd);
	}
	else{
		printf("lseek successfully\n");
	}
	if(write(fd,buf,strlen(buf))==-1){
		printf("Error written file\n");
		close(fd);
	}
	else{
		printf("write successfully\n");
		printf("O_APPEND FLAG IS make the file offset is positioned at the end of the file as if with lseek(2)\n");
	}
	close(fd);
	return 0;
}	

