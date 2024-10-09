#pragma once

#include "external.h"

typedef struct IcmpEchoRequest {
    uint8_t type;         // ICMP packet type
    uint8_t code;         // Type sub code
    uint16_t checksum;    // Checksum of the ICMP header and data
    uint16_t identifier;  // Identifier to match requests and replies
    uint16_t sequence;    // Sequence number to match requests and replies
    uint64_t data;    // Payload data
    // Followed by data payload (optional)
} IcmpEchoRequest;

typedef struct IcmpEchoReply {
    struct iphdr ipHeader;
    struct icmphdr icmpHeader;
    struct timeval timeSent;
    struct timeval timeReceived;
} IcmpEchoReply;
