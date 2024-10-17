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

void sig_handler1(int signum) {
    printf("Im signal SIGHUP, sig number: %d\n: ",signum);
}

void sig_handler2(int signum) {
    printf("Im signal SIGINT, sig number: %d\n: ",signum);
}

void sig_handler3(int signum) {
    printf("Im signal SIGQUIT, sig number: %d\n: ",signum);
}

void printlog(){
        printf("MODE_1: SIG_SETMARK\n"); 
        printf("MODE_2: SIG_BLOCK\n"); 
        printf("MODE_3: SIG_UNBLOCK\n");
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

// int get_func(const char *func) {
//     if (strcmp(func, "FUNC_1") == 0) {
//         return FUNC_1;
//     } else if (strcmp(func, "FUNC_2") == 0) {
//         return FUNC_2;
//     } else if (strcmp(func, "FUNC_3") == 0) {
//         return FUNC_3;
//     } else if (strcmp(func, "FUNC_4") == 0) {
//         return FUNC_4;
//     } else {
//         printlog();
//         exit(EXIT_FAILURE);
//     }
// }

void set_current_sig(sigset_t *new_set, sigset_t *old_set, int sigquitnb,int sigintnb,int sighupnb){
    printf("SIG SIGQUIT SIGINT SIGHUP\n");
    printf("BIT       %d      %d      %d\n",sigquitnb,sigintnb,sighupnb);
    if(sigquitnb==1){
       sigaddset(new_set, SIGQUIT); //blocking SIGINT 
    }
    else if(sigquitnb==0){
       sigdelset(new_set, SIGQUIT); //blocking SIGINT 
    }
    else{
        printf("sig number 0 or 1\n");
        exit(EXIT_FAILURE);
    }

    if(sigintnb==1){
       sigaddset(new_set, SIGINT); //blocking SIGINT 
    }
    else if(sigintnb==0){
        sigdelset(new_set, SIGINT); //blocking SIGINT 
    }
    else{
        printf("sig number 0 or 1\n");
        exit(EXIT_FAILURE);
    }

    if(sighupnb==1){
       sigaddset(new_set, SIGHUP); //blocking SIGINT 
    }
    else if(sighupnb==0){
        sigdelset(new_set, SIGHUP); //blocking SIGINT 
    }
    else{
        printf("sig number 0 or 1\n");
        exit(EXIT_FAILURE);
    }
    sigprocmask(SIG_SETMASK, new_set, old_set);
}

static mode_func_t mf;

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        log("Usage: ./file-infor MODE_x FUNC_x");
        printlog();
        exit(EXIT_FAILURE);
    }

    mf.mode_number = get_mode(argv[1]);

    sigset_t new_set, old_set;
    if (signal(SIGINT, sig_handler2) == SIG_ERR) {
        fprintf(stderr, "Cannot handle SIGINT\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, sig_handler3) == SIG_ERR) {
        fprintf(stderr, "Cannot handle SIGQUIT\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGHUP, sig_handler1) == SIG_ERR) {
        fprintf(stderr, "Cannot handle SIGHUP\n");
        exit(EXIT_FAILURE);
    }
    sigemptyset(&new_set);
    sigemptyset(&old_set);

    switch (mf.mode_number){
        case MODE_1:
        {
            printf("MODE_1: SIG_SETMARK\n"); 
            /*Set Current Mask*/
            set_current_sig(&new_set,&old_set,1,1,0);
            sigaddset(&new_set, SIGINT); //blocking SIGINT
            //printf("new_set is %x\n", new_set);
            if (sigprocmask(SIG_SETMASK, &new_set, &old_set) == 0) {
                // sigprocmask(SIG_SETMASK, NULL, &old_set);
                if (sigismember(&new_set, SIGINT) == 1 ) {
                    printf("SIGINT exist\n");
                } else if (sigismember(&new_set, SIGINT) == 0) {
                    printf("SIGINT does not exist\n");
                }
                if (sigismember(&new_set, SIGHUP) == 1 ) {
                    printf("SIGHUP exist\n");
                } else if (sigismember(&new_set, SIGHUP) == 0) {
                    printf("SIGHUP does not exist\n");
                }
                if (sigismember(&new_set, SIGQUIT) == 1 ) {
                    printf("SIGQUIT exist\n");
                } else if (sigismember(&new_set, SIGQUIT) == 0) {
                    printf("SIGQUIT does not exist\n");
                }
            }
            while (1){              
            }
        }
            break;
        case MODE_2:
        {
            printf("MODE_2: SIG_BLOCK\n");
            set_current_sig(&new_set,&old_set,0,1,0);
            sigdelset(&new_set, SIGQUIT); //unblocking SIGQUIT
            sigdelset(&new_set, SIGINT); //unblocking SIGINT
            sigaddset(&new_set, SIGHUP); //blocking SIGHUP
            // 0  1 0 
            // 0  0 1
            // 0  1 1  
            //printf("new_set is %x\n", new_set);
            if (sigprocmask(SIG_BLOCK, &new_set, &old_set) == 0) {
                // sigprocmask(SIG_SETMASK, NULL, &old_set);
                if (sigismember(&new_set, SIGINT) == 1 ) {
                    printf("SIGINT exist\n");
                } else if (sigismember(&new_set, SIGINT) == 0) {
                    printf("SIGINT does not exist\n");
                }
                if (sigismember(&new_set, SIGHUP) == 1 ) {
                    printf("SIGHUP exist\n");
                } else if (sigismember(&new_set, SIGHUP) == 0) {
                    printf("SIGHUP does not exist\n");
                }
                if (sigismember(&new_set, SIGQUIT) == 1 ) {
                    printf("SIGQUIT exist\n");
                } else if (sigismember(&new_set, SIGQUIT) == 0) {
                    printf("SIGQUIT does not exist\n");
                }
            }
            while (1){           
            }
        }
            break;
        case MODE_3:
        {
            printf("MODE_3: SIG_UNBLOCK\n");
            set_current_sig(&new_set,&old_set,1,0,1);
            sigdelset(&new_set, SIGQUIT); //unblocking SIGQUIT
            sigdelset(&new_set, SIGINT); //unblocking SIGINT
            sigaddset(&new_set, SIGHUP); //blocking SIGHUP
            // 1  0 1
            // 0  0 1
            // 1  0 0  
            //printf("new_set is %x\n", new_set);
            if (sigprocmask(SIG_UNBLOCK, &new_set, &old_set) == 0) {
                // sigprocmask(SIG_SETMASK, NULL, &old_set);
                if (sigismember(&new_set, SIGINT) == 1 ) {
                    printf("SIGINT exist\n");
                } else if (sigismember(&new_set, SIGINT) == 0) {
                    printf("SIGINT does not exist\n");
                }
                if (sigismember(&new_set, SIGHUP) == 1 ) {
                    printf("SIGHUP exist\n");
                } else if (sigismember(&new_set, SIGHUP) == 0) {
                    printf("SIGHUP does not exist\n");
                }
                if (sigismember(&new_set, SIGQUIT) == 1 ) {
                    printf("SIGQUIT exist\n");
                } else if (sigismember(&new_set, SIGQUIT) == 0) {
                    printf("SIGQUIT does not exist\n");
                }
            }
            while (1){           
            }
        }
            break;
        default:
            exit(EXIT_FAILURE);
        break;
    }	
	return 0;
}