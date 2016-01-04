#include "stdafx.h"
#include "FileWorker.h"
#include "Exception.h"

#define STATISTIC	"statistic.txt"
#define VOCABULARY	"vocabulary.txt"
#define USERS		"users.txt"


string FileWorker::ReadFile(string fileName)
{
	ifstream* file = new ifstream();

	if (!fileName.empty())
		file->open(fileName);

	if (!file->is_open()) throw Exception("Can't open the file!");

	if (!file || file->eof())
	{
		throw Exception("Can't read the file!");
	}

	string content = "";

	while (!file->eof())
	{
		char* temp = new char[LINESIZE];
		file->getline(temp, LINESIZE);

		content += temp;
	}

	if (file) file->close();

	return content;
}

FileWorker::FileWorker()
{
}


FileWorker::~FileWorker()
{
}

void FileWorker::WriteFile(string fileName, string s)
{
	ofstream* file = new ofstream();

	if (!fileName.empty())
		file->open(fileName);

	if (!file->is_open()) throw Exception("Can't open the file!");

	if (!file)
	{
		throw Exception("Can't read the file!");
	}

	*file << s << endl;

	if (file) file->close();
}

vector<string> FileWorker::Split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

Users* FileWorker::LoadUsers()
{
	string s = ReadFile(USERS);

	vector<string> splittedS = Split(s, ' ');

	Users* users = new Users();

	for (int i = 0; i < splittedS.size(); i++)
	{
		auto temp = Split(splittedS[i], ':');
		User* user = new User(stoi(temp[0]), temp[1], temp[2]);

		users->AddUser(user);
	}
	
	return users;
}

Vocabulary* FileWorker::LoadVocabulary()
{
	string s = ReadFile(VOCABULARY);

	auto temp = Split(s, ' ');
	Vocabulary* vocabulary = new Vocabulary();

	for (int i = 0; i < temp.size(); i++)
	{
		vocabulary->AddWord(temp[i]);
	}

	return vocabulary;
}

void FileWorker::SaveStatistic(Statistic* statistic)
{
	WriteFile(STATISTIC, statistic->GetScoreList());
}

Statistic* FileWorker::LoadStatistic(Users* users)
{
	string s = ReadFile(STATISTIC);

	auto line = Split(s, ' ');
	Statistic* statistic = new Statistic();

	for (int i = 0; i < line.size(); i++)
	{
		auto temp = Split(line[i], ':');
	
		statistic->AddScore(users->FindUser(temp[0]), atoi(temp[1].c_str()));
	}

	return statistic;
}