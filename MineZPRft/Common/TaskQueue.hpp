/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Thread-safe Queue declarations
 */

#ifndef __COMMON_TASKQUEUE_HPP__
#define __COMMON_TASKQUEUE_HPP__

#include <mutex>
#include <list>

typedef std::unique_lock<std::mutex> Lock;

/**
 * Template implementing a thread-safe FIFO task queue.
 *
 * The queue will keep the tasks pushed by producer and withhold them until consumer will start
 * processing them.
 *
 * Template parameter T refers to value returned by task. By default TaskQueue assumes void.
 *
 * To call 
 */
template <typename T = void>
class TaskQueue
{
public:
    /**
     * Push a task to queue.
     *
     * @param task Task to be pushed. It is recommended to use std::bind for argument support.
     *
     * The template method is designed to be used by producer to populate it with tasks to do.
     * Consumer thread shall use Pop() template method to call tasks and acquire their return
     * values.
     */
    void Push(const std::function<T()>& task);

    /**
     * Pop a task from top of the queue, call it and return its value.
     *
     * @return Return value of called task.
     *
     * The template method will call the next task awaiting on top of the queue. It is designed
     * to be called by consumer thread to perform tasks requested by producer.
     *
     * The function will hang until the task is finished. Afterwards, the task is removed from the
     * queue and return value of the task is propagated further.
     *
     * If there is no tasks in the queue, the function will hang until producer will provide a task
     * to process.
     */
    T Pop();

private:
    typedef std::list<std::function<T()>> QueueType;

    QueueType mList;
    std::mutex mMutex;
    std::condition_variable mCV;
};

template <typename T>
void TaskQueue<T>::Push(const std::function<T()>& task)
{
    Lock lock (mMutex);

    mList.push_back(task);
    mCV.notify_all();
}

void TaskQueue<void>::Pop()
{
    Lock lock(mMutex);

    // wait until we have something to process
    while (mList.empty())
        mCV.wait(lock);

    // call the function
    mList.front()();
    mList.pop_front();
}

template <typename T>
T TaskQueue<T>::Pop()
{
    Lock lock(mMutex);

    // wait until we have something to process
    while (mList.empty())
        mCV.wait(lock);

    // call the function and return its value
    T ret = mList.front()();
    mList.pop_front();
    return ret;
}

#endif // __COMMON_TASKQUEUE_HPP__
