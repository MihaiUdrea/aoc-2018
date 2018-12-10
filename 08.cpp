#include "pch.h"
#include "utils.h"
#include <list>


auto ParseLines(string & aLine)
{
  const regex rx(R"((\d+) players; last marble is worth (\d+) points)");
  smatch match;
  regex_match(aLine, match, rx);

  return make_pair(atoi(match[1].str().c_str()), atoi(match[2].str().c_str()) * 100);
}

list<int> tableList;
map<int, long long unsigned int> playerScore;

int crMarblePos = 0;
int nextMarble = 0;
int crPlayer = 0;

template <typename T>
void ClockWise(T & aCrPos)
{
  if (tableList.empty())
    return;

  for (auto idx : irange(0, 2))
  {
    
    advance(aCrPos, 1);
    if (aCrPos == tableList.end())
      aCrPos = tableList.begin();
  }
}

template <typename T>
void CounterClockWise(T & aCrPos)
{
  if (tableList.empty())
    return;

    for (auto idx : irange(0, 7))
    {
      if (aCrPos == tableList.begin())
        aCrPos = tableList.end();
      advance(aCrPos, -1);
    }
}


int main()
{
  auto gLines = GetLinesFromFile();
  auto data = ParseLines(gLines.front());

  for(auto crMarbleListIt = tableList.end();;)
  {
    for (auto player : irange(0, data.first))
    {
      if (nextMarble != 0 && nextMarble % 23 == 0)
      {
        // keep it
        playerScore[player] += nextMarble;

        // count the stone to remove
        CounterClockWise(crMarbleListIt);
        playerScore[player] += *crMarbleListIt;

        crMarbleListIt = tableList.erase(crMarbleListIt, next(crMarbleListIt));
      }
      else
      {
        ClockWise(crMarbleListIt);

        crMarbleListIt = tableList.insert(crMarbleListIt, nextMarble);
      }

      nextMarble++;

      if (nextMarble > data.second)
      {
        auto res = max_element(playerScore.begin(), playerScore.end(), [](auto & l, auto & r) {
          return l.second < r.second;
        });

        cout << "S2: " << res->second << endl;
        exit(1);
      }
    }
  }
}
