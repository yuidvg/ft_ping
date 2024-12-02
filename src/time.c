#include "../include/all.h"

struct timeval timeDifference(const struct timeval start, const struct timeval end)
{
    struct timeval result;
    timersub(&end, &start, &result);
    return result;
}

struct timeval timeSum(const struct timeval time1, const struct timeval time2)
{
    struct timeval result;
    timeradd(&time1, &time2, &result);
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
