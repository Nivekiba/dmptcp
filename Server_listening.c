#define _MESSAGE_H_

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>  
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <time.h>

#include "datastructures/Cluster.h"
#include "datastructures/Message.h"
#include "dmptcp_proto.h"

// Constants
#define PORT 13100
#define SA struct sockaddr
#define MAX_NUMBER_CONNECTION 200
#define MAX_BUFFER_LENGTH 10240

// Global variables
int token = 0;
int number_connections_received = 0; // This is used to control how many clients connected over the time
int local_server_sock = -1;

//=============================================================================================
//=========================== FUNCTION PROTOTYPES =============================================

void requests(int connfd);
void initiateConn();
void sendANSWERPacket(int sock_id, struct Message* msg);

//=============================================================================================
//=========================== MAIN FUNCTION ===================================================

int main()
{
    srand(time(NULL));
    initiateConn();
}

//=============================================================================================
//======================= CREATION OF LOCAL SERVER FOR THE DESTINATION PROTOCOL ===============

void connect_local_server(struct Message* msg){
    // socket create and varification 
    local_server_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (local_server_sock == -1) { 
        printf("socket local creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket local successfully created..\n");  
    
    // assign IP, PORT  
    struct sockaddr_in servaddr ;
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(msg->port);

    // connect the client socket to server socket 
    if (connect(local_server_sock, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        perror("Connection to local server failed");
        printf("Connection with the local server failed on port %d...\n", msg->port); 
        exit(0); 
    } 
    else
        printf("connected to the local server on port %d\n", msg->port); 
}

//=============================================================================================
//==================== FUNCTIONS TO PROCESS CLIENT REQUESTS ===================================

void requests(int connfd) {
    char *buffer = malloc(MAX_BUFFER_LENGTH);
    struct Message *message;
    message = malloc(sizeof(struct Message));
    int received_token = 0;
    ssize_t len_recv;
message_br:
    printf("\nWaiting for messages on socket n: %d ...\n", connfd);
    while( (len_recv = recv(connfd, (struct Message*)message, sizeof(struct Message), MSG_WAITALL)) == 0 );
    buffer[len_recv] = '\0';
    if(len_recv < 0){
        printf("error while receiving data\n");
        exit(0);
    }
    // We copy the content of the message in message variable
    // dmptcp_proto_parse_pkt2(message, buffer);
    dmptcp_debug_pkt(message);

    // CONN requests
    if(message->type == CONN) {

        connect_local_server(message);
        received_token = atoi(message->data);
        
        // if no client yet connected
        if(number_connections_received == 0)
        {
            number_connections_received ++;
            token = received_token;
            printf("\nToken == %d", token);

            char* num_c = malloc(sizeof(char*));
            sprintf(num_c, "%d", number_connections_received);
            struct Message* msg = createMessage(ANSWER, 21, message->num, num_c);
            sendANSWERPacket(connfd, msg);
        }
        // if a client attempt to connect to the server (not for the first time) and gives the right token
        else if(number_connections_received > 0 && received_token == token) 
        {
            printf("Connected to a client");
            number_connections_received ++;
            printf("Received Token == %d", received_token);

            char* num_c = malloc(sizeof(char*));
            sprintf(num_c, "%d", number_connections_received);
            struct Message* msg = createMessage(ANSWER, 21, message->num, num_c);
            sendANSWERPacket(connfd, msg);
        }
        // a client attempt to connect to the server (not for the first time) and gives the wrong token
        else if(number_connections_received > 0 && received_token != token) 
        {
            printf("Token == %d \t Received Token == %d", token, received_token);
            
            char* num_c = malloc(sizeof(char*));
            sprintf(num_c, "%d", -1);
            struct Message* msg = createMessage(ANSWER, 21, message->num, num_c);
            sendANSWERPacket(connfd, msg);
            
            close(connfd);
        }
    }

    // DATA requests
    if(message->type == DATA){
        printf("Dar good message: %s\n", message->data);

        struct Message* msg = createMessage(ANSWER, 21, message->num, NULL);
        sendANSWERPacket(connfd, msg);
    }


    // RELEASE requests
    if(message->type == RELEASE) {
        close(connfd);
        number_connections_received --;
        if(number_connections_received == 0)
            close(local_server_sock);
    } else
        goto message_br;
    
}


// Process clients' requests
/*void toClient(int connfd) {
   int token[1] = {0};
   int  valread = read( connfd , token, 1024); 
    printf("From client = %d", token[0]);
}*/


//=============================================================================================
//=====================  INTIALIZE CONNECTION FUNCTION ========================================

void initiateConn() {

    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, MAX_NUMBER_CONNECTION)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    while(1) {
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("server accept failed...\n"); 
            exit(0); 
        } 
        else{
            int random = rand();
            printf("server accept the client...\nSeed: %d\n", random);
        }

        // pid_t child_pid = fork();
        // if(child_pid < 0){
        //     perror("Fork error");
        // }
        // if(child_pid == 0){
            /* Process client requests here */
            requests(connfd);
            printf("Closed client");
        // }
        // else{
        //     pid_t attended_pid = waitpid(child_pid, NULL, 0);
        //     if(attended_pid != child_pid){
        //         printf("Child error");
        //     }
        // }
    }
       
}

void sendANSWERPacket(int sockfd, struct Message* message) {

    char *buff = malloc(MAX_BUFFER_LENGTH);

    // Next, we transform message into byte stream via dmptcp_proto
    dmptcp_proto_create_pkt2(message, buff);
    
    // Then send the transformed message to the server
    send(sockfd, buff, sizeof(struct Message), 0);
}