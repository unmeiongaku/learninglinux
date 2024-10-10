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
pthread_cond_t cond_write  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_read  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_insert  = PTHREAD_COND_INITIALIZER;

pthread_t thread_id1, thread_id2,thread_id3;

bool isreadyforread = true;
bool writtensuccess = false;

#define BUFFER_SIZE 1024

int fd;

const char *path = "./students_infor.txt";

typedef struct {
    char name[30];
    char birthday[30];
    char birthplace[30];
    int stt;
}student_t;

static void *handle_th(void *args) 
{
    pthread_t tid = pthread_self();
    student_t *data = (student_t *)args;
    if(pthread_equal(tid, thread_id1)){
        while (1) {
            //pthread_mutex_lock(&mutex);
            printf("I'm thread_handle_id1\n");
            printf("Enter %d Student Information:\n",data->stt);

            printf("Name: ");
            fgets(data->name, sizeof(data->name), stdin);  // Đọc tên có khoảng trắng
            data->name[strcspn(data->name, "\n")] = '\0';  // Xóa ký tự xuống dòng
            //printf("%s\n", data->name);

            printf("Birthday: ");
            fgets(data->birthday, sizeof(data->birthday), stdin);  // Đọc ngày sinh
            data->birthday[strcspn(data->birthday, "\n")] = '\0';  // Xóa ký tự xuống dòng
            //printf("%s\n", data->birthday);

            printf("Birthplace: ");
            fgets(data->birthplace, sizeof(data->birthplace), stdin);  // Đọc nơi sinh
            data->birthplace[strcspn(data->birthplace, "\n")] = '\0';  // Xóa ký tự xuống dòng
            //printf("%s\n", data->birthplace);
            pthread_cond_signal(&cond_write);
            pthread_cond_wait(&cond_insert, &mutex); // chờ thread 3 kết thúc
            //pthread_mutex_unlock(&mutex);  
        }
        return NULL;
    }
    else if(pthread_equal(tid, thread_id2)){
        while (1) {
            //pthread_mutex_lock(&mutex); 
            pthread_cond_wait(&cond_write,&mutex);
            printf("I'm thread_handle_id2\n");
            fd = open("students_infor.txt",O_RDWR|O_APPEND|O_CREAT,0666);
            if(fd==-1){
                printf("Open file text.txt faild\n");
                return 0;
            }
            else{
                printf("open fd id: %d successfully\n",fd);
            }
            printf("Writting Data...\n");
            write(fd, data->name, strlen(data->name));
            write(fd, " ",1);
            write(fd, data->birthday, strlen(data->birthday));
            write(fd, " ",1);
            write(fd, data->birthplace, strlen(data->birthplace));
            write(fd, "\n",1);
            printf("Success...\n");
            close(fd);
            isreadyforread = false;
            writtensuccess = true;
            pthread_cond_signal(&cond_read); // thông báo cho thread 3
            //pthread_mutex_unlock(&mutex);
        }
    }
    else{
        while(1){
            pthread_cond_wait(&cond_read, &mutex); // thông báo cho thread 3
            int file = open("students_infor.txt", O_RDONLY);
            if (file < 0) {
                perror("Cannot open file");
            }           
            // Di chuyển con trỏ file về cuối file
            off_t file_size = lseek(file, 0, SEEK_END);
            if (file_size == -1) {
                perror("lseek error");
                close(file);
            }

            // Bắt đầu đọc ngược từ cuối file để tìm dòng cuối cùng
            off_t position = file_size - 1;
            char buffer[BUFFER_SIZE];
            int found_newline = 0; // Biến để đánh dấu khi tìm thấy ký tự xuống dòng

            while (position >= 0 && !found_newline) {
                // Di chuyển con trỏ file từng byte
                lseek(file, position, SEEK_SET);

                // Đọc 1 byte từ file
                char c;
                if (read(file, &c, 1) == -1) {
                    perror("read error");
                    close(file);
                }

                // Kiểm tra nếu gặp ký tự xuống dòng
                if (c == '\n' && position != file_size - 1) {
                    found_newline = 1;
                    break;
                }
                position--;
            }

            // Đọc dòng cuối cùng
            off_t last_line_start = position + 1;
            lseek(file, last_line_start, SEEK_SET);

            // Đọc từ vị trí đó đến hết file
            ssize_t bytes_read = read(file, buffer, file_size - last_line_start);
            if (bytes_read == -1) {
                perror("read error");
                close(file);
            }

            // Thêm ký tự null terminator để biến buffer thành chuỗi
            buffer[bytes_read] = '\0';

            // In ra dòng cuối cùng
            printf("Last line: %s\n", buffer);

            // Đóng file
            close(file);
            data->stt++;
            pthread_cond_signal(&cond_insert); // thông báo cho thread 3
        }
    }
}


int main(int argc, char const *argv[]){
    int status, rv;
    pid_t pid = fork();
    if(pid>=0){
        if(pid==0){
            printf("fork() successfully\n");
            printf("i'm child processs, my PID: %d\n",getpid());
            if (access(path, F_OK) != -1) {
                // Tệp tồn tại
                printf("File students_infor.txt available, removing file...\n");
                printf("Run command <rm - file> after 1 seconds\n");
                sleep(1);
                execl("/bin/rm", "rm", "-r", "-f", "students_infor.txt",(char *)NULL);
                exit(EXIT_FAILURE);
            }
            else{
                // Tệp khong tồn tại
                printf("File students_infor.txt unavailable\n");
                exit(EXIT_FAILURE);
            }
        }
        else{
            rv = wait(&status);
            if (rv == -1) {
                printf("wait() unsuccessful\n");
            }
            printf("Im the parent process, PID child process: %d\n", rv);
            int ret;
            student_t std;
            std.stt = 1;
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

            //while(1);
            // used to block for the end of a thread and release
            pthread_join(thread_id1,NULL); 
            pthread_join(thread_id2,NULL); 
            pthread_join(thread_id3,NULL); 

            while(1);
        }
    }
    return 0;
}