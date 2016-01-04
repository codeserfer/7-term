#pragma once
#include <string>
#include "User.h"

using namespace std;
class Score
{
	User* user;
	int highScore;
public:
	Score(User*, int);
	~Score();
	int GetUserID();
	User* GetUser();
	int GetHighScore();
	void UpdateScore(int);
};

