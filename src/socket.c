#include "../include/all.h"

int createRawSocketOrExitFailure()
{
    int rawSockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (rawSockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return rawSockfd;
}

ssize_t recvfromOrExitFailure(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr)
{
    socklen_t addrlen = sizeof(*src_addr);                                         // Changed from pointer to value
    ssize_t bytesReceived = recvfrom(sockfd, buf, len, flags, src_addr, &addrlen); // Pass the address of addrlen
    if (bytesReceived < 0)
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    return bytesReceived;
}
