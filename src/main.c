#include "../include/all.h"

static volatile sig_atomic_t catchedSigint = 0;

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
           "%zu packets transmitted, %zu packets received, %.0f%% packet loss\n"
           "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           hostname, packetsTransmitted, packetsReceived, packetLoss, stats.min, stats.mean, stats.max, stddev);

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
    if (ac == 1)
    {
        printf("ft_ping: missing host operand\nTry 'ft_ping -?' for more information.\n");
        exit(EX_USAGE);
    }
    else if (ac >= 2)
    {
        Arguments arguments = parseArguments(ac, av);

        signal(SIGINT, setCatchedSigint);

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
        printf("ft_ping %s (%s): %zu data bytes\n", arguments.hostname, arguments.hostname,
               sizeof(((IcmpEchoRequest *)0)->data) * BYTE);

        while (!catchedSigint)
        {
            const IcmpEchoRequest icmpEchoRequest = constructIcmpEchoRequest(getpid(), sequenceNumber);
            sendIcmpEchoRequest(rawSockfd, &icmpEchoRequest, &remoteAddress);                    // -->
            const IcmpEchoReply icmpEchoReply = receiveIcmpEchoReply(rawSockfd, &remoteAddress); // <--

            struct timeval timeDiff = timeDifference(&icmpEchoReply.timeReceived, &icmpEchoReply.timeSent);
            const double_t rtt = timeValInMiliseconds(&timeDiff);
            stats = getUpdatedStats(stats, rtt);

            char ip_str[INET_ADDRSTRLEN];
            printf("%lu bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n", sizeof(IcmpEchoReply),
                   byteAddressToString(icmpEchoReply.ipHeader.saddr, ip_str), icmpEchoReply.icmpHeader.un.echo.sequence,
                   icmpEchoReply.ipHeader.ttl, rtt);
            ++sequenceNumber;
            sleep(1);
        }
        const size_t packetsTransmitted = sequenceNumber;
        const size_t packetsReceived = stats.n;
        printConclusion(arguments.hostname, packetsTransmitted, packetsReceived, stats);
    }
}
