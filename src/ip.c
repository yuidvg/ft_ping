#include "../include/all.h"

struct sockaddr_in constructIpHeader(const char *destinationIp)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destinationIp);
    return dest_addr;
}

struct sockaddr_in resolveHostname(const char *hostname)
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
        fprintf(stderr, "getaddrinfoエラー: %s\n", gai_strerror(errcode));
        return (struct sockaddr_in){0};
    }

    struct sockaddr_in addr = *(struct sockaddr_in *)res->ai_addr;

    freeaddrinfo(res);
    return addr;
}

char *byteAddressToString(uint32_t address, char *buffer)
{
    inet_ntop(AF_INET, &address, buffer, INET_ADDRSTRLEN);

    return buffer;
}
