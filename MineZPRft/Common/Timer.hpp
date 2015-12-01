/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Timer class definition and method declarations.
 */

#ifndef __COMMON_TIMER_HPP__
#define __COMMON_TIMER_HPP__

#if defined(WIN32)
#include <Windows.h>
#elif defined(__LINUX__) | defined(__linux__)
#include <time.h>
#endif // defined(WIN32)

/**
 * Class used to manage high resolution time measurement.
 *
 * API provided by this class is designed to measure time difference between two time points
 * specified by user with Start() and Stop() calls. Time measurement is done with highest precision
 * available on the system (using either Windows QueryPerformanceCounter or Linux clock_gettime).
 *
 * The reason for this class existence is to avoid using std::chrono, which in some cases might not
 * have high enough resolution.
 */
class Timer
{
private:
#if defined(WIN32)
    LARGE_INTEGER mStart; // start point
#elif defined(__LINUX__) | defined(__linux__)
    struct timespec mStart;
#endif // defined(WIN32)

public:
    Timer();

    /**
     * Start time measurement.
     */
    void Start() noexcept;

    /**
     * Stop time measurement and return time elapsed since last Start() call.
     *
     * @return Seconds elapsed since last Start() call
     */
    double Stop() noexcept;
};

#endif // __COMMON_TIMER_HPP__
