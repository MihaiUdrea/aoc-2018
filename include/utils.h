#ifndef UTILS_H
#define UTILS_H

const regex colSepRx("\\s");

void ReadFileToString(const wchar_t * aFileName, string & aContent)
{
	ifstream inFile;
	inFile.open(aFileName);
	if (!inFile) {
		cerr << "Unable to open file input.txt";
		exit(1);   // call system to stop
	}

	string & str = aContent;

	inFile.seekg(0, std::ios::end);
	str.reserve((const unsigned int)inFile.tellg());
	inFile.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(inFile)),
		std::istreambuf_iterator<char>());

}

string ToString(int aNr)
{
	char buff[100];
	_itoa_s(aNr, buff, 10);
	return buff;
}

vector<string> GetLines(const string & aLines)
{
  static const regex lineSepRx("\\n");

  vector<string> lines;
  for (sregex_token_iterator iter(aLines.begin(), aLines.end(), lineSepRx, -1), end; iter != end; ++iter)
  {
    lines.push_back(iter->str());
  }

  return lines;
}

vector<string> GetLinesFromFile()
{
  string input;
  ReadFileToString(LR"(D:\aoc\2018\input)", input);

  return GetLines(input);
}


void toClipboard(const std::string &s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);

	cout << endl << s.c_str() << " is in clipboad." << endl;
}

void toClipboard(int nr) {
	toClipboard(ToString(nr));
}

class irange
{
  int aFirst;
  int aLast;
  int aStep;

  class Iter
  {
    int idx;
    int step;
  public:
    Iter(int x, int s) :idx(x), step(s) {}
    Iter(const Iter& mit) : idx(mit.idx), step(mit.step) {}
    Iter& operator++() { idx += step; return *this; }
    Iter operator++(int) { Iter tmp(*this); operator++(); return tmp; }
    bool operator==(const Iter& rhs) const { return step > 0 ? idx >= rhs.idx : idx <= rhs.idx; }
    bool operator!=(const Iter& rhs) const { return step > 0 ? idx < rhs.idx : idx>rhs.idx; }
    int& operator*() { return idx; }
  };

public:

  irange(int aFirst, int aLast, int pStep = 1) : aFirst(aFirst), aLast(aLast), aStep(pStep)
  {
    assert(aStep > 0);

    if (aFirst > aLast)
      aStep = -aStep;
  }

  Iter begin()
  {
    return Iter(aFirst, aStep);
  }

  Iter end()
  {
    return Iter(aLast, aStep);
  }
};

#endif // UTILS_H
