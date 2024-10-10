#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char const * argv[]){
    pid_t child_pid;
    int counter = 0;

    printf("Gia tri khoi tao cua counter: %d\n",counter);

    child_pid = fork();

    if(child_pid>=0){
        if(child_pid == 0){
            printf("\nIm the child process, counter: %d\n",++counter);
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
        }
        else{
            printf("\nIm the parent process, counter: %d\n",++counter);
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
        }   
        while(1);
    }
    else{
        printf("fork() unsuccessfully\n");
    }
    return 0;
}
