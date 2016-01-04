#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "TCP.h"


TCP::TCP()
{
}


TCP::~TCP()
{
}

void TCP::Startup()
{
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		throw Exception("Initialization failed");
	}
}

void TCP::InitSocket()
{
	s = socket(
		AF_INET,
		SOCK_STREAM, //_DGRAM, _RAW
		0);

	if (!s)
	{
		throw Exception("Error in socket connection");
	}
}

void TCP::Connect(int port, string host)
{
	DWORD addr = inet_addr(host.c_str());
	HOSTENT *hinfo = gethostbyaddr((char*)&addr, 4, AF_INET);

	if (!hinfo)
		throw Exception("Can't find host " + host);

	// заполнение структуры для соединения
	sockaddr_in tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(port);

	memcpy((char FAR *)&(tcpaddr.sin_addr.s_addr), hinfo->h_addr_list[0], hinfo->h_length);

	if (connect(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr)) == -1)
	{
		throw Exception("Connection error: " + to_string(WSAGetLastError()));
	}
}

void TCP::SendData(string str)
{
	send(s, str.c_str(), str.length(), 0);
}

bool TCP::FindExclamation(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == '!')
			return true;
	}
	return false;
}

string TCP::ReceiveData()
{
	char str[10240];

	int countRecieved = recv(s, str, sizeof(str), 0);

	if (countRecieved == SOCKET_ERROR)
	{
		throw Exception("Receiving data error: " + to_string(WSAGetLastError()));
	}
	str[countRecieved] = '\0';

	return string(str);
}

string TCP::ReceiveDataExclamation()
{
	string temp = "";
	while (true)
	{
		temp += ReceiveData();
		if (FindExclamation(temp))
		{
			break;
		}
	}
	return temp;
}

void TCP::CloseConnection()
{
	closesocket(s);
	WSACleanup();
}