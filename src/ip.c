#include "../include/all.h"

struct sockaddr_in constructIpHeader(const char *destinationIp)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destinationIp);
    return dest_addr;
}

struct sockaddr_in resolveHostnameOrExitFailure(const char *hostname)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if ((errcode = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
        fprintf(stderr, "ft_ping: unknown host\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = *(struct sockaddr_in *)res->ai_addr;

    freeaddrinfo(res);
    return addr;
}

void printByteAddressToString(uint32_t address)
{
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address, buffer, INET_ADDRSTRLEN);
    printf("%s", buffer);
}

void setTtlOrExitFailure(int rawSockfd, const uint8_t ttl)
{
    // Set the TTL for the socket
    if (setsockopt(rawSockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("setsockopt");
        close(rawSockfd);
        exit(EXIT_FAILURE);
    }
}
