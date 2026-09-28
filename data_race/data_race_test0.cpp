#include <iostream>
#include <thread>
#include <atomic>

int counter = 0;

void add()
{
	for (int i = 0; i < 100'00; ++i)
	{
		++counter;
	}
}

std::atomic<int> counter1{ 0 };

void add1()
{
	for (int i = 0; i < 100'00; ++i)
	{
		++counter1;
	}
}

int main()
{
	std::thread a(add);
	std::thread b(add);
	std::thread c(add1);
	std::thread d(add1);

	a.join();
	b.join();
	c.join();
	d.join();
	std::cout << counter << "\n" << counter1 <<"\n";
}

