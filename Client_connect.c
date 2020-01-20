#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "datastructures/Cluster.h"
#include "datastructures/Message.h"


#define PORT 2000
#define SA struct sockaddr

void toServer(int sockfd) {
    int MAX = 50;
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("Received message: %s\t : ", buff); 
    }    
        
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
  
    // function for chat 
    toServer(sockfd); 
  
    // close the socket 
    // RELEASE Function called
    //close(sockfd); 
}

void main() {

    int number_of_outside_servers = 2;
    struct sockaddr_in *servaddr_array = calloc(number_of_outside_servers,
                                                sizeof(servaddr_array[0]));

    //************************************************************************
    // assign IP, PORT  
    servaddr_array[0].sin_family = AF_INET; 
    servaddr_array[0].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[0].sin_port = htons(PORT);

    servaddr_array[1].sin_family = AF_INET; 
    servaddr_array[1].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[1].sin_port = htons(PORT);

    //************************************************************************

    int i = 0;

    for (i = 0; i < number_of_outside_servers; i++)
    {
        connectToServer(servaddr_array[i]);
    }
    

}