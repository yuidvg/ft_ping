#include "../include/all.h"

struct timeval time_diff(struct timeval *start, struct timeval *end)
{
    struct timeval diff;
    diff.tv_sec = end->tv_sec - start->tv_sec;
    diff.tv_usec = end->tv_usec - start->tv_usec;
    if (diff.tv_usec < 0)
    {
        diff.tv_sec -= 1;
        diff.tv_usec += 1000000;
    }
    return diff;
}
