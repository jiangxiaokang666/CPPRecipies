#include "thread_pool.hpp"

#include <iostream>
#include <cassert>

int main()
{
	ThreadPool pool(3);
	auto value = pool.Submit([](int a, int b) { return a + b; }, 20, 22);
	auto nothing = pool.Submit([] {});
	auto moved = pool.Submit([](std::unique_ptr<int> p) { return *p; },
		std::make_unique<int>(7));
	auto error = pool.Submit([]() -> int {
		throw std::runtime_error("task failed");
		});
	assert(value.get() == 42);
	nothing.get();
	assert(moved.get() == 7);
	try { (void)error.get(); assert(false); }
	catch (const std::runtime_error& e) { assert(std::string(e.what()) == "task failed"); }
	pool.Shutdown();
	try { pool.Submit([] {}); assert(false); }
	catch (const std::runtime_error&) {}
	std::cout << "value / void / move-only / exception / shutdown: passed\n";
	return 0;
}