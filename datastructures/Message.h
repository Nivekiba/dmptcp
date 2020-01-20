#include <stdlib.h>
#include <stdio.h>

// Enumeration containing the differents types of messages
/*
    CONN_IN: establishment of connection between host of the same cluster
    CONN_OUT: establishment of connection between host of different clusters
    DATA: for exchanging data
    RELEASE: to turn off connections
*/
enum MessageType {
    CONN_IN = 1,
    CONN_OUT = 2,
    DATA = 3,
    RELEASE = 4
}; 

// Definition of the structure used to represent messages
struct Message {
    enum MessageType type;
    unsigned short int port;
    char* data;
    char* signature;
};

// Message wrapping
void writeMessage(struct Message* message, 
                    enum MessageType messageType, 
                    unsigned short int port,
                    char* data) {

    message->type = messageType;
    message->port = port;
    message->data = data;
    //message->signature = hash(data);
}

// Functions for getting isolate elements of a message
MessageType getType(struct Message* message) {
    return message->type;
}

unsigned short int getPort(struct Message* message) {
    return message->port;
}

char* getData(struct Message* message) {
    return message->data;
}

char* getSignature(struct Message* message) {
    return message->signature;
}