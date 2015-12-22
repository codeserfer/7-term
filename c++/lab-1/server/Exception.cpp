#include "stdafx.h"
#include "Exception.h"


using namespace std;

Exception::Exception(string msg)
{
	this->msg = msg;
}


Exception::~Exception()
{
	msg.clear();
}


ostream& operator<<(ostream& os, const Exception* e)
{
	string err = e->msg;

	os << "Exception: " << err.c_str() << "\n";
	return os;
	//os << "fff";
	//return os;
}