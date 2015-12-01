/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  High resolution timer class definition for Linux systems
 */

#include "../Timer.hpp"


Timer::Timer()
{
    mStart.tv_sec = 0;
    mStart.tv_nsec = 0;
}

void Timer::Start() noexcept
{
    clock_gettime(CLOCK_MONOTONIC, &mStart);
}

double Timer::Stop() noexcept
{
    struct timespec mStop;
    clock_gettime(CLOCK_MONOTONIC, &mStop);

    // negative time difference should not occur when using CLOCK_MONOTONIC
    time_t sec_result = (mStop.tv_sec - mStart.tv_sec) * 1000000000;
    double result = static_cast<double>(sec_result + (mStop.tv_nsec - mStart.tv_nsec)) * 1e-9;
    return result;
}
