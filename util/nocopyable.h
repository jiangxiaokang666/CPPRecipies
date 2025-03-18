#pragma once
//learn from boost noncopyable :https://www.boost.org/doc/libs/1_87_0/boost/core/noncopyable.hpp
class noncopyable 
{
protected:
    noncopyable() = default;
    ~noncopyable() = default;

    noncopyable(const noncopyable& other) = delete;
    noncopyable& operator=(const noncopyable& other) = delete;
};