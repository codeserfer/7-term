#pragma once

#include <winsock.h>
#include "FileProcessing.h"
#include <time.h>
#include <iostream>


#define PORT 69
#define TIMEOUT 5000

class TFTP
{
	sockaddr_in ServerAddr;
	SOCKET s;

public:
	TFTP();
	~TFTP();
	void InitClient();
	void SendFile(char*);
	void RecvFile(char*);
	void CloseSocket();
};

