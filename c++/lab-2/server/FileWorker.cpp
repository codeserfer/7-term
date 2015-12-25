#include "stdafx.h"
#include "FileWorker.h"
#include <iostream>;

using namespace std;


FileWorker::FileWorker()
{
	m_partNum = 0;
	m_hFile = INVALID_HANDLE_VALUE;
}


FileWorker::~FileWorker()
{
	Close();
}

// закрываем хэндл
void FileWorker::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}


int FileWorker::ReadData(void* buf, size_t maxSize)
{
	DWORD dwSize;
	BOOL bResult = ReadFile(m_hFile, buf, maxSize, &dwSize, NULL);
	return dwSize;
}

// открываем файл с указанным именем
bool FileWorker::Open(string &strFileName)
{
	Close();
	m_hFile = CreateFileA(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	m_strFileName = strFileName;
	return (m_hFile != INVALID_HANDLE_VALUE);
}

// создаем файл с указанным именем
bool FileWorker::Create(const string &strFileName)
{
	Close();

	m_hFile = CreateFileA(strFileName.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	m_strFileName = strFileName;
	return (m_hFile != INVALID_HANDLE_VALUE);
}

bool FileWorker::Delete(const string &strFileName)
{
	Close();
	bool bResult = DeleteFileA(strFileName.c_str());
	return bResult;
}

void FileWorker::Abort()
{
	Close();
	Delete(m_strFileName);
	cout << "deleted :" << m_strFileName << " file";
}

string FileWorker::GetFileName()
{
	return m_strFileName;
}

int FileWorker::GetPartNum()
{
	return m_partNum;
}

// делаем результирующий файл 
bool FileWorker::WriteData(void *buf, size_t size)
{
	DWORD dNum;
	bool bResult = WriteFile(m_hFile, buf, size, &dNum, nullptr);

	if (bResult)
		m_partNum++;

	return bResult;
}