/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  FPS counter definition and method declarations.
 */

#ifndef __COMMON_FPSCOUNTER_HPP__
#define __COMMON_FPSCOUNTER_HPP__

#include "Timer.hpp"
#include <deque>

/**
 * Class used to count FPS.
 * Every few seconds (period variable) FPS counted is being logged.
 * FPS is also counted every call and last 100 values are averaged - it can be
 * obtained with GetAvgFrameCount() method.
 */
class FPSCounter
{
private:
    Timer mTimer;
    double mTime;
    int mPeriod;
    int mCounterPeriod;
    int mAvgRange;
    std::deque<float> mFPSAvg;
    float mFPSPeriod;

public:
    FPSCounter();
    FPSCounter(int period);

    /**
     * Counter loop that does all the counting.
     * Needs to be run in main application loop.
     */
    void Run();

    /**
     * Return average frame count.
     *
     * @return Averaged FPS from last 100 Run() calls.
     *         0 if Run() was called less than 100 times.
     */
    float GetAvgFrameCount();

    /**
     * Return last average period frame count.
     * Variable is cleared upon call.
     *
     * @return Averaged FPS since last @period seconds.
     */
    float GetPeriodFrameCount();

    /**
     * Return period between logging FPS counts.
     *
     * @return Period in seconds.
     */
    int GetPeriod();

    /**
     * Set period between logging FPS counts.
     * @param period Period in seconds.
     */
    void SetPeriod(int period);
};

#endif // __COMMON_FPSCOUNTER_HPP__
