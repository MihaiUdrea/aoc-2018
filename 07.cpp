#include "pch.h"
#include "utils.h"

#define AVAIL_ELF 5
#define TASK_LENGTH_BASE 60

vector<pair<char, char>> gPoints;
set<char> gAllChars;


map<char, set<char>> outArrows;
map<char, set<char>> inArrows;

auto Parse(vector<string> aLines)
{
	vector<pair<char, char>> list;
	const regex rx(R"(Step (.) must be finished before step (.) can begin.)");

	for (auto crLine : aLines)
	{
		smatch match;
		regex_match(crLine, match, rx);
		list.emplace_back(match[1].str()[0], match[2].str()[0]);
		gAllChars.insert(list.back().first);
		gAllChars.insert(list.back().second);
		outArrows[list.back().first].insert(list.back().second);
		inArrows[list.back().second].insert(list.back().first);
	}

	return list;
}

int WorkNeeded(int Ch)
{
	return Ch - 'A' + TASK_LENGTH_BASE + 1;
}

template <typename Ret, typename OpT>
vector<char> gCharsList;
int main()
{
	Parse(GetLinesFromFile());
	set<char> waiting = gAllChars;

	/* S1 starts* /
	cout << "S1: ";
	for(auto next = waiting.end();!waiting.empty(); waiting.erase(*next))
	{
		next = min_element(waiting.begin(), waiting.end(), [](auto l, auto r) {
			return Compare(inArrows[l].size(),inArrows[r].size(),
				true, l < r,false);
		});
		cout << *next;

		for (auto waitingTask : outArrows[*next])
			inArrows[waitingTask].erase(*next);
	}
	cout << endl;
	/*S1 ends*/

	int ticks = 0;
	vector<pair<char, char>> tasksQue;
	for (; !waiting.empty() || !tasksQue.empty();)
	{
		// add tasks to que
		vector<char> readyToStart;
		copy_if(waiting.begin(), waiting.end(), back_inserter(readyToStart), [&](auto el) {
			return inArrows[el].empty(); });

		sort(readyToStart.begin(), readyToStart.end());

		transform(readyToStart.begin(), readyToStart.end(), back_inserter(tasksQue), [&](auto el) {
			waiting.erase(el);//remove them from waiting list
			return make_pair(el, el);
		});

		// perform work
		auto workingElfsCount = min(tasksQue.size(), AVAIL_ELF);
		auto idleElfsCount = AVAIL_ELF - workingElfsCount;

		// next task to finish
		auto nextTaskToComplete = min_element(tasksQue.begin(), tasksQue.begin() + workingElfsCount, [](auto l, auto r) {
			return l.first < r.first;
		});

		// count ticks
		auto steps = WorkNeeded(nextTaskToComplete->first);
		ticks += steps;

		// decrease in progress tasks remaining length
		for (auto idx : irange(0, workingElfsCount))
		{
			// cout << tasksQue[idx].second;
			auto & el = tasksQue[idx];
			el.first -= steps;

			if (WorkNeeded(el.first) == 0)
				for (auto waitingEl : outArrows[el.second])
					inArrows[waitingEl].erase(el.second);
		}

		// remove done tasks
		tasksQue.erase(remove_if(tasksQue.begin(), tasksQue.end(), [&](auto el) {
			return WorkNeeded(el.first) == 0; }), tasksQue.end());
	}

	cout << "S2: " << ticks << endl;
}


