#include "../include/all.h"

int main(int ac, char *av[])
{
    if (ac == 1)
    {
        printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
    }
    else if (ac == 2)
    {
        // Create raw socket
        const int rawSockfd = createRawSocketOrExitFailure();
        const struct sockaddr_in remoteAddress = constructIpHeader(av[1]);

        int sequenceNumber = 0;
        while (1)
        {
            // Send ICMP Echo Request
            const IcmpEchoRequest icmpEchoRequest = constructIcmpEchoRequest(getpid(), sequenceNumber);

            sendIcmpEchoRequest(rawSockfd, &icmpEchoRequest, &remoteAddress);

            // Receive ICMP Echo Reply and Print (Maybe Somewhere Else Like in A Handler)
            const IcmpEchoReply icmpEchoReply = receiveIcmpEchoReply(rawSockfd, &remoteAddress);

            struct timeval timeDiff = timeDifference(&icmpEchoReply.timeReceived, &icmpEchoReply.timeSent);
            printf("%lu bytes from %s: icmp_seq=%u ttl=%d time=%.2f ms\n",
                   sizeof(IcmpEchoReply),
                   inet_ntoa(remoteAddress.sin_addr),
                   icmpEchoReply.icmpHeader.un.echo.sequence,
                   icmpEchoReply.ipHeader.ttl,
                   timeValInMiliseconds(&timeDiff));
            ++sequenceNumber;
        }
    }
}
