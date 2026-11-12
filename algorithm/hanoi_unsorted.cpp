#include <vector>
#include <array>
#include <iostream>
#include <map>
#include <queue>
#include <algorithm>
#include <string>

// 包含3个柱子的状态
using State = std::array<std::vector<int>, 3>;

struct Move
{
	int id = 0;
	int from = 0;
	int to = 0;

	void PrintMove() const
	{
		std::cout << "\n Step move"
				  << id << " " << from << " ->" << to << "\n";
	}
};

struct Trace
{
	State previous;
	Move move;
};

std::string center(const std::string& text, std::size_t width, char fill = ' ') {
	const std::size_t padding = width - text.size();
	return std::string(padding / 2, fill) + text
		+ std::string(padding - padding / 2, fill);
}

void PrintState(const State& state, const std::vector<int>& largestFirst)
{
	std::size_t labelWidth = 1;
	for (int disk : largestFirst) {
		labelWidth = std::max(labelWidth, std::to_string(disk).size());
	}
	labelWidth |= 1;
	const std::size_t columnWidth = labelWidth + 2 * largestFirst.size();

	for (std::size_t level = largestFirst.size(); level > 0; --level) {
		std::string row;
		for (const auto& disks : state) {
			if (!row.empty()) {
				row += "  ";
			}
			std::string shape = "|";
			if (level <= disks.size()) {
				const int disk = disks[level - 1];
				const auto position = std::find(largestFirst.begin(), largestFirst.end(), disk);
				const auto rank = static_cast<std::size_t>(largestFirst.end() - position);
				shape = center(std::to_string(disk), labelWidth + 2 * rank, '=');
			}
			row += center(shape, columnWidth);
		}
		row.erase(row.find_last_not_of(' ') + 1);
		std::cout << row << '\n';
	}

	const std::string base(columnWidth, '-');
	std::cout << base << "  " << base << "  " << base << '\n';
	const std::string labels = center("A", columnWidth) + "  "
		+ center("B", columnWidth) + "  " + std::string(columnWidth / 2, ' ') + "C";
	std::cout << labels << '\n';
}


void PrintMoves(const State& initState, const State& goal, const std::vector<Move>& moves)
{
	State state = initState;
	std::cout << "Total step : " << moves.size() << "\n";
	std::cout << "Initial:\n";
	PrintState(state, goal[2]);
	for (std::size_t i = 0; i < moves.size(); ++i)
	{
		const Move& cur = moves[i];
		state[cur.from].pop_back();
		state[cur.to].push_back(cur.id);
		cur.PrintMove();
		PrintState(state, goal[2]);
	}
}

int main()
{
	int n;
	if (!(std::cin >> n) || n < 0 || n>8)
	{
		return 0;
	}
	State initial;//默认放入第一个柱子
	for (int i = 0; i < n; ++i)
	{
		int id = 0;
		if (!(std::cin >> id))
		{
			return 0;
		}
		initial[0].push_back(id);
	}
	//目标是按顺序放入第三个柱子
	State goal;
	goal[2] = initial[0];
	std::sort(goal[2].begin(), goal[2].end(), std::greater<int>{});
	
	std::queue<State> pending;
	pending.push(initial);
	std::map<State, Trace> visited;//去重+还原记录的查询表
	// BFS
	while (!pending.empty())
	{
		const State current = pending.front();
		pending.pop();
		if (current == goal)
		{
			std::vector<Move> moves;
			State state = goal;
			while (state != initial)
			{
				const Trace& trace = visited[state];
				moves.push_back(trace.move);
				state = trace.previous;
			}
			std::reverse(moves.begin(), moves.end());
			PrintMoves(initial, goal, moves);
			return 0;
		}
		for (int from = 0; from < 3; ++from)
		{
			if (current[from].empty())
			{
				continue;
			}
			const int id = current[from].back();
			for (int to = 0; to < 3; ++to)
			{
				if (from == to)
				{
					continue;
				}

				if (!current[to].empty() && current[to].back() <= id)
				{
					continue;
				}

				State next = current;
				next[from].pop_back();
				next[to].push_back(id);
				if (visited.emplace(next, Trace{ current,{id, from, to} }).second)
				{
					pending.push(next);
				}
			}
		}
	}
	std::cout << "-1\n";
}