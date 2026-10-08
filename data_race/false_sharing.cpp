#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <thread>
#include <vector>

constexpr std::size_t kThreads = 4;
constexpr std::uint64_t kIterations = 20'000'000;

struct CompactCounter
{
	std::atomic<std::uint64_t> value{ 0 };
};

struct alignas(64) PaddedCounter
{
	std::atomic<std::uint64_t> value{ 0 };
};

static_assert(alignof(PaddedCounter) >= 64);
static_assert(sizeof(PaddedCounter) >= 64);

template <typename Counter>
void run_case(const char* name)
{
	std::vector<Counter> counters(kThreads);
	std::atomic<bool> start{ false };
	std::vector<std::thread> workers;
	workers.reserve(kThreads);

	for (std::size_t i = 0; i < kThreads; ++i)
	{
		workers.emplace_back([&, i] 
			{
			while (!start.load(std::memory_order_acquire)) {
				std::this_thread::yield();
			}
			for (std::uint64_t n = 0; n < kIterations; ++n) {
				counters[i].value.fetch_add(1, std::memory_order_relaxed);
			}
			});
	}

	const auto begin = std::chrono::steady_clock::now();
	start.store(true, std::memory_order_release);
	for (auto& worker : workers) 
	{
		worker.join();
	}
	const auto end = std::chrono::steady_clock::now();

	std::uint64_t total = 0;
	for (const auto& counter : counters) 
	{
		total += counter.value.load(std::memory_order_relaxed);
	}
	const auto ms = std::chrono::duration<double, std::milli>(end - begin).count();
	std::cout << name << ": " << ms << " ms; total=" << total << '\n';
	if (total != kThreads * kIterations) 
	{
		std::cerr << "unexpected total\n";
		std::terminate();
	}
}

int main() 
{
	std::cout << "sizeof CompactCounter=" << sizeof(CompactCounter)
		<< ", PaddedCounter=" << sizeof(PaddedCounter) << '\n';
	for (int repeat = 0; repeat < 5; ++repeat) 
	{
		run_case<CompactCounter>("compact");
		run_case<PaddedCounter>("padded ");
	}
}