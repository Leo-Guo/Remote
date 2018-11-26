#pragma once
#include "Manager.h"
#include "Common.h"



class CClientCmdManager :
	public CManager
{
public:
	CClientCmdManager(CIOCPClient* ClientObject);
	~CClientCmdManager();
	virtual void CClientCmdManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);

	static  DWORD WINAPI  ReceiveProcedure(LPVOID ParameterData);
private:
	HMODULE m_CmdModuleBase;
	f1 m_SetPipeCommunication;
	f1 m_UnsetPipeCommunication;
	f2 m_GetPipeData;
	f5 m_SetPipeData;
	HANDLE  m_ThreadHandle;
	BOOL    m_IsLoop;
};

