#include "ThreadPoolWithPackagedTask.hpp"

ThreadPoolWithPackgedTask::ThreadPoolWithPackgedTask()
{
    m_stop = true;
}

ThreadPoolWithPackgedTask::~ThreadPoolWithPackgedTask()
{
    if(!m_stop){
        stop();
    }
}

void ThreadPoolWithPackgedTask::start(size_t numThreads)
{
    m_stop = false;
    m_workers.reserve(numThreads);
    for(size_t i = 0; i< numThreads; ++i){
        m_workers.emplace_back(std::bind(&ThreadPoolWithPackgedTask::runInThread,this));
    }
}

void ThreadPoolWithPackgedTask::stop()
{
    {
        std::lock_guard<std::mutex> lg(m_mut);
        m_stop = true;
        m_cv.notify_all();
    }
    for(auto& t:m_workers){
        t.join();
    }
}

void ThreadPoolWithPackgedTask::runInThread()
{
    while(!m_stop){
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mut);
            m_cv.wait(lock,[this]{return this->m_stop || !this->m_tasks.empty();});
            if(!m_tasks.empty()){
                task = m_tasks.front();
                m_tasks.pop();
            }
        }
        if(task){
            task();
        }
    }
}
