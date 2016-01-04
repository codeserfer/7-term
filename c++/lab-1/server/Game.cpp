#include "stdafx.h"
#include "Game.h"

#define MAXMISTAKES 6
#include <iostream>


Status Game::GetStatus()
{
	return status;
}

void Game::StartGame()
{
	status = InGame;
}

vector<int>* Game::Lose(vector<int>* returnVector)
{
	status = Autorized;

	this->word = vocabulary->GetRandomWord();
	this->guessedLetters->clear();
	this->mistakes = 0;
	this->guessedCount = 0;

	cout << "New word is " << word << endl;

	returnVector->push_back(-2);

	return returnVector;
}

Game::Game(User* user, Vocabulary* vocabulary, Statistic* statistic)
{
	this->user = user;
	this->word = vocabulary->GetRandomWord();
	this->guessedLetters = new vector<char>();
	this->status = Status::Autorized;
	this->mistakes = 0;
	this->guessedCount = 0;
	this->vocabulary = vocabulary;
	this->statistic = statistic;

	std::cout << "For user " << user->GetUsername() << " word is " << word << std::endl;
}

Game::~Game()
{
}



int ::Game::GetCount()
{
	return word.length();
}

vector<int>* Game::Match(char c)
{
	vector<int>* returnVector = new vector<int>();
	bool q = true; // fuck fuck fuck 

	for (int i = 0; i < word.length(); i++)
	{
		if (word[i] == c)
		{
			// check, if user has guessed this litter
			bool hasGuessed = false;

			for (int i = 0; i < guessedLetters->size() && q; i++)
				if (guessedLetters->at(i) == c)
					hasGuessed = true;

			if (!hasGuessed)
			{
				guessedLetters->push_back(c);
				returnVector->push_back(i + 1); // numerating starts at 1
				guessedCount++;
				q = false;
			}
		}
	}

	if (returnVector->size() == 0)
	{
		mistakes++;

		// Lose
		if (mistakes >= MAXMISTAKES)
		{
			returnVector = Lose(returnVector);
			statistic->AddScore(user, statistic->GetScore(user)-100);
		}
		// No such letter
		else
		{
			returnVector->push_back(-1);
		}
	}
	else
	{
		// Win
		if (guessedCount == word.length())
		{
			returnVector->clear(); // deletimg all returning positions (It doesn't make sense)

			this->word = vocabulary->GetRandomWord();
			this->guessedLetters->clear();
			this->status = Status::Win;
			this->mistakes = 0;
			this->guessedCount = 0;
			this->statistic->AddScore(user, statistic->GetScore(user) + 100);

			returnVector->push_back(0);

			cout << "New word is " << word << endl;

			this->status = Status::Autorized;
		}
	}

	return returnVector;
}