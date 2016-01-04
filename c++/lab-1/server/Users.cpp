#include "stdafx.h"
#include "Users.h"
#include "Exception.h"


Users::Users()
{
	users = new vector < User* >();
}


Users::~Users()
{
}

void Users::AddUser(User* user)
{
	users->push_back(user);
}

User* Users::GetUser(int id)
{
	return users->at(id);
}

User* Users::FindUser(string userName)
{
	for (int i = 0; i < users->size(); i++)
	{
		if (users->at(i)->GetUsername() == userName)
			return users->at(i);
	}

	return nullptr;
}

int Users::UsernameToID(string username)
{
	for (int i = 0; i < users->size(); i++)
	{
		if (users->at(i)->GetUsername() == username)
			return i;
	}

	throw new Exception("No such user!");
}

string Users::UsernameFromID(int id)
{
	for (int i = 0; i < users->size(); i++)
	{
		if (users->at(i)->GetId() == id)
			return users->at(i)->GetUsername();
	}

	throw new Exception("No such user!");
}

string Users::GetUserList()
{
	string returnList = "";

	for (int i = 0; i < users->size(); i++)
	{
		User* user = users->at(i);
		returnList += user->GetUsername() + "\n";
	}

	return returnList;
}