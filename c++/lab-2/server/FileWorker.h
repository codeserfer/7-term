#pragma once
#include <Windows.h>
#include <string>
using namespace std;
class FileWorker
{
public:
	FileWorker();
	~FileWorker();
	bool Open(string &strFileName);
	bool Create(const string &strFileName);
	bool Delete(const string &strFileName);
	int ReadData(void *buf, size_t maxSize);
	bool WriteData(void *buf, size_t size);
	void Abort();
	void Close();

	string GetFileName();
	int GetPartNum();

private:
	HANDLE m_hFile;
	int m_partNum = 0;
	string m_strFileName = "";
};