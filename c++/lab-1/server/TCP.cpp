#include "stdafx.h"
#include "TCP.h"
#include "Exception.h"

using namespace std;

void TCP::Startup()
{
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		throw Exception("Initialization failed");
	}
}

SOCKET TCP::InitSocket()
{
	SOCKET s = socket(
		AF_INET,
		SOCK_STREAM, //_DGRAM, _RAW
		0);

	if (!s)
	{
		throw Exception("Error in socket connection");
	}

	const int nonblocking = 1;
	if (ioctlsocket(s, FIONBIO, (unsigned long*)&nonblocking) == SOCKET_ERROR)
	{
		throw Exception("Error in socket init: " + to_string(WSAGetLastError()));
	}

	return s;
}

void TCP::Connect(SOCKET s, int port, string host)
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
		throw Exception("Connection error: " + to_string(WSAGetLastError()));
	}
}

void TCP::SendData(SOCKET s, string str)
{
	send(s, str.c_str(), str.length(), 0);
}

string TCP::ReceiveData(SOCKET s)
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

void TCP::Bind(SOCKET s, int port)
{
	// заполнение структур для соединения
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(port);
	tcpaddr.sin_addr.s_addr = htons(INADDR_ANY); //слушаем все ip адреса через порт

	cout << "Listening port " << port << endl;

	if (bind(s, (SOCKADDR*)&tcpaddr, sizeof(tcpaddr)) != 0)
	{
		throw Exception("Can't bind");
	}
}

bool TCP::FindExclamationPoint(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == '!')
			return true;
	}

	return false;
}

void TCP::ListenMain(vector<SocketGame*>* sockets, int queueString)
{
	int list = listen(sockets->at(0)->socket, queueString);

	if (list == -1)
		throw Exception("Socket listening error!");

	char str[1024];

	int buf = sizeof(tcpaddr);
	SOCKET client = accept(sockets->at(0)->socket, (SOCKADDR*)&tcpaddr, &buf);

	int intClient = (int)client;

	if (intClient != -1)
	{
		sockets->push_back(new SocketGame(intClient, nullptr));
		cout << "First connection completed!" << endl;
	}
}

string TCP::ListenMinor(SOCKET client, int queueString)
{
	listen(client, queueString);

	char str[1024];

	int buf = sizeof(tcpaddr);


	int rec = recv(client, str, (int)strlen(str), 0);
	string receivedString = "";

	if (rec == -1)
		return string("");
	if (rec == 0)
		return string("");

	else
	{
		str[rec] = '\0';
		receivedString += string(str);

		// time restrict
		time_t timeStart = time(0);

		while (!FindExclamationPoint(receivedString))
		{
			rec = recv(client, str, (int)strlen(str), 0);

			if (rec != -1)
			{

				str[rec] = '\0';

				receivedString += string(str);
			}


			if (difftime(time(0), timeStart) > 10)
			{
				cout << "Client is so slow... Cansel recv\n";
				break;
			}
		}
	}

	cout << "Second connection completed! Your number in queue is " << client << endl;

	return receivedString;
}

void TCP::CloseConnection(SOCKET s)
{
	closesocket(s);
	WSACleanup();
}