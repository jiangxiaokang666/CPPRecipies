#pragma once

#include <stdexcept>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>
#include <memory>

class ThreadPool
{
public:
	enum class StopMode
	{
		Drain,
		CancelPending,
	};

	explicit ThreadPool(std::size_t count)
	{
		if (count == 0)
		{
			throw std::invalid_argument("worker count must be positive");
		}
		try
		{
			workers_.reserve(count);
			for (std::size_t i = 0; i < count; ++i)
			{
				workers_.emplace_back([this]() { WorkerLoop(); });
			}
		}
		catch (...)
		{
			Shutdown();
			throw;
		}
	}

	~ThreadPool()
	{
		Shutdown();
	}

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	template<class F, class... Args>
	auto Submit(F&& f, Args&&... args)
		-> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
	{
		using R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>... >;
		auto bound = [fn = std::make_shared<std::decay_t<F>>(std::forward<F>(f)),
					values = std::make_shared<std::tuple<std::decay_t<Args>...>>(std::make_tuple(std::forward<Args>(args)...))]
					() mutable -> R{
				return std::apply([&fn](auto&&... xs) -> R {
					return std::invoke(std::move(*fn), std::forward<decltype(xs)>(xs)...);
				}, std::move(*values));
			};
		auto packaged = std::make_shared<std::packaged_task<R()>>(std::move(bound));
		auto result = packaged->get_future();
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (closed_)
			{
				throw std::runtime_error("thread pool is closed");
			}
			tasks_.emplace([packaged] { (*packaged)(); });
		}
		cv_.notify_one();
		return result;
	}

	void Shutdown(StopMode mode = StopMode::Drain)
	{
		if (currentPool_ == this)
		{
			throw std::logic_error("shutdown must be called outsite this pool");
		}

		std::lock_guard<std::mutex> joinLock(joinMutex_);
		std::queue<std::function<void()>> discarded;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (!closed_)
			{
				closed_ = true;
				if (mode == StopMode::CancelPending)
				{
					tasks_.swap(discarded);
				}
			}
		}
		cv_.notify_all();
		while (!discarded.empty())
		{
			discarded.pop();
		}
		for (auto& worker : workers_)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}

private:
	void WorkerLoop()
	{
		currentPool_ = this;
		while (true)
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				cv_.wait(lock, [this]() {
					return closed_ || !tasks_.empty();
					});
				if (tasks_.empty())
				{
					break;
				}
				task = std::move(tasks_.front());
				tasks_.pop();
			}
			task();
		};
		currentPool_ = nullptr;
	}

	inline static thread_local ThreadPool* currentPool_ = nullptr;//处理禁止工作线程中shutdown
	std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<std::function<void()>> tasks_;
	bool closed_ = false;
	std::mutex joinMutex_;
	std::vector<std::thread> workers_;
};