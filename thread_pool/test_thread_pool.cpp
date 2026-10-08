#include "./thread_pool.h"

#include <iostream>

int main()
{
	ThreadPool pool(2);

	int value = 0;

	auto first = pool.submit([&value] {
		value = 42;
		});

	auto second = pool.submit([] {
		throw std::runtime_error("resource loading failed");
	});

	// 等待任务完成后再读取 value。
	first.get();
	std::cout << "value = " << value << '\n';

	try {
		second.get();
	}
	catch (const std::exception& e) {
		std::cout << "task exception: " << e.what() << '\n';
	}

	return 0;
}