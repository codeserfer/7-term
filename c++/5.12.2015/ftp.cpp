// 5.12.5015.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "WinSock2.h"
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

#define  PORT 110
#define  HOST "127.0.0.1"

#include <string>
#include <vector>
#include <sstream>


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

	bool FindCRLF(string s)
	{
		for (int i = 0; i < s.length() - 4; i++)
		{
			if (s[i] == '\r' && s[i + 1] == '\n' && s[i + 2] == '.' && s[i + 3] == '\r' && s[i + 4] == '\n')
				return true;
		}
		return false;
	}

	string ReceiveData()
	{
		char str[100240];

		int countRecieved = recv(s, str, sizeof(str), 0);

		if (countRecieved == SOCKET_ERROR)
		{
			throw string("Receiving data error: " + to_string(WSAGetLastError()));
		}
		str[countRecieved] = '\0';

		return string(str);
	}

	string ReceiveDataCRLF()
	{
		string temp = "";
		while (true)
		{
			temp += ReceiveData();
			if (FindCRLF(temp))
			{
				break;
			}
		}
		return temp;
	}

	void CloseConnection()
	{
		closesocket(s);
		WSACleanup();
	}
};

vector<string> Split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}


int _tmain(int argc, _TCHAR* argv[])
{

	TCP* tcp = new TCP();
	tcp->Startup();
	tcp->InitSocket();
	//tcp->Connect(21, "88.212.196.79");
	tcp->Connect(21, "217.65.4.104");

	cout << tcp->ReceiveData();

	tcp->SendData("USER anonymous\r\n");

	cout << tcp->ReceiveData();

	tcp->SendData("PASS anonymous\r\n");

	cout << tcp->ReceiveData();

	tcp->SendData("PASV\r\n");

	string ans = tcp->ReceiveData();

	cout << ans;

	int start = ans.find('(');
	int finish = ans.find(')');

	string stringForParsing = ans.substr(start + 1, finish - start - 1 );

	cout << "stringForParsing: " <<  stringForParsing << endl;

	vector<string> splitted = Split(stringForParsing, ',');


	int port1 = atoi(splitted.at(4).c_str());
	int port2 = atoi(splitted.at(5).c_str());

	int finalPort = port1 * 256 + port2;

	cout << "final port: " << finalPort << endl;



	TCP* tcp2 = new TCP();
	tcp2->Startup();
	tcp2->InitSocket();
	//tcp2->Connect(finalPort, "88.212.196.79");
	tcp2->Connect(finalPort, "217.65.4.104");
	

	tcp->SendData("RETR robots.txt\r\n");

	cout << "file: \n";
	cout << tcp2->ReceiveData();

}


