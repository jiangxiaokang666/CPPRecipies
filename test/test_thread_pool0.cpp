#include "../thread/thread_pool.h"
#include "../thread/countdown_latch.h"

#include <stdio.h>
#include <chrono>

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

int main()
{
    Test(0);
    Test(1);
    Test(5);
    Test(20);
    Test(50);
    return 0;
}