#include "../thread/thread_pool.h"
#include "../thread/countdown_latch.h"

#include <stdio.h>
#include <chrono>
#include <atomic>

void Print()
{
    printf("tid = %ld\n",std::this_thread::get_id());
}

void printString(const std::string& str)
{
    printf("%s\n",str.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Test(size_t max_size)
{
    printf("Test thread pool max_size = %d\n",max_size);
    ThreadPool pool;
    pool.SetMaxQueueSize(max_size);
    pool.Start(5);
    pool.Run(Print);
    pool.Run(Print);
    for(size_t i = 0 ; i< 100; ++i)
    {
        std::string str = "task " + std::to_string(i);
        pool.Run(std::bind(printString,str)); 
    }
    printf("Done add task.\n");
    CountDownLatch count_down(1);
    pool.Run(std::bind(&CountDownLatch::CountDown,&count_down));
    pool.Stop();
}
std::atomic_int g_count;

void LongFunc()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    printf("execute long func %dd %d\n",std::this_thread::get_id(),g_count++);
}

void Test2()
{
    g_count = 0;
    printf("Test thread pool early stop\n");
    ThreadPool pool;
    pool.SetMaxQueueSize(5);
    pool.Start(5);
    std::thread thread0([&pool]()
    {
        for(size_t  i = 0 ; i<20;++i)
        {
            pool.Run(std::bind(LongFunc));
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pool.Stop();
    thread0.join();
    pool.Run(Print);
    printf("Test 2 done.\n");
}

int main()
{
    // Test(0);
    // Test(1);
    // Test(5);
    // Test(20);
    // Test(50);
    Test2();
    return 0;
}