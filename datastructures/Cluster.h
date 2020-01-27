#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#define _CLUSTER_H_

struct Cluster {
    int nb_of_nodes;
    int *sockfds;
    struct sockaddr_in* nodes_ip; 
};

struct Cluster* createCluster(struct sockaddr_in *nodes, int number_of_nodes) 
{
    int i = 0;
    struct Cluster *cluster = (struct Cluster *) malloc(sizeof(struct Cluster));
    cluster->nodes_ip = (struct sockaddr_in *) malloc(number_of_nodes * sizeof(struct sockaddr_in));
    cluster->sockfds = malloc(number_of_nodes * sizeof(int));
    cluster->nb_of_nodes = number_of_nodes;
    
    for ( i = 0; i < number_of_nodes; i++)
    {
        cluster->nodes_ip[i] = nodes[i];
    }
    return cluster;
}
