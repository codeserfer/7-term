#include "stdafx.h"
#include "Score.h"


Score::Score(User* user, int highScore)
{
	this->highScore = highScore;
	this->user = user;
}

Score::~Score()
{
}

User* Score::GetUser()
{
	return user;
}

int Score::GetUserID()
{
	return user->GetId();
}

int Score::GetHighScore()
{
	return highScore;
}

void Score::UpdateScore(int score)
{
	highScore = score;
}