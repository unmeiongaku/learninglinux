#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


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


pthread_t thread_id1, thread_id2, thread_id3, thread_id4;

typedef struct {
    char name[30];
    char msg[30];
    char mode[30];
    int number;
} thr_data_t;

static void *thr_handle1(void *args)
{
    thr_data_t *thr = (thr_data_t *)args;
    sleep(1);
    while (1) {
        printf("hello %s%d !\n", thr->name,thr->number);
        printf("thread1 handler\n");
        sleep(2);
    }
    //pthread_exit(NULL); // exit

}

static void *thr_handle2(void *args)
{
    thr_data_t *thr = (thr_data_t *)args;
    sleep(2);
    //pthread_exit(NULL); // exit
    //exit(1);
    while (1) {
        for(int i=0;i<=5;i++){
            printf("thread2 handler %d\n",i); 
   	        sleep(1);  
        }
        if(strcmp(thr->mode, "MODE_1")==0){
            printf("thread2 handler pthread_exit\n"); 
            pthread_exit(NULL); // exit
        }
        else if(strcmp(thr->mode, "MODE_2")==0){
            printf("thread2 handler call systerm exit\n"); 
            exit(1);
        }
    };
}

static void *thr_handle3(void *args)
{
    thr_data_t *thr = (thr_data_t *)args;
    while (1) {
        printf("thread3 handler\n");
        sleep(2);
    }
    //pthread_exit(NULL); // exit

}

static void *thr_handle4(void *args)
{
    pthread_detach(pthread_self());
    //sleep(2);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]){

    if (argc != 2) {
        log("Usage: ./file-infor MODE_1 or MODE_2");
        printf("MODE_1: Thr_handle2 call pthread_exit\n");
        printf("MODE_2: Thr_handle2 call systerm exit\n");
        printf("MODE_3: pthread create counter\n");
        exit(EXIT_FAILURE);
    }

    /* code */
    int ret, counter = 0;
    int retval;
    thr_data_t data = {0};

    strncpy(data.name, "desmtiny", sizeof(data.name));                 
    strncpy(data.msg, "Posix thread programming\n", sizeof(data.msg));
    strncpy(data.mode, argv[1], sizeof(data.mode));

    data.number = 33;
    
    if(strcmp(argv[1], "MODE_3")!=0){
        if (ret = pthread_create(&thread_id1, NULL, &thr_handle1, &data)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
        }

        if (ret = pthread_create(&thread_id2, NULL, &thr_handle2, &data)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
        }

        if (ret = pthread_create(&thread_id3, NULL, &thr_handle3, NULL)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
        }

    }
    while(strcmp(argv[1], "MODE_3")==0){
        if (ret = pthread_create(&thread_id4, NULL, &thr_handle4, NULL)) {
        	printf("pthread_create() error number=%d\n", ret);
        	break;
    	}
	    counter++;
  	    //pthread_join(thread_id4, NULL);

        if (counter%1000 == 0) {
            printf("Thread created: %d\n", counter);
        }
    }
    while(1);
    return 0;
}