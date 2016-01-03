/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  TaskQueue specialization definitions
 */

#include "TaskQueue.hpp"

template <>
void TaskQueue<void>::Pop()
{
    TaskType task;

    {
        Lock lock(mMutex);

        // wait until we have something to process
        while (mList.empty())
            mCV.wait(lock);

        task = mList.front();
        mList.pop_front();
    }

    // we can unlock here and allow producer to continue calculating
    // call the function and return its value
    task();
}
