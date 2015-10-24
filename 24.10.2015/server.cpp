#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 4229

#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


void CloseConnection(SOCKET s)
{
	closesocket(s);
	WSACleanup();

	cout << "Good bue" << endl;
}

void Server()
{
	WSAData wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (iResult != 00)
	{
		cout << "Не удалось инициализировать библиотеку WinSock" << endl;
		return;
	}

	//handle или дескриптор - элемент для управления сокетом
	SOCKET s = socket(
		AF_INET, // The Internet Protocol version 4 (IPv4) address family.
		SOCK_STREAM, //A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
		0); //

	// заполнение структур для соединения
	sockaddr_in tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(PORT);
	tcpaddr.sin_addr.s_addr = htons(INADDR_ANY); //слушаем все ip адреса через порт

	//для сервера
	iResult = bind(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr));

	if (iResult != 0)
	{
		cout << WSAGetLastError() << endl;

		CloseConnection(s);

		return;
	}

	listen(s, 5); //сервер, слушаем сокет, 5 - длина очереди

	char str[1024];

	cout << "Server has been started successfully!\n";
	cout << "Listening port " << PORT << endl;

	while (true)
	{


		int buf = sizeof(tcpaddr);
		SOCKET client = accept(s, (SOCKADDR*)&tcpaddr, &buf);

		int a = recv(client, str, buf, 0);

		if (a == -1)
		{
			cout << WSAGetLastError() << endl;

			CloseConnection(s);

			return;
		}
		str[a] = '\0';

		cout << "Reseived: ";
		cout << str << endl;

		if (!strcmp(str, "exit"))
		{
			CloseConnection(s);
		}


		strcat_s (str, "_add");
		send(client, str, strlen(str), 0);
	}
	
}


int main()
{
	setlocale(LC_ALL, "rus");

	Server();

	return 0;
}
