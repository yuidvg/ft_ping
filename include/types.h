#pragma once

#include "external.h"

typedef struct
{
    uint8_t type;        // ICMP packet type
    uint8_t code;        // Type sub code
    uint16_t checksum;   // Checksum of the ICMP header and data
    uint16_t identifier; // Identifier to match requests and replies
    uint16_t sequence;   // Sequence number to match requests and replies
    uint64_t data;        // Payload data
    // Followed by data payload (optional)
} IcmpEchoRequest;

typedef struct
{
    size_t bytesReceived;
    struct iphdr ipHeader;
    struct icmphdr icmpHeader;
    double_t rtt;
} IcmpReply;

typedef struct
{
    size_t n;
    double_t mean;
    double_t M2;
    double_t min;
    double_t max;
} Stats;

typedef struct
{
    bool help;      // -? option for help
    bool verbose;   // -v option for verbose output
    size_t ttl;     // --ttl option for time to live
    char *hostname; // Hostname to ping
} Arguments;
