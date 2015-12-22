#pragma once

#include "WinSock2.h"
#pragma comment(lib, "Ws2_32.lib")
#define  PORT 5555
#define  HOST "192.168.0.102"
#include "Exception.h"
#include <string>

using namespace std;

class TCP
{
private:
	SOCKET s;
	WSAData wsaData;

public:
	TCP();
	~TCP();

	void Startup();

	void InitSocket();

	void Connect(int port, string host);

	void SendData(string str);

	bool FindExclamation(string s);

	string ReceiveData();

	string ReceiveDataExclamation();

	void CloseConnection();
};
