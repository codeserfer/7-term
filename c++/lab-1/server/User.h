#pragma once
#include <string>

using namespace std;

class User
{
	int id;
	string username;
	string password;

public:
	User(int, string, string);
	~User();

	int GetId();
	string GetUsername();
	bool CheckPassword(string);
};

