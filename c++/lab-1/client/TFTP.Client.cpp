#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "FileProcessing.h"
#include <time.h>
#include "TFTP.h"
#include  "Exception.h"



using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	
	TFTP* tftp = new TFTP();

	try
	{

		int input = -1;
		char* fileName = new char[255];
		while (input != 0)
		{

			tftp->InitClient();

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

				tftp->RecvFile(fileName);
				break;
			case 2:
				cout << "Input filename \n";
				cin >> fileName;

				tftp->SendFile(fileName);
				break;

			default:
				break;
			}
		}

		delete[] fileName;
	}
	catch (Exception e)
	{
		cout << &e << endl;
	}

	tftp->CloseSocket();

	return 0;
}

