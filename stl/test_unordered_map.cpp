#include <unordered_map>
#include <iostream>

int main()
{
	std::unordered_map<int, int>  m;
	m.reserve(10);
	for (int i = 0; i < 10; ++i)
	{
		m.emplace(i, i * 10);
		std::cout << "size = " << m.size()
			<< " buckets = " << m.bucket_count()
			<< " load = " << m.load_factor()
			<< "\n";
	}
	for (int i = 0; i < m.bucket_count(); ++i)
	{
		if (m.bucket_size(i) != 0)
		{
			std::cout << "bucket " << i
				<< ": " << m.bucket_size(i)
				<< " elements\n";
		}
	}

	return 0;
}