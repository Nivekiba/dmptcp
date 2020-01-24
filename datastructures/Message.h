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
    CONN = 1,
    DATA = 2,
    RELEASE = 3
};

//========================================================================
//======= Definition of the structure used to represent messages =========

struct Message
{
    enum MessageType type;
    char data[32];
    unsigned short int port;
    unsigned int num;
    //char signature[10];
};

//=======================================================================
//==================== Message wrapping =================================

struct Message* createMessage(
                  enum MessageType messageType,
                  unsigned short int port,
                  unsigned int num,
                  char *data)
{
    struct Message *message = (struct Message *)malloc(sizeof(struct Message));
    message->type = messageType;
    message->port = port;
    message->num = num;
    if(data != NULL) sprintf(message->data, "%s", data);

    return message;
}
