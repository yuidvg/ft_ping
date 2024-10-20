#include "../include/all.h"

struct timeval timeDifference(const struct timeval start, const struct timeval end)
{
    struct timeval result;
    timersub(&end, &start, &result);
    return result;
}

double_t timeValInMiliseconds(const struct timeval timeVal)
{
    return timeVal.tv_sec * 1000.0 + timeVal.tv_usec / 1000.0;
}

struct timeval timeOfDay()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time;
}

uint64_t serializeTimeval(const struct timeval timeval)
{
    return ((uint64_t)timeval.tv_sec << 32) | (uint64_t)timeval.tv_usec;
}

struct timeval deserializeTimeval(const uint64_t timeval)
{
    struct timeval result;
    result.tv_sec = (time_t)(timeval >> 32);
    result.tv_usec = (suseconds_t)(timeval & 0xFFFFFFFF);
    return result;
}
