#include <stdlib.h>
#include <stdio.h>

#define _MESSAGE_H_

// Enumeration containing the differents types of messages
/*
    CONN: establishment of connection between host
    DATA: for exchanging data
    RELEASE: to turn off connections
*/
enum MessageType
{
    CONN = 0,
    DATA = 1,
    RELEASE = 2
};

//========================================================================
//======= Definition of the structure used to represent messages =========

struct Message
{
    enum MessageType type;
    unsigned short int port;
    unsigned int num;
    char data[100];
    char signature[10];
};

//=======================================================================
//==================== Message wrapping =================================

struct Message* createMessage(
                  enum MessageType messageType,
                  unsigned short int port,
                  char *data)
{
    struct Message *message = (struct Message *)malloc(sizeof(struct Message));
    message->type = messageType;
    message->port = port;
    if(data != NULL) strcpy(data, message->data);
    char* sig = "124\0";
    //strcpy(sig, message->signature);

    return message;
}
