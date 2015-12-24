#include "stdafx.h"
#include "FileProcessing.h"
#include <iostream>;
using namespace std;


FileProcessing::FileProcessing()
{
	m_partNum = 0;
	m_hFile = INVALID_HANDLE_VALUE;
}


FileProcessing::~FileProcessing()
{
	Close();
}

//закрываем хэндл
void FileProcessing::Close()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

//считываем один символ
int FileProcessing::ReadData(void* buf, size_t maxSize)
{
	DWORD dwSize;
	BOOL bResult = ReadFile(
		m_hFile, buf, maxSize, &dwSize, NULL);
	return dwSize;
}

//открываем файл с указанным именем
bool FileProcessing::Open(string &strFileName)
{
	Close();
	m_hFile = CreateFile(
		strFileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	m_strFileName = strFileName;
	return (m_hFile != INVALID_HANDLE_VALUE);
}

//создаем файл с указанным именем
bool FileProcessing::Create(const string &strFileName)
{
	Close();

	m_hFile = CreateFile(
		strFileName.c_str(),
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	m_strFileName = strFileName;
	return (m_hFile != INVALID_HANDLE_VALUE);
}

bool FileProcessing::Delete(const string &strFileName)
{
	Close();
	bool bResult = DeleteFile(strFileName.c_str());
	return bResult;
}

void FileProcessing::Abort()
{
	Close();
	Delete(m_strFileName);
	cout << "deleted :" << m_strFileName << " file";
}

string FileProcessing::GetFileName()
{
	return m_strFileName;
}

int FileProcessing::GetPartNum()
{
	return m_partNum;
}

//делаем результирующий файл 
bool FileProcessing::WriteData(void *buf, size_t size)
{
	DWORD dNum;
	bool bResult = WriteFile(
		m_hFile,
		buf,
		size,
		&dNum,
		NULL
		);
	if (bResult)
		m_partNum++;
	return bResult;
}