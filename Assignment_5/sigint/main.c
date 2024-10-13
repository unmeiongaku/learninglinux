#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

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

#define MODE_1 1
#define MODE_2 2
#define MODE_3 3

#define FUNC_1 1
#define FUNC_2 2
#define FUNC_3 3
#define FUNC_4 4

#define WAIT_NUMBER 10

typedef struct{
    int mode_number;
    int function_number;
}mode_func_t;

void printlog(){
        printf("MODE_1: Perform The Default Action\n"); //Hành Động phụ thuộc từng loại signal
        printf("    =>  FUNC_1: Exit By Crtl+C\n");
        printf("MODE_2: Catch And Handle The Signal\n");
        printf("    =>  FUNC_1: EXIT Automatic By SIGINT Or Crtl+C\n");
        printf("    =>  FUNC_2: Catch SIGINT Or Crtl+C\n");
        printf("    =>  FUNC_3: Catch SIGKILL & SIGSTOP\n");
        printf("MODE_3: Ignore The Signal\n");
        printf("    =>  FUNC_1: Perform The Default Action\n");
        printf("    =>  FUNC_2: Perform The Default Action\n");
}

int get_mode(const char *mode) {
    if (strcmp(mode, "MODE_1") == 0) {
        return MODE_1;
    } else if (strcmp(mode, "MODE_2") == 0) {
        return MODE_2;
    } else if (strcmp(mode, "MODE_3") == 0) {
        return MODE_3;
    } else {
        printlog();
        exit(EXIT_FAILURE);
    }
}

int get_func(const char *func) {
    if (strcmp(func, "FUNC_1") == 0) {
        return FUNC_1;
    } else if (strcmp(func, "FUNC_2") == 0) {
        return FUNC_2;
    } else if (strcmp(func, "FUNC_3") == 0) {
        return FUNC_3;
    } else if (strcmp(func, "FUNC_4") == 0) {
        return FUNC_4;
    } else {
        printlog();
        exit(EXIT_FAILURE);
    }
}


static mode_func_t mf;

void sig_handler1(int num)
{
	printf("\nIm signal handler1: %d\n", num);
	//while (1);
	exit(EXIT_SUCCESS);
}

void sig_handler2(int num)
{
	printf("Im signal handler2: %d\n", num);
}

void sig_handler3(int num)
{
    printf("Im signal handler3: %d\n", num);
	exit(EXIT_SUCCESS);
}


int main(int argc, char const *argv[]){
    if (argc != 3) {
        log("Usage: ./file-infor MODE_x FUNC_x");
        printlog();
        exit(EXIT_FAILURE);
    }

    mf.mode_number = get_mode(argv[1]);
    mf.function_number = get_func(argv[2]);

    switch (mf.mode_number)
    {
    case MODE_1:
        {
            printf("MODE_1: Perform The Default Action\n");
            switch (mf.function_number)
            {
            case FUNC_1:
                {
                    printf("    =>  FUNC_1: Exit By Crtl+C\n");
                    while (1)
                    {
                         // do nothing.
                        printf("Press Crtl+C to exit\n");
                        sleep(1);
                    }
                }
                break;
            default:
                exit(EXIT_FAILURE);
                break;
            }
        }
        break;
    case MODE_2:
        {
            printf("MODE_2: Catch And Handle The Signal\n");
            switch (mf.function_number)
            {
            case FUNC_1:
                {
                    pid_t child_pid;
                    child_pid = fork();
                    if(child_pid>=0){
                        if(child_pid==0){
                            char pid_str[10];
                            sprintf(pid_str, "%d",getppid());
                            for(int i=0;i<WAIT_NUMBER;i++){
                               sleep(1); 
                            }
                            execl("/bin/kill", "kill", "-2",pid_str,(char *)NULL);
                        }
                        else{
                            printf("    =>  FUNC_1: EXIT Automatic By SIGINT\n");
                            if (signal(SIGINT, sig_handler1) == SIG_ERR) {
                                fprintf(stderr, "Cannot handle SIGINT\n");
                                exit(EXIT_FAILURE);
                            }
                            for(int i=0;i<WAIT_NUMBER;i++){
                                printf("Press Crtl+C (SIGINT) To Exit Or Waiting %ds To Exit Automatically\n",WAIT_NUMBER-i);
                                sleep(1);
                            }
                            while(1);
                        }
                    }
                }
                break;
            case FUNC_2:
            {
                pid_t child_pid;
                child_pid = fork();
                if(child_pid>=0){
                    if(child_pid==0){
                        char pid_str[10];
                        sprintf(pid_str, "%d",getppid());
                        for(int i=0;i<WAIT_NUMBER;i++){
                            sleep(1); 
                        }
                        execl("/bin/kill", "kill", "-2",pid_str,(char *)NULL);
                    }
                    else{
                        printf("    =>  FUNC_2: Catch SIGINT Or Crtl+C but not Exit\n");
                        if (signal(SIGINT, sig_handler2) == SIG_ERR) {
                            fprintf(stderr, "Cannot handle SIGINT\n");
                            exit(EXIT_FAILURE);
                        }
                        for(int i=0;i<WAIT_NUMBER;i++){
                            printf("Press Crtl+C (SIGINT) Or Waiting %ds To Call The Handler Function\n",WAIT_NUMBER-i);
                            sleep(1);
                        }
                        while(1){
                            printf("To Exit, Please Using Kill -9 %d\n",getpid());
                            sleep(1);
                        }
                    }
                }
            }
                break;
            case FUNC_3:
            {
                pid_t child_pid;
                child_pid = fork();
                if(child_pid>=0){
                    if(child_pid==0){
                        char pid_str[10];
                        sprintf(pid_str, "%d",getppid());
                        sleep(1); 
                        for(int i=0;i<WAIT_NUMBER;i++){
                            //execl("/bin/kill", "kill", "-2",pid_str,(char *)NULL);
                            sleep(1); 
                        }
                        printf("SIGKILL or SIGSTOP\n");
                        sleep(3); 
                        execl("/bin/kill", "kill", "-9",pid_str,(char *)NULL);
                    }
                    else{
                        printf("    =>  FUNC_3: Catch SIGKILL\n");
                        if (signal(SIGINT, sig_handler2) == SIG_ERR) {
                            fprintf(stderr, "Cannot handle SIGINT\n");
                            exit(EXIT_FAILURE);
                        }
                        if (signal(SIGKILL, sig_handler2) == SIG_ERR) {
                            fprintf(stderr, "Cannot handle SIGKILL\n");
                        }
                        if (signal(SIGSTOP, sig_handler2) == SIG_ERR) {
                            fprintf(stderr, "Cannot handle SIGSTOP\n");
                        }
                        if (signal(SIGTERM, sig_handler2) == SIG_ERR) {
                            fprintf(stderr, "Cannot handle SIGSTERM\n");
                        }
                        for(int i=0;i<WAIT_NUMBER;i++){
                            printf("Press Crtl+C (SIGINT) or SIGTERM To Call The Handler Function\n");
                            sleep(1);
                        }
                        while(1){
                            printf("To Exit, Please Using Kill -9 %d\n",getpid());
                            sleep(1);
                        }
                    }
                }
            }
                break;
            default:
                printf("No Correct Value\n");
                exit(EXIT_FAILURE); 
                break;
            }
        }
        break;
    case MODE_3:
        {
            printf("MODE_3: Ignore The Signal\n");
        }
        break;
    default:
        printf("Uncorrect Mode\n");
        exit(EXIT_FAILURE);
        break; 
    }
    return 0;
}