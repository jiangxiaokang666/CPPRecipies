#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>

class ThreadPoolWithPackgedTask
{
public:
    ThreadPoolWithPackgedTask();
    ~ThreadPoolWithPackgedTask();

    template<typename F,typename... Args>
    auto run(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>;

    void start(size_t numThreads);

    void stop();

private:
    ThreadPoolWithPackgedTask(const ThreadPoolWithPackgedTask &) = delete;
    ThreadPoolWithPackgedTask& operator= (const ThreadPoolWithPackgedTask &) = delete;

    void runInThread();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;

    std::mutex m_mut;
    std::condition_variable m_cv;
    bool m_stop = false;
};

template <typename F, typename... Args>
auto ThreadPoolWithPackgedTask::run(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;//加上typename提示编译器，是一个类型 c++ 11

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f),std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_mut);
        if(m_stop){
            throw std::runtime_error("run on stoped ThreadPool");
        }
        m_tasks.emplace([task](){(*task)();});
    }
    m_cv.notify_one();
    return res;
}
