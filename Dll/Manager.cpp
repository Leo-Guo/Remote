#include "Manager.h"



CManager::CManager(CIOCPClient* ClientObject)
{
	m_ClientObject = ClientObject;

	m_ClientObject->SetManagerObject(this);

	m_EventDlgOpenHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
}
VOID CManager::WaitForServerDialogOpen()
{
	WaitForSingleObject(m_EventDlgOpenHandle, INFINITE);
	//�����Sleep,��ΪԶ�̴��ڴ�InitDialog�з���COMMAND_NEXT����ʾ��Ҫһ��ʱ��
	Sleep(150);
}

VOID CManager::NotifyDialogIsOpen()
{
	SetEvent(m_EventDlgOpenHandle);
}

BOOL CManager::EnableSeDebugPrivilege(IN const CHAR*  PriviledgeName, BOOL IsEnable)
{
	// ��Ȩ������

	HANDLE  ProcessHandle = GetCurrentProcess();
	HANDLE  TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges = { 0 };
	if (!OpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return FALSE;
	}
	LUID			 v1;
	if (!LookupPrivilegeValue(NULL, PriviledgeName, &v1))		// ͨ��Ȩ�����Ʋ���uID
	{
		CloseHandle(TokenHandle);
		TokenHandle = NULL;
		return FALSE;
	}

	TokenPrivileges.PrivilegeCount = 1;		// Ҫ������Ȩ�޸���
	TokenPrivileges.Privileges[0].Attributes = IsEnable == TRUE ? SE_PRIVILEGE_ENABLED : 0;    // ��̬���飬�����С����Count����Ŀ
	TokenPrivileges.Privileges[0].Luid = v1;


	if (!AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges,
		sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		printf("%d\r\n", GetLastError());
		CloseHandle(TokenHandle);
		TokenHandle = NULL;
		return FALSE;
	}
	CloseHandle(TokenHandle);
	TokenHandle = NULL;
	return TRUE;
}


CManager::~CManager()
{
}
