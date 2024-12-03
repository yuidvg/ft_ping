#include "../include/all.h"

// Send

static void setPaddings(uint8_t *payload, size_t dataLen, char *padPattern, size_t padPatternLen)
{

    if (padPattern)
    {
        size_t indexOnAByteInPadPattern = 0;
        for (uint8_t *pointerToAByteInPayload = payload; pointerToAByteInPayload < payload + dataLen;
             pointerToAByteInPayload++)
        {
            *pointerToAByteInPayload = padPattern[indexOnAByteInPadPattern];
            if (++indexOnAByteInPadPattern >= padPatternLen)
                indexOnAByteInPadPattern = 0;
        }
    }
    else
    {
        size_t indexOnAByteInPayload = 0;
        for (uint8_t *pointerToAByteInPayload = payload; pointerToAByteInPayload < payload + dataLen;
             pointerToAByteInPayload++)
            *pointerToAByteInPayload = indexOnAByteInPayload++;
    }
}

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber, char *padPattern, size_t padPatternLen,
                                         size_t dataLen)
{
    IcmpEchoRequest icmpEchoRequest;
    icmpEchoRequest.icmpHeader.type = 8;
    icmpEchoRequest.icmpHeader.code = 0;
    icmpEchoRequest.icmpHeader.checksum = 0;
    icmpEchoRequest.icmpHeader.un.echo.id = id;
    icmpEchoRequest.icmpHeader.un.echo.sequence = sequenceNumber;
    icmpEchoRequest.dataLen = dataLen;
    if (icmpEchoRequest.dataLen >= sizeof(struct timeval))
    {
        const struct timeval timeSent = timeOfDay();
        memcpy(icmpEchoRequest.data, &timeSent, sizeof(struct timeval));
        setPaddings(icmpEchoRequest.data + sizeof(struct timeval), icmpEchoRequest.dataLen - sizeof(struct timeval),
                    padPattern, padPatternLen);
    }
    else
        setPaddings(icmpEchoRequest.data, icmpEchoRequest.dataLen, padPattern, padPatternLen);
    return icmpEchoRequest;
}

static void encodeIcmpEchoRequest(IcmpEchoRequest icmpEchoRequest, char *buffer)
{
    const uint16_t temporaryChecksum = 0;
    buffer[0] = icmpEchoRequest.icmpHeader.type;
    buffer[1] = icmpEchoRequest.icmpHeader.code;

    buffer[2] = (temporaryChecksum >> 8) & 0xFF;
    buffer[3] = temporaryChecksum & 0xFF;

    buffer[4] = (icmpEchoRequest.icmpHeader.un.echo.id >> 8) & 0xFF;
    buffer[5] = icmpEchoRequest.icmpHeader.un.echo.id & 0xFF;

    buffer[6] = (icmpEchoRequest.icmpHeader.un.echo.sequence >> 8) & 0xFF;
    buffer[7] = icmpEchoRequest.icmpHeader.un.echo.sequence & 0xFF;

    /* Copy data */
    memcpy(buffer + 8, icmpEchoRequest.data, icmpEchoRequest.dataLen);
    const uint16_t checksum = calculateChecksum(buffer, icmpEchoRequest.dataLen + 8);
    buffer[3] = (checksum >> 8) & 0xFF;
    buffer[2] = checksum & 0xFF;
}

void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest icmpEchoRequest, struct sockaddr_in destAddress)
{
    char buffer[PAYLOAD_SIZE_MAX + sizeof(struct icmphdr)];
    encodeIcmpEchoRequest(icmpEchoRequest, buffer);
    if (sendto(rawSockfd, buffer, icmpEchoRequest.dataLen + ICMP_ECHO_REQUEST_HEADER_SIZE, 0,
               (struct sockaddr *)&destAddress, sizeof(destAddress)) <= 0)
    {
        perror("sendto");
        close(rawSockfd);
        exit(EXIT_FAILURE);
    }
}

// Receive

IcmpReply receiveIcmpReplyOrExitFailure(int rawSockfd, struct sockaddr_in remoteAddress)
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
            uint8_t *data = (uint8_t *)(buffer + sizeof(struct iphdr) + sizeof(struct icmphdr));
            if (icmpReply.bytesReceived >= sizeof(struct timeval) + sizeof(struct iphdr) + sizeof(struct icmphdr))
                icmpReply.rtt = timeValInMiliseconds(timeDifference(*(struct timeval *)data, timeReceived));
            else
                icmpReply.rtt = NAN;
        }
        return icmpReply;
    }
    else
        return (IcmpReply){0};
}

void printIcmpCodeDescriptions(int type, int code)
{

    const struct icmpCodeDescriptions
    {
        int type;
        int code;
        char *description;
    } ICMP_CODE_DESCRIPTIONS[] = {{ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
                                  {ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
                                  {ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
                                  {ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
                                  {ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
                                  {ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
                                  {ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Network Unknown"},
                                  {ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Host Unknown"},
                                  {ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host Isolated"},
                                  {ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Destination Network Unreachable At This TOS"},
                                  {ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Destination Host Unreachable At This TOS"},
                                  {ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Network"},
                                  {ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Host"},
                                  {ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Type of Service and Network"},
                                  {ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Type of Service and Host"},
                                  {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
                                  {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"}};

    for (const struct icmpCodeDescriptions *p = ICMP_CODE_DESCRIPTIONS;
         p < ICMP_CODE_DESCRIPTIONS + NUMBER_OF_ITEMS(ICMP_CODE_DESCRIPTIONS); p++)
        if (p->type == type && p->code == code)
        {
            printf("%s\n", p->description);
            return;
        }

    printf("Unknown Code: %d\n", code);
}
