#include "stdafx.h"
#include "User.h"
#include <iostream>


User::User(int id, string nickname, string password)
{
	this->id = id;
	this->username = nickname;
	this->password = password;
}


User::~User()
{
}

int User::GetId()
{
	return id;
}

string User::GetUsername()
{
	return username;
}

bool User::CheckPassword(string password)
{
	//cout << "MATCHING! " << password << " and " << this->password << endl;
	//cout << "Function returns " << (this->password == password) << endl;
	return this->password == password;
}