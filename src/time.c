#include "../include/all.h"

struct timeval timeDifference(const struct timeval *start, const struct timeval *end)
{
    struct timeval result;
    timersub(start, end, &result);
    return result;
}

double_t timeValInMiliseconds(const struct timeval *timeVal)
{
    return timeVal->tv_sec * 1000.0 + timeVal->tv_usec / 1000.0;
}

struct timeval timeOfDay()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time;
}
