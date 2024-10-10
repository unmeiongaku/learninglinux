#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define LOG     0x01
#define log(...) {  \
    do {    \
        if ((LOG)) { \
            printf("\n[%s:%s:%d] ==>> ", __FILE__, __func__, __LINE__); \
            printf(__VA_ARGS__); \
            printf("\n"); \
        } \
    } while (0); \
} // Logging

int main(int argc, char const *argv[]){
    if (argc != 2) {
        log("Usage: ./file-infor MODE_1 or MODE_2");
        printf("MODE_1: Auto terminate child process\n");
        printf("MODE_2: Terminate child process by exec\n");
        exit(EXIT_FAILURE);
    }
    pid_t child_pid;
    int status, rv;
    printf("wait child process app\n");
    child_pid = fork();
    if(child_pid>=0){
        if(child_pid==0){
            printf("fork() successfully\n");
            printf("i'm child processs, my PID: %d\n",getpid());
            if(strcmp(argv[1], "MODE_1")==0){
                printf("Child process will be terminated after 5 seconds\n");
                sleep(5);
                exit(0);
            }
            else if(strcmp(argv[1], "MODE_2")==0){
                char pid_str[10];
                sprintf(pid_str, "%d",getpid());
                printf("Run command <kill -9 %d> after 2 seconds\n",getpid());
                sleep(2);
                execl("/bin/kill", "kill", "-9",pid_str,NULL);
            }
            else{
                printf("Wrong Command\n");
                printf("Auto exit in 3 seconds\n");
                sleep(3);
                exit(EXIT_FAILURE);
            }
        }
        else{
            rv = wait(&status);
            if (rv == -1) {
                printf("wait() unsuccessful\n");
            }
            printf("Im the parent process, PID child process: %d\n", rv);
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