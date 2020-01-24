#ifndef _DMPTCP_PROTO_H_
#define _DMPTCP_PROTO_H_

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <sys/types.h> 

#ifndef _CLUSTER_H_
#include "datastructures/Cluster.h"
#endif
#ifndef _MESSAGE_H_
#include "datastructures/Message.h"
#endif

void dmptcp_proto_parse_pkt2(struct Message *msg, unsigned char *buf){
    memcpy(msg, buf, sizeof(struct Message));
}

void dmptcp_proto_create_pkt2(struct Message *msg, unsigned char *buf){

    memcpy(buf, msg, sizeof(struct Message));
}

#endif
