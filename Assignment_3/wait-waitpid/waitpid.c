#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char const *argv[]){
    pid_t childpid;
    printf("waitpid child process app\n");
    childpid = fork();
    if(childpid>=0){
        if(childpid==0){
            printf("fork() successfully\n");
            printf("i'm child processs,PID: %d\n",getpid());
            printf("Child processs will be termination in 5 seconds\n");
            sleep(5);
            exit(0);
        }
        else{
            printf("Im the parent process, PID: %d\n",getpid());
            int status;
            waitpid(childpid, &status, 0); // Chờ tiến trình con kết thúc
            if (WIFEXITED(status)) {
               printf("Normally termination, status = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
               printf("killed by signal, value = %d\n", WTERMSIG(status));
            }
            while (1)
            {
                sleep(2);
                printf("Doing Parents Job\n");
            }
        }
    }
    else{
        printf("fork() unsucessfully\n");
    }
    return 0;
}