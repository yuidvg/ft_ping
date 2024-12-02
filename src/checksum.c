#include "../include/all.h"

uint16_t calculateChecksum(void *data, size_t length)
{
    uint16_t *buf = data;
    uint32_t sum = 0;
    uint16_t result;

    for (sum = 0; length > 1; length -= 2)
        sum += *buf++;
    if (length == 1)
        sum += *(uint8_t *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
