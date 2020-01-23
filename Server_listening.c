#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "datastructures/Cluster.h"
#include "datastructures/Message.h"
#include "dmptcp_proto.h"

// Constants
#define PORT 3000
#define SA struct sockaddr
#define MAX_NUMBER_CONNECTION 200
#define MAX_BUFFER_LENGTH 1024

// Global variables
struct Cluster cluster;
int token = 0;
int number_connections_received = 0; // This is used to control how many clients connected over the time


//=============================================================================================
//=========================== FUNCTION PROTOTYPES =============================================

void CONNRequests(int connfd);
void DATARequests(int connfd);
void RELEASERequests(int connfd);
void initiateConn();

//=============================================================================================
//=========================== MAIN FUNCTION ===================================================

int main()
{
    initiateConn();
}


//=============================================================================================
//==================== FUNCTIONS TO PROCESS CLIENT REQUESTS ===================================

void CONNRequests(int connfd) {

    char buffer[MAX_BUFFER_LENGTH] = {0};
    struct Message *message;
    message = malloc(sizeof(struct Message));
    int received_token = 0;

    // We copy the content of the message in message variable
    dmptcp_proto_parse_pkt2(message, buffer);

    if(message->type == CONN) {

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
            number_connections_received ++;
        }

    }
    
}

void DATARequests(int connfd) {
    char buffer[MAX_BUFFER_LENGTH] = {0};
    struct Message *message;
    message = malloc(sizeof(struct Message));

     // We copy the content of the message in message variable
    dmptcp_proto_parse_pkt2(message, buffer);
    if(message->type == DATA) {

    }

}

void RELEASERequests(int connfd) {
    char buffer[MAX_BUFFER_LENGTH] = {0};
    struct Message *message;
    message = malloc(sizeof(struct Message));

     // We copy the content of the message in message variable
    dmptcp_proto_parse_pkt2(message, buffer);
    if(message->type == RELEASE) {
        close(connfd);
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
        CONNRequests(connfd);
        DATARequests(connfd);
        RELEASERequests(connfd);
        //toClient(connfd);     
    }
       
}
