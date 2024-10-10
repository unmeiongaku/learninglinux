#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


void signal_handler(int signum){
    printf("I'm signal handle function\n");
    printf("This time you cann't see zombie process of Child process\n");
    printf("Use Kill -17 + parents number to callback function\n");
}

int main(int agrc, char const *agrv[]){
    pid_t child_pid;
    int status, rv;
    printf("Safe Fork\n");
    child_pid = fork();
    if(child_pid>=0){
        if(child_pid == 0){
            printf("\nIm the child process, PID: %d\n",getpid());
            printf("Killchild process by using kill -9 %d\n",getpid());
            while (1);
        }
        else{
            signal(SIGCHLD,signal_handler);
            printf("\nIm the parent process, PID: %d\n",getpid());
            while(1){
                printf("Doing Parents Job\n");
                sleep(2);
            }
        }   
    }
    else{
        printf("fork() unsuccessfully\n");
    }
    return 0;
}