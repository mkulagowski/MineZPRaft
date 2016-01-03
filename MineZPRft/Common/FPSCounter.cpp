/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  FPS counter class definition
 */

#include "FPSCounter.hpp"
#include "Logger.hpp"

#include <numeric>

FPSCounter::FPSCounter()
    : FPSCounter(5)
{
}

FPSCounter::FPSCounter(int period)
    : mTime(0)
    , mPeriod(period)
    , mCounterPeriod(0)
    , mAvgRange(100)
    , mFPSPeriod(0)
{
    mTimer.Start();
}

void FPSCounter::Run()
{
    double loopTime = mTimer.Stop();
    mTimer.Start();

    // Count average FPS per frame
    mFPSAvg.push_back(1 / static_cast<float>(loopTime));
    if (mFPSAvg.size() > mAvgRange)
        mFPSAvg.pop_front();

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
    size_t queueSize = mFPSAvg.size();
    if (queueSize < mAvgRange)
        return 0;

    float fps = std::accumulate(mFPSAvg.begin(), mFPSAvg.end(), 0.0f);
    fps /= queueSize;
    return fps;
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