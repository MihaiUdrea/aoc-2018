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

struct node
{
	node(char id, node * parent, int childCount, int metaCount)
	: id(id), parent(parent)
	{
		childs.reserve(childCount);
		meta.reserve(metaCount);
	}

	char id;

	node * parent = nullptr;
	vector<node*> childs;
	vector<int> meta;
};

template <typename T>
bool Completed(const vector<T> & aVec)
{
	return aVec.size() == aVec.capacity();
}

auto Parse(vector<int> aList)
{
	list<node> list;
	
	int nodeIdGenerator = 0;
	
	node * parentNode = nullptr;
	node * crNode = nullptr;
	
	for (auto it = aList.begin(); it != aList.end();)
	{
		if (!crNode)
		{
			// create new node with child count and metacount 
			auto childsCount = *(it++);
			auto metaCount   = *(it++);

			crNode = &list.emplace_back('A' + nodeIdGenerator++, parentNode, 
				childsCount, metaCount);

			// add it to its parent;
			if (parentNode)
				parentNode->childs.push_back(crNode);
		}
		else if(!Completed(crNode->childs))
		{
			// load child
			parentNode = crNode;
			crNode = nullptr;
		}	
		else
		{
			// load meta
			copy(it, it + crNode->meta.capacity(), back_inserter(crNode->meta));
			it += crNode->meta.capacity();

			// restore parent
			crNode = crNode->parent;
		}
	}

	return list;
}

int Calc(node * n)
{
	return accumulate(n->meta.begin(), n->meta.end(), 0, [n](auto & sum, auto & metaEl)
	{
		return sum + 
			(n->childs.empty() 
				? metaEl 
				: (metaEl <= n->childs.size() 
					? Calc(n->childs[metaEl - 1]) 
					: 0));
	});
}

int main()
{
	auto lines = GetLinesFromFile();	
	assert(lines.size() > 0);

	auto initialList = SplitLineInColumns(lines.front());
	assert(initialList.size() > 0);

	auto list = Parse(initialList);

	cout << "S1: " << accumulate(list.begin(), list.end(), 0, [](int & sum, auto & el) {
		return sum + accumulate(el.meta.begin(), el.meta.end(), 0);
	}) << endl;

	toClipboard(Calc(&list.front()));
}


