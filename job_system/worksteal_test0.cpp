#include <mutex>
#include <cstddef>
#include <deque>
#include <vector>
#include <memory>
#include <thread>
#include <cassert>
#include <iostream>

class WorkQueue
{
private:
	std::mutex mutex_;
	std::deque<std::size_t> jobs_;

public:
	void Push(std::size_t job)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		jobs_.push_back(job);
	}

	bool Pop(std::size_t& job)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (jobs_.empty())
		{
			return false;
		}
		job = jobs_.back();
		jobs_.pop_back();//LIFO
		return true;
	}

	bool Steal(std::size_t& job)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (jobs_.empty())
		{
			return false;
		}
		job = jobs_.front();
		jobs_.pop_front();
		return true;
	}
};

int main()
{
	constexpr std::size_t N = 20000;
	constexpr std::size_t W = 4;
	std::vector<std::unique_ptr<WorkQueue>> queues;
	for (std::size_t i = 0; i < W; ++i)
	{
		queues.push_back(std::make_unique<WorkQueue>());
	}
	//先给第一个queue添加所有任务
	for (std::size_t i = 0; i < N; ++i)
	{
		queues[0]->Push(i);
	}
	/*
	* 初始化统计数据
	*/
	std::vector<std::atomic<unsigned>> executed(N);
	for (auto& count : executed)
	{
		count.store(0);
	}
	std::vector<std::uint64_t> output(N);
	std::vector<std::size_t> local(W, 0), stolen(W, 0);//统计每个工作线程，是自己的还是steal来的
	/*
	* 启动线程池执行所有任务，每个线程对应一个任务队列
	* 其他线程需要从第一个线程获取任务来执行
	*/
	auto worker = [&](std::size_t i)
	{
			while (true)
			{
				std::size_t job = 0;
				bool found = queues[i]->Pop(job);
				if (found)
				{
					++local[i];
				}
				else
				{
					for (std::size_t offset = 1; offset < W; ++offset)
					{
						std::size_t j = (i + offset) % W;
						if (queues[j]->Steal(job))
						{
							found = true;
							++stolen[i];
							break;
						}
					}
				}
				if (!found)
				{
					break;
				}
				auto x = static_cast<std::uint64_t>(job + 1);
				for (int k = 0; k < 1000; ++k)
				{
					x = x * 1664525u + 1013904223u;
				}
				output[job] = x;
				executed[job].fetch_add(1, std::memory_order_relaxed);
			};
	};

	std::vector<std::thread> threads;
	try 
	{
		for (std::size_t i = 0; i < W; ++i)
		{
			threads.emplace_back(worker, i);
		}
	}
	catch (...)
	{
		for (auto& t : threads)
		{
			t.join();
		}
		throw;
	}
	for (auto& t : threads)
	{
		t.join();
	}
	//check result
	std::uint64_t checksum = 0;
	for (std::size_t i = 0; i < N; ++i)
	{
		assert(executed[i].load() == 1);
		checksum += output[i];
	}
	for (std::size_t i = 0; i < W; ++i)
	{
		std::cout << "worker = " << i << " local = " << local[i] << " stolen = " << stolen[i] << "\n";
	}

	std::cout << "checksum = " << checksum << "\n";
	system("pause");
	return 0;
}