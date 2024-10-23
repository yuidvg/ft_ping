#pragma once

#define BYTE 8
#define DEFAULT_TTL 60

extern volatile sig_atomic_t catchedSigint;

#define ICMP_ECHO_REQUEST_PAYLOAD_SIZE 56
#define ICMP_ECHO_REQUEST_PAYLOAD_TIMESTAMP_SIZE 8
#define DEFAULT_INTERVAL                                                                                               \
    (struct timeval)                                                                                                   \
    {                                                                                                                  \
        .tv_sec = 1                                                                                                    \
    }
