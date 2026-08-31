#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <intrin.h>
#include <cstdint>

uint64_t ReadTSCStart()
{
    int cpuInfo[4];

    __cpuid(cpuInfo, 0);

    return __rdtsc();
}

uint64_t ReadTSCEnd()
{
    unsigned int aux;

    uint64_t t = __rdtscp(&aux);

    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    return t;
}

double MeasureLatency(size_t count)
{
    std::vector<int> data(count);
    std::vector<int> permutation(count);

    for (size_t i = 0; i < count; ++i)
    {
        permutation[i] = static_cast<int>(i);
    }

    std::mt19937 rng(12345);

    std::shuffle(
        permutation.begin(),
        permutation.end(),
        rng);

    for (size_t i = 0; i < count; ++i)
    {
        int current = permutation[i];
        int next =
            permutation[(i + 1) % count];

        data[current] = next;
    }

    int index = permutation[0];

    // warmup
    for (size_t i = 0; i < count; ++i)
    {
        index = data[index];
    }

    constexpr int iterations = 1'000'000;

    uint64_t start = ReadTSCStart();

    for (int i = 0; i < iterations; ++i)
    {
        index = data[index];
    }

    uint64_t end = ReadTSCEnd();

    // 防止编译器认为 index 最终没有意义
    volatile int sink = index;
    (void)sink;

    return
        static_cast<double>(end - start)
        / iterations;
}

int main()
{
    for (size_t sizeKB :
        { 4, 8, 16, 32, 64, 128, 256,
          512, 1024, 2048, 4096,
          8192, 16384, 32768, 65536 })
    {
        size_t count =
            sizeKB * 1024 / sizeof(int);

        double cycles =
            MeasureLatency(count);

        std::cout
            << sizeKB
            << " KB : "
            << cycles
            << " cycles/access\n";
    }
}