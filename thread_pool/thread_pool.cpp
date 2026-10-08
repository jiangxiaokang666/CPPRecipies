#include "./thread_pool.h"

ThreadPool::ThreadPool(std::size_t count)
{
	if (count == 0)
	{
		throw std::invalid_argument("threadcount must be positive");
	}

	workers_.reserve(count);
	try
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			workers_.emplace_back([this] {
				wokerLoop();
			});
		}
	}
	catch(...)
	{
		stopAndJoin();
		throw;
	}
}


ThreadPool::~ThreadPool()
{
	stopAndJoin();
}

std::future<void> ThreadPool::submit(std::function<void()> task)
{
	auto packaged = std::make_shared<std::packaged_task<void()>>(std::move(task));
	auto result = packaged->get_future();
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (stopping_)
		{
			throw std::runtime_error("ThreadPool is stopping");
		}
		tasks_.push([packaged] {
			(*packaged)();
		});
	}
	cv_.notify_one();
	return result;
}

void ThreadPool::wokerLoop()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this]() {
				return stopping_ || !tasks_.empty();
			});
			if (stopping_ && tasks_.empty())
			{
				return;
			}
			task = std::move(tasks_.front());
			tasks_.pop();
		}
		task();
	}
}

void ThreadPool::stopAndJoin()
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		stopping_ = true;
	}
	cv_.notify_all();
	for (auto& worker : workers_)
	{
		if (worker.joinable())
		{
			worker.join();
		}
	}
}

