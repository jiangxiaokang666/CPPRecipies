#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:
	explicit ThreadPool(std::size_t count);

	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	std::future<void> submit(std::function<void()> task);

private:
	void wokerLoop();

	void stopAndJoin();

	std::vector<std::thread> workers_;
	std::queue<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::condition_variable cv_;
	bool stopping_ = false;
};