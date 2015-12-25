#pragma once

#include <winsock.h>
#include "FileProcessing.h"
#include <time.h>
#include <iostream>
#include "Exception.h"


#define PORT 69
#define TIMEOUT 5000
#define HOST "127.0.0.1"
#define DATA_COMMAND 3

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

