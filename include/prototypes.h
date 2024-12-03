#pragma once

#include "types.h"

// Socket
IcmpEchoRequest constructIcmpEchoRequest(uint16_t id, uint16_t sequenceNumber, char *padPattern, size_t padPatternLen,
                                         size_t dataLen);
int createRawSocketOrExitFailure();
bool isReadableOrExitFailure(const int sockfd, const struct timeval timeout);

// Checksum
uint16_t calculateChecksum(void *data, size_t length);

// Time
struct timeval timeDifference(const struct timeval start, const struct timeval end);
struct timeval timeSum(const struct timeval time1, const struct timeval time2);
double_t timeValInMiliseconds(const struct timeval timeVal);
struct timeval timeOfDay();

// IP
struct sockaddr_in constructIpHeader(const char *destinationIp);
struct sockaddr_in resolveHostnameOrExitFailure(const char *hostname);
void setTtlOrExitFailure(int rawSockfd, const uint8_t ttl);
void printByteAddressToString(uint32_t address);

// ICMP
void sendIcmpEchoRequest(int rawSockfd, const IcmpEchoRequest icmpEchoRequest, struct sockaddr_in remoteAddress);
IcmpReply receiveIcmpReplyOrExitFailure(int rawSockfd, struct sockaddr_in remoteAddress);
ssize_t recvfromOrExitFailure(int sockfd, void *buf, size_t len, int flags, struct sockaddr_in src_addr);
void printIcmpCodeDescriptions(int type, int code);

// Stats
Stats getUpdatedStats(Stats stats, const double_t rtt);
