#include "../include/all.h"

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber)
{
    IcmpEchoRequest icmpEchoRequest;
    icmpEchoRequest.type = 8;
    icmpEchoRequest.code = 0;
    icmpEchoRequest.checksum = 0;
    icmpEchoRequest.identifier = id;
    icmpEchoRequest.sequence = sequenceNumber;
    icmpEchoRequest.timestampData = serializeTimeval(timeOfDay());
    memset(icmpEchoRequest.data, 0, sizeof(icmpEchoRequest.data));
    icmpEchoRequest.checksum = calculateChecksum(&icmpEchoRequest, sizeof(icmpEchoRequest));
    return icmpEchoRequest;
}

void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest icmpEchoRequest, struct sockaddr_in destAddress)
{
    if (sendto(rawSockfd, &icmpEchoRequest, sizeof(icmpEchoRequest), 0, (struct sockaddr *)&destAddress,
               sizeof(destAddress)) <= 0)
    {
        perror("sendto");
        close(rawSockfd);
        exit(EXIT_FAILURE);
    }
}

IcmpReply receiveIcmpReply(int rawSockfd, const struct sockaddr_in remoteAddress)
{
    IcmpReply icmpReply;

    char buffer[IP_MAXPACKET];
    const ssize_t recvfromReturn =
        recvfromOrExitFailure(rawSockfd, buffer, sizeof(buffer), 0, remoteAddress); // Can be interrupted by SIGINT
    if (recvfromReturn > 0)
    {
        icmpReply.bytesReceived = recvfromReturn;
        const struct timeval timeReceived = timeOfDay();

        // Parse buffer to IP Header, ICMP Header and Time Sent
        icmpReply.ipHeader = *(struct iphdr *)buffer;
        icmpReply.icmpHeader = *(struct icmphdr *)(buffer + (icmpReply.ipHeader.ihl * 4));

        if (icmpReply.icmpHeader.type == ICMP_ECHOREPLY)
        {
            uint64_t data = *(uint64_t *)(buffer + sizeof(struct iphdr) + sizeof(struct icmphdr));
            icmpReply.rtt = timeValInMiliseconds(timeDifference(deserializeTimeval(data), timeReceived));
        }
        return icmpReply;
    }
    else
        return (IcmpReply){0};
}
