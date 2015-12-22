#pragma once
#include "User.h"
#include <vector>

class Users
{
	vector<User*>* users;
public:
	Users();
	~Users();

	void AddUser(User*);
	User* GetUser(int);
	User* FindUser(string);

	int UsernameToID(string);
	string UsernameFromID(int);
	string GetUserList();
};

