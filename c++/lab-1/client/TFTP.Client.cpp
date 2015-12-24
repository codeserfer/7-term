// TFTP.Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "FileProcessing.h"



using namespace std;
sockaddr_in ServerAddr;
SOCKET Socket;


void InitClient()
{
	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(1461);
	clientAddr.sin_addr.s_addr = htons(INADDR_ANY);

	bind(Socket, (sockaddr *)&clientAddr, sizeof(clientAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerAddr.sin_port = htons(69);

	int timeout = 5000;
	setsockopt(Socket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&timeout,
		sizeof(timeout));
}

void SendFile(char* FileName)
{
	FileProcessing *file = new FileProcessing();
	int FileNameSize = strlen(FileName);
	int datasize;
	char* recvmsg = new char[128];
	void * sendData = new char[516];
	void* data = malloc(512);

	//запрашиваем файл
	char* request = new char[520];	//запрос
	request[0] = 0, request[1] = 2;					//write
	memcpy(request + 2, FileName, FileNameSize);

	request[2 + FileNameSize] = 0;
	memcpy(request + 3 + FileNameSize, "octet\0", 6);
	sendto(Socket, request, 3 + FileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));
	delete[] request;

	unsigned int Sendedbyte = 0;
	sockaddr_in from;
	int structSize = sizeof(from);
	int SendBlockNumber = 0;
	int AckBlockNumber = 0;
	while (true)
	{
		int RetValue = recvfrom(Socket, recvmsg, 128, 0, (sockaddr*)&from, &structSize);
		if (RetValue > 0)
		{
			int op = recvmsg[1];
			if (op == 4) //команда 
			{
				memcpy(&AckBlockNumber, recvmsg + 3, 1);
				memcpy((void*)((size_t)&AckBlockNumber + 1), recvmsg + 2, 1);
				

				if (AckBlockNumber != SendBlockNumber)
				{
					sendto(Socket, (char*)sendData, datasize + 4, 0, (sockaddr *)&from, sizeof(from));
					continue;
				}

				if (AckBlockNumber == 0)
				{
					string f = FileName;
					file->Open(f);
				}

				DWORD nul = 0;
				memcpy(sendData, &nul, 4);
				short int code = 3;
				SendBlockNumber = AckBlockNumber+1;
				memcpy((void*)((size_t)sendData + 1), &code, 1);
				memcpy((void*)((size_t)sendData), (void*)((size_t)&code + 1), 1);

				memcpy((void*)((size_t)sendData + 2), (void*)((size_t)&SendBlockNumber + 1), 1);
				memcpy((void*)((size_t)sendData + 3), &SendBlockNumber, 1);

				
				
				datasize = file->ReadData(data, 512);
				memcpy((void*)((size_t)sendData + 4), data, datasize);

				Sendedbyte += datasize;
				cout << "Sended block:" << SendBlockNumber << '\n';
				if (datasize != 512)
				{
					cout << "file " << FileName << " sended\n";
					cout << "size " << Sendedbyte << '\n';
					file->Close();
					sendto(Socket, (char*)sendData, datasize+4, 0, (sockaddr *)&from, sizeof(from));
					break;
				}
				sendto(Socket, (char*)sendData, datasize + 4, 0, (sockaddr *)&from, sizeof(from));
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
		else if (RetValue == 0)
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
	sendto(Socket, request, 3 + FileNameSize + 6, 0, (sockaddr *)&ServerAddr, sizeof(ServerAddr));
	delete [] request;

	unsigned int Receivedbyte = 0;
	sockaddr_in from;
	int structSize = sizeof(from);
	int BlockNumber = 1;
	while (true)
	{
		int RetValue = recvfrom(Socket, recvpart, 520, 0, (sockaddr*)&from, &structSize);
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
					sendto(Socket, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));
					continue;
				}
				string f = FileName;
				if (recvBlockNumber == 1)
					file->Create(f);
					
				Receivedbyte += (RetValue - 4);
				
				file->WriteData((void*)(recvpart+4), RetValue - 4);

				memcpy((void*)((size_t)ack + 2), (void*)((size_t)&BlockNumber + 1), 1);
				memcpy((void*)((size_t)ack + 3), &BlockNumber, 1);
				sendto(Socket, (char*)ack, 4, 0, (sockaddr *)&from, sizeof(from));
				cout << "Received block:" << BlockNumber << '\n';
				BlockNumber++;
				if (RetValue - 4 != 512)
				{
					cout << "file " << FileName << " received\n";
					cout << "size " << Receivedbyte << '\n';
					file->Close();
					sendto(Socket,(char*) ack, 4, 0, (sockaddr *)&from, sizeof(from));
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
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	InitClient();
	int input = 0;
	char* fileName = new char[255];
	while (input != 3)
	{
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
			"Enter filename \n";
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
	closesocket(Socket);
	WSACleanup();
	return 0;
}

