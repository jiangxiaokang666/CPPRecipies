#pragma once
#include "../util/nocopyable.h"

#include <mutex>
#include <condition_variable>

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int32_t count);
    ~CountDownLatch();

    void Wait();

    void CountDown();

    int getCount() const;

private:
    mutable std::mutex m_mut;
    std::condition_variable m_cv;
    int32_t m_count;
};