#include "../include/all.h"

Stats getUpdatedStats(Stats stats, const double_t rtt)
{
    // Welford's online algorithm
    stats.n = stats.n + 1;
    const double_t deltaFromOldMean = rtt - stats.mean;
    stats.mean = stats.mean + deltaFromOldMean / stats.n;
    const double_t deltaFromNewMean = rtt - stats.mean;
    stats.M2 = stats.M2 + deltaFromOldMean * deltaFromNewMean;

    stats.min = (rtt < stats.min) ? rtt : stats.min;
    stats.max = (rtt > stats.max) ? rtt : stats.max;
    return stats;
}
