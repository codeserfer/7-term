#pragma once
#include <iostream>

class Exception
{
	std::string msg;
public:
	Exception(std::string);
	~Exception();
	friend std::ostream& operator<<(std::ostream& os, const Exception* e);
};