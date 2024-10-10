#include <stdio.h>
#include <sys/stat.h> 
#include <sys/file.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include<string.h> 

int main(void){
    int fd;
    char text[10]={0};
    struct flock fl; 

    sprintf(text, "desmtiny\n");

    fd = open("text.txt",O_RDWR);
    if(fd==-1){
        printf("open text.txt faild\n");
    }
    else{
        printf("open text.txt successfully\n");
    }
    //Try to write data into by 1 to 5
    fl.l_start = 1;         /* Offset where the lock begins */
    fl.l_len = 5;           /* Number of bytes to lock; 0 means "until EOF" */
    fl.l_type = F_WRLCK;    /* Lock type: F_RDLCK, F_WRLCK, F_UNLCK */
    fl.l_whence = SEEK_SET; /* How to interpret 'l_start': SEEK_SET, SEEK_CUR, SEEK_END */

    if(fcntl(fd,F_SETLK,&fl)==-1){
        printf("can not write into text from bytes %ld to %ld\n",fl.l_start,fl.l_len);
    }

    //Try to write others data into by 6 to 11
    fl.l_start = 6;         /* Offset where the lock begins */
    fl.l_len = 11;           /* Number of bytes to lock; 0 means "until EOF" */
    fl.l_type = F_WRLCK;    /* Lock type: F_RDLCK, F_WRLCK, F_UNLCK */
    fl.l_whence = SEEK_SET; /* How to interpret 'l_start': SEEK_SET, SEEK_CUR, SEEK_END */

    if(fcntl(fd,F_SETLK,&fl)==-1){
        printf("can not write into text from bytes %ld to %ld\n",fl.l_start,fl.l_len);
        return 0;
    }
    else{
        printf("writting data from %ld to %ld\n",fl.l_start,fl.l_len);
        lseek(fd, 6, SEEK_SET);
        if(write(fd,text,strlen(text))==-1){
            printf("Can not write to text.txt\n");
            return 0;
        }
        else{
            printf("file had been written\n"); 
        }
    }
    close(fd); 
    return 0;
}