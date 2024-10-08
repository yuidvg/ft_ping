#include "../include/all.h"

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber)
{
    IcmpEchoRequest icmpEchoRequest;
    icmpEchoRequest.type = 8;
    icmpEchoRequest.code = 0;
    icmpEchoRequest.checksum = 0;
    icmpEchoRequest.identifier = id;
    icmpEchoRequest.sequence = sequenceNumber;
    icmpEchoRequest.data = 0;
    icmpEchoRequest.checksum = calculateChecksum(&icmpEchoRequest, sizeof(icmpEchoRequest));
    return icmpEchoRequest;
}