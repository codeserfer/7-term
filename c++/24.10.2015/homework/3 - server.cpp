/*
* Клиент отправляет число.
* Сервер возвращает набор цифр, составляющих это число.
*/

#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define PORT 4229

#include <WinSock2.h>
#include <iostream>
#include <algorithm>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
#include <string>
#include <vector>

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

	vector<string> split(string str, char delimiter) {
		vector<string> internal;
		stringstream ss(str); // Turn the string into a stream.
		string tok;

		while (getline(ss, tok, delimiter)) {
			internal.push_back(tok);
		}

		return internal;
	}


	void Listen(int queueString)
	{
		listen(s, queueString);

		char str[1024];

		cout << "Listening...\n";

		
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

		string sending = to_string(strlen(str));

		cout << "sending: " << sending << endl;

		// sending count of digits
		send(client, sending.c_str(), sending.length(), 0);


		for (int i = 0; i < strlen(str); i++)
		{
			string temp = string(1, str[i]);

			cout << "sending " << temp << endl;

			send(client, temp.c_str(), 1, 0);
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
		while (true) tcp->Listen(10);
	}
	catch (string s)
	{
		cout << s;

		tcp->CloseConnection();
	}

	return 0;
}
