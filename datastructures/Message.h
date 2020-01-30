#include <stdlib.h>
#include <stdio.h>

#define _MESSAGE_H_

// Enumeration containing the differents types of messages
/*
    CONN: establishment of connection between host
    DATA: for exchanging data
    RELEASE: to turn off connections
    ANSWER: Acquitement send by the server to the client
*/
enum MessageType
{
    CONN = 0,
    DATA = 1,
    RELEASE = 2,
    ANSWER = 3
};

//========================================================================
//======= Definition of the structure used to represent messages =========

struct Message
{
    enum MessageType type; // le type du message comme defini plus haut
    char data[32]; // les données à envoyer au serveur
    unsigned short int port; // le port du serveur qui pointe sur le vrai service à réaliser
    unsigned int num; // num de sequence, qui permet au client de resequencer les messages qui arrivent des différents serveurs
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
    if(data != NULL) sprintf(message->data, "%s\0", data);

    return message;
}
