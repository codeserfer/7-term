/*
* Клиент отправляет точку координат центра окружности в виде x,y и радиус;
* И также отправляется точка поиска.
* Сервер возвращает признак вхождения точки поиска внутрь заданной окружности.
*/

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "WinSock2.h"
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

#define  PORT 4229
#define  HOST "127.0.0.1"

using namespace std;


class TCP
{
private:
	SOCKET s;
	WSAData wsaData;

public:
	void Startup()
	{
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		{
			throw string("Initialization failed");
		}
	}

	void InitSocket()
	{
		s = socket(
			AF_INET,
			SOCK_STREAM, //_DGRAM, _RAW
			0);

		if (!s)
		{
			throw string("Error in socket connection");
		}
	}

	void Connect(int port, string host)
	{
		DWORD addr = inet_addr(host.c_str());
		HOSTENT *hinfo = gethostbyaddr((char*)&addr, 4, AF_INET);

		// заполнение структуры для соединения
		sockaddr_in tcpaddr;
		tcpaddr.sin_family = AF_INET;
		tcpaddr.sin_port = htons(port);

		memcpy((char FAR *)&(tcpaddr.sin_addr.s_addr), hinfo->h_addr_list[0], hinfo->h_length);

		if (connect(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr)) == -1)
		{
			string error = "Connection error: " + to_string(WSAGetLastError());

			throw error;
		}
	}

	void SendData(string str)
	{

		cout << str << endl;

		send(s, str.c_str(), str.length(), 0);
	}

	string ReceiveData()
	{
		char str[1024];

		int countRecieved = recv(s, str, sizeof(str), 0);

		if (countRecieved == SOCKET_ERROR)
		{
			string error = "Receiving data error: " + to_string(WSAGetLastError());

			throw error;
		}
		str[countRecieved] = '\0';

		return string(str);
	}

	void CloseConnection()
	{
		closesocket(s);
		WSACleanup();
	}
};


void Send(TCP* tcp, string s, bool closeConnection = true)
{
	tcp->Startup();
	tcp->InitSocket();
	tcp->Connect(PORT, HOST);
	tcp->SendData(s);
	if (closeConnection) tcp->CloseConnection();
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	TCP* tcp = new TCP();

	try
	{
		int centerX, centerY, r, pointX, pointY;

		cout << "Enter the X coordinate of the center of the a circle: ";
		cin >> centerX;

		cout << "Enter the Y coordinate of the center of the a circle: ";
		cin >> centerY;

		string sendingString = to_string(centerX) + "," + to_string(centerY);

		Send(tcp, sendingString);
		

		cout << "Enter the radius of the center of the a circle: ";
		cin >> r;
		Send(tcp, to_string(r));

		cout << "Enter the X coordinate of a point: ";
		cin >> pointX;

		cout << "Enter the Y coordinate of a point: ";
		cin >> pointY;

		sendingString = to_string(pointX) + "," + to_string(pointY);
		Send(tcp, sendingString, false);

		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;
		tcp->CloseConnection();
		
	}
	catch (string error)
	{
		cout << error;
		tcp->CloseConnection();
	}
}
