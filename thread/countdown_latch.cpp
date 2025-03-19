#include "countdown_latch.h"

CountDownLatch::CountDownLatch(int32_t count)
    : m_count(count)
{
}

CountDownLatch::~CountDownLatch()
{
}

void CountDownLatch::Wait()
{
    std::unique_lock<std::mutex> lock(m_mut);
    m_cv.wait(lock, [this]()
              { return m_count <= 0; });
}

void CountDownLatch::CountDown()
{
    std::lock_guard<std::mutex> lock(m_mut);
    --m_count;
    if (m_count <= 0)
    {
        m_cv.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::lock_guard<std::mutex> lock(m_mut);
    return m_count;
}
