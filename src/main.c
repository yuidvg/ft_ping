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
    printf("--- %s ping statistics ---\n"
           "%zu packets transmitted, %zu packets received, %.0f%% packet loss\n"
           "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
           hostname, packetsTransmitted, packetsReceived, packetLoss, stats.min, stats.mean, stats.max, stddev);

    exit(0);
}

static Arguments parseArguments(const int ac, char *av[])
{
    Arguments arguments = {false, false, 225, NULL}; // Default values
    int opt;
    struct option longOptions[] = {
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, '?'},
        {"ttl", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ((opt = getopt_long(ac, av, "v?", longOptions, &optind)) != -1)
    {
        switch (opt)
        {
        case 'v':
            arguments.verbose = true;
            break;
        case '?':
            arguments.help = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-v] [-?] [--ttl ttl_value] hostname\n", av[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Check for --ttl option
    for (int i = optind; i < ac; i++)
    {
        if (strcmp(av[i], "--ttl") == 0)
        {
            if (i + 1 < ac)
            {
                arguments.ttl = atoi(av[i + 1]);
                i++; // Skip the ttl value
            }
            else
            {
                fprintf(stderr, "Usage: %s [-v] [-?] [--ttl ttl_value] hostname\n", av[0]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            arguments.hostname = av[i];
        }
    }

    if (arguments.hostname == NULL)
    {
        fprintf(stderr, "Usage: %s [-v] [-?] [--ttl ttl_value] hostname\n", av[0]);
        exit(EXIT_FAILURE);
    }

    return arguments;
}

int main(int ac, char *av[])
{
    if (ac == 1)
    {
        printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
        exit(64);
    }
    else if (ac >= 2)
    {
        Arguments arguments = parseArguments(ac, av);
        const char *hostname = arguments.hostname;
        signal(SIGINT, setCatchedSigint);
        // Create raw socket
        const int rawSockfd = createRawSocketOrExitFailure();
        const struct sockaddr_in remoteAddress = resolveHostname(hostname);

        size_t sequenceNumber = 0;
        Stats stats = {0, 0, 0, INFINITY, 0};
        printf("PING %s (%s): %zu data bytes\n", hostname, hostname, sizeof(((IcmpEchoRequest *)0)->data) * BYTE);
        while (!catchedSigint)
        {
            // Send ICMP Echo Request
            const IcmpEchoRequest icmpEchoRequest = constructIcmpEchoRequest(getpid(), sequenceNumber);
            sendIcmpEchoRequest(rawSockfd, &icmpEchoRequest, &remoteAddress);                    // -->
            const IcmpEchoReply icmpEchoReply = receiveIcmpEchoReply(rawSockfd, &remoteAddress); // <--

            struct timeval timeDiff = timeDifference(&icmpEchoReply.timeReceived, &icmpEchoReply.timeSent);
            const double_t rtt = timeValInMiliseconds(&timeDiff);

            // Welford's online algorithm
            stats.n = stats.n + 1;
            const double_t deltaFromOldMean = rtt - stats.mean;
            stats.mean = stats.mean + deltaFromOldMean / stats.n;
            const double_t deltaFromNewMean = rtt - stats.mean;
            stats.M2 = stats.M2 + deltaFromOldMean * deltaFromNewMean;

            stats.min = (rtt < stats.min) ? rtt : stats.min;
            stats.max = (rtt > stats.max) ? rtt : stats.max;

            printf("%lu bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n", sizeof(IcmpEchoReply),
                   inet_ntoa(remoteAddress.sin_addr), icmpEchoReply.icmpHeader.un.echo.sequence,
                   icmpEchoReply.ipHeader.ttl, rtt);

            ++sequenceNumber;
            sleep(1);
        }
        const size_t packetsTransmitted = sequenceNumber;
        const size_t packetsReceived = stats.n;
        printConclusion(hostname, packetsTransmitted, packetsReceived, stats);
    }
}
