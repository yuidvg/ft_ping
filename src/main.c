#include "../include/all.h"

volatile sig_atomic_t catchedSigint = 0;

static void setCatchedSigint(const int sig)
{
    (void)sig;
    catchedSigint = 1;
}

static void printConclusion(const char *hostname, const size_t packetsTransmitted, const size_t packetsReceived,
                            const Stats stats)
{
    const double_t packetLoss =
        packetsTransmitted ? (packetsTransmitted - packetsReceived) / (double)packetsTransmitted * 100.0 : 0.0;
    const double_t stddev = stats.n > 1 ? sqrt(stats.M2 / (stats.n - 1)) : 0;
    printf("--- %s ft_ping statistics ---\n"
           "%zu packets transmitted, %zu packets received, %.0f%% packet loss\n",
           hostname, packetsTransmitted, packetsReceived, packetLoss);
    if (packetsReceived > 0)
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats.min, stats.mean, stats.max, stddev);
    exit(0);
}

static Arguments parseArguments(const int ac, char *av[])
{
    Arguments arguments = {false, false, DEFAULT_TTL, NULL}; // Default values
    int opt;
    const struct option longOptions[] = {{"ttl", required_argument, NULL, 't'}, {NULL, 0, NULL, 0}};
    while ((opt = getopt_long(ac, av, "v?", longOptions, &optind)) != -1)
    {
        switch (opt)
        {
        case 'v':
            arguments.verbose = true;
            break;
        case '?':
            printf("Usage: ft_ping [OPTION...] HOST ...\n"
                   "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
                   "Options:\n"
                   "  -ttl=N\tspecify N as time-to-live\n"
                   "  -v\t\tverbose output\n"
                   "  -?\t\tgive this help list\n");
            exit(EX_OK);
        case 't':
            arguments.ttl = atoi(optarg);
            break;
        default:
            fprintf(stderr, "ft_ping: invalid option -- '%c'\nTry 'ft_ping -?' for more information.\n", optopt);
            exit(EX_USAGE);
        }
    }

    arguments.hostname = av[optind];

    if (arguments.hostname == NULL)
    {
        fprintf(stderr, "ft_ping: missing host operand\nTry 'ft_ping -?' for more information.\n");
        exit(EX_USAGE);
    }

    return arguments;
}

int main(int ac, char *av[])
{
    if (ac >= 2)
    {
        Arguments arguments = parseArguments(ac, av);

        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = setCatchedSigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGINT, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        const int rawSockfd = createRawSocketOrExitFailure();
        const struct sockaddr_in remoteAddress = resolveHostname(arguments.hostname);

        // Set the TTL for the socket
        if (setsockopt(rawSockfd, IPPROTO_IP, IP_TTL, &arguments.ttl, sizeof(arguments.ttl)) < 0)
        {
            perror("setsockopt");
            close(rawSockfd);
            exit(EXIT_FAILURE);
        }

        size_t sequenceNumber = 0;
        Stats stats = {0, 0, 0, INFINITY, 0};
        printf("PING %s (%s): %zu data bytes\n", arguments.hostname, inet_ntoa(remoteAddress.sin_addr),
               sizeof(((IcmpEchoRequest *)0)->timestampData) + sizeof(((IcmpEchoRequest *)0)->data));

        while (!catchedSigint)
        {
            const IcmpEchoRequest icmpEchoRequest = constructIcmpEchoRequest(getpid(), sequenceNumber);
            sendIcmpEchoRequest(rawSockfd, icmpEchoRequest, remoteAddress);         // -->
            const IcmpReply icmpReply = receiveIcmpReply(rawSockfd, remoteAddress); // <--
            if (!catchedSigint)
            {
                if (icmpReply.icmpHeader.type == ICMP_ECHOREPLY)
                {
                    stats = getUpdatedStats(stats, icmpReply.rtt);
                    printf("%lu bytes from ", icmpReply.bytesReceived - sizeof(((IcmpReply *)0)->ipHeader));
                    printByteAddressToString(icmpReply.ipHeader.saddr);
                    printf(": icmp_seq=%u ttl=%d time=%.3f ms\n", icmpReply.icmpHeader.un.echo.sequence,
                           icmpReply.ipHeader.ttl, icmpReply.rtt);
                }
                else if (arguments.verbose)
                {
                    printf("%lu bytes from ", icmpReply.bytesReceived - sizeof(((IcmpReply *)0)->ipHeader));
                    printByteAddressToString(icmpReply.ipHeader.saddr);
                    printf(": type=%u code=%u\n", icmpReply.icmpHeader.type, icmpReply.icmpHeader.code);
                }
                ++sequenceNumber;
                sleep(1);
            }
        }
        const size_t packetsTransmitted = sequenceNumber;
        const size_t packetsReceived = stats.n;
        printConclusion(arguments.hostname, packetsTransmitted, packetsReceived, stats);
    }
    else if (ac == 1)
    {
        printf("ft_ping: missing host operand\nTry 'ft_ping -?' for more information.\n");
        exit(EX_USAGE);
    }
}
