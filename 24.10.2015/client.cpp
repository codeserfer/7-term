// 24.10.2015.cpp: определяет точку входа для консольного приложения.
//
// Клиент
// Сервер должен преобразовать переданный текст в верхний регистр
// TCP сокет

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "WinSock2.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

#define  PORT 4229
#define  HOST "127.0.0.1"

using namespace std;

void CloseConnection(SOCKET connectSocket)
{
	closesocket(connectSocket);
	WSACleanup();
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (iResult != 0)
	{
		cout << "Initialization failed" << endl;
		
		return 0;
	}


	SOCKET s = socket(
		AF_INET,
		SOCK_STREAM, //_DGRAM, _RAW
		0);

	if (!s)
	{
		cout << "Error in socket connection" << endl;
		cout << WSAGetLastError() << "\n";

		CloseConnection(s);

		return 0;
	}

	DWORD addr = inet_addr(HOST);
	HOSTENT *hinfo = gethostbyaddr((char*)&addr, 4, AF_INET);

	// заполнение структуры для соединения
	sockaddr_in tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(PORT);

	memcpy((char FAR *)&(tcpaddr.sin_addr.s_addr), hinfo->h_addr_list[0], hinfo->h_length);

	iResult = connect(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr));

	if (iResult == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
		
		CloseConnection(s);

		return 0;
	}

	char str[1024];

	cout << "Input sending text: ";
	cin >> str;

	send(s, str, strlen(str), 0);

	int countRecieved = recv(s, str, sizeof(str), 0);

	if (countRecieved == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;

		CloseConnection(s);
		
		return 0;
	}
	str[countRecieved] = '\0';

	cout << str;

	CloseConnection(s);
}
