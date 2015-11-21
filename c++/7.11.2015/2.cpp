/*
* Клиент отправляет почтовое сообщение по протоколу SMTP
* Используется AUTH LOGIN авторизация
*/

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "WinSock2.h"
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

#define  PORT 25
#define  HOST "127.0.0.1"

#include <string>
#include <vector>
#include <sstream>

std::string base64_encode(unsigned char const*, unsigned int len);
std::string base64_decode(std::string const& s);

using namespace std;

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;
}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}


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

	try
	{
		tcp->Startup();
		tcp->InitSocket();
		tcp->Connect(PORT, HOST);



		tcp->SendData("EHLO domain.local\r\n");
		cout << "EHLO domain.local" << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl;

		
		tcp->SendData("AUTH LOGIN\r\n");
		cout << "AUTH LOGIN" << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl << endl;

		char const* buffer = "stud_08";
		string sendingString = base64_encode(buffer, strlen(buffer)) + "\r\n";
		
		tcp->SendData(sendingString);
		cout << sendingString << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;
		

		cout << endl << endl << endl;

		tcp->SendData(sendingString);
		cout << sendingString << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;


		cout << endl << endl;

		tcp->SendData("MAIL FROM:<stud_08>\r\n");
		cout << "MAIL FROM:<stud_08>" << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl;

		tcp->SendData("RCPT TO:<stud_07>\r\n");
		cout << "RCPT TO:<stud_07>" << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl;

		tcp->SendData("DATA\r\n");
		cout << "DATA" << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl;

		tcp->SendData("I want to sleep!\r\n.\r\n");
		cout << "I want to sleep!" << endl;
		cout << "Reseive: " << endl;
		cout << tcp->ReceiveData() << endl;

		cout << endl << endl;

		tcp->SendData("QUIT\r\n");
		cout << "QUIT" << endl;
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
