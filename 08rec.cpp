#include "pch.h"
#include "utils.h"

auto SplitLineInColumns(string & aLine)
{
	vector<int> res;

	for (sregex_token_iterator iter(aLine.begin(), aLine.end(), colSepRx, -1), end; iter != end; ++iter)
	{
		res.push_back(atoi(iter->str().c_str()));
	}
	return res;
}

int pos = 0;
int stageOne = true;
vector<int> gArray;
int RecParse()
{
	int childCount = gArray[pos++];
	int metaCount = gArray[pos++];
	vector<int> childResult(childCount);
	for (int i = 0; i < childCount; i++)
		childResult[i] = RecParse();
	int res = 0;
	for (int i = 0; i < metaCount; i++)
	{
		int metaData = gArray[pos++];
		if (childCount == 0 || stageOne)
			res += metaData;
		else
			res += metaData <= childCount ? childResult[metaData - 1] : 0;
	}
	return res;
}

int main()
{
	auto lines = GetLinesFromFile();	
	
	gArray = SplitLineInColumns(lines.front());
	
	cout << "S1: " << RecParse() << endl;
	stageOne = false;
	pos = 0;
	cout << "S2: " << RecParse() << endl;
}


