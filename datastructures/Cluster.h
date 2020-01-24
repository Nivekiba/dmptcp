#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#define _CLUSTER_H_

struct Cluster {
    struct sockaddr_in* nodes_ip; 
};

struct Cluster* createCluster(struct sockaddr_in *nodes, int number_of_nodes) {
    int i = 0;
    struct Cluster *cluster;
    for ( i = 0; i < number_of_nodes; i++)
    {
        cluster->nodes_ip[i] = nodes[i];
    }
    return cluster;
}