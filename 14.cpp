#include "pch.h"
#include "utils.h"

struct Solution
{
  Solution(wstring filePath = LR"(D:\aoc\2018\input)")
  {

    string input;
    ReadFileToString(filePath.c_str(), input);

    gLines = GetLines(input);

    stopStep = atoi(gLines.front().c_str());

    for (auto & ch : gLines.front())
      inputDigits.push_back(ch - '0');
  }

  stringstream result;

  vector<string> gLines;
  
  long long stopStep;
  
  vector<BYTE> board = { 3, 7 };
  vector<BYTE> inputDigits;

  int GetObjectsSize()
  {
    return stopStep;
  }

  void Print()
  {
    cout << "------------------------------" << endl;


  }

  void AdvancePosition(size_t & pos, size_t steps)
  {
    pos += steps;
    pos %= board.size();
  }

  auto AdvanceElf(size_t & aIdx)
  {
    AdvancePosition(aIdx, 1 + board[aIdx]);
  };

  string One()
  {
    result.str("");
    //--GO--------------------------------

    size_t FirstElfPos = 0;
    size_t SecondElfPos = 1;
    for (; board.size() < 10 + stopStep;)
    {
      // add recipe
      auto sum = board[FirstElfPos] + board[SecondElfPos];
      if (sum >= 10)
      {
        board.push_back(sum / 10);
        board.push_back(sum % 10);
      }
      else
      {
        board.push_back(sum % 10);
      }

      // move 
      AdvancePosition(FirstElfPos, 1 + board[FirstElfPos]);
      AdvancePosition(SecondElfPos, 1 + board[SecondElfPos]);
    }

    for (auto idx : irange(stopStep, stopStep+10))
      result << (char)('0' + board[idx]);

    return result.str();
  }

  auto PushAndFound(BYTE aElem)
  {
    board.push_back(aElem);
    if (equal(inputDigits.begin(), inputDigits.end(), board.begin() + board.size() - inputDigits.size(), board.end()))
    {
      result << board.size() - inputDigits.size();
      return true;
    }
    else
      return false;
  };

  string Two()
  {
    result.str("");
    //--GO--------------------------------

    size_t FirstElfPos = 0;
    size_t SecondElfPos = 1;
    for (;;)
    {
      // add recipe
      auto sum = board[FirstElfPos] + board[SecondElfPos];
      if (sum >= 10)
      {
        if (PushAndFound(sum / 10))
          break;
      }

      if (PushAndFound(sum % 10))
        break;

      // move 
      AdvanceElf(FirstElfPos);
      AdvanceElf(SecondElfPos);
    }

    return result.str();
  }
};

int main()
{
  if (1)
  {
    Solution sample(LR"(D:\aoc\2018\sample)");
    //assert(sample.GetObjectsSize() == 5);

    //auto sample1 = sample.One();
    //assert(sample1 == "0124515891");

    auto sample2 = sample.Two();
    assert(sample2 == "9");
  }

  if (1)
  {
    Solution otherSample(LR"(D:\aoc\2018\sample2)");
    //assert(otherSample.GetObjectsSize() == 9);

    //auto sample1 = otherSample.One();
    //assert(sample1 == "5158916779");

    auto otherSample2 = otherSample.Two();
    assert(otherSample2 == "2018");
  }

  Solution s;
  //assert(s.GetObjectsSize() == 3841138812);

  //auto sol1 = s.One();
  //cout << "S1: " << sol1 << endl;

  auto sol2 = s.Two();
  cout << "S2: " << sol2 << endl;
  
}
