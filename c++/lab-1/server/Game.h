#pragma once
#include "Vocabulary.h"
#include "User.h"
#include <vector>
#include "Statistic.h"

enum Status
{
	Autorized, InGame, Win, Lose
};

class Game
{
	string word;
	vector<char>* guessedLetters;
	int guessedCount;
	int mistakes;
	User* user;
	Status status;
	Vocabulary* vocabulary;
	Statistic* statistic;

public:
	Game(User*, Vocabulary*, Statistic*);
	~Game();

	int GetCount();
	vector<int>* Match(char);
	Status GetStatus();
	void StartGame();
	vector<int>* Lose(vector<int>*);
};

