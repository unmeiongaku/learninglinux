#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     //  Thư viện chứa các hằng số, cấu trúc khi sử dụng địa chỉ trên internet
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define HELP_COMMAND_DEFINE 1
#define MYIP_COMMAND_DEFINE 2
#define MYPORT_COMMAND_DEFINE 3
#define CONNECT_COMMAND_DEFINE 4
#define LIST_COMMAND_DEFINE 5
#define TERMINATE_COMMAND_DEFINE 6
#define SEND_COMMAND_DEFINE 7
#define EXIT_COMMAND_DEFINE 8
#define CLEAR_TERMINAL_DISPLAY 9

#define MAX_CLIENTS 50
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_menu();
int get_command(const char *cmd, int cmdnb);

static void *receive_handle(void *args);
static void *send_command_handle(void *args);


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
    printf("9. clear                            : Clear dislay terminal\n");
    printf("\n******************************************************\n\n");
}

typedef struct{
    char ip[INET_ADDRSTRLEN];  // Địa chỉ IP của client
    int port;                  // Port của client
}ClientInfo_t;

typedef struct{
    /*Port Processs*/
    int port_no; 
    int len; 
    int opt;
    int server_fd;
    int new_socket_fd;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in client_addr;
    /*Clients*/
    int cllen; 
    int clopt;
    struct sockaddr_in client_send_addr;
    int client_fd; 
    int hostportnumber;
    bool AlreadyConnected;
    int command;
    int client_count;
    ClientInfo_t clients[MAX_CLIENTS];
}app_server_client_t;

void chat_func(app_server_client_t *app);

pthread_t receive_thr, send_command_thr;

static app_server_client_t appchat;


int get_command(const char *cmd, int cmdnb){
    if (strcmp(cmd, "help") == 0) {
        return HELP_COMMAND_DEFINE;
    } else if (strcmp(cmd, "myip") == 0) {
        return MYIP_COMMAND_DEFINE;
    } else if (strcmp(cmd, "myport") == 0) {
        return MYPORT_COMMAND_DEFINE;
    } else if (strcmp(cmd, "list") == 0) {
        return LIST_COMMAND_DEFINE;
    } else if (strcmp(cmd, "exit") == 0) {
        return EXIT_COMMAND_DEFINE;
    } else if (strcmp(cmd, "clear") == 0) {
        return CLEAR_TERMINAL_DISPLAY;
    }
    else{
        return cmdnb;
    }
}

void chat_func(app_server_client_t *app){
    int numb_write, numb_read;
    char recvbuff[BUFF_SIZE];
}

static void *receive_handle(void *args)
{
    app_server_client_t *app = (app_server_client_t *)args;
    int port = htons(app->port_no);
    while(1){
        printf("Application is listening at port : %d \n",port);
        app->new_socket_fd  = accept(app->server_fd, (struct sockaddr*)&app->client_addr, (socklen_t *)&app->len); 
        if (app->new_socket_fd == -1)
            handle_error("accept()");

		//system("clear");
        // printf("Server : got connection \n");  
        inet_ntop(AF_INET, &app->client_addr.sin_addr, app->clients[app->client_count].ip, INET_ADDRSTRLEN);
        app->clients[app->client_count].port = ntohs(app->client_addr.sin_port);
        app->client_count++;
        printf("New client connected: IP %s, Port %d\n", app->clients[app->client_count-1].ip, app->clients[app->client_count-1].port);
            chat_func(app);
    }
}

static void *send_command_handle(void *args)
{
    int numb_read, numb_write;
    char commandbuff[BUFF_SIZE];
    app_server_client_t *app = (app_server_client_t *)args;
    while(1){
        char commandbuff[BUFF_SIZE];
        char *command;
        char *destination;
        char *port;
        char *connectionid;
        memset(commandbuff, 0, BUFF_SIZE);  // Đặt toàn bộ bộ nhớ về 0 thay vì '0'
        printf("chat-application$ ");
        fgets(commandbuff, BUFF_SIZE, stdin);

        // Xóa ký tự xuống dòng '\n' nếu có
        commandbuff[strcspn(commandbuff, "\n")] = 0;

        // Tách từ đầu tiên trong chuỗi lệnh (chính là 'command')
        command = strtok(commandbuff, " ");

        if (command != NULL && strcmp(command, "connect") == 0) {
            // Lấy destination và port
            destination = strtok(NULL, " ");
            port = strtok(NULL, " ");
            app->command = CONNECT_COMMAND_DEFINE;
            if (destination != NULL && port != NULL) {
                printf("Command: %s\n", command);
                printf("Destination: %s\n", destination);
                printf("Port: %s\n", port);
                app->hostportnumber = atoi(port);
            } else {
                printf("Invalid command format. Usage: connect <destination> <port>\n");
            }
        }
        else if (command != NULL && strcmp(command, "terminate") == 0) {
            // Lấy connection id
            connectionid = strtok(NULL, " ");
            app->command = TERMINATE_COMMAND_DEFINE;
            if (connectionid != NULL) {
                printf("Command: %s\n", command);
                printf("Connection id: %s\n", connectionid);
            } else {
                printf("Invalid command format. Usage: terminate <Connection id>\n");
            }
        }
        else if (command != NULL && strcmp(command, "send") == 0) {
            connectionid = strtok(NULL, " ");
            app->command = SEND_COMMAND_DEFINE;
            if (connectionid != NULL) {
                printf("Command: %s\n", command);
                printf("Connection id: %s\n", connectionid);
            } else {
                printf("Invalid command format. Usage: send <Connection id>\n");
            }
        }
        else {
            app->command = 0;
        }
        app->command = get_command(commandbuff,app->command);
        // printf("Command number %d\n",app->command);
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
            case MYPORT_COMMAND_DEFINE:
            {

            }
                break;
            case CONNECT_COMMAND_DEFINE:
            {
                if(app->AlreadyConnected == false){
                    app->AlreadyConnected = true;
                    /* Khởi tạo địa chỉ server */
                    app->client_send_addr.sin_family = AF_INET;
                    app->client_send_addr.sin_port   = htons(app->hostportnumber);
                    if (inet_pton(AF_INET,destination, &app->client_send_addr.sin_addr) == -1) 
                        handle_error("inet_pton()");
                    app->client_fd = socket(AF_INET, SOCK_STREAM, 0);
                    if (app->client_fd == -1)
                        handle_error("socket()");
                    if (connect(app->client_fd, (struct sockaddr *)&app->client_send_addr, sizeof(app->client_send_addr)) == -1)
                        handle_error("connect()");
                }
                else{
                    printf("Server Already Connect To Server\n");
                }

                
            }
                break;
            case LIST_COMMAND_DEFINE:
            {
                printf("********************************************\n");
                printf("ID |       IP Address      |       Port No.\n");
                if(app->client_count>0){
                    for(int i=0;i<app->client_count;i++){
                        printf("%d  |    %s    |       %d\n",i,app->clients[i].ip,app->clients[i].port);
                    }  
                }
                else{
                    printf("No ID connection\n");
                }
                printf("********************************************\n");
            }
                break;
            case TERMINATE_COMMAND_DEFINE:
            {

            }
                break;
            case SEND_COMMAND_DEFINE:
            {
                if(app->AlreadyConnected == true){
                    

                }else{
                    printf("Please Connect  To Any Host Server\n");
                }


            }
                break;
            case EXIT_COMMAND_DEFINE:
            {

            }
                break;
            case CLEAR_TERMINAL_DISPLAY:
            {
                system("clear");
            }
                break;
            default:
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
    appchat.AlreadyConnected = false;
    appchat.client_count = 0;
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
    if (listen(appchat.server_fd, MAX_CLIENTS) == -1)
        handle_error("listen()");

    /* Dùng để lấy thông tin client */
	appchat.len = sizeof(appchat.client_addr);

    print_menu();

    int ret = 0;

    if (ret = pthread_create(&receive_thr, NULL, &receive_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }
    usleep(100);

    if (ret = pthread_create(&send_command_thr, NULL, &send_command_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    pthread_join(send_command_thr,NULL); 
    pthread_join(receive_thr,NULL); 

    while(1);

    return 0;
}
