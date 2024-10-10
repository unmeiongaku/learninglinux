#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include<fcntl.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_hdr1  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_hdr2  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_hdr3  = PTHREAD_COND_INITIALIZER;

pthread_t thread_id1, thread_id2,thread_id3;

bool isreadyforread = true;
bool writtensuccess = false;

#define BUFFER_SIZE 1024

int fd;

int st = 1;

const char *path = "./students_infor.txt";


typedef struct {
    int counter;
}student_t;

static void *handle_th(void *args) 
{
    pthread_t tid = pthread_self();
    student_t *data = (student_t *)args;
    if(pthread_equal(tid, thread_id1)){
        while (1) {
            //pthread_mutex_lock(&mutex);
            printf("Thread 1: %d\n", data->counter);
            data->counter++;
            sleep(1);
            pthread_cond_signal(&cond_hdr1); // thông báo cho thread 2
            pthread_cond_wait(&cond_hdr3, &mutex); // chờ thread 3 kết thúc
            //pthread_mutex_unlock(&mutex);
        }
    }
    else if(pthread_equal(tid, thread_id2)){
        while (1) {
            //pthread_mutex_lock(&mutex);
            pthread_cond_wait(&cond_hdr1, &mutex); // chờ thread 1 kết thúc
            printf("Thread 2: %d\n", data->counter);
            data->counter++;
            sleep(1);
            pthread_cond_signal(&cond_hdr2); // thông báo cho thread 3
            //pthread_mutex_unlock(&mutex);
        }
    }
    else{
        while(1){
            //pthread_mutex_lock(&mutex);
            pthread_cond_wait(&cond_hdr2, &mutex); // chờ thread 2 kết thúc
            printf("Thread 3: %d\n", data->counter);
            data->counter++;
            sleep(1);
            pthread_cond_signal(&cond_hdr3); // thông báo cho thread 1
            //pthread_mutex_unlock(&mutex);
            
        }
    }
}


int main(int argc, char const *argv[]){
    int status, rv;
    int ret;
    student_t std;
    std.counter = 0;
    if (ret = pthread_create(&thread_id1, NULL, &handle_th, &std)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    } 
    if (ret = pthread_create(&thread_id2, NULL, &handle_th, &std)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    } 
    if (ret = pthread_create(&thread_id3, NULL, &handle_th, &std)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    } 

    
    // used to block for the end of a thread and release
    pthread_join(thread_id1,NULL); 
    pthread_join(thread_id2,NULL); 
    pthread_join(thread_id3,NULL); 
    while(1);
    return 0;
}