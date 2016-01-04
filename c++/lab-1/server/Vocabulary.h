#pragma once
#include <vector>
#include <time.h>

using namespace std;

class Vocabulary
{
	vector<string>* vocabulary;

public:

	Vocabulary();
	~Vocabulary();

	void AddWord(string);
	string GetRandomWord();
};

