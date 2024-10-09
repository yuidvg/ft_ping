#pragma once

#include "types.h"

IcmpEchoHeader constructIcmpEchoHeader(uint16_t id, uint16_t sequenceNumber);
uint16_t calculateChecksum(void *data, int length);
struct timeval time_diff(struct timeval *start, struct timeval *end);
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *ping_ip, char *rev_host);
