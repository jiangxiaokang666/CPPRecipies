#include "rbtree.hpp"

#include <string>
#include <iostream>

int main()
{
	RBTree<int, std::string> players;

	players.insert(30, "Alice");
	players.insert(10, "Bob");
	players.insert(20, "Carol");

	auto [value, inserted] = players.insert(20, "Duplicate");

	assert(!inserted);
	assert(*value == "Carol");
	assert(players.validate());

	if (auto* name = players.find(10)) {
		*name = "David";
	}

	players.forEach([](const auto& item) {
		std::cout << item.first
			<< ": "
			<< item.second
			<< '\n';
		});

	players.erase(20);

	assert(!players.contains(20));
	assert(players.validate());

	// 自定义比较器：中序遍历结果为 30、20、10。
	RBTree<int, std::string, std::greater<int>> descending;

	descending.insert(10, "A");
	descending.insert(30, "C");
	descending.insert(20, "B");

	descending.forEach([](const auto& item) {
		std::cout << item.first << ' ';
		});
	std::cout << '\n';

	// Value 也可以是只能移动、不可复制的类型。
	RBTree<int, std::unique_ptr<int>> objects;

	objects.insert(1, std::make_unique<int>(42));

	assert(objects.find(1) != nullptr);
	assert(**objects.find(1) == 42);
	assert(objects.validate());
}