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
	//必须的Sleep,因为远程窗口从InitDialog中发送COMMAND_NEXT到显示还要一段时间
	Sleep(150);
}

VOID CManager::NotifyDialogIsOpen()
{
	SetEvent(m_EventDlgOpenHandle);
}

BOOL CManager::EnableSeDebugPrivilege(IN const CHAR*  PriviledgeName, BOOL IsEnable)
{
	// 打开权限令牌

	HANDLE  ProcessHandle = GetCurrentProcess();
	HANDLE  TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges = { 0 };
	if (!OpenProcessToken(ProcessHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
	{
		return FALSE;
	}
	LUID			 v1;
	if (!LookupPrivilegeValue(NULL, PriviledgeName, &v1))		// 通过权限名称查找uID
	{
		CloseHandle(TokenHandle);
		TokenHandle = NULL;
		return FALSE;
	}

	TokenPrivileges.PrivilegeCount = 1;		// 要提升的权限个数
	TokenPrivileges.Privileges[0].Attributes = IsEnable == TRUE ? SE_PRIVILEGE_ENABLED : 0;    // 动态数组，数组大小根据Count的数目
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
