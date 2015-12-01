/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  High resolution timer class definition for Windows system
 */

#include "../Timer.hpp"

namespace {
static double GetCounterPeriod()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return 1.0 / static_cast<double>(freq.QuadPart);
}

static const double mPeriod = GetCounterPeriod();
} // namespace


Timer::Timer()
{
    mStart.QuadPart = 0;
}

void Timer::Start() noexcept
{
    QueryPerformanceCounter(&mStart);
}

double Timer::Stop() noexcept
{
    LARGE_INTEGER stop;
    QueryPerformanceCounter(&stop);

    return static_cast<double>(stop.QuadPart - mStart.QuadPart) * mPeriod;
}
