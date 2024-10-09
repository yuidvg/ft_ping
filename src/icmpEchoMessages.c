#include "../include/all.h"

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber)
{
    IcmpEchoRequest icmpEchoRequest;
    icmpEchoRequest.type = 8;
    icmpEchoRequest.code = 0;
    icmpEchoRequest.checksum = 0;
    icmpEchoRequest.identifier = id;
    icmpEchoRequest.sequence = sequenceNumber;
    icmpEchoRequest.data = 0;
    icmpEchoRequest.checksum = calculateChecksum(&icmpEchoRequest, sizeof(icmpEchoRequest));
    return icmpEchoRequest;
}

void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest *icmpEchoRequest, const struct sockaddr_in *destAddress)
{
    if (sendto(rawSockfd, icmpEchoRequest, sizeof(*icmpEchoRequest), 0, (struct sockaddr *)destAddress,
               sizeof(*destAddress)) <= 0)
    {
        perror("sendto");
        close(rawSockfd);
        exit(EXIT_FAILURE);
    }
}

struct IcmpEchoReply receiveIcmpEchoReply(int rawSockfd, const struct sockaddr_in *remoteAddress)
{
    char buffer[1024];
    ssize_t bytes_received = recvfrom(rawSockfd, buffer, sizeof(buffer), 0,
                                  (struct sockaddr *)remoteAddress, sizeof(*remoteAddress));
    if (bytes_received < 0) {
        perror("Recvfrom failed");
        exit(EXIT_FAILURE);
    }

    const struct timeval timeReceived = timeOfDay();

    const struct iphdr *ipHeader = (struct iphdr *)buffer;
    const struct icmphdr *icmpHeader = (struct icmphdr *)(buffer + (ipHeader->ihl * 4));

    if (icmpHeader->type == ICMP_ECHOREPLY)
    {
        IcmpEchoReply icmpEchoReply;
        icmpEchoReply.ipHeader = *ipHeader;
        icmpEchoReply.icmpHeader = *icmpHeader;
        icmpEchoReply.timeReceived = timeReceived;
        return icmpEchoReply;
    }
    return (struct IcmpEchoReply) {0};
}
