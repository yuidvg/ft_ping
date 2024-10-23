#pragma once

#include "types.h"

// Socket
IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber);
int createRawSocketOrExitFailure();
bool isReadableOrExitFailure(const int sockfd, const struct timeval timeout);

// Checksum
uint16_t calculateChecksum(void *data, int length);

// Time
struct timeval timeDifference(const struct timeval start, const struct timeval end);
struct timeval timeSum(const struct timeval time1, const struct timeval time2);
double_t timeValInMiliseconds(const struct timeval timeVal);

// IP
struct sockaddr_in constructIpHeader(const char *destinationIp);
void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest icmpEchoRequest, struct sockaddr_in remoteAddress);
IcmpReply receiveIcmpReplyOrExitFailure(int rawSockfd, struct sockaddr_in remoteAddress);
struct timeval timeOfDay();
struct sockaddr_in resolveHostnameOrExitFailure(const char *hostname);

// Stats
Stats getUpdatedStats(Stats stats, const double_t rtt);

// Utils
void printByteAddressToString(uint32_t address);
ssize_t recvfromOrExitFailure(int sockfd, void *buf, size_t len, int flags, const struct sockaddr_in src_addr);
uint64_t serializeTimeval(const struct timeval timeval);
struct timeval deserializeTimeval(const uint64_t timeval);
void setTtlOrExitFailure(int rawSockfd, const uint8_t ttl);
