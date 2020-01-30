#define _MESSAGE_H_
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "datastructures/Cluster.h"
#include "datastructures/Message.h"
#include "dmptcp_proto.h"

// Constants

#define PORT 13100
#define SA struct sockaddr
#define MAX_BUFFER_LENGTH 1024 
#define STANDARD_MESSAGE_BLOCK_SIZE 10

// Global variables

struct Cluster *cluster;

//===========================================================================================
//=========================== Function prototypes ===========================================

void sendCONNPacket(int sockfd, struct Message *message, int token);
// not implement yet
// void sendCONNPacketC(struct Cluster *cluster_, struct Message *message, int token);

void sendDATAPacket(struct Cluster *cluster_, char *message);
void sendRELEASEPacket(int sockfd);
void sendRELEASEPacketC(struct Cluster *cluster_);

void toServer(int sockfd, int *token);
int connectToServer(struct sockaddr_in server_addr, int token, struct Message *message, int index_serv);
int generateToken(struct sockaddr_in *server_addresses, unsigned int number_of_servers);

char* recvDATAPacket(struct Cluster *clust);


// ==========================================================================================
// =========================== MAIN FUNCTION ================================================

int main() {
    int number_of_outside_servers = 1;
    struct sockaddr_in *servaddr_array = calloc(number_of_outside_servers,
                                                sizeof(servaddr_array[0]));
    int token = 0;                                            
    struct Message* message = createMessage(CONN, 3306, 100, NULL);

    // assign IP, PORT  
    servaddr_array[0].sin_family = AF_INET; 
    servaddr_array[0].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[0].sin_port = htons(PORT);

    // Cluster creation
    cluster = createCluster(servaddr_array, number_of_outside_servers);

    
    /*servaddr_array[1].sin_family = AF_INET; 
    servaddr_array[1].sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr_array[1].sin_port = htons(PORT);*/

    // Generate token 
    token = generateToken(servaddr_array, number_of_outside_servers);

    int i = 0;

    for (i = 0; i < number_of_outside_servers; i++)
    {
        connectToServer(servaddr_array[i], token, message, i);
    }
    printf("token = %d", generateToken(servaddr_array, number_of_outside_servers));

    // Function calls for sending DATA messages
    char* msg = "Le caliptus  nouveau conceptS";
    message = createMessage(DATA, 3306, 1, /*(char*)"Le caliptus"*/msg);

    sendDATAPacket(cluster, msg);
    

    char *received = recvDATAPacket(cluster);
    printf("\nReceived message from servers == %s", received);

    sendDATAPacket(cluster, message);
    recvDATAPacket(cluster);
    sendRELEASEPacketC(cluster);
}



//============================================================================================
//========================== FUNCTIONS FOR SENDING PACKETS ===================================   

void sendCONNPacket(int sockfd, struct Message* message, int token) {

    char *buff = malloc(MAX_BUFFER_LENGTH);
    // Firstly, we set token to data field of message (we need to convert token to a string)
    sprintf(message->data, "%d", token);

    // Next, we transform message into byte stream via dmptcp_proto
    dmptcp_proto_create_pkt2(message, buff);

    //debug packet transformation
    dmptcp_proto_parse_pkt2(message, buff);
    dmptcp_debug_pkt(message);

    // Then send the transformed message to the server
    send(sockfd, buff, sizeof(struct Message), 0);

}

void sendDATAPacket(struct Cluster *cluster, char *message) 
{
    // First, getting the size of data field of the structure message
    int message_size = strlen(message);

    // Then, we form message blocks and send it to servers
    // We count the number of blocks, we do this while the obtained number is greater than the number of servers in the cluster
    int number_of_blocks = 1;
    printf("\nNumber of blocks: %d\n", number_of_blocks);
    int current_message_block_size = STANDARD_MESSAGE_BLOCK_SIZE;
    do
    {
        number_of_blocks = message_size % current_message_block_size == 0 ? 
                                        message_size / current_message_block_size :
                                        message_size / current_message_block_size + 1;

        if(number_of_blocks > cluster->nb_of_nodes)
            current_message_block_size *= 2;
        
    }while(number_of_blocks > cluster->nb_of_nodes); // Here, we are sure that number_of_blocks <= number of servers in a cluster
    
    if(number_of_blocks == 0) {
        number_of_blocks = 1;
    }
    if(number_of_blocks == 1)
        current_message_block_size = message_size;
        
    printf("Current block size : %d\n", current_message_block_size);
    printf("Message size : %d", message_size);
    char buff[current_message_block_size];
    struct Message message_blocks[number_of_blocks];
    int i = 0;
    for(i = 0; i < number_of_blocks; i++) 
    {
        strncpy(message_blocks[i].data, message, current_message_block_size); 
        message_blocks[i].num = i;
        message_blocks[i].port = PORT;
        message_blocks[i].type = DATA;
        //message_blocks[i].signature = message->signature; 
        dmptcp_proto_create_pkt2(&message_blocks[i], buff);
        int res = send(cluster->sockfds[i], buff, sizeof(struct Message), 0);
    }
}

void sendRELEASEPacket(int sockfd) {
    struct Message* msg = createMessage(RELEASE, 0, 0, NULL);
    char *buff = malloc(MAX_BUFFER_LENGTH);

    dmptcp_proto_create_pkt2(msg, buff);

    send(sockfd, buff, sizeof(struct Message), 0);
    close(sockfd);
}

void sendRELEASEPacketC(struct Cluster *cluster){
    int i=0;
    for(i=0; i<cluster->nb_of_nodes; i++){
        sendRELEASEPacket(cluster->sockfds[i]);
    }
}


//=============================================================================================
//========================= FUNCTION FOR RECEIVING DATA =======================================

char* recvDATAPacket(struct Cluster *clust)
{
    // First, create a liste of received message blocks
    struct Message *received_messages = (struct Message *) malloc(
                                        clust->nb_of_nodes * sizeof(struct Message));

    struct Message *tmp = (struct Message *) malloc(sizeof(struct Message));  
    char received_message[clust->nb_of_nodes * 32 + 1];
                                    
    //char buff[MAX_BUFFER_LENGTH] = {0};
    int i, j;
    for(i = 0; i < clust->nb_of_nodes; i++)
    {
        recv(cluster->sockfds[i], tmp, sizeof(struct Message), 0);
        received_messages[tmp->num] = *tmp; 
    }
    for (i = 0; i < clust->nb_of_nodes; i++)
    {
        /*for (j = 0; j < strlen(received_messages[i].data); i++)
        {
            received_message[j] = received_messages[i].data[j];
        }*/
        strncat(received_message, received_messages[i].data, strlen(received_messages[i].data));
        strncat(received_message, " ", 1);
    }
    
    return received_message;
}

//=============================================================================================
//=========================  AUXILIARY FUNCTIONS ==============================================


int connectToServer(struct sockaddr_in server_addr, int token, struct Message *message, int ind_server) {
    int sockfd;  
    int i = 0;
    
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n");  

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&server_addr, sizeof(server_addr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
    
    cluster->sockfds[i] = sockfd;
    // Functions for communication

    if(message->type == CONN)
        sendCONNPacket(sockfd, message, token);
    else if(message->type == RELEASE)
        sendRELEASEPacket(sockfd);
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

