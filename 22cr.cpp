#include "pch.h"
#include "utils.h"

//#define FILE_OUT_MODE ofstream::trunc
#define FILE_OUT_MODE  ofstream::/**/trunc/** /app/**/

//FILE_PRINT_LEVELS  0 -no file printing; 1 - only Print1(true) 2 Any Printf; 3 - Any PrintF and Any PrintLineF
namespace PrintLevels {
	enum { none, onlyExtraPrintF, anyPrintf, all };
}

#ifdef _DEBUG
#define OUT_FILE LR"(outdebug.txt)"
#define TEST true
#define RUNONE false
#define RUNTWO false
#define FILE_PRINT_LEVEL PrintLevels::all
#else
#define OUT_FILE LR"(outrelease982.txt)"
#define TEST false
#define RUNONE true
#define RUNTWO true
#define FILE_PRINT_LEVEL PrintLevels::onlyExtraPrintF
#endif // DEBUG

#define TEST1 true // ok
#define TEST2 false
#define TEST3 false
#define TEST4 false
#define TEST5 false
#define TEST6 false

enum ToolSet { neither, torch, climbg, last };

using Point = pair<int, int>;

struct MapPointData
{
	bool old{ true };
	int minutes{ MAXINT };
};

using TimeMap = map<pair<Point, ToolSet>, MapPointData>;
using PointToolTime = pair<pair<Point, ToolSet>, int>;

bool operator <(const PointToolTime& _Left, const PointToolTime& _Right)
{
	return _Left.second < _Right.second;
}

struct Solution
{
	enum Pot
	{
		rock = 0,
		wet,
		narrow
	};

	map<Point, Pot> type;

	TimeMap distMap; // pt , data

	priority_queue<PointToolTime, vector<PointToolTime>, greater<>> que;

	vector<vector<int>> geoIndex;
	vector<vector<int>> erosionLevel;


	stringstream mResult;
	vector<string> gLines;
	ofstream outM;

	
	

	int depth;

	int tLine{};
	int tCol{};

	int maxLine{};
	int maxCol{};

	Solution(wstring filePath = LR"(input)")
	{
		//wcout << L"-----------------MATCH " << filePath << L"-------------" << endl;

		string input;
		ReadFileToString(filePath.c_str(), input);

		gLines = GetLines(input);

		outM.open(LR"(outM.txt)", ofstream::app);

		const regex rx(R"(depth: (\d+))");
		smatch match;
		regex_match(gLines.front(), match, rx);

		depth = atoi(match[1].str().c_str());

		const regex rx2(R"(target: (\d+),(\d+))");
		regex_match(gLines[1], match, rx2);
		tLine = atoi(match[2].str().c_str());
		tCol = atoi(match[1].str().c_str());		
	}

	void SetErosionAndType(int l, int c)
	{
		erosionLevel[l][c] = (geoIndex[l][c] + depth) % 20183LL;

		if (erosionLevel[l][c] < 0)
		{
			PrintLineF("Overflow: " + to_string(l) + ", " + to_string(c) + "\n");
		}

		type[{l, c}] = static_cast<Pot>(erosionLevel[l][c] % 3);
	}

	void PrintF(bool aExtra = false, bool aEnd = false)
	{
		if (FILE_PRINT_LEVEL == PrintLevels::none || (FILE_PRINT_LEVEL == PrintLevels::onlyExtraPrintF && !aExtra))
			return;

		int charWidth = 1;
		if (aExtra)
			charWidth = 9;

		int minCol = 0;

		int lastCol = maxCol + 1;
		//lastCol = 35;

		int minLine = 0;

		int lastLine = maxLine + 1;

		ofstream out;
		out.open(OUT_FILE, FILE_OUT_MODE);

		//out << "------------------------------" << path << "------------" << endl;

		// print top/cols count header 
		auto topHeader = [&](int minCol, int maxCol) {
			out << std::setfill(' ') << setw(4) << ' ';
			for (auto col : irange(minCol, maxCol))
			{
				out << setw(charWidth) << col / 100;
			}
			out << endl;
			out << std::setfill(' ') << setw(4) << ' ';
			for (auto col : irange(minCol, maxCol))
			{
				out << setw(charWidth) << (col % 100) / 10;
			}
			out << endl;
			out << std::setfill(' ') << setw(4) << ' ';
			for (auto col : irange(minCol, maxCol))
			{
				out << setw(charWidth) << col % 10;
			}
			out << endl;
		};

		topHeader(minCol, lastCol);

		for (auto l : irange(minLine, lastLine))
		{
			out << left << std::setfill(' ') << setw(4) << l;
			for (auto c : irange(minCol, lastCol))
			{
				//out << setw(charWidth) << type[l][c];
				/**/
				if (l == tLine && c == tCol)
					out << 'X';
				else if (type[{l,c}] == rock) // rock
					out << '.';
				else if (type[{l,c}] == wet) // wet
					out << '=';
				else if (type[{l,c}] == narrow) // narrow
					out << '|';
				
				if (aExtra)
				for (auto tool = ToolSet::neither; tool < ToolSet::last; tool = static_cast<ToolSet>(tool + 1))
				{
					if (AcceptTool({l,c}, tool))
					if (distMap[{{l, c}, tool}].minutes < MAXINT && (!aEnd || distMap[{ {l, c}, tool}].old))
						out << "," << setw(3) << distMap[{ {l, c}, tool}].minutes;
					else
						out << "," << setw(3) << " ";
				}
			}
			out << endl;
		}
		out.close();
	}

	void PrintLineF(string line)
	{
		if (FILE_PRINT_LEVEL < PrintLevels::all)
			return;

		ofstream cout;
		cout.open(OUT_FILE, ofstream::app);

		cout << line;

		cout.close();
	}

	string One()
	{
		Expand(tLine + 1, tCol + 1);

		PrintLineF(gLines.front() + "\n");
		PrintF();

		int sum = 0;
		for (auto l : irange(0, tLine + 1))
			for (auto c : irange(0, tCol + 1))
				sum += type[{l, c}];

		mResult << sum;

		PrintLineF(mResult.str() + "\n");

		return mResult.str();
	}

	bool AcceptTool(Point pt, ToolSet tool)
	{
		auto where = type.find(pt);
		if (where == type.end())
		{
			cout << "Point overflow: " << pt.first << ", " << pt.second;
			exit(1);
		}

		switch (where->second)
		{
		case rock:
			return tool != neither;
		case wet:
			return tool != torch;
		case narrow:
			return tool != climbg;
		}

		return false;
	}


	int GetToolTime(ToolSet a, ToolSet b)
	{
		return a != b ? 7 : 0;
	}

	int GetPosTime(int dl, int dc)
	{
		return  abs(dl) + abs(dc);
	}

	void SetType(int line, int col)
	{
		if (line == tLine && col == tCol)
		{
			geoIndex[tLine][tCol] = 0;
			SetErosionAndType(tLine, tCol);
			return;
		}

		if (line == 0)
		{
			geoIndex[0][col] = col * 16807;
			SetErosionAndType(0, col);
		}
		else if (col == 0)
		{
			geoIndex[line][0] = line * 48271;
			SetErosionAndType(line, 0);
		}
		else
		{
			geoIndex[line][col] = erosionLevel[line - 1][col] * erosionLevel[line][col - 1];
			SetErosionAndType(line, col);
		}
	}

	void Expand(int aLines, int aCols)
	{
		geoIndex.resize(aLines);
		erosionLevel.resize(aLines);

		for (auto line : irange(0, maxLine))
		{
			geoIndex[line].resize(aCols);
			erosionLevel[line].resize(aCols);

			for (auto col : irange(maxCol, aCols))
				SetType(line, col);
		}

		for (auto line : irange(maxLine, aLines))
		{
			geoIndex[line].resize(aCols);
			erosionLevel[line].resize(aCols);

			for (auto col : irange(0, aCols))
				SetType(line, col);
		}


		maxCol = aCols;
		maxLine = aLines;
	}

	string Two()
	{
		mResult.str("");
		//--GO--------------------------------

		distMap[{ { 0, 0 }, torch}] = { 0, false };
		que.push({ { { 0, 0 }, torch }, 0});
		
		for(; distMap[{ {tLine, tCol}, torch }].minutes == MAXINT;)
		{
			auto el = que.top();
			que.pop();

			auto ExpandAround = [&](int dl, int dc) {
				Point pt = {el.first.first.first + dl, el.first.first.second + dc};

				Expand(max(pt.first + 1, maxLine), max(pt.second + 1, maxCol));

				if (pt.first < 0 || pt.second < 0)
					return;

				for (auto tool = ToolSet::neither; tool < ToolSet::last; tool = static_cast<ToolSet>(tool + 1))
				{
					if (tool == el.first.second && dl == 0 && dc == 0)
						continue; // keep tool, don't stand

					if (tool != el.first.second && (dl != 0 || dc != 0))
						continue; // change tool, don't move
						
					if (AcceptTool(pt, tool))
					{
						auto & crData = distMap[{pt, tool}];
						auto newTime = el.second + GetToolTime(tool, el.first.second) + GetPosTime(dl, dc);
						if (crData.minutes > newTime)
						{
							crData.minutes = newTime;
							crData.old = false;

							que.push({ {pt, tool}, newTime });
						}
					}
				}
			};

			array<int, 5> dlList = { 0, 1, 0, -1,  0 };
			array<int, 5> dcList = { 0, 0, 1,  0, -1 };
			for (auto i : irange(0, dcList.size()))
			{
				ExpandAround(dlList[i], dcList[i]);
			}
		}

		PrintF(true);

		// trace back
		PrintBackTrace({ {tLine, tCol}, torch });

		PrintF(true, true);

		mResult << distMap[{ {tLine, tCol}, torch }].minutes;

		return mResult.str();
	}

	void PrintBackTrace(pair<Point, ToolSet> aCrPt)
	{
		auto & el = *distMap.find(aCrPt);

		pair<Point, ToolSet> prevPt = { {-1, -1}, neither };

		auto TraceAround = [&](int dl, int dc) {
			Point pt = { el.first.first.first + dl, el.first.first.second + dc };
			if (pt.first >= maxLine || pt.first < 0 || pt.second >= maxCol || pt.second < 0)
				return false;

			for (auto tool = ToolSet::neither; tool < ToolSet::last; tool = static_cast<ToolSet>(tool + 1))
			{
				if (AcceptTool(pt, tool) && (pt != el.first.first || tool != el.first.second))
				{
					auto & crData = distMap[{pt, tool}];
					auto newTime = el.second.minutes - GetToolTime(tool, el.first.second) - GetPosTime(dl, dc);
					if (crData.minutes == newTime)
					{
						crData.old = true;
						prevPt = { pt, tool };
						return true;
					}
				}
			}

			return false;
		};


		array<int, 5> dlList = { 0, 1, 0, -1,  0 };
		array<int, 5> dcList = { 0, 0, 1,  0, -1};
		for (auto i : irange(0, dcList.size()))
		{
			if (TraceAround(dlList[i], dcList[i]))
				break;
		}
		
		if (prevPt.first.first != -1)
			PrintBackTrace(prevPt);
	}
};

int main()
{
	if (TEST)
	{
		if (TEST1)
		{
			Solution otherSample(LR"(sample)");
			auto res = otherSample.One();
			assert(res == "114");
			res = otherSample.Two();
    		assert(res == "45");
		}

		{
			if (TEST2)
			{
				Solution otherSample(LR"(sample2)");
				auto res = otherSample.One();
				assert(res == "6318");
				//cout << "4 S2: " << res.first << " Score: " << res.second << endl;

			}

			if (TEST3)
			{
				Solution otherSample(LR"(sample3)");
				auto res = otherSample.One();
				assert(res == "10");

			}

			if (TEST4)
			{
				Solution otherSample(LR"(sample4)");
				auto res = otherSample.One();
				assert(res == "64");
			}

			if (0)
			{
				Solution otherSample(LR"(sampleMy1)");
				//assert(otherSample.GetObjectsSize() == 2018);

				auto sample1 = otherSample.One();
				//			assert(sample1 == "13428");
				exit(1);
				//auto otherSample2 = otherSample.Two();
				//assert(otherSample2 == "");
			}
		}
	}


	Solution s;
	//assert(s.GetObjectsSize() == );

	if (RUNONE)
	{
		auto sol1 = s.One();
		cout << "S1: " << sol1 << endl;
		toClipboard(sol1);

	}

	if (RUNTWO)
	{
		auto sol1 = s.Two();
		cout << "S2: " << sol1;
		toClipboard(sol1);
	}
}
