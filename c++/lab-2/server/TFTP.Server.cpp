#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WinSock2.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#include <string>
#include <sstream>
#include <map>
#include <ctime>
#include <fstream>
#include "FileWorker.h"
#include <string.h>
#include <tchar.h>
#include <algorithm>


using namespace std;


struct SendFileThreadParams
{
	sockaddr_in ClientAddr;
	int port;
	string fileName;
};

struct ReceiveFileThreadParams
{
	sockaddr_in ClientAddr;
	int port;
	string fileName;
};

bool IsFileExists (string fileName)
{
	ifstream infile(fileName.c_str());
	return infile.good();
}

string ToUpperCase(string s)
{
	transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}


DWORD WINAPI SendFileDataThread(void* p)
{
	srand(time(0));
	SendFileThreadParams *params = (SendFileThreadParams*)p;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	sockaddr_in ClientAddr = params->ClientAddr;
	SOCKET ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);// = params->socket;


	sockaddr_in ServAddr;
	ServAddr.sin_addr.s_addr = INADDR_ANY;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(20000 + (rand() % 20000));//params->ClientAddr.sin_port;//htons(params->port);



	int RetValue = bind(ClientSocket, (sockaddr *)&ServAddr, sizeof(ServAddr));

	if (RetValue < 0)
	{
		// отправить сообщение об ошибке
		cout << "Can't bind" << endl;

		closesocket(ClientSocket);
		return -1;
	}

	char* recvbuf = new char[2048];


	//int timeout = 3000;
	int timeout = 30000;

	setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));



	FileWorker* file = new FileWorker();

	if (!file->Open(params->fileName))
	{
		// отправить сообщение об ошибке

		cout << "Can't open file" << endl;

		return -2;
	}



	bool needToContinue = false;

	sockaddr_in ClientAddr2;

	int addr2size = sizeof(ClientAddr2);

	DWORD dwTick;
	int partNum = 1;
	int ackNum = 0;
	int tryCount = 0;
	int retryCount = 0;
	int respCode = -1;
	int resp = 3;
	bool ack = true;
	void* fileBuf = malloc(512);
	DWORD bytesRead = 0;
	bool stillSending = true;

	while (stillSending)
	{
		tryCount++;

		// считаем, что клиент отсоединился, т.к. не ответил в течение 1 сек после отправки ему части файла
		if (needToContinue && tryCount > 5)
		{
			cout << "We were trying, but..." << endl;
			file->Close();
			closesocket(ClientSocket);
			return -3;
		}

		// выставляем флаг для дальнейшей работы с ним

		needToContinue = true;

		if (ack)
		{
			bytesRead = file->ReadData(fileBuf, 512);
			cout << "We have read " << bytesRead << " bytes from file " << params->fileName << endl;
		}

		ack = false;


		if (bytesRead < 0)
		{
			file->Close();
			closesocket(ClientSocket);
			return -2;
		}

		void* sendBuf = malloc(4 + bytesRead);

		char* t = new char[2];

		DWORD nul = 0;
		memcpy(sendBuf, &nul, 4);

		memcpy((void*)((size_t)sendBuf + 1), &resp, 1);
		memcpy((void*)((size_t)sendBuf), (void*)((size_t)&resp + 1), 1);
		memcpy((void*)((size_t)sendBuf + 2), (void*)((size_t)&partNum + 1), 1);
		memcpy((void*)((size_t)sendBuf + 3), &partNum, 1);
		memcpy((void*)((size_t)sendBuf + 4), fileBuf, bytesRead);

		ClientAddr.sin_port = htons(params->port);

		sendto(ClientSocket, (char*)sendBuf, 4 + bytesRead, 0, (sockaddr *)&ClientAddr, sizeof(ClientAddr));

		cout << endl << "sending part number: " << partNum << endl;

		if (bytesRead == 0)
		{
			file->Close();
			closesocket(ClientSocket);

			cout << "Close file" << endl;
			return 1;
		}



		dwTick = GetTickCount();

		while (needToContinue)
		{
			if (GetTickCount() - dwTick > 5000)
			{
				tryCount++;
				cout << "Too much waititng time..." << endl;

				break;
			}

			RetValue = recvfrom(ClientSocket, recvbuf, 2048, 0, (sockaddr*)&ClientAddr2, &addr2size);

			if (RetValue < 0)
			{
				int err = WSAGetLastError();
				cout << endl << "sock err: " << err << endl;
				break;
			}

			if (ClientAddr2.sin_addr.s_addr != ClientAddr.sin_addr.s_addr)
			{
				cout << endl << "received packet from another client, ignoring..." << endl;
				continue;
			}



			if (RetValue > 0)
			{
				respCode = 0;
				ackNum = 0;

				memcpy(&respCode, recvbuf + 1, 1);
				memcpy((void*)((size_t)&respCode + 1), recvbuf, 1);
				memcpy(&ackNum, recvbuf + 3, 1);
				memcpy((void*)((size_t)&ackNum + 1), recvbuf + 2, 1);

				cout << endl << "received: code = " << respCode << ", part number =  " << ackNum << endl;

				if (respCode == 4 && ackNum == partNum)
				{
					partNum++;
					retryCount += tryCount;

					tryCount = 0;

					ack = true;

					needToContinue = false;

					// если мы выслали последний пакет и получили на него подтверждение, то выходим

					if (bytesRead < 512)
					{
						stillSending = false;

						cout << "We have sent all file, bytesRead = " << bytesRead  << endl;

						break;
					}

				}
			}
		}



		delete sendBuf;
	}
	file->Close();
	closesocket(ClientSocket);

	cout << "SendFileDataThread finished with " << retryCount - partNum + 1 << " retries" << endl;

	return 0;

}

DWORD WINAPI ReceiveFileDataThread(void* p)
{
	srand(time(0));
	ReceiveFileThreadParams* params = (ReceiveFileThreadParams*)p;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockaddr_in ClientAddr = params->ClientAddr;

	SOCKET ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);// = params->socket;


	sockaddr_in ServAddr;
	ServAddr.sin_addr.s_addr = INADDR_ANY;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(20000 + (rand() % 20000));//params->ClientAddr.sin_port;//htons(params->port);

	int RetValue = bind(ClientSocket, (sockaddr *)&ServAddr, sizeof(ServAddr));

	if (RetValue < 0)
	{
		// отправить сообщение об ошибке
		closesocket(ClientSocket);
		return -1;
	}

	int timeout = 1000;
	setsockopt(ClientSocket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&timeout,
		sizeof(timeout));

	FileWorker *file = new FileWorker();
	if (!file->Create(params->fileName))
	{
		// отправить сообщение об ошибке
		return -2;
	}

	bool needToContinue = false;
	sockaddr_in ClientAddr2;
	int addr2size = sizeof(ClientAddr2);
	DWORD dwTick;
	int partNum = 0;
	int ackNum = 0;
	int tryCount = 0;
	int retryCount = 0;
	int respCode = -1;
	int resp = 4;
	bool ack = false;
	char* recvbuf = new char[2200];
	void* fileBuf = malloc(512);
	DWORD bytesReceived = 0;
	bool stillReceiving = true;
	while (stillReceiving)
	{
		tryCount++;
		if (needToContinue && tryCount > 5)
			// считаем, что клиент отсоединился, т.к. не ответил в течение 1 сек после отправки ему части файла
		{
			file->Close();
			closesocket(ClientSocket);
			return -3;
		}
		// выставляем флаг для дальнейшей работы с ним
		needToContinue = true;
		if (ack)
		{
			file->WriteData(fileBuf, bytesReceived);
		}

		if (bytesReceived < 0)
		{

			file->Close();
			closesocket(ClientSocket);
			return -2;
		}

		void* sendBuf = malloc(4);

		memcpy((void*)((size_t)sendBuf + 1), &resp, 1);
		memcpy((void*)((size_t)sendBuf), (void*)((size_t)&resp + 1), 1);

		memcpy((void*)((size_t)sendBuf + 2), (void*)((size_t)&partNum + 1), 1);
		memcpy((void*)((size_t)sendBuf + 3), &partNum, 1);

		ClientAddr.sin_port = htons(params->port);
		sendto(ClientSocket, (char*)sendBuf, 4, 0, (sockaddr *)&ClientAddr, sizeof(ClientAddr));

		if (bytesReceived < 512 && ack)
		{
			file->Close();
			closesocket(ClientSocket);

			cout << "ReceiveFileDataThread finished!" << endl;
			return 1;
		}

		dwTick = GetTickCount();
		ack = false;

		while (needToContinue)
		{
			if (GetTickCount() - dwTick > 5000)
			{
				tryCount++;
				break;
			}
			RetValue = recvfrom(ClientSocket, recvbuf, 2200, 0, (sockaddr*)&ClientAddr2, &addr2size);
			if (RetValue < 0)
			{
				auto err = WSAGetLastError();
				break;
			}
			if (ClientAddr2.sin_addr.s_addr != ClientAddr.sin_addr.s_addr)
				continue;

			if (RetValue > 0)
			{
				respCode = 0;
				ackNum = 0;
				memcpy(&respCode, recvbuf + 1, 1);
				memcpy((void*)((size_t)&respCode + 1), recvbuf, 1);
				memcpy(&ackNum, recvbuf + 3, 1);
				memcpy((void*)((size_t)&ackNum + 1), recvbuf + 2, 1);
				cout << endl << "received part number: " << ackNum << endl;
				if (respCode == 3 && ackNum == partNum + 1)
				{
					bytesReceived = RetValue - 4;
					if (bytesReceived < 0)
						break;
					memcpy(fileBuf, recvbuf + 4, bytesReceived);
					partNum++;
					retryCount += tryCount;
					tryCount = 0;
					ack = true;
					needToContinue = false;
				}
			}

		}

		delete sendBuf;
	}
	file->Close();
	closesocket(ClientSocket);

	cout << "ReceiveFileDataThread finished!" << endl;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	cout << "Server has been started" << endl;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(69);
	ServAddr.sin_addr.s_addr = INADDR_ANY;

	int RetValue = bind(ClientSocket, (sockaddr*)&ServAddr, sizeof(ServAddr));
	if (RetValue != 0)
	{
		cout << "Can't get access to 69 port!\nServer shutdown..\n";
		system("pause");
		return -1;
	}
	stringstream ss;
	while (true)
	{
		char recvbuf[4096];
		ss.str("");
		ss.clear();

		sockaddr_in ClientAddr;
		int s = sizeof(ClientAddr);
		RetValue = recvfrom(ClientSocket, recvbuf, 4096, 0, (sockaddr*)&ClientAddr, &s);
		if (RetValue < 0)
			continue;
		recvbuf[RetValue] = '\0';
		cout << endl << "received: " << recvbuf << endl;
		int operation = 0;
		memcpy(&operation, recvbuf + 1, 1);
		switch (operation)
		{
		case 1:
			for (int i = 2; i < RetValue; i++)
			{
				if (recvbuf[i] != '\0')
					ss << recvbuf[i];
				else
				{
					string filename = ss.str();
					if (IsFileExists(filename))
					{
						i++;
						ss.str("");
						ss.clear();
						for (; i < RetValue; i++)
						{
							if (recvbuf[i] != '\0')
								ss << recvbuf[i];
							else
							{
								if (strcmp(ToUpperCase(ss.str()).c_str(), "OCTET") == 0)
								{
									cout << "This is OCTET" << endl;
									SendFileThreadParams* params = new SendFileThreadParams();
									params->ClientAddr = ClientAddr;
									params->fileName = filename;
									params->port = ntohs(ClientAddr.sin_port);
									CreateThread(NULL, 0, SendFileDataThread, params, 0, NULL);
								}

								break;
							}
						}
					}
					else
					{
						// файл не существует
					}


					break;
				}
			}
			break;
		case 2:
			for (int i = 2; i < RetValue; i++)
			{
				if (recvbuf[i] != '\0')
					ss << recvbuf[i];
				else
				{
					string filename = ss.str();

					i++;
					ss.str("");
					ss.clear();
					for (; i < RetValue; i++)
					{
						if (recvbuf[i] != '\0')
							ss << recvbuf[i];
						else
						{
							if (strcmp(ToUpperCase(ss.str()).c_str(), "OCTET") == 0)
							{
								cout << "This is octet" << endl;
								ReceiveFileThreadParams* params = new ReceiveFileThreadParams();
								params->ClientAddr = ClientAddr;
								params->fileName = filename;
								params->port = ntohs(ClientAddr.sin_port);
								CreateThread(NULL, 0, ReceiveFileDataThread, params, 0, NULL);
							}

							break;
						}
					}
					break;
				}
			}
			break;

		default:
			break;
		}
	}


	return 0;
}