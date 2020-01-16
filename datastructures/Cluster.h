#include <stdlib.h>
#include <stdio.h>

struct Cluster {
    char* master_ip;
    char** nodes_ip; 
};

// Setting a values to a cluster
void initialize(struct Cluster *cluster, char* master_ip, char** nodes_ip) {
    cluster->master_ip = master_ip;
    cluster->nodes_ip = nodes_ip;
}

// Getting values from the structure
char* getMasterIP(struct Cluster *cluster) {
    return cluster->master_ip;
}

char** getNodesIP(struct Cluster *cluster) {
    return cluster->nodes_ip;
}