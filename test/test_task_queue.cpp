#include "../thread/task_queue.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <chrono>


struct Element
{
    int a = 0;
};

TaskQueue<Element> g_task;

std::deque<Element> g_no_safe_deque;

void AddTask()
{
    int count = 0;
    while(count<100){
        g_task.push_back({count++});
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

void PopTask()
{
    int count = 0;
    while(count<100){
        Element e;
        if(g_task.try_pop(e)){
            printf("pop task %d\n",e.a);
        }
        else{
            printf("pop task error %d\n",std::this_thread::get_id());
        }
        count++;
    }
}


void AddTask1()
{
    int count = 0;
    while(count<100){
        g_no_safe_deque.push_back({count++});
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

void PopTask1()
{
    int count = 0;
    while(count<100){
        if(!g_no_safe_deque.empty()){
            Element e = g_no_safe_deque.front();
            g_no_safe_deque.pop_front();
            printf("pop task %d\n",e.a);
        }
        else{
            printf("pop task error %d\n",std::this_thread::get_id());
        }
     
        count++;
    }
}

int main()
{

    std::thread add_thread(AddTask);
    std::thread pop_thread(PopTask);

    add_thread.join();
    pop_thread.join();

    return 0;
}