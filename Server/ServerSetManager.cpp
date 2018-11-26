#include "stdafx.h"
#include "ServerSetManager.h"


CServerSetManager::CServerSetManager()
{

	InitConfigFile();
}

CServerSetManager::~CServerSetManager()
{

	int a = 0;
}
BOOL CServerSetManager::InitConfigFile()
{
	
	CHAR  FileFullPathData[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, FileFullPathData,MAX_PATH);   

	CHAR* v1 = NULL;


	//Z:\2018Remote\Server\Debug\Server.exe
	v1 = strstr(FileFullPathData, ".");   
	if (v1 != NULL)
	{
		*v1 = '\0';
		strcat(FileFullPathData, ".ini");
	}
	//Z:\2018Remote\Server\Debug\Server.ini
	

	//���ļ�
	HANDLE FileHandle = CreateFile(FileFullPathData, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE ,   //��ռ
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN, NULL);   //ͬ��  �첽   

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	m_FileFullPathData = FileFullPathData;   

	ULONG HighLength = 0;
	ULONG LowLength = GetFileSize(FileHandle,&HighLength);  //����ļ��Ĵ�С
	if (LowLength > 0||HighLength>0)   //���ǿ��ļ�  
	{
		CloseHandle(FileHandle);

		return FALSE;
	}
	WritePrivateProfileString("Settings", "Port", "2356", m_FileFullPathData);
	WritePrivateProfileString("Settings", "MaxConnections", "10", m_FileFullPathData);
	CloseHandle(FileHandle);
	return TRUE;

}
int CServerSetManager::GetInt(CString MainKey, CString SubKey)   //"Setting" 
{
	return ::GetPrivateProfileInt(MainKey, SubKey, 0, m_FileFullPathData);   //  InI�ļ��ľ���·��
}
BOOL CServerSetManager::SetInt(CString MainKey, CString SubKey, int BufferData)
{
	CString v1;
	v1.Format("%d", BufferData);
	return ::WritePrivateProfileString(MainKey, SubKey, v1, m_FileFullPathData);
}



