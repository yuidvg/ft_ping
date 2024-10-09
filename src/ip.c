#include "../include/all.h"

struct sockaddr_in constructIpHeader(const char *destinationIp)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(destinationIp);
    return dest_addr;
}
