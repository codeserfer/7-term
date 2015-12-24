#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "FileProcessing.h"
#include <time.h>

#define PORT 69
#define TIMEOUT 5000

using namespace std;
sockaddr_in ServerAddr;
SOCKET s;


void InitClient()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	srand(time(nullptr));
	int port = rand() % 6000 + 2000;

	cout << "New port is " << port << endl;

	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(port);
	clientAddr.sin_addr.s_addr = htons(INADDR_ANY);

	bind(s, (sockaddr *)&clientAddr, sizeof(clientAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(PORT);

	int timeout = TIMEOUT;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
}

void SendFile(char* fileName)
{
	FileProcessing* file = new FileProcessing();
	int fileNameSize = strlen(fileName);
	int datasize = 0;
	char* recvmsg = new char[128];
	void* sendData = new char[516];
	void* data = malloc(512);

	// запрашиваем файл
	char* request = new char[520];
	request[0] = 0; // запрос на запись
	request[1] = 2;	// запрос на запись
	memcpy(request + 2, fileName, fileNameSize);
	request[2 + fileNameSize] = 0;
	memcpy(request + 3 + fileNameSize, "octet\0", 6);
	sendto(s, request, 3 + fileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));

	delete[] request;

	sockaddr_in from;

	unsigned int sentbyte = 0;
	int structSize = sizeof(from);
	int sendBlockNumber = 0;
	int ackBlockNumber = 0;

	while (true)
	{
		int retValue = recvfrom(s, recvmsg, 128, 0, (sockaddr*)&from, &structSize);

		if (retValue > 0)
		{
			int op = recvmsg[1];

			if (op == 4) // команда 
			{
				memcpy(&ackBlockNumber, recvmsg + 3, 1);
				memcpy((void*)((size_t)&ackBlockNumber + 1), recvmsg + 2, 1);
				

				if (ackBlockNumber != sendBlockNumber)
				{
					sendto(s, (char*)sendData, datasize + 4, 0, (sockaddr *)&from, sizeof(from));
					continue;
				}

				if (ackBlockNumber == 0)
				{
					string f = fileName;
					file->Open(f);
				}

				
				DWORD nul = 0;

				memcpy(sendData, &nul, 4);

				short int code = 3;

				sendBlockNumber = ackBlockNumber+1;

				memcpy((void*)((size_t)sendData + 1), &code, 1);
				memcpy((void*)((size_t)sendData), (void*)((size_t)&code + 1), 1);
				memcpy((void*)((size_t)sendData + 2), (void*)((size_t)&sendBlockNumber + 1), 1);
				memcpy((void*)((size_t)sendData + 3), &sendBlockNumber, 1);

				
				
				datasize = file->ReadData(data, 512);
				memcpy((void*)((size_t)sendData + 4), data, datasize);

				sentbyte += datasize;
				cout << "Sent block:" << sendBlockNumber << '\n';
				if (datasize != 512)
				{
					cout << "file " << fileName << " (" << sentbyte << " byte) has been sent\n";
					
					file->Close();
					sendto(s, (char*)sendData, datasize+4, 0, (sockaddr *)&from, sizeof(from));
					break;
				}
				sendto(s, (char*)sendData, datasize + 4, 0, (sockaddr *)&from, sizeof(from));
			}
			else
			{
				if (recvmsg[1] == 5)
				{
					cout << "protocol error\n";
					break;
				}
			}
		}
		else if (retValue == 0)
		{
			cout << "server closed\n";
			return;
		}
		else
		{
			cout << "socket error";
			return;
		}
	}
	delete[]recvmsg;
	delete[]sendData;
	free(data);

}

void RecvFile(char* fileName)
{
	FileProcessing* file = new FileProcessing();
	int fileNameSize = strlen(fileName);
	
	char* recvpart = new char[520];
	void * ack = new char[4];

	//запрашиваем файл
	char* request = new char[520];	//запрос
	request[0] = 0, request[1] = 1;					//read
	memcpy(request + 2, fileName, fileNameSize);
	request[2 + fileNameSize] = 0;
	memcpy(request + 3 + fileNameSize, "octet\0", 6);
	sendto(s, request, 3 + fileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));
	delete [] request;

	unsigned int receivedbyte = 0;
	sockaddr_in from;
	int structSize = sizeof(from);
	int blockNumber = 1;
	while (true)
	{
		int returnValue = recvfrom(s, recvpart, 520, 0, (sockaddr*)&from, &structSize);

		if (returnValue > 0)
		{
			int op = recvpart[1];

			if (op == 3) // DATA command
			{
				int recvBlockNumber = 0;
				
				memcpy(&recvBlockNumber, recvpart + 3, 1);
				memcpy((void*)((size_t)&recvBlockNumber + 1), recvpart + 2, 1);

				DWORD nul = 0;
				memcpy(ack, &nul, 4);
				short int code = 4;
				memcpy((void*)((size_t)ack + 1), &code, 1);
				memcpy((void*)((size_t)ack), (void*)((size_t)&code + 1), 1);


				if (recvBlockNumber != blockNumber)
				{
					memcpy((void*)((size_t)ack + 2), (void*)((size_t)&recvBlockNumber + 1), 1);
					memcpy((void*)((size_t)ack + 3), &recvBlockNumber, 1);
					sendto(s, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));
					continue;
				}

				string f = fileName;
				if (recvBlockNumber == 1)
				{
					file->Create(f);
				}
					
				receivedbyte += (returnValue - 4);
				
				file->WriteData((void*)(recvpart+4), returnValue - 4);

				memcpy((void*)((size_t)ack + 2), (void*)((size_t)&blockNumber + 1), 1);
				memcpy((void*)((size_t)ack + 3), &blockNumber, 1);
				sendto(s, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));

				cout << "Received block:" << blockNumber << '\n';

				blockNumber++;
				if (returnValue - 4 != 512)
				{
					cout << "file " << fileName << " has been received (" << receivedbyte << "bytes)" << endl;

					file->Close();
					sendto(s,(char*) ack, 4, 0, (sockaddr *)&from, sizeof(from));
					break;
				}
			}
			else
			{
				if (recvpart[1] == 5)
				{
					cout << "protocol error\n";
					break;
				}
			}
		}
		else if (returnValue == 0)
		{
			cout << "server closed\n";
			return;
		}
		else
		{
			cout << "socket error";
			break;
		}
	}
	delete[]recvpart;
	delete[]ack;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int input = -1;
	char* fileName = new char[255];
	while (input != 0)
	{
		
		InitClient();

		cout << "0. Exit" << endl;
		cout << "1. Get file" << endl;
		cout << "2. Send file" << endl;
		
		cin >> input;
		switch (input)
		{
			case 0:
				break;

			case 1:
				cout << "Input filename \n";
				cin >> fileName;

				RecvFile(fileName);
				break;
			case 2:
				cout << "Input filename \n";
				cin >> fileName;

				SendFile(fileName);
				break;
		
			default:
				break;
		}
	}

	delete[] fileName;

	closesocket(s);
	WSACleanup();
	return 0;
}

