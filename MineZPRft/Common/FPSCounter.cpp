/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  FPS counter class definition
 */

#include "FPSCounter.hpp"
#include "Logger.hpp"

FPSCounter::FPSCounter()
    : FPSCounter(5)
{
}

FPSCounter::FPSCounter(int period)
    : mTime(0)
    , mPeriod(period)
    , mCounterPeriod(0)
    ,mFPSAvg(0)
    , mFPSPeriod(0)
{
    mTimer.Start();
}

void FPSCounter::Run()
{
    double loopTime = mTimer.Stop();
    mTimer.Start();

    // Count average FPS per frame
    mFPSAvg = (mFPSAvg + 1 / static_cast<float>(loopTime)) / 2;

    // Count average FPS per time period
    mTime += loopTime;
    mCounterPeriod++;
    if (mTime >= mPeriod)
    {
        mFPSPeriod = static_cast<float>(mCounterPeriod / mTime);
        LOG_I("Average FPS from last " << mPeriod << " seconds: "
              << mFPSPeriod);
        mCounterPeriod = 0;
        mTime = 0;
    }
}

float FPSCounter::GetAvgFrameCount()
{
    return mFPSAvg;
}

float FPSCounter::GetPeriodFrameCount()
{
    float temp = mFPSPeriod;
    mFPSPeriod = 0;
    return temp;
}

int FPSCounter::GetPeriod()
{
    return mPeriod;
}

void FPSCounter::SetPeriod(int period)
{
    mPeriod = period;
}