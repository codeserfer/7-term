#pragma once
#include <string>
#include <vector>
#include "Score.h"
using namespace std;

class Statistic
{
	vector<Score*> scoreList;

public:
	Statistic();
	~Statistic();

	void AddScore(User*, int);
	string GetScoreList();
	string ExportScoreList();
	int GetScore(User*);
};

