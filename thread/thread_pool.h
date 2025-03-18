#pragma once
#include "../util/nocopyable.h"

#include <functional>
#include <deque>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool : noncopyable
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const std::string& name_args = std::string("ThreadPool"));
    ~ThreadPool();

    void SetMaxQueueSize(size_t max_size);

    void Start(size_t num_threads);

    void Stop();

    const std::string& Name() const;

    size_t QueueSize() const;

    void Run(Task f);//pass by value and move

private:
    bool IsFull();//should run after m_mut locked
    void RunInThread();
    Task Take();


    std::string m_name;
    bool m_running = false;
    std::vector<std::thread> m_threads;
    mutable std::mutex m_mut;//access by const function
    std::condition_variable m_not_full_cv;//用于子线程通知任务队列未满信号，添加任务处理
    std::condition_variable m_not_empty_cv;//用于主线程通知任务非空信号，子线程执行
    std::deque<Task> m_tasks;
    size_t m_max_task_size;

};