// C program to illustrate 
// I/O system Calls 
#include<stdio.h> 
#include<string.h> 
#include<unistd.h> 
#include<fcntl.h> 

int main(void){
    int fd; //file descriptor
    int numb_read,numb_write;
    char buf[14] = "minh dep trai\n";
    //openfile minh.txt , if minh.txt hadn't been created, use flag 0_CREAT
    fd = open("minh.txt",O_RDWR|O_CREAT,0644);
    if(-1==fd){
        printf("open() minh.txt failed\n");
    }
    //write buf into minh.txt
    numb_write = write(fd,buf,strlen(buf));
    printf("Write %d bytes into minh.txt\n",numb_write);
    //write sieu cap into minh.txt
    lseek(fd,5,SEEK_SET);
    write(fd,"sieu cap dep trai\n",strlen("sieu cap dep trai\n"));
    close(fd);
    return 0;
}
