#pragma once
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WinSock2.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include "Exception.h"
#include <vector>
#include "Game.h"


using namespace std;

struct SocketGame
{
	SOCKET socket;
	Game* game;

	SocketGame(SOCKET socket, Game* game)
	{
		this->socket = socket;
		this->game = game;
	}
};

class TCP
{
private:
	WSAData wsaData;
	sockaddr_in tcpaddr;

public:
	void Startup();

	SOCKET InitSocket();

	void Connect(SOCKET s, int port, string host);

	void SendData(SOCKET s, string str);


	string ReceiveData(SOCKET s);

	void Bind(SOCKET s, int port);

	bool FindExclamationPoint(string s);

	void ListenMain(vector <SocketGame*>* sockets, int queueString);

	string ListenMinor(SOCKET client, int queueString);


	void CloseConnection(SOCKET s);
};

