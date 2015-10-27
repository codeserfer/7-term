/*
* Сервер принимает строку.
* Возвращает ее в верхнем регистре.
*/

#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 4229

#include <WinSock2.h>
#include <iostream>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")
#include <string>

using namespace std;

class Server
{
private:
	SOCKET s;
	WSAData wsaData;
	sockaddr_in tcpaddr;

public:

	void Startup()
	{
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 00)
		{
			throw string("Can't init WinSock lib");
		}
	}

	void CloseConnection()
	{
		closesocket(s);
		WSACleanup();

		cout << "Good bue" << endl;
	}

	void InitSocket()
	{
		//handle или дескриптор - элемент для управления сокетом
		s = socket(
			AF_INET, // The Internet Protocol version 4 (IPv4) address family.
			SOCK_STREAM, //A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism. This socket type uses the Transmission Control Protocol (TCP) for the Internet address family (AF_INET or AF_INET6).
			0); //
		if (!s)
		{
			throw string("Error in socket connection");
		}
	}

	void Bind(int port)
	{
		// заполнение структур для соединения
		tcpaddr.sin_family = AF_INET;
		tcpaddr.sin_port = htons(port);
		tcpaddr.sin_addr.s_addr = htons(INADDR_ANY); //слушаем все ip адреса через порт

		cout << "Listening port " << port << endl;

		if (bind(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr)) != 0)
		{
			throw string("Can't bind");
		}
	}

	void Listen(int queueString)
	{
		listen(s, queueString);

		char str[1024];

		cout << "Server has been started successfully!\n";

		while (true)
		{
			int buf = sizeof(tcpaddr);
			SOCKET client = accept(s, (SOCKADDR*)&tcpaddr, &buf);

			int a = recv(client, str, buf, 0);

			if (a == -1)
			{
				string error = "Receiving error: " + to_string(WSAGetLastError());

				throw error;
			}
			str[a] = '\0';

			cout << "Reseived: ";
			cout << str << endl;

			if (!strcmp(str, "exit"))
			{
				CloseConnection();
			}


			string strString = string(str);

			transform(strString.begin(), strString.end(), strString.begin(), toupper);

			const char* newString = strString.c_str();

			cout << "Sending: " << newString << endl;

			send(client, newString, strlen(newString), 0);
		}
	}
};




int main()
{
	setlocale(LC_ALL, "rus");

	Server* tcp = new Server();

	try
	{
		tcp->Startup();
		tcp->InitSocket();
		tcp->Bind(PORT);
		tcp->Listen(10);
	}
	catch (string s)
	{
		cout << s;

		tcp->CloseConnection();
	}

	return 0;
}
