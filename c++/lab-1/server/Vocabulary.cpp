#include "stdafx.h"
#include "Vocabulary.h"


Vocabulary::Vocabulary()
{
	vocabulary = new vector <string>();
}


Vocabulary::~Vocabulary()
{
}

void Vocabulary::AddWord(string word)
{
	vocabulary->push_back(word);
}

string Vocabulary::GetRandomWord()
{
	srand(time(NULL));

	int r = rand();
	int size = vocabulary->size();


	int randIndex = r % size;
	return vocabulary->at(randIndex);
}