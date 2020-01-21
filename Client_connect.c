#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "datastructures/Cluster.h"
#include "datastructures/Message.h"


#define PORT 3000
#define SA struct sockaddr


// Global variables
int token = 0;

// Function prototypes
int generateToken(struct sockaddr_in *server_addresses, unsigned int number_of_servers);

void toServer(int sockfd, int *token) {
    send(sockfd , token , sizeof(int) , 0 ); 
    printf("Message sent to the server");
}

void connectToServer(struct sockaddr_in servaddr) {
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
    
    // function for communication
    toServer(sockfd, &token); 
  
    // close the socket 
    // RELEASE Function called
    //close(sockfd); 
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

int main() {

    int number_of_outside_servers = 1;
    struct sockaddr_in *servaddr_array = calloc(number_of_outside_servers,
                                                sizeof(servaddr_array[0]));

    //************************************************************************
    // assign IP, PORT  
    servaddr_array[0].sin_family = AF_INET; 
    servaddr_array[0].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[0].sin_port = htons(PORT);

    
    /*servaddr_array[1].sin_family = AF_INET; 
    servaddr_array[1].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[1].sin_port = htons(PORT);*/
    

    //************************************************************************

    // Generate token 
    token = generateToken(servaddr_array, number_of_outside_servers);

    int i = 0;

    for (i = 0; i < number_of_outside_servers; i++)
    {
        connectToServer(servaddr_array[i]);
    }
    printf("token = %d", generateToken(servaddr_array, number_of_outside_servers));

}