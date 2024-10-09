#pragma once

#include "types.h"

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber);
uint16_t calculateChecksum(void *data, int length);
struct timeval timeDifference(const struct timeval *start, const struct timeval *end);
double_t timeValInMiliseconds(const struct timeval *timeVal);
struct sockaddr_in constructIpHeader(const char *destinationIp);
void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest *icmpEchoRequest,
                         const struct sockaddr_in *remoteAddress);
struct IcmpEchoReply receiveIcmpEchoReply(int rawSockfd, const struct sockaddr_in *remoteAddress);
struct timeval timeOfDay();
