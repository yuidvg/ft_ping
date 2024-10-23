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

bool isReadableOrExitFailure(const int sockfd, struct timeval timeout)
{
    fd_set readFdSet;
    FD_ZERO(&readFdSet);
    FD_SET(sockfd, &readFdSet);
    if (timeout.tv_sec < 0 || timeout.tv_usec < 0)
        timeout.tv_sec = timeout.tv_usec = 0;
    const int result = select(sockfd + 1, &readFdSet, NULL, NULL, &timeout);
    if (result > 0)
        return true;
    else if (result == 0 || catchedSigint)
        return false;
    else
    {
        perror("select failed");
        exit(EXIT_FAILURE);
    }
}
