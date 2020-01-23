#include <stdlib.h>
#include <stdio.h>

// Enumeration containing the differents types of messages
/*
    CONN: establishment of connection between host
    DATA: for exchanging data
    RELEASE: to turn off connections
*/
enum MessageType {
    CONN = 1,
    DATA = 2,
    RELEASE = 3
}; 


//======================================================================== 
//======= Definition of the structure used to represent messages =========

struct Message {
    enum MessageType type;
    unsigned short int port;
    unsigned int num;
    char* data;
    char* signature;
};


//=======================================================================
//==================== Message wrapping =================================

void writeMessage(struct Message* message, 
                    enum MessageType messageType, 
                    unsigned short int port,
                    char* data) {

    message->type = messageType;
    message->port = port;
    message->data = data;
    message->signature = NULL;
}
