/*
* Сервер принимает точку координат центра окружности в виде x,y и радиус;
* Также принимается точка поиска.
* Возвращает признак вхождения точки поиска внутрь заданной окружности.
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

		int centerX = 0, centerY = 0, r = 0, pointX = 0, pointY = 0;
		SOCKET client = 0;

		for (int i = 0; i < 3; i++)
		{
			int buf = sizeof(tcpaddr);
			client = accept(s, (SOCKADDR*)&tcpaddr, &buf);

			int a = recv(client, str, buf, 0);

			if (a == -1)
			{
				string error = "Receiving error: " + to_string(WSAGetLastError());

				throw error;
			}
			str[a] = '\0';

			cout << "Reseived: ";
			cout << str << endl;

			string s;

			switch (i)
			{
			case 0:
			{
				s = string(str);

				vector<string> center = split(s, ',');

				centerX = atoi(center[0].c_str());
				centerY = atoi(center[1].c_str());

				cout << "centerX = " << centerX << endl;
				cout << "centerY = " << centerY << endl;
				break;
			}
			case 1:
				r = atoi(str);

				cout << "r = " << r << endl;
				break;
			case 2:
				s = string(str);
				
				vector<string> point = split(s, ',');

				pointX = atoi(point[0].c_str());
				pointY = atoi(point[1].c_str());

				cout << "centerX = " << pointX << endl;
				cout << "centerY = " << pointY << endl;
			}
		}

		if (sqrt(pow(pointX - centerX, 2) + pow(pointY - centerY, 2)) < r)
			send(client, "yes", 3, 0);
		else
			send(client, "no", 2, 0);
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
