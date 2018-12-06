#include "pch.h"
#include "utils.h"

string gInput;
vector<string> gLines;

using Point = pair<int, int>;
vector<Point> gPoints;
Point limits;

set<int> infiniteAreas;
vector<int> gCountList;

auto Parse(vector<string> aLines)
{
	vector<Point> list;
	const regex rx(R"((\d+), (\d+))");

	for (auto crLine : aLines)
	{
		smatch match;
		regex_match(crLine, match, rx);
		list.emplace_back( atoi(match[1].str().c_str()), atoi(match[2].str().c_str()));
	}

	return list;
}

int operator-(const Point &a, const Point &b)
{
	return abs(a.first - b.first) + abs(a.second - b.second);
}

auto FindNear(Point aPt)
{
	auto maxDist = limits - make_pair(0, 0);
	auto res = accumulate(gPoints.begin(), gPoints.end(), make_tuple(maxDist, maxDist, -1), [&](auto minDist, Point & r) {
		auto rDist = r - aPt;

		if (rDist <= get<0>(minDist))
		{
			auto ret = make_tuple(rDist, get<0>(minDist), &r - &gPoints.front());
			return ret;
		}
		else
			return minDist;
	});

	if (get<0>(res) == get<1>(res))
		return -1;
	else
		return get<2>(res);
}

auto IsNear(Point aP)
{
	auto res = accumulate(gPoints.begin(), gPoints.end(), 0, [&](auto &l, auto & el) {
		return l + (el - aP);
	});

	return res < 10000;
}

int main()
{
	gPoints = Parse(GetLinesFromFile());
	gCountList.resize(gPoints.size());

	limits = accumulate(gPoints.begin(), gPoints.end(), make_pair(0, 0), [](auto l, auto r) {
		return make_pair(max(l.first, r.first), max(l.second, r.second));
	});

	// s1 
	for (auto i : irange(0, limits.first + 1))
		for (auto j : irange(0, limits.second + 1))
			if (auto neareastPt = FindNear(make_pair(i, j)); neareastPt >= 0)
			{
				gCountList[neareastPt]++;

				if (i == 0 || j == 0 || i == limits.first || j == limits.second)
					infiniteAreas.insert(neareastPt);
			}

	auto res = max_element(gCountList.begin(), gCountList.end(), [&](auto & l, auto & r) {
		return l < r && infiniteAreas.find(&r - &gCountList.front()) == infiniteAreas.end();
	});
	
	cout << "S1: " << *res << endl;

	int count = 0;
	for (auto i : irange(0, limits.first + 1))
		for (auto j : irange(0, limits.second + 1))
			if (IsNear(make_pair(i, j)))
				count++;

	cout << "S2: " << count << endl;
}

