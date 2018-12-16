#include "pch.h"
#include "utils.h"

struct Solution
{
	stringstream result;

	struct Inst
	{
		int OpCote{};
		int A{};
		int B{};
		int C{};
	};

	using RegisterState = vector<int>;

	struct Recording
	{
		Inst inst;
		RegisterState before;
		RegisterState after;
		int matchCount{};
		vector<string> matchOpCodes{};
	};

	vector<int> CpuRegs;
	multimap<int, vector<pair<int, string>>> Findings;

	vector<Recording> recordings;
	vector<Inst> program;

	Solution(wstring filePath = LR"(D:\aoc-2018\input)")
	{
		//wcout << L"-----------------MATCH " << filePath << L"-------------" << endl;

		string input;
		ReadFileToString(filePath.c_str(), input);

		gLines = GetLines(input);

		const regex rxBef(R"(Before: \[(\d+), (\d+), (\d+), (\d+)\])");
		const regex rxInstr(R"((\d+) (\d+) (\d+) (\d+))");
		const regex rxAfter(R"(After:\s+\[(\d+), (\d+), (\d+), (\d+)\])");

		int i = 0;
		for (; i < gLines.size(); i += 4)
		{
			Recording rec;

			string & beforeLine = gLines[i];

			if (beforeLine.empty())
				break;

			string & InstLine = gLines[i + 1];
			string & afterLine = gLines[i + 2];

			{
				smatch match;
				regex_match(beforeLine, match, rxBef);
				for (int j = 1; j <= 4; j++)
					rec.before.push_back(atoi(match[j].str().c_str()));
			}

			{
				smatch match;
				regex_match(InstLine, match, rxInstr);
				rec.inst = { atoi(match[1].str().c_str()),atoi(match[2].str().c_str()), atoi(match[3].str().c_str()),atoi(match[4].str().c_str()) };
			}

			{
				smatch match;
				regex_match(afterLine, match, rxAfter);
				for (int j = 1; j <= 4; j++)
					rec.after.push_back(atoi(match[j].str().c_str()));
			}

			recordings.push_back(rec);
		}

		CpuRegs.resize(4);

		for (int j = 0; j < 16; j++)
			instOrder[j] = -1;
		
		// read program
		i++;
		for (; i < gLines.size(); i++)
		{
			string & crLine = gLines[i];

			if (crLine.empty())
				continue;

			smatch match;
			regex_match(crLine, match, rxInstr);
			program.push_back( { atoi(match[1].str().c_str()),atoi(match[2].str().c_str()), atoi(match[3].str().c_str()),atoi(match[4].str().c_str()) });
		}
	}

	void ForEachCharInLine(vector<string> & aLines)
	{

	}

	int Val(int x)
	{
		return x;
	}

	int Reg(int x)
	{
		return CpuRegs[x];
	}


	void addr(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) + Reg(inst.B);}               //(add register)result of     adding register A and register B.
	void addi(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) + Val(inst.B);}               //(add immediate)result of    adding register A and value B.
	void mulr(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) * Reg(inst.B);}               //(multiply register)result of multiplying register A and register B.	
	void muli(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) * Val(inst.B);}               //(multiply immediate)result of multiplying register A and value B.	
	void banr(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) & Reg(inst.B);}               //(bitwise AND register)result of the bitwise AND of register A and register B.
	void bani(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) & Val(inst.B);}               //(bitwise AND immediate)result of the bitwise AND of register A and value B.
	void borr(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) | Reg(inst.B);}               //(bitwise OR register)result of the bitwise OR of register A and register B.
	void bori(Inst & inst){CpuRegs[inst.C] = Reg(inst.A) | Val(inst.B);}               //(bitwise OR immediate)result of the bitwise OR of register A and value B.
	void setr(Inst & inst){CpuRegs[inst.C] = Reg(inst.A);}                             //(set register) copies the contents of register A into register C. (Input B is ignored.)
	void seti(Inst & inst){CpuRegs[inst.C] = Val(inst.A);}                             //(set immediate) stores value A into register C. (Input B is ignored.)
	void gtir(Inst & inst){CpuRegs[inst.C] = ((Val(inst.A) > Reg(inst.B)) ? 1 : 0);}   //(greater - than immediate / register) sets register C to 1 if value A is greater than register B.Otherwise, register C is set to 0.
	void gtri(Inst & inst){CpuRegs[inst.C] = ((Reg(inst.A) > Val(inst.B)) ? 1 : 0);}   //(greater - than register / immediate) sets register C to 1 if register A is greater than value B.Otherwise, register C is set to 0.
	void gtrr(Inst & inst){CpuRegs[inst.C] = ((Reg(inst.A) > Reg(inst.B)) ? 1 : 0);}   //(greater - than register / register) sets register C to 1 if register A is greater than register B.Otherwise, register C is set to 0.
	void eqir(Inst & inst){CpuRegs[inst.C] = ((Val(inst.A) == Reg(inst.B)) ? 1 : 0);}  //(equal immediate / register) sets register C to 1 if value A is equal to register B.Otherwise, register C is set to 0.
	void eqri(Inst & inst){CpuRegs[inst.C] = ((Reg(inst.A) == Val(inst.B)) ? 1 : 0);}  //(equal register / immediate) sets register C to 1 if register A is equal to value B.Otherwise, register C is set to 0.
	void eqrr(Inst & inst){CpuRegs[inst.C] = ((Reg(inst.A) == Reg(inst.B)) ? 1 : 0);}  //(equal register / register) sets register C to 1 if register A is equal to register B.Otherwise, register C is set to 0.
	
	void RunExact(int OpCode, Inst & inst)
	{
		switch (OpCode)
		{
		case 0:  addr(inst); return;
		case 1:  addi(inst); return;
		case 2:  mulr(inst); return;
		case 3:  muli(inst); return;
		case 4:  banr(inst); return;
		case 5:  bani(inst); return;
		case 6:  borr(inst); return;
		case 7:  bori(inst); return;
		case 8:  setr(inst); return;
		case 9:  seti(inst); return;
		case 10: gtir(inst); return;
		case 11: gtri(inst); return;
		case 12: gtrr(inst); return;
		case 13: eqir(inst); return;
		case 14: eqri(inst); return;
		case 15: eqrr(inst); return;
		}
	}

	string GetNameExact(int OpCode)
	{
		switch (OpCode)
		{
		case 0:  return "addr";
		case 1:  return "addi";
		case 2:  return "mulr";
		case 3:  return "muli";
		case 4:  return "banr";
		case 5:  return "bani";
		case 6:  return "borr";
		case 7:  return "bori";
		case 8:  return "setr";
		case 9:  return "seti";
		case 10: return "gtir";
		case 11: return "gtri";
		case 12: return "gtrr";
		case 13: return "eqir";
		case 14: return "eqri";
		case 15: return "eqrr";
		}
		return "";
	}
	
	array<int, 16> instOrder;

	vector<string> gLines;
	
	auto GetObjectsSize()
	{
		return 1;
	}

	void Print()
	{	

	}


	string One(bool stopOnElfDies = false)
	{
		Print();

		int threeOrMore = 0;
		
		for (auto & rec : recordings)
		{
			auto crPos = Findings.insert(make_pair(rec.inst.OpCote, vector<pair<int, string>>()));

			//cout << "Before: [" << rec.before[0] << ", " << rec.before[1] << ", " << rec.before[2] << ", " << rec.before[3] << "]" << endl;
			//cout << rec.inst.OpCote << " " << rec.inst.A << " " << rec.inst.B << " " << rec.inst.C << endl;
			//cout << "After: [" << rec.after[0] << ", " << rec.after[1] << ", " << rec.after[2] << ", " << rec.after[3] << "]" << endl;
			for (int i = 0; i <= 15; i++)
			{
				CpuRegs = rec.before;
				RunExact(i, rec.inst);

				if (CpuRegs == rec.after)
				{
					//cout << "Matching: " << GetNameExact(i) << endl;
					rec.matchCount++;
					rec.matchOpCodes.push_back(GetNameExact(i));

					crPos->second.push_back(make_pair(i, GetNameExact(i)));
				}
			}

			if (rec.matchCount >= 3)
				threeOrMore++;
			
			//cout << endl;
		}
		result << threeOrMore;

		// find the one with a single inst
		for (int k = 0; k < 16; ++k)
		{
			set<string> toRemove;
			for (auto it = Findings.begin(); it != Findings.end(); ++it)
			{
				if (it->second.size() == 1)
				{
					instOrder[it->first] = it->second.front().first;
					toRemove.insert(it->second.front().second);
				}
			}
			
			// reduce remove this opcodes from findings
			for (auto it = Findings.begin(); it != Findings.end(); ++it)
			{
				it->second.erase(remove_if(it->second.begin(), it->second.end(), [&](auto &el) {
					auto searchToRemove = el.second;
					bool found = toRemove.find(searchToRemove) != toRemove.end();
					return found;
				}), it->second.end());
			}
		}

		for (auto & reg : CpuRegs)
			reg = 0;

		return result.str();
	}

	auto Two()
	{
		string res;
		result.str("");
		//--GO--------------------------------
		for (auto & inst : program)
		{
			RunExact(instOrder[inst.OpCote], inst);
		}

		result << (int) CpuRegs[0];

		/**/
		return result.str();
	}
};

#ifdef _DEBUG
#define TEST true
#define RUNONE true
#define RUNTWO true

#else
#define TEST false
#define RUNONE true
#define RUNTWO false
#endif // DEBUG

#define TEST1 false// ok
#define TEST2 false
#define TEST3 false
#define TEST4 false
#define TEST5 false
#define TEST6 false


int main()
{
	if (TEST)
	{
		if (TEST1)
		{
			Solution otherSample(LR"(D:\aoc-2018\sample)");
			auto res = otherSample.One();
			assert(res == "1");
			//cout << "3 S2: " << res.first << " Score: " << res.second << endl;
		}

		{
			if (TEST2)
			{
				Solution otherSample(LR"(D:\aoc-2018\sample2)");
				auto res = otherSample.One();
				assert(res == "1");

			}

			if (TEST5)
			{
				Solution otherSample(LR"(D:\aoc-2018\sample5)");
				auto res = otherSample.Two();
				//assert(res.first == "12" && res.second == "6474");
				//cout << "5 S2: " << res.first << " Score: " << res.second << endl;

			}

			if (TEST6)
			{
				Solution otherSample(LR"(D:\aoc-2018\sample6)");
				auto res = otherSample.Two();
				//assert(res.first == "34	" && res.second == "1140");
				//cout << "6 S2: " << res.first << " Score: " << res.second << endl;
			}

			if (0)
			{
				Solution otherSample(LR"(D:\aoc-2018\sampleMy1)");
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
	}

	if (RUNTWO)
	{
		auto sol1 = s.Two();
		cout << "S2: " << sol1 << endl;
	}

	//auto sol2 = s.Two();
	//cout << "S2: " << sol2 << endl;
	//system("pause");
}
