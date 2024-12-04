#pragma once

#include "external.h"
#include "values.h"

typedef struct
{
    struct icmphdr icmpHeader;
    uint8_t data[PAYLOAD_SIZE_MAX]; // Payload data
    size_t dataLen; // Data length
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
    char padPattern[PADDING_PATTERN_SIZE_MAX]; // Padding pattern
    size_t padPatternLen; // Padding pattern length
    size_t dataLen;   // Data length
    size_t secondsToRun; // Stop after n packets
    bool numericOutputOnly; // -n option for numeric output only
} Arguments;
