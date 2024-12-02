#pragma once
#include "external.h"

#define MAXIPLEN	60
#define MAXICMPLEN	76
#define BYTE 8
#define DEFAULT_TTL 60
#define PAYLOAD_SIZE_MAX (65535 - MAXIPLEN - MAXICMPLEN)
#define PADDING_PATTERN_SIZE_MAX 16
#define ICMP_ECHO_REQUEST_HEADER_SIZE sizeof(struct icmphdr)

extern volatile sig_atomic_t catchedSigint;

#define DEFAULT_PAYLOAD_SIZE 56
#define ICMP_ECHO_REQUEST_PAYLOAD_TIMESTAMP_SIZE 8
#define DEFAULT_INTERVAL (struct timeval){.tv_sec = 1}
