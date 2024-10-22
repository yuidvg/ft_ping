#pragma once

#include "types.h"

IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber);
int createRawSocketOrExitFailure();
uint16_t calculateChecksum(void *data, int length);
struct timeval timeDifference(const struct timeval start, const struct timeval end);
double_t timeValInMiliseconds(const struct timeval timeVal);
struct sockaddr_in constructIpHeader(const char *destinationIp);
void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest icmpEchoRequest, struct sockaddr_in remoteAddress);
IcmpReply receiveIcmpReply(int rawSockfd, const struct sockaddr_in remoteAddress);
struct timeval timeOfDay();
struct sockaddr_in resolveHostnameOrExitFailure(const char *hostname);

Stats getUpdatedStats(Stats stats, const double_t rtt);
void printByteAddressToString(uint32_t address);
ssize_t recvfromOrExitFailure(int sockfd, void *buf, size_t len, int flags, const struct sockaddr_in src_addr);
uint64_t serializeTimeval(const struct timeval timeval);
struct timeval deserializeTimeval(const uint64_t timeval);
