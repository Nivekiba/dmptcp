#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

struct Cluster {
    struct sockaddr_in master_ip;
    struct sockaddr_in out;
    struct sockaddr_in* nodes_ip; 
};

// Setting a values to a cluster
void initialize(struct Cluster *cluster, struct sockaddr_in master_ip, struct sockaddr_in *nodes_ip) {
    cluster->master_ip = master_ip;
    cluster->nodes_ip = nodes_ip;
}

// Getting values from the structure
sockaddr_in getMasterIP(struct Cluster *cluster) {
    return cluster->master_ip;
}

sockaddr_in* getNodesIP(struct Cluster *cluster) {
    return cluster->nodes_ip;
}