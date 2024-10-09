#include "../include/all.h"

int createRawSocketOrExitFailure()
{
    int rawSockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (rawSockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return rawSockfd;
}
