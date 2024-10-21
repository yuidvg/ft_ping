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

ssize_t recvfromOrExitFailure(int sockfd, void *buf, size_t len, int flags, const struct sockaddr_in src_addr)
{
    socklen_t addrlen = sizeof(src_addr);
    ssize_t bytesReceived = recvfrom(sockfd, buf, len, flags, (struct sockaddr *)&src_addr, &addrlen);
    if (bytesReceived > 0)
        return bytesReceived;
    else if (catchedSigint)
        return -1;
    else
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
}
