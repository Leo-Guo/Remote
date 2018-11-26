#include "ClientCmdManager.h"
#include "Common.h"



CClientCmdManager::CClientCmdManager(CIOCPClient* ClientObject) :CManager(ClientObject)
{

	printf("Cmd构造函数\r\n");

	m_ThreadHandle = NULL;

	m_CmdModuleBase = LoadLibrary("Cmd.dll");

	if (m_CmdModuleBase == NULL)
	{

	}
	m_SetPipeCommunication = (f1)GetProcAddress(m_CmdModuleBase, "SetPipeCommunication");
	m_UnsetPipeCommunication = (f1)GetProcAddress(m_CmdModuleBase, "UnsetPipeCommunication");
	m_GetPipeData = (f2)GetProcAddress(m_CmdModuleBase, "GetPipeData");
	m_SetPipeData = (f5)GetProcAddress(m_CmdModuleBase, "SetPipeData");
	if (m_SetPipeCommunication == NULL||m_UnsetPipeCommunication == NULL||
		m_GetPipeData==NULL||m_SetPipeData==NULL)
	{
		ShowErrorMessage("CClientCmdManager","导出函数==NULL");

		return;
	}

	if (m_SetPipeCommunication() == FALSE)
	{

	}
	BYTE	IsToken = CLIENT_CMD_MANAGER_REPLY;            //包含头文件 Common.h     
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	WaitForServerDialogOpen();   //该函数是父类();

	m_ThreadHandle = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)ReceiveProcedure, (LPVOID)this, 0, NULL);   //Client 读取管道数据 
}
DWORD CClientCmdManager::ReceiveProcedure(LPVOID ParameterData)
{

	
	DWORD	BufferLength = 0;
	char*   BufferData = NULL;
	CClientCmdManager *This = (CClientCmdManager*)ParameterData;
	while (This->m_IsLoop)
	{
		
		Sleep(1);
		BufferData = This->m_GetPipeData(BufferLength);
		if (BufferLength!=0&&BufferData!=NULL)
		{
			This->m_ClientObject->OnServerSending((char*)BufferData, BufferLength);

			LocalFree(BufferData);

			BufferData = NULL;
		}

	}

	if (BufferData != NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}

	cout << "接收管道数据线程退出" << endl;
	return 0;
}


CClientCmdManager::~CClientCmdManager()
{
	printf("Cmd析构函数\r\n");


	m_IsLoop = FALSE;
	Sleep(1);

	if (m_UnsetPipeCommunication!=NULL)
	{
		m_UnsetPipeCommunication();
	}
	if (m_CmdModuleBase != NULL)
	{
		FreeLibrary(m_CmdModuleBase);
		m_CmdModuleBase = NULL;
	}

	m_SetPipeCommunication = NULL;
	m_UnsetPipeCommunication = NULL;
	m_GetPipeData = NULL;
	m_ThreadHandle = NULL;
}

void CClientCmdManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_GO_ON:
	{

		NotifyDialogIsOpen();
		break;
	}
	default:
	{
		if (m_SetPipeData!=NULL)
		{
			m_SetPipeData((char*)BufferData,BufferLength);
		}
		break;
	}
	}
}

