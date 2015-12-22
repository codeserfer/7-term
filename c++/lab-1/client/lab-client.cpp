#include "stdafx.h"


#include <iostream>
#include <vector>
#include <sstream>
#include "TCP.h"


using namespace std;


vector<string> Split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}


/* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
static const size_t InitialFNV = 2166136261U;
static const size_t FNVMultiple = 16777619;

size_t myhash(const string &s)
{
	size_t hash = InitialFNV;
	for (size_t i = 0; i < s.length(); i++)
	{
		hash = hash ^ (s[i]);
		hash = hash * FNVMultiple;
	}
	return hash;
}

string DeleteLastLeter(string s)
{
	return s.substr(0, s.length() - 1);
}


int _tmain(int argc, _TCHAR* argv[])
{

	TCP* tcp = new TCP();

	try
	{
		tcp->Startup();
		tcp->InitSocket();
		tcp->Connect(PORT, HOST);

		//cout << "Sleep for 10 sec...\n";
		//Sleep(10 * 1000);
		//cout << "Wake up!\n";


		string login;
		cout << "Input yout login: ";
		cin >> login;

		string password;
		cout << "Input yout password: ";
		cin >> password;

		cout << "Hash is " << myhash(password) << endl;

		tcp->SendData(login + ":" + to_string(myhash(password)) + "!");
		string ans;
		if ((ans = tcp->ReceiveDataExclamation()) != "OK!")
			cout << "Incorrect login or password. Good bye.\n";
		else
		{
			while (true)
			{
				string s;

				cout << "0. Exit\n";
				cout << "1. Statistic\n";
				cout << "2. Start game\n";
				cin >> s;

				// Exit
				if (s == "0")
				{
					break;
				}
				// Statistic
				else if (s == "1")
				{
					tcp->SendData("STATISTIC!");
					if ((ans = tcp->ReceiveDataExclamation()) == "ERROR!")
					{
						throw Exception("Incorrect command. Correct your program's code!\n");
					}

					cout << endl;
					cout << DeleteLastLeter(ans) << endl;
					cout << endl;
				}
				// Game
				else if (s == "2")
				{
					tcp->SendData("GAME!");
					if ((ans = tcp->ReceiveDataExclamation()) == "ERROR!")
					{
						throw Exception("Incorrect command. Correct your program's code!\n");
					}

					int lettersCount = atoi(DeleteLastLeter(ans).c_str());

					cout << endl;
					cout << "The number of letters - " << lettersCount << endl;
					cout << endl;

					int mistakes = 0;
					string guessedWord(lettersCount, '-');
					

					cout << "Word: " << guessedWord << "\t" << "Count of mistakes: " << mistakes << "/6\n";

					while (true)
					{
						string letter;
						cout << "Input letter: ";
						cin >> letter;

						cout << "you has inputed: '" << letter << "'\n";

						if (letter.length() > 1)
							cout << "Your should input only one letter! Letter '" << letter.at(0) << "' has been sent.\n";

						// kostile
						string temp;
						temp.push_back(letter[0]);
						temp.push_back('!');
						//cout << "Sending letter '" << temp << "'\n";
						tcp->SendData(temp);
						if ((ans = tcp->ReceiveDataExclamation()) == "ERROR!")
						{
							throw Exception("Incorrect command. Correct your program's code!\n");
						}

						if (ans == "0!")
						{
							cout << "You win!\n";
							break;
						}
						else if (ans == "-1!")
						{
							cout << "This letter is absent\n";
							mistakes++;

							cout << "Word: " << guessedWord << "\t" << "Count of mistakes: " << mistakes << "/6\n";
						}
						else if (ans == "-2!")
						{
							cout << "Game over! You are lose.\n";
							break;
						}
						else
						{
							//cout << "Exists letters: " << ans << endl;

							auto ansVector = Split(ans, ',');
							for (int i = 0; i < ansVector.size(); i++)
							{
								int index = atoi(ansVector[i].c_str());
								guessedWord[index - 1] = letter.c_str()[0];
							}

							cout << guessedWord << endl;
						}
					}

				}
				else if (s == "EXIT!!!")
				{
					tcp->SendData(s);
					throw Exception("Hey, admin! Be kinder!");
				}
				// Something else?..
				else
				{
					cout << "Unrecognised command!\n";
				}
			}

			cout << "See your later!\n";
			tcp->SendData("Exit!");
			tcp->CloseConnection();
		}
	}
	catch (Exception e)
	{
		cout << &e;
		tcp->CloseConnection();
	}
}