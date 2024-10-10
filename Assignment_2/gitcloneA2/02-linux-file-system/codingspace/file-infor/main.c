#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define LOG 0X01
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
    struct stat sb;
    /*Checking there are 2 parameters or not ( Including name of the process and path of file ) */
    if(argc!=2){
        log("Usage: ./file-infor /path/to/file");
        exit(EXIT_FAILURE);
    }
    /*Checking that we can get the file infor or not*/
    //int lstat(const char *pathname, struct stat *statbuf);
    if(-1 == lstat(argv[1], &sb)){
        log("lstat");
        exit(EXIT_FAILURE);
    }
    printf("ID of containing device:  [%x,%x]\n",
    major(sb.st_dev),
    minor(sb.st_dev));
    printf("File Type: ");
    switch(sb.st_mode & S_IFMT){
        case S_IFBLK:   printf("block device\n");       break;
        case S_IFCHR:   printf("character device\n");   break;
        case S_IFDIR:   printf("directory\n");          break;
        case S_IFIFO:   printf("FIFO/pipe\n");          break;
        case S_IFLNK:   printf("symlink\n");            break;
        case S_IFREG:   printf("regular file\n");       break;
        case S_IFSOCK:  printf("socket\n");             break;
        default:        printf("unknown?\n");
    }
    
    printf("I-node number:            %ju\n", (uintmax_t) sb.st_ino);

    printf("Mode:                     %jo (octal)\n",
            (uintmax_t) sb.st_mode);

    printf("Link count:               %ju\n", (uintmax_t) sb.st_nlink);
    printf("Ownership:                UID=%ju   GID=%ju\n",
            (uintmax_t) sb.st_uid, (uintmax_t) sb.st_gid);

    printf("Preferred I/O block size: %jd bytes\n",
            (intmax_t) sb.st_blksize);
    printf("File size:                %jd bytes\n",
            (intmax_t) sb.st_size);
    printf("Blocks allocated:         %jd\n",
            (intmax_t) sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));

    exit(EXIT_SUCCESS);

    return 0;
}

