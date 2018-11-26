#pragma once
#include "Manager.h"
#include "Common.h"



class CClientWindowManager :
	public CManager
{
public:
	CClientWindowManager(CIOCPClient* ClientObject);
	~CClientWindowManager();
	BOOL CClientWindowManager::SendClientWindowList();
	virtual void CClientWindowManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
private:
	HMODULE m_WindowModuleBase = NULL;

	f2 m_GetWindowList;
	f3 m_SetWindowStatus;
	f3 m_PostWindowMessage;
};

