#include "pch.h"
#include "utils.h"

int maxX = MININT;
int maxY = MININT;

int minX = MAXINT;
int minY = MAXINT;

struct PtAndSpeed
{
  int x;
  int y;
  int dx;
  int dy;
};

auto ParseLines(vector<string> & aLines)
{
  vector<PtAndSpeed> list;
  const regex rx(R"(position=<([^,]+),([^>]+)> velocity=<([^,]+),([^>]+)>)");

  for (auto & crLine : aLines)
  {
    smatch match;
    regex_match(crLine, match, rx);

    list.push_back({ atoi(match[1].str().c_str()), atoi(match[2].str().c_str()), atoi(match[3].str().c_str()), atoi(match[4].str().c_str()) });

  }
  return list;
}

void Print(vector<PtAndSpeed> & gPoints)
{
  for (auto idx : irange(minY, maxY + 1))
  {
    for (auto jdx : irange(minX, maxX + 1))
    {
      auto where = find_if(gPoints.begin(), gPoints.end(), [&](auto & val) {
        return val.x == jdx && val.y == idx;
      });

      cout << (where == gPoints.end() ? ' ' : '#');
    }
    cout << endl;
  }
}

int main(int argc, char *argv[])
{
  auto gLines = GetLinesFromFile();
  auto ptList = ParseLines(gLines);

  for (size_t i = 0;; i++)
  {
    maxX = MININT;
    maxY = MININT;

    minX = MAXINT;
    minY = MAXINT;

    for (auto & el : ptList)
    {
      el.x += el.dx;
      el.y += el.dy;

      minX = min(minX, el.x);
      minY = min(minY, el.y);

      maxX = max(maxX, el.x);
      maxY = max(maxY, el.y);
    }

    if (maxY - minY <= 10)
    {
      Print(ptList);
      cout << "Iterations: " << i + 1 << endl;
      
      exit(1);
    }

  }
}
