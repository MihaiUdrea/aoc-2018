#include "pch.h"
#include "utils.h"

auto Reduce(string & line)
{
  for(;;)
  {
    for (auto i : irange(0, line.size() - 1) )
      if (abs(line[i] - line[i + 1]) == 'a' - 'A')
        line[++i] = line[i] = ' ';
    
    if (auto where = remove(line.begin(), line.end(), ' '); where == line.end())
      return line.length();
    else
      line.erase(where, line.end());
  }
}

auto MinSize(vector<string> &gLines)
{
  vector<int> newSizes;
  for (auto ch : irange('a', 'z' + 1))
  {
    auto line = gLines.front();
    line.erase(remove_if(line.begin(), line.end(), [ch](char el) { return tolower(el) == ch; }), line.end());
    newSizes.push_back(Reduce(line));
  };
  return *min_element(newSizes.begin(), newSizes.end());
}

int main()
{
  auto gLines = GetLinesFromFile();
  cout << "S1 : " << Reduce(gLines.front()) << endl;
  cout << "S2 : " << MinSize(gLines) << endl;
}
