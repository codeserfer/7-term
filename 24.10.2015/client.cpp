// Клиент
// Сервер принимает точку координат центра окружности в виде x, y и радиус, также принимается точка поиска.
// Возвращает признак вхождения точки поиска внутрь заданной окружности
// TCP сокет

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




int _tmain(int argc, _TCHAR* argv[])
{
	TCP* tcp = new TCP();

	try 
	{
		tcp->Startup();
		tcp->InitSocket();
		tcp->Connect(PORT, HOST);

		cout << "Input string to send: ";

		string s;
		cin >> s;

		tcp->SendData(s);
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
