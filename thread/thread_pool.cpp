#include "thread_pool.h"

#include <assert.h>
#include <stdio.h>

ThreadPool::ThreadPool(const std::string &name_args)
    : m_name(name_args),
      m_max_task_size(0),
      m_running(false)
{
}

ThreadPool::~ThreadPool()
{
    if (m_running)
    {
        Stop();
    }
}

void ThreadPool::SetMaxQueueSize(size_t max_size)
{
    m_max_task_size = max_size;
}

void ThreadPool::Start(size_t num_threads)
{
    assert(m_threads.empty());
    m_running = true;
    m_threads.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i)
    {
        m_threads.emplace_back(std::thread(&ThreadPool::RunInThread, this));
    }
}

void ThreadPool::Stop()
{
    {
        std::lock_guard<std::mutex> lg(m_mut);
        m_running = false;
        m_not_empty_cv.notify_all();
        m_not_full_cv.notify_all();
    }
    for (auto &thread : m_threads)
    {
        thread.join();
    }
}

const std::string &ThreadPool::Name() const
{
    return m_name;
}

size_t ThreadPool::QueueSize() const
{
    std::lock_guard<std::mutex> lg(m_mut);
    return m_tasks.size();
}

void ThreadPool::Run(Task f)
{
    if (m_threads.empty())
    {
        f();
    }
    else
    {
        std::unique_lock<std::mutex> lock(m_mut);
        m_not_full_cv.wait(lock, [this]()
                           { return !this->IsFull() || !this->m_running; });
        if (!m_running)
        {
            return;
        }
        m_tasks.push_back(std::move(f)); // move copy
        m_not_empty_cv.notify_one();
    }
}

bool ThreadPool::IsFull()
{
    return m_max_task_size > 0 && m_tasks.size() > 0;//0代表没有上限
}

void ThreadPool::RunInThread()
{
    try
    {
        while (m_running)
        {
            Task task(Take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const std::exception &e)
    {
        fprintf(stderr, "exception caught in thread pool name %s\n", m_name.c_str());
        fprintf(stderr, "reason:%s\n", e.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", m_name.c_str());
        throw;
    }
}

ThreadPool::Task ThreadPool::Take()
{
    std::unique_lock<std::mutex> lock(m_mut);
    m_not_empty_cv.wait(lock, [this]()
                        { return !this->m_tasks.empty() || !this->m_running; });
    Task task;
    if (!m_tasks.empty())
    {
        task = m_tasks.front();
        m_tasks.pop_front();
        if (m_max_task_size > 0)
        {
            m_not_full_cv.notify_one();
        }
    }
    return task;
}
