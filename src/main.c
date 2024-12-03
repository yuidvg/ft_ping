#include "../include/all.h"

volatile sig_atomic_t catchedSigint = 0;

static void setCatchedSigint(const int sig __attribute__((unused)))
{
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
    if (packetsReceived > 0 && !isnan(stats.mean))
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", stats.min, stats.mean, stats.max, stddev);
    exit(0);
}

static void setSignalHandlerOrExitFailure(void (*handler)(int))
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

static size_t parsePattern(const char *text, char *pattern_data)
{
    int c, off;
    size_t i = 0;

    for (; text && *text && i < PADDING_PATTERN_SIZE_MAX; i++)
    {
        if (sscanf(text, "%2x%n", &c, &off) != 1)
        {
            fprintf(stderr, "error in pattern near %s", text);
            exit(EXIT_FAILURE);
        }
        text += off;
        pattern_data[i] = c;
    }
    return i;
}

static size_t parseNumber(const char *optarg, size_t maxval, bool allowZero)
{
    char *p;
    if (optarg)
    {
        unsigned long n = 0;
        n = strtoul(optarg, &p, 0);
        if (*p)
        {
            fprintf(stderr, "invalid value (`%s' near `%s')\n", optarg, p);
            exit(EXIT_FAILURE);
        }
        if (n == 0 && !allowZero)
        {
            fprintf(stderr, "option value too small: %s\n", optarg);
            exit(EXIT_FAILURE);
        }
        if (maxval && n > maxval)
        {
            fprintf(stderr, "option value too big: %s\n", optarg);
            exit(EXIT_FAILURE);
        }
        return n;
    }
    else
        return 0;
}

static Arguments parseArguments(const int ac, char *av[])
{
    Arguments arguments = {.help = false,
                           .verbose = false,
                           .ttl = DEFAULT_TTL,
                           .hostname = NULL,
                           .padPattern = {0},
                           .padPatternLen = 0,
                           .dataLen = DEFAULT_PAYLOAD_SIZE,
                           .secondsToRun = PAYLOAD_SIZE_MAX,
                           .numericOutputOnly = false}; // Default values
    int opt;
    const struct option longOptions[] = {{"ttl", required_argument, NULL, 't'}, {NULL, 0, NULL, 0}};
    while ((opt = getopt_long(ac, av, "v?np:w:s:", longOptions, &optind)) != -1)
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
                   "  --ttl=N\tspecify N as time-to-live\n"
                   "  -v\t\tverbose output\n"
                   "  -?\t\tgive this help list\n"
                   "  -n\t\tdo not resolve host addresses\n"
                   "  -p=PATTERN\t\tfill ICMP packet with given pattern (hex)\n"
                   "  -w=N\t\tstop after N seconds\n"
                   "  -s=NUMBER\t\tsend NUMBER data octets\n");
            exit(EX_OK);
        case 't':
            arguments.ttl = parseNumber(optarg, UINT8_MAX, false);
            break;
        case 'n':
            arguments.numericOutputOnly = true;
            break;
        case 'p':
            arguments.padPatternLen = parsePattern(optarg, arguments.padPattern);
            break;
        case 'w':
            arguments.secondsToRun = parseNumber(optarg, INT_MAX, false);
            break;
        case 's':
            arguments.dataLen = parseNumber(optarg, PAYLOAD_SIZE_MAX, true);
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

static void printHeaderMessage(const Arguments arguments, const struct sockaddr_in remoteAddress)
{
    printf("PING %s (%s): %zu ", arguments.hostname, inet_ntoa(remoteAddress.sin_addr), arguments.dataLen);
    if (arguments.verbose)
        printf("(%zu) ", arguments.dataLen + sizeof(struct iphdr) + sizeof(struct icmphdr));
    printf("data bytes\n");
}

int main(int ac, char *av[])
{
    if (ac >= 2)
    {
        Arguments arguments = parseArguments(ac, av);

        setSignalHandlerOrExitFailure(setCatchedSigint);

        const int rawSockfd = createRawSocketOrExitFailure();
        const struct sockaddr_in remoteAddress = resolveHostnameOrExitFailure(arguments.hostname);

        setTtlOrExitFailure(rawSockfd, arguments.ttl);

        size_t sequenceNumber = 0;
        Stats stats = {0, 0, 0, INFINITY, 0};

        printHeaderMessage(arguments, remoteAddress);
        const struct timeval timeStarted = timeOfDay();
        while (!catchedSigint && (size_t)timeDifference(timeStarted, timeOfDay()).tv_sec < arguments.secondsToRun)
        {
            const IcmpEchoRequest icmpEchoRequest = constructIcmpEchoRequest(
                getpid(), sequenceNumber, arguments.padPattern, arguments.padPatternLen, arguments.dataLen);
            sendIcmpEchoRequest(rawSockfd, icmpEchoRequest, remoteAddress);
            ++sequenceNumber;
            const struct timeval lastSent = timeOfDay();
            while (
                isReadableOrExitFailure(rawSockfd, timeDifference(timeOfDay(), timeSum(lastSent, DEFAULT_INTERVAL))) &&
                !catchedSigint)
            {
                const IcmpReply icmpReply = receiveIcmpReplyOrExitFailure(rawSockfd, remoteAddress);
                if (!catchedSigint)
                {
                    if (icmpReply.bytesReceived > 0)
                    {
                        if (icmpReply.icmpHeader.type == ICMP_ECHOREPLY)
                        {
                            stats = getUpdatedStats(stats, icmpReply.rtt);
                            printf("%lu bytes from ", icmpReply.bytesReceived - sizeof(struct iphdr));
                            printByteAddressToString(icmpReply.ipHeader.saddr);
                            printf(": icmp_seq=%u ttl=%d", ntohs(icmpReply.icmpHeader.un.echo.sequence),
                                   icmpReply.ipHeader.ttl);
                            if (!isnan(icmpReply.rtt))
                                printf(" time=%.3f ms\n", icmpReply.rtt);
                            else
                                printf("\n");
                        }
                        else
                        {
                            printf("%lu bytes from ", icmpReply.bytesReceived - sizeof(struct iphdr));
                            printByteAddressToString(icmpReply.ipHeader.saddr);
                            if (arguments.verbose)
                            {
                                printf(": type=%u code=%u\n", icmpReply.icmpHeader.type, icmpReply.icmpHeader.code);
                            }
                            else
                            {
                                printf(": ");
                                printIcmpCodeDescriptions(icmpReply.icmpHeader.type, icmpReply.icmpHeader.code);
                            }
                        }
                    }
                }
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
