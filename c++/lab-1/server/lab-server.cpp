#include "stdafx.h"
#include "Exception.h"
#include "FileWorker.h"
#include <iostream>
#include "TCP.h"
#include "Game.h"
#include "Statistic.h"

using namespace std;

string VectorToString(vector<int>* vector)
{
	string returnString = "";

	for (int i = 0; i < vector->size(); i++)
	{
		returnString += to_string(vector->at(i));

		if (i != vector->size() - 1)
			returnString += ",";
	}

	return returnString;
}

vector<string> Split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}


int _tmain(int argc, _TCHAR* argv[])
{
	FileWorker* f = nullptr;
	Statistic* statistic = nullptr;

	try
	{
		TCP* tcp = new TCP();
		f = new FileWorker();
		Vocabulary* vocabulary = f->LoadVocabulary();
		Users* users = f->LoadUsers();
		statistic = f->LoadStatistic(users);

		cout << "Statistic:" << endl;
		cout << statistic->GetScoreList() << endl << endl;

		cout << "Users:" << endl;
		cout << users->GetUserList() << endl << endl;
	
		tcp->Startup();
		SOCKET startSocket = tcp->InitSocket();
		tcp->Bind(startSocket, 5555);

		vector <SocketGame*>* sockets = new vector<SocketGame*>();
		sockets->push_back(new SocketGame(startSocket, nullptr));

		// infinite loop by sockets 
		for (int i = 0; i < sockets->size(); i++)
		{
			if (i == 0)
			{
				tcp->ListenMain(sockets, 10);

			}
			else
			{
				string s = tcp->ListenMinor(sockets->at(i)->socket, 10);

				// Admins backdoor
				if (s == "EXIT!!!")
				{
					cout << "Admin says exit" << endl;
					cout << "Good bye!" << endl;

					throw Exception("Not Exception! Just fast exit.");
				}

				if (!s.empty())
				{

					cout << "Minor socket has received: : " << s << endl;

					if (sockets->at(i)->game == nullptr)
					{
						cout << "Try to login!";

						if (s.find(':') != -1)
						{
							auto w = Split(s, ':');
							w[1] = w[1].substr(0, w[1].length() - 1);

							cout << "Login is " << w[0] << endl;
							cout << "Password is " << w[1] << endl;

							User* currentUser = users->FindUser(w[0]);

							if (!currentUser)
							{
								cout << "User wasn't found!" << endl;
								tcp->SendData(sockets->at(i)->socket, "Loginerror!");
							}
							else
							{
								cout << "User " << currentUser->GetUsername() << " was found!" << endl;

								if (!currentUser->CheckPassword(w[1]))
								{
									cout << "Password is incorrect!" << endl;
									tcp->SendData(sockets->at(i)->socket, "Loginerror!");
								}
								else
								{
									cout << "Password is correct. Welcome, " << currentUser->GetUsername() << endl;
									sockets->at(i)->game = new Game(currentUser, vocabulary, statistic);
									tcp->SendData(sockets->at(i)->socket, "OK!");
								}
							}
						}
						else
						{
							cout << "Attempt to break server!!!\n";
						}
					}
					else if (sockets->at(i)->game->GetStatus() == Autorized)
					{
						if (s == "STATISTIC!")
						{
							tcp->SendData(sockets->at(i)->socket, statistic->ExportScoreList());
						}
						else if (s == "GAME!")
						{
							sockets->at(i)->game->StartGame();
							tcp->SendData(sockets->at(i)->socket, to_string(sockets->at(i)->game->GetCount()) + "!");
						}
						else
						{
							tcp->SendData(sockets->at(i)->socket, "ERROR!");
						}
					}
					// Game
					else if (sockets->at(i)->game->GetStatus() == InGame)
					{
						if (!s.empty())
						{
							// Early exit
							if (s[0] != '0')
								tcp->SendData(sockets->at(i)->socket, VectorToString(sockets->at(i)->game->Match(s[0])) + "!");
							else
							{
								vector<int>* returnVector = new vector<int>();
								sockets->at(i)->game->Lose(returnVector);

								tcp->SendData(sockets->at(i)->socket, VectorToString(returnVector) + "!");
							}
						}
					}
				}
			}

			// infinite loop by sockets
			if (i == sockets->size() - 1) i = -1;

		}
	}
	catch (Exception e)
	{
		cout << &e;

		if (f && statistic)
			f->SaveStatistic(statistic);
	}

	return 0;
}

