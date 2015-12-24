// TFTP.Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "FileProcessing.h"
#include <time.h>



using namespace std;
sockaddr_in ServerAddr;
SOCKET s;


void InitClient()
{
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	srand(time(NULL));
	int port = rand() % 6000 + 2000;

	cout << "New port is " << port << endl;

	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(port);
	clientAddr.sin_addr.s_addr = htons(INADDR_ANY);

	bind(s, (sockaddr *)&clientAddr, sizeof(clientAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(69);

	int timeout = 5000;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
}

void SendFile(char* FileName)
{
	FileProcessing* file = new FileProcessing();
	int fileNameSize = strlen(FileName);
	int datasize = 0;
	char* recvmsg = new char[128];
	void* sendData = new char[516];
	void* data = malloc(512);

	// запрашиваем файл
	char* request = new char[520];
	request[0] = 0; // запрос на запись
	request[1] = 2;	// запрос на запись
	memcpy(request + 2, FileName, fileNameSize);

	request[2 + fileNameSize] = 0;
	memcpy(request + 3 + fileNameSize, "octet\0", 6);
	sendto(s, request, 3 + fileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));
	delete[] request;

	unsigned int sendedbyte = 0;
	sockaddr_in from;
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
					string f = FileName;
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

				sendedbyte += datasize;
				cout << "Sended block:" << sendBlockNumber << '\n';
				if (datasize != 512)
				{
					cout << "file " << FileName << " sended\n";
					cout << "size " << sendedbyte << '\n';
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

void RecvFile(char* FileName)
{
	FileProcessing *file = new FileProcessing();
	int FileNameSize = strlen(FileName);
	
	char* recvpart = new char[520];
	void * ack = new char[4];

	//запрашиваем файл
	char* request = new char[520];	//запрос
	request[0] = 0, request[1] = 1;					//read
	memcpy(request + 2, FileName, FileNameSize);
	request[2 + FileNameSize] = 0;
	memcpy(request + 3 + FileNameSize, "octet\0", 6);
	sendto(s, request, 3 + FileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));
	delete [] request;

	unsigned int Receivedbyte = 0;
	sockaddr_in from;
	int structSize = sizeof(from);
	int BlockNumber = 1;
	while (true)
	{
		int RetValue = recvfrom(s, recvpart, 520, 0, (sockaddr*)&from, &structSize);
		if (RetValue > 0)
		{
			int op = recvpart[1];
			if (op == 3) //команда DATA
			{
				int recvBlockNumber = 0;
				
				memcpy(&recvBlockNumber, recvpart + 3, 1);
				memcpy((void*)((size_t)&recvBlockNumber + 1), recvpart + 2, 1);

				DWORD nul = 0;
				memcpy(ack, &nul, 4);
				short int code = 4;
				memcpy((void*)((size_t)ack + 1), &code, 1);
				memcpy((void*)((size_t)ack), (void*)((size_t)&code + 1), 1);

				

				if (recvBlockNumber != BlockNumber)
				{
					memcpy((void*)((size_t)ack + 2), (void*)((size_t)&recvBlockNumber + 1), 1);
					memcpy((void*)((size_t)ack + 3), &recvBlockNumber, 1);
					sendto(s, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));
					continue;
				}
				string f = FileName;
				if (recvBlockNumber == 1)
					file->Create(f);
					
				Receivedbyte += (RetValue - 4);
				
				file->WriteData((void*)(recvpart+4), RetValue - 4);

				memcpy((void*)((size_t)ack + 2), (void*)((size_t)&BlockNumber + 1), 1);
				memcpy((void*)((size_t)ack + 3), &BlockNumber, 1);
				sendto(s, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));
				cout << "Received block:" << BlockNumber << '\n';
				BlockNumber++;
				if (RetValue - 4 != 512)
				{
					cout << "file " << FileName << " received\n";
					cout << "size " << Receivedbyte << '\n';
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
		else if (RetValue == 0)
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
	int input = 0;
	char* fileName = new char[255];
	while (input != 3)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		InitClient();

		cout << "\n1 Get file\n2 Send file\n3 Exit\n";
		cin >> input;
		switch (input)
		{
		case 1:
			cout << "Enter filename \n";
	
			cin >> fileName;
			RecvFile(fileName);
			break;
		case 2:
			cout << "Enter filename \n";
			cin >> fileName;
			SendFile(fileName);
			break;
		case 3:
			break;
		default:
			break;
		}
	}
	delete[]fileName;
	closesocket(s);
	WSACleanup();
	return 0;
}

