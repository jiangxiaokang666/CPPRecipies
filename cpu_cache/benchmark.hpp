#pragma once

#include <chrono>

template<typename Func>
double BenchMark(Func&& func, int iterations)
{
    using Clock = std::chrono::steady_clock;
    auto begin = Clock::now();
    for(int i = 0; i < iterations ; ++i)
    {
        func();
    }
    auto end = Clock::now();
    return std::chrono::duration<double, std::milli>(end - begin).count();
}