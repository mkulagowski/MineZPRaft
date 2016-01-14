/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Thread-safe Queue declarations
 */

#ifndef __COMMON_TASKQUEUE_HPP__
#define __COMMON_TASKQUEUE_HPP__

#include <condition_variable>
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
 * To call the tasks consumer thread can use TaskQueue::Pop() Method. Execution will use the
 * caller thread, so it is developer's duty to separate producer and consumer threads if needed.
 */
template <typename T = void>
class TaskQueue
{
public:
    /**
     * Typedef for task type used in TaskQueue.
     */
    typedef std::function<T()> TaskType;

    /**
     * Push a task to queue.
     *
     * @param task Task to be pushed. It is recommended to use std::bind for argument support.
     *
     * The template method is designed to be used by producer to populate it with tasks to do.
     * Consumer thread shall use Pop() template method to call tasks and acquire their return
     * values.
     */
    void Push(const TaskType& task);

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

    /**
     * Empties the queue, blocking execution until all tasks are completed.
     *
     * Return type of the task is discarded.
     */
    void EmptyWait();

    /**
     * Clears all tasks inside a Queue. This will drop all the tasks from the Queue and revert it
     * to a state without any tasks to perform.
     */
    void Clear();

    /**
     * Informs whether the queue is empty.
     */
    bool IsEmpty();

private:
    typedef std::list<TaskType> QueueType;

    QueueType mList;
    std::mutex mMutex;
    std::condition_variable mCV;
};

template <typename T>
void TaskQueue<T>::Push(const TaskType& task)
{
    Lock lock(mMutex);

    mList.push_back(task);
    mCV.notify_all();
}

template <typename T>
T TaskQueue<T>::Pop()
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

    // we can unlock here and allow producer to continue adding new tasks for us
    // in the meantime, call the function and return its value
    return task();
}

template <typename T>
void TaskQueue<T>::EmptyWait()
{
    Lock lock(mMutex);

    TaskType task;

    // wait until we have something to process
    while (!mList.empty())
    {
        task = mList.front();
        mList.pop_front();
        task();
    }
}

template <typename T>
void TaskQueue<T>::Clear()
{
    Lock lock(mMutex);
    mList.clear();
}

template <typename T>
bool TaskQueue<T>::IsEmpty()
{
    Lock lock(mMutex);
    return mList.empty();
}

// specialization declarations
template <>
void TaskQueue<void>::Pop();

#endif // __COMMON_TASKQUEUE_HPP__
