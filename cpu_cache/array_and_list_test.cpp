#include <list>
#include <vector>
#include <chrono>
#include <array>
#include <iostream>

const int N = 10000000;

void testArray()
{
    //栈溢出
    //std::array<int, N> array;
    int* array = new int[N];
    for (int i = 0; i < N; ++i)
    {
        array[i] = i;
    }
    auto start = std::chrono::high_resolution_clock::now();
    long long sum = 0;
    for (int i = 0; i < N; ++i)
    {
        sum += array[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "array: " << std::chrono::duration<double, std::milli>(end - start).count() 
              << " ms, sum:" << sum <<"\n";
    delete[] array;            
}

void testList()
{
    std::list<int> l;
    for (std::size_t i = 0; i < N; ++i)
    {
        l.push_back(i);
    }
    auto start = std::chrono::high_resolution_clock::now();
    long long sum = 0;
    for (auto x : l)
    {
        sum += x;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "list: " << std::chrono::duration<double, std::milli>(end - start).count()       
              << " ms, sum:" << sum <<"\n";  
}

int main()
{
    testArray();
    testList();
    /*
    output example:
    array: 1.2921 ms, sum:49999995000000
    list: 97.6052 ms, sum:49999995000000
    */
    return 0;
}