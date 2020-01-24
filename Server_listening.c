#define _MESSAGE_H_

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include "datastructures/Cluster.h"
#include "datastructures/Message.h"
#include "dmptcp_proto.h"

// Constants
#define PORT 13000
#define SA struct sockaddr
#define MAX_NUMBER_CONNECTION 200
#define MAX_BUFFER_LENGTH 1024

// Global variables
struct Cluster cluster;
int token = 0;
int number_connections_received = 0; // This is used to control how many clients connected over the time
int local_server_sock = -1;

//=============================================================================================
//=========================== FUNCTION PROTOTYPES =============================================

void requests(int connfd);
void initiateConn();

//=============================================================================================
//=========================== MAIN FUNCTION ===================================================

int main()
{
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
        perror("Conection to local server failed");
        printf("connection with the local server failed on port %d...\n", msg->port); 
        exit(0); 
    } 
    else
        printf("connected to the local server..\n"); 
}

//=============================================================================================
//==================== FUNCTIONS TO PROCESS CLIENT REQUESTS ===================================

void requests(int connfd) {

    char buffer[MAX_BUFFER_LENGTH] = {0};
    struct Message *message;
    message = malloc(sizeof(struct Message));
    int received_token = 0;
    printf("buff before: %s", buffer);
    printf("Waiting for messages on socket n: %d ...\n", connfd);
    if(recv(connfd, buffer, MAX_BUFFER_LENGTH, 0) < 0){
        printf("error while receiving data\n");
        exit(0);
    } else {
        printf("we receive data\n");
    }
    printf("buff after: %s", buffer);

    // We copy the content of the message in message variable
    dmptcp_proto_parse_pkt2(message, buffer);

    // CONN requests
    if(message->type == CONN) {

        connect_local_server(message);
        received_token = atoi(message->data);
        
        // if no client yet connected
        if(number_connections_received == 0)
        {
            number_connections_received ++;
            token = received_token;
            printf("Token == %d", token);
        }
        // if a client attempt to connect to the server (not for the first time) and gives the right token
        else if(number_connections_received > 0 && received_token == token) 
        {
            printf("Connected to a client");
            number_connections_received ++;
            printf("Received Token == %d", received_token);
        }
        // a client attempt to connect to the server (not for the first time) and gives the wrong token
        else if(number_connections_received > 0 && received_token != token) 
        {
            printf("Token == %d \t Received Token == %d", token, received_token);
            close(connfd);
        }
        
    }

    // RELEASE requests
    if(message->type == RELEASE) {
        close(connfd);
        number_connections_received --;
        if(number_connections_received == 0)
            close(local_server_sock);
    }
    
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
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n");

        // Process client requests here
        requests(connfd);
        //toClient(connfd);     
    }
       
}
