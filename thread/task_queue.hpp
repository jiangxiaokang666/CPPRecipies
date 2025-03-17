#pragma once
#include <deque>
#include <mutex>

/*
    thread safe queue of type T
*/

template <typename T>
class TaskQueue
{
public:
    TaskQueue() = default;
    TaskQueue(const TaskQueue& other) = delete; //delete copy construct

    void push_back(const T& v){
        std::lock_guard lock(m_mut);
        m_deque.emplace_back(v);
    }

    void push_back(T&& v){
        std::lock_guard lock(m_mut);
        m_deque.emplace_back(std::move(v));
    }

    void push_front(const T& v){
        std::lock_guard lock(m_mut);
        m_deque.emplace_front(v);
    }

    void push_front(T&& v){
        std::lock_guard lock(m_mut);
        m_deque.emplace_front(std::move(v));
    }

    bool try_pop(T& tmp){
        std::lock_guard lock(m_mut);
        if(m_deque.empty()){
            return false;
        }
        tmp = std::move(m_deque.front());
        m_deque.pop_front();
        return true;
    }
    
    size_t  size(){
        std::lock_guard lock(m_mut);
        return m_deque.size();
    }

private:
    std::mutex m_mut;
    std::deque<T> m_deque;
};