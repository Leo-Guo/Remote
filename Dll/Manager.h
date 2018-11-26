#pragma once
#include "IOCPClient.h"

class CIOCPClient;
class CManager
{
public:
	CManager(CIOCPClient* ClientObject);
	~CManager();
	virtual VOID CManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
	{

	}
	BOOL CManager::EnableSeDebugPrivilege(IN const CHAR*  PriviledgeName, BOOL IsEnable);
	VOID CManager::WaitForServerDialogOpen();
	VOID CManager::NotifyDialogIsOpen();

	VOID CManager::ShowErrorMessage(CHAR* Class,CHAR* Message)
	{
		MessageBox(NULL, Message, Class, 0);
	}
protected:
	CIOCPClient* m_ClientObject;
	HANDLE  m_EventDlgOpenHandle;

};


