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

static void *clients_connection_handle(void *args);
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
    printf("7. send <userid> <'message'>        : send a message to the connection\n");
    printf("8. exit                             : Close all connections & terminate this app\n");
    printf("9. clear                            : Clear dislay terminal\n");
    printf("\n******************************************************\n\n");
}

typedef struct{
    char nickname[BUFF_SIZE];
    int userid;
    char ip[INET_ADDRSTRLEN];  // Địa chỉ IP của client
    int port;                  // Port của client
    int clients_socket_fd;
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
    bool UnblockingReceiveHostMsg;
    int command;
    int client_count;
    ClientInfo_t clients[MAX_CLIENTS];
}app_server_client_t;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_server  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_clients  = PTHREAD_COND_INITIALIZER;

pthread_t clients_connection_thr, send_command_thr;

pthread_t server_receive_chat_thr, clients_receive_chat_thr;

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

void send_message(char *msg, app_server_client_t *app, int id){
    int numb_write;
    char sendbuff[BUFF_SIZE];
    memset(sendbuff, '0', BUFF_SIZE);
    strncpy(sendbuff, msg, BUFF_SIZE - 1);
    numb_write = write(app->clients[id].clients_socket_fd, sendbuff, sizeof(sendbuff));
    printf("Sent\n");
}

void *chat_receive_clients_func(void *args){
    app_server_client_t *app = (app_server_client_t *)args;
    int numb_write, numb_read;
    char recvbuff[BUFF_SIZE];
    while(1){
        pthread_cond_wait(&cond_server,&mutex);
        memset(recvbuff, '0', BUFF_SIZE);
        printf("chat_receive_clients_func blocking\n");
        numb_read = read(app->new_socket_fd, recvbuff, sizeof(recvbuff));
        if(numb_read == -1)
            handle_error("read()");
        if (strncmp("exit", recvbuff, 4) == 0) {
            system("clear");
            break;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(app->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("*******************************\n");
        printf("* Message Received From Clients: %s\n",client_ip);
        printf("* Sender's port: %d\n",app->client_addr.sin_port);
        printf("* Message: %s\n",recvbuff);
        printf("*******************************\n");
    }
    close(app->server_fd); /*close*/
}

void *chat_receive_server_func(void *args){
    app_server_client_t *app = (app_server_client_t *)args;
    int numb_write, numb_read;
    char recvbuff[BUFF_SIZE];
    while(1){
        printf("chat_receive_server_func blocking\n");
        if(app->UnblockingReceiveHostMsg == false){
            pthread_cond_wait(&cond_clients,&mutex);
        }
        printf("chat_receive_server_func unblocking\n");
        memset(recvbuff, '0', BUFF_SIZE);
        numb_read = read(app->client_fd, recvbuff, sizeof(recvbuff));
        if(numb_read == -1)
            //handle_error("read()");
        if (strncmp("exit", recvbuff, 4) == 0) {
            system("clear");
            break;
        }
        char host_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(app->client_send_addr.sin_addr), host_ip, INET_ADDRSTRLEN);
        printf("*******************************\n");
        printf("* Message Received From Host: %s\n",host_ip);
        printf("* Host's port: %d\n",app->client_send_addr.sin_port);
        printf("* Message: %s\n",recvbuff);
        printf("*******************************\n");
        app->UnblockingReceiveHostMsg = true;
    }
    close(app->client_fd); /*close*/
}


static void *clients_connection_handle(void *args)
{
    app_server_client_t *app = (app_server_client_t *)args;
    int port = htons(app->port_no);
    while(1){
        printf("Application is listening at port : %d \n",port);
        app->new_socket_fd  = accept(app->server_fd, (struct sockaddr*)&app->client_addr, (socklen_t *)&app->len); 
        if (app->new_socket_fd == -1)
            handle_error("accept()");
        pthread_cond_signal(&cond_server);
        inet_ntop(AF_INET, &app->client_addr.sin_addr, app->clients[app->client_count].ip, INET_ADDRSTRLEN);
        app->clients[app->client_count].port = ntohs(app->client_addr.sin_port);
        snprintf(app->clients[app->client_count].nickname, BUFF_SIZE, "user%d", app->client_count);
        app->clients[app->client_count].userid = app->client_count;
        app->clients[app->client_count].clients_socket_fd = app->new_socket_fd;
        app->client_count++;
        printf("New client connected: ID %d, NN %s, IP %s, Port %d\n",app->clients[app->client_count-1].userid,app->clients[app->client_count-1].nickname, app->clients[app->client_count-1].ip, app->clients[app->client_count-1].port);
        app->AlreadyConnected = true;
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
        int portclients;
        char *connectionid;
        char *userid;
        char *message;
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
            userid = strtok(NULL, " ");
            message = strtok(NULL, "'"); // Tìm đến dấu nháy đơn đầu tiên
            // Tìm dấu nháy đơn đầu tiên
            char *start_quote = strchr(commandbuff, '\'');
            if (start_quote != NULL) {
                // Di chuyển con trỏ qua dấu nháy đơn đầu tiên
                start_quote++;
                // Tìm dấu nháy đơn cuối cùng
                char *end_quote = strchr(start_quote, '\'');
                if (end_quote != NULL) {
                    *end_quote = '\0';  // Thêm ký tự kết thúc chuỗi
                    message = start_quote;
                }
            }
            app->command = SEND_COMMAND_DEFINE;

            for(int i=0; i<=MAX_CLIENTS;i++){
                if(app->clients[i].userid == atoi(userid)){
                    destination = app->clients[i].ip;
                    portclients = app->clients[i].port;
                    break;
                }
            }

            if (userid != NULL) {
                printf("*******************************\n");
                printf("Command: %s\n", command);
                printf("user id: %d\n", atoi(userid));
                printf("Message: %s\n", message);
                printf("IP: %s\n",destination);
                printf("Port: %d\n",portclients);
                printf("*******************************\n");
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
                    printf("App-chat Already Connect To Host Server\n");
                }
                pthread_cond_signal(&cond_clients);
            }
                break;
            case LIST_COMMAND_DEFINE:
            {
                printf("********************************************\n");
                printf("ID |       IP Address      |     Port No.\n");
                if(app->client_count>0){
                    for(int i=0;i<app->client_count;i++){
                        printf("%d  |    %s    |       %d\n",app->clients[i].userid,app->clients[i].ip,app->clients[i].port);
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
                    pthread_cond_signal(&cond_clients);
                    send_message(message,app,atoi(userid));
                }else{
                    printf("Please Connect To Any Host Server\n");
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
    appchat.UnblockingReceiveHostMsg = false;
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

    if (ret = pthread_create(&clients_connection_thr, NULL, &clients_connection_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }
    usleep(100);

    if (ret = pthread_create(&send_command_thr, NULL, &send_command_handle, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    usleep(100);

    if (ret = pthread_create(&server_receive_chat_thr, NULL, &chat_receive_server_func, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    usleep(100);    

    if (ret = pthread_create(&clients_receive_chat_thr, NULL, &chat_receive_clients_func, &appchat)) {
            printf("pthread_create() error number=%d\n", ret);
            return -1;
    }

    pthread_join(send_command_thr,NULL); 
    pthread_join(clients_connection_thr,NULL); 
    pthread_join(server_receive_chat_thr,NULL); 
    pthread_join(clients_receive_chat_thr,NULL); 


    while(1);

    return 0;
}
