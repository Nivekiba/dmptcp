#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include "datastructures/Cluster.h"
#include "datastructures/Message.h"
#include "dmptcp_proto.h"


#define PORT 13000
#define SA struct sockaddr
#define MAX_BUFFER_LENGTH 1024


//===========================================================================================
//=========================== Function prototypes ===========================================

void sendCONNPacket(int sockfd, struct Message *message, int *token);
void sendDATAPacket(int sockfd, struct Message *message);
void sendREALESEPacket(int sockfd);
void toServer(int sockfd, int *token);
void connectToServer(struct sockaddr_in servaddr, int *token, struct Message *message);
int generateToken(struct sockaddr_in *server_addresses, unsigned int number_of_servers);



// ==========================================================================================
// =========================== MAIN FUNCTION ================================================

int main() {

    int number_of_outside_servers = 1;
    struct sockaddr_in *servaddr_array = calloc(number_of_outside_servers,
                                                sizeof(servaddr_array[0]));
    int token = 0;                                            
    struct Message* message = createMessage(CONN, 3306, NULL);

    // assign IP, PORT  
    servaddr_array[0].sin_family = AF_INET; 
    servaddr_array[0].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[0].sin_port = htons(PORT);

    
    /*servaddr_array[1].sin_family = AF_INET; 
    servaddr_array[1].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[1].sin_port = htons(PORT);*/

    // Generate token 
    token = generateToken(servaddr_array, number_of_outside_servers);

    int i = 0;

    for (i = 0; i < number_of_outside_servers; i++)
    {
        connectToServer(servaddr_array[i], &token, message);
    }
    printf("token = %d", generateToken(servaddr_array, number_of_outside_servers));

}



//============================================================================================
//========================== FUNCTIONS FOR SENDING PACKETS ===================================   

void sendCONNPacket(int sockfd, struct Message *message, int *token) {

    char buff[MAX_BUFFER_LENGTH] = {0};
    // Firstly, we set token to data field of message (we need to convert token to a string)
    sprintf(message->data, "%d", *token);

    // Next, we transform message into byte stream via dmptcp_proto
    dmptcp_proto_create_pkt2(message, buff);
    printf("message type: %d", message->type);

    // Then send the transformed message to the server
    send(sockfd, buff, sizeof(buff), 0);

}

void sendDATAPacket(int sockfd, struct Message *message) {

}

void sendREALESEPacket(int sockfd) {
    close(sockfd);
}



//=============================================================================================
//=========================  AUXILIARY FUNCTIONS ==============================================


void connectToServer(struct sockaddr_in servaddr, int *token, struct Message *message) {
    int sockfd, connfd;  
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n");  
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    printf("sockfd = %d", sockfd);
    
    // Functions for communication

    if(message->type == CONN)
        sendCONNPacket(sockfd, message, token);
    else if(message->type == DATA)
        sendDATAPacket(sockfd, message);
    else if(message->type == RELEASE)
        sendREALESEPacket(sockfd);
 
}

int generateToken(struct sockaddr_in *server_addresses, unsigned int number_of_servers) {
    int token_ = 0;
    int i = 0;
    for (i = 0; i < number_of_servers; i++)
    {
        token_ += (server_addresses+i)->sin_addr.s_addr;
    }
    return token_;
}

