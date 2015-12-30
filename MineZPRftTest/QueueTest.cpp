/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Queue template tests
 */

#include <gtest/gtest.h>

#include <functional>
#include <thread>
#include <chrono>

#include "Common/TaskQueue.hpp"


namespace {

const unsigned int TASK_COUNT = 500;

} // namespace


/**
 * Test if TaskQueue works at all on single threaded environment
 */
TEST(TaskQueue, SingleThread)
{
    auto lambdaRetParam = [](int i) -> int {
        return i;
    };

    TaskQueue<int> queue;
    queue.Push(std::bind(lambdaRetParam, 1));
    queue.Push(std::bind(lambdaRetParam, 2));
    queue.Push(std::bind(lambdaRetParam, 3));

    // The order of popped variables should be the same as pushed ones (FIFO queue).
    ASSERT_EQ(queue.Pop(), 1);
    ASSERT_EQ(queue.Pop(), 2);
    ASSERT_EQ(queue.Pop(), 3);
}

/**
 * Test producer and consumer on separate threads
 */
TEST(TaskQueue, SingleProducerSingleConsumer)
{
    auto lambdaRetParam = [](int i) -> int {
        return i;
    };

    TaskQueue<int> queue;

    // Create our producer and consumer
    auto producer = [&queue, &lambdaRetParam]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
            queue.Push(std::bind(lambdaRetParam, i));
    };
    auto consumer = [&queue]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
            ASSERT_EQ(queue.Pop(), i);
    };

    // Put our producer and consumer into separate threads and wait until they finish.
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);
    producerThread.join();
    consumerThread.join();
}

/**
 * Test if the consumer will wait for producer before processing the tasks.
 */
TEST(TaskQueue, ConsumerWaitForProducer)
{
    auto lambdaRetParam = [](int i) -> int {
        return i;
    };

    TaskQueue<int> queue;

    // Create our producer and consumer
    auto producer = [&queue, &lambdaRetParam]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
            queue.Push(std::bind(lambdaRetParam, i));
    };
    auto consumer = [&queue]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
            ASSERT_EQ(queue.Pop(), i);
    };

    // Put our producer and consumer into separate threads
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);

    // Start consumer
    consumerThread.join();

    // Wait a little bit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start producer
    producerThread.join();
}

/**
 * Make producer wait a little bit before pushing a new task.
 *
 * Consumer should dutifully wait until producer will create a new task.
 */
TEST(TaskQueue, LazyProducer)
{
    auto lambdaRetParam = [](int i) -> int {
        return i;
    };

    TaskQueue<int> queue;

    // Create our producer and consumer
    auto producer = [&queue, &lambdaRetParam]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
        {
            // Create a small "lag" before pushing a new task.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            queue.Push(std::bind(lambdaRetParam, i));
        }
    };
    auto consumer = [&queue]() {
        for (unsigned int i = 0; i < TASK_COUNT; ++i)
            ASSERT_EQ(queue.Pop(), i);
    };

    // Put our producer and consumer into separate threads and wait until they finish.
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);
    producerThread.join();
    consumerThread.join();
}

/**
 * Check if destroying a queue will not cause a hang.
 */
TEST(TaskQueue, DestroyConsumer)
{
    {
        TaskQueue<> queue;
        auto consumer = [&queue]() {
            queue.Pop();
        };

        std::thread consumerThread(consumer);
        consumerThread.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
