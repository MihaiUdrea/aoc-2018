#include "pch.h"
#include "utils.h"

struct Event
{
  string type;     // "Guard #xx" or "wakes up" or "falls asleep"
  int minutStart;  // when event occurs
  string rawEv;    // entire line for sorting
};

struct GuardData
{
  array<int, 60> daysLog{ 0 }; // array for each minute to count how many times guard was asleep

  int totalSlept{0};
  int maxFreq{0};
  int maxFreqMinute{0};
};

auto ParseLines(vector<string> & aLines)
{
  vector<Event> events;

  const regex rx(R"(\[\d+-\d+-\d+ \d+:(\d+)\] (Guard #\d+|wakes up|falls asleep) ?( begins shift)?)");
  for (auto & crLine : aLines)
  {
    smatch match;    
    regex_match(crLine, match, rx);

    events.push_back({ match[2].str(), atoi(match[1].str().c_str()), crLine });
  }

  return events;
}

bool IsGuardChange (const string & aOpType)
{
  return aOpType.substr(0, 5) == "Guard";
}

int main()
{
  auto gLines = GetLinesFromFile();
  auto events = ParseLines(gLines);

  // sort input - we can do it directly G < f
  sort(events.begin(), events.end(), [](auto & left, auto & rt) {
    return left.rawEv < rt.rawEv;
  });
  
  // parse events and log sleeping minutes
  map<int, GuardData> guardsData;
  for (int eventIdx = 0; eventIdx < events.size();)
  { 
    Event & ev = events[eventIdx++];
    assert(IsGuardChange(ev.type));

    int guardId = atoi(ev.type.substr(7).c_str());

    // continue until a guard change
    for (; eventIdx < events.size() && !IsGuardChange(events[eventIdx].type);)
    { 
      Event & fallEv = events[eventIdx++];
      assert(fallEv.type == "falls asleep");

      Event & raiseEv = events[eventIdx++];
      assert(raiseEv.type == "wakes up");

      for (auto i : irange(0, raiseEv.minutStart - fallEv.minutStart + 1))
      {
        guardsData[guardId].daysLog[fallEv.minutStart + i]++;
      }
    }
  }

  // compute for each guard
  for (auto & guard : guardsData)
  {
    auto & data = guard.second.daysLog;
    guard.second.totalSlept = accumulate(data.begin(), data.end(), 0);
    
    auto where = max_element(data.begin(), data.end());
    guard.second.maxFreq = *where;
    guard.second.maxFreqMinute = where - data.begin();
  }

  // compute result 1  
  auto res = max_element(guardsData.begin(), guardsData.end(), [](auto & l, auto & r) {
    return l.second.totalSlept < r.second.totalSlept;
  });

  cout << "Step 1 : " << res->first * res->second.maxFreqMinute << endl;

  // compute result 2
  auto res2 = max_element(guardsData.begin(), guardsData.end(), [](auto & l, auto & r) {
    return l.second.maxFreq < r.second.maxFreq;
  });

  cout << "Step 2 : " << res2->first * res2->second.maxFreqMinute << endl;
}
