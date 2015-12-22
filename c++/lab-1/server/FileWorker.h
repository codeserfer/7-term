#pragma once

#define LINESIZE 1024

#include "User.h"
#include "Vocabulary.h"
#include "Statistic.h"
#include <list>
#include <fstream>
#include <vector>
#include <sstream>
#include "Users.h"

using namespace std;
class FileWorker
{
	vector<string> Split(string, char);
	void WriteFile(string fileName, string s);
	string ReadFile(string);

public:	
	FileWorker();
	~FileWorker();
	
	Users* LoadUsers();

	Vocabulary* LoadVocabulary();
	
	void SaveStatistic(Statistic*);
	Statistic* LoadStatistic(Users*);
};

