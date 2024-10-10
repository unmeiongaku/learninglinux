#include <stdio.h>
#include <sys/stat.h> 
#include <sys/file.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include<string.h> 

int main(void){
    int fd; 
    char text[16] = {0}; 
    struct flock fl; 

    sprintf(text, "hello world\n"); 

    fd = open("text.txt",O_RDWR | O_CREAT, 0666);

    if(fd == -1){
        printf("can not create file \n"); 
        return 0; 
    }
    else{
         printf("create file test.txt\n"); 
    }
    printf("fd id %d\n",fd);
    if(write(fd,text,strlen(text))==-1){
        printf("Can not write to text.txt");
        return 0;
    }
    else{
        printf("write file \n"); 
    }

    fl.l_start = 1;         /* Offset where the lock begins */
    fl.l_len = 5;           /* Number of bytes to lock; 0 means "until EOF" */
    fl.l_type = F_WRLCK;    /* Lock type: F_RDLCK, F_WRLCK, F_UNLCK */
    fl.l_whence = SEEK_SET; /* How to interpret 'l_start': SEEK_SET, SEEK_CUR, SEEK_END */

    if(fcntl(fd,F_SETLK,&fl)==-1){
        printf("can not set write lock byte 1-5\n"); 
    }
    else { 
        printf("set write lock byte 1-5 \n"); 
    } 
    /* MAKE PROCESS A RUN ALL TIME
    */
    while (1) { 
        sleep(1); 
    } 
    close(fd);
    return 0;
}
