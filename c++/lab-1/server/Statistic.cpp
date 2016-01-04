#include "stdafx.h"
#include "Statistic.h"


Statistic::Statistic()
{
	//scoreList = new vector<Score*>();
}


Statistic::~Statistic()
{
}

void Statistic::AddScore(User* user, int score)
{
	bool found = false;

	for (int i = 0; i < scoreList.size(); i++)
	{
		if (scoreList.at(i)->GetUser() == user)
		{
			scoreList.at(i)->UpdateScore(score);
			found = true;
			break;
		}
	}

	if (!found)
		scoreList.push_back(new Score(user, score));
}

string Statistic::GetScoreList()
{
	string returnList = "";

	for (int i = 0; i < scoreList.size(); i++)
	{
		Score* score = scoreList[i];
		returnList += score->GetUser()->GetUsername() + ":" + to_string(score->GetHighScore()) + " ";
	}

	return returnList;
}

string Statistic::ExportScoreList()
{
	string returnList = "";

	for (int i = 0; i < scoreList.size(); i++)
	{
		Score* score = scoreList[i];
		returnList += score->GetUser()->GetUsername() + ": " + to_string(score->GetHighScore()) + "\n";
	}

	return (returnList + "!");
}

int Statistic::GetScore(User* user)
{
	for (int i = 0; i < scoreList.size(); i++)
	{
		if (scoreList.at(i)->GetUser() == user)
		{
			return scoreList.at(i)->GetHighScore();
		}
	}

	return 0;
}