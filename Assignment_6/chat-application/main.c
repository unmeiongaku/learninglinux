#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>


#define HELP_COMMAND_DEFINE 1;
#define MYIP_COMMAND_DEFINE 2;
#define MYPORT_COMMAND_DEFINE 3;
#define CONNECT_COMMAND_DEFINE 4;
#define LIST_COMMAND_DEFINE 5;
#define TERMINATE_COMMAND_DEFINE 6;
#define SEND_COMMAND_DEFINE 7;
#define EXIT_COMMAND_DEFINE 8;


#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


void print_menu(){
    printf("\n*****************Chat Application*********************\n\n");
    printf("Use these commands below:\n\n");
    printf("1. help                             : Display user interface options\n");
    printf("2. myip                             : Display IP address of this app\n");
    printf("3. myport                           : Display listening port of this app\n");
    printf("4. connect <destination> <port no>  : Display listening port of this app\n");
    printf("5. list                             : List all the connections of this app\n");
    printf("6. terminate <connection id>        : terminate the connections\n");
    printf("7. send <connection id> <message>   : send a message to the connection\n");
    printf("8. exit                             : Close all connections & terminate this app\n");
    printf("\n******************************************************\n\n");
}

typedef struct{
    int port_no; 
    int len; 
    int opt;
    int server_fd;
    int new_socket_fd;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in client_addr;
    int command;
}app_server_client_t;

pthread_t receive_thr, send_command_thr;

static app_server_client_t appchat;


int get_command(const char *cmd){
    if (strcmp(mode, "help") == 0) {
        return HELP_COMMAND_DEFINE;
    } else if (strcmp(mode, "myip") == 0) {
        return MYIP_COMMAND_DEFINE;
    } else if (strcmp(mode, "myport") == 0) {
        return MYPORT_COMMAND_DEFINE;
    }
    else if (strcmp(mode, "list") == 0) {
        return LIST_COMMAND_DEFINE;
    }
    else if (strcmp(mode, "exit") == 0) {
        return EXIT_COMMAND_DEFINE;
    }
}

static void *receive_handle(void *args)
{
    app_server_client_t *app = (app_server_client_t *)args;
}

static void *send_command_handle(void *args)
{
    int numb_read, numb_write;
    char commandbuff[BUFF_SIZE];
    app_server_client_t *app = (app_server_client_t *)args;
    while(1){
        printf("Command: ");
        fgets(commandbuff, BUFF_SIZE, stdin);

        switch(app->command){
            case HELP_COMMAND_DEFINE:
            {
                print_menu();
            }
                break;
            case MYIP_COMMAND_DEFINE:
            {

            }
                break;
        }
    }
}


int main(int argc, char *argv[]){

    if (argc < 2) {
        printf("No port provided\ncommand: ./server <port number>\n");
        exit(EXIT_FAILURE);
    }
    else{
       appchat.port_no = atoi(argv[1]); 
    }

    /* Tạo socket */
    appchat.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (appchat.server_fd == -1) handle_error("socket()");

    /* Ngăn lỗi : “address already in use” */
    if (setsockopt(appchat.server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &appchat.opt, sizeof(appchat.opt)))
        handle_error("setsockopt()");  

    /* Khởi tạo địa chỉ cho server */
    appchat.serv_addr.sin_family = AF_INET;
    appchat.serv_addr.sin_port = htons(appchat.port_no);
    appchat.serv_addr.sin_addr.s_addr =  INADDR_ANY;
    
    /* Gắn socket với địa chỉ server */
    if (bind(appchat.server_fd, (struct sockaddr*)&appchat.serv_addr, sizeof(appchat.serv_addr)) == -1)
        handle_error("bind()");

    /* Nghe tối đa 50 kết nối trong hàng đợi */
    if (listen(appchat.server_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen()");

    /* Dùng để lấy thông tin client */
	appchat.len = sizeof(appchat.client_addr);

    print_menu();

    int ret = 0;

    if (ret = pthread_create(&send_command_thr, NULL, &send_command_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    if (ret = pthread_create(&receive_thr, NULL, &receive_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    return 0;
}
