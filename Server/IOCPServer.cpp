#include "stdafx.h"
#include "IOCPServer.h"
#include "zconf.h"
#include "zlib.h"

CIOCPServer::CIOCPServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		return;
	}
	memcpy(m_PacketFlagData, "Shine", PACKET_FLAG_LENGTH);
	m_ListenSocket	     = INVALID_SOCKET;
	m_ListenThreadHandle = NULL;
	m_KillEventHandle    = NULL;
	m_ListenEventHandle  = WSA_INVALID_EVENT;   //�����¼�(�¼�ģ��)
	InitializeCriticalSection(&m_CriticalSection);
	m_CompletionPortHandle = NULL;

	m_ThreadPoolMin = 0;
	m_ThreadPoolMax = 0;
	m_ProcessorHighThreadsHold = 0;
	m_ProcessorLowThreadsHold = 0;
	m_WorkThreadCount = 0;
	m_CurrentThreadCount = 0;
	m_BusyThreadCount = 0;

	m_TimeToKill = FALSE;

}


CIOCPServer::~CIOCPServer()
{
	Sleep(10);
	SetEvent(m_KillEventHandle);


	WaitForSingleObject(m_ListenThreadHandle, INFINITE);


	if (m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	if (m_ListenEventHandle != WSA_INVALID_EVENT)
	{
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;
	}

	if (m_CompletionPortHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_CompletionPortHandle);
		m_CompletionPortHandle = INVALID_HANDLE_VALUE;

	}

	if (m_KillEventHandle != NULL)
	{
		CloseHandle(m_KillEventHandle);
		m_KillEventHandle = NULL;
	}

	m_ThreadPoolMin = 0;
	m_ThreadPoolMax = 0;
	m_ProcessorHighThreadsHold = 0;
	m_ProcessorLowThreadsHold = 0;
	m_WorkThreadCount = 0;
	m_CurrentThreadCount = 0;
	m_BusyThreadCount = 0;
	DeleteCriticalSection(&m_CriticalSection);
	m_TimeToKill = TRUE;
	WSACleanup();
}


BOOL CIOCPServer::StartServer(pfnWindowNotifyProcedure WindowNotifyProcedure,USHORT Port)
{


	m_KillEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (m_KillEventHandle == NULL)
	{
		return FALSE;
	}

	//�����첽�׽���(����)
	
	//�������Ա���� 
	m_WindowNotifyProcedure = WindowNotifyProcedure;

	int Result = 0;
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);     //���������׽���

	if (m_ListenSocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	m_ListenEventHandle = WSACreateEvent();   
	if (m_ListenEventHandle==WSA_INVALID_EVENT)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return FALSE;
	}

	//����
	Result = WSAEventSelect(m_ListenSocket,	//�������׽������¼����й���������FD_ACCEPT������
		m_ListenEventHandle,
		FD_ACCEPT|FD_CLOSE); 

	if (Result ==SOCKET_ERROR)
	{			
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		return FALSE;
	}
	SOCKADDR_IN	ServerAddress;
	ServerAddress.sin_port = htons(Port);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY;  
	Result = bind(m_ListenSocket,
		(sockaddr*)&ServerAddress,
		sizeof(ServerAddress));

	if (Result == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;
		return FALSE;
	}
	Result = listen(m_ListenSocket, SOMAXCONN);
	if (Result == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;

		return FALSE;
	}
	//���������߳�
	m_ListenThreadHandle =
		(HANDLE)CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)ListenThreadProcedure,
			(void*)this,	      //��Thread�ص���������this �������ǵ��̻߳ص��������еĳ�Ա    
			0,
			NULL);
	if (m_ListenThreadHandle == INVALID_HANDLE_VALUE)
	{
		closesocket(m_ListenSocket);

		m_ListenSocket = INVALID_SOCKET;
	
		WSACloseEvent(m_ListenEventHandle);
		m_ListenEventHandle = WSA_INVALID_EVENT;
		return FALSE;
	}
	
	//�������ݵ�IOCP
	InitializeIOCP();

}


DWORD CIOCPServer:: ListenThreadProcedure(LPVOID ParameterData)  
{
	CIOCPServer* This = (CIOCPServer*)ParameterData;
	int EventIndex = 0;
	WSANETWORKEVENTS NetWorkEvents;
	while (1)
	{
		EventIndex = WaitForSingleObject(This->m_KillEventHandle, 100);
		EventIndex = EventIndex - WAIT_OBJECT_0;
		if (EventIndex == 0)
		{
			break;
		}

		DWORD Result;
		Result = WSAWaitForMultipleEvents(1,
			&This->m_ListenEventHandle,
			FALSE,  //bwaifall
			100,
			FALSE);

		if (Result == WSA_WAIT_TIMEOUT)
		{
			continue;   //���¼�û������
		}

		Result = WSAEnumNetworkEvents(This->m_ListenSocket,
			//����¼����� ���Ǿͽ����¼�ת����һ�������¼� ���� �ж�
			This->m_ListenEventHandle,
			&NetWorkEvents);

		if (Result == SOCKET_ERROR)   //������
		{
			break;
		}
		if (NetWorkEvents.lNetworkEvents & FD_ACCEPT)   //�����׽�������   --->
		{
			if (NetWorkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				//�����һ�������������Ǿͽ���OnAccept()�������д���
			    This->OnAccept();   //Accept  ����  ����OK ����ͨ��Client


	
			}
			else
			{
				break;
			}

		}

	}

	return 0;
}
BOOL CIOCPServer::InitializeIOCP(VOID)
{
	m_CompletionPortHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);



	if (m_CompletionPortHandle == NULL)   //������ɶ˿�
	{

		return FALSE;
	}

	if (m_CompletionPortHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	SYSTEM_INFO si;
	GetSystemInfo(&si);  //���PC���м���

	m_ThreadPoolMin = 1;
    m_ThreadPoolMax = si.dwNumberOfProcessors * 2;
	
	m_ProcessorLowThreadsHold = 10;
	m_ProcessorHighThreadsHold = 75;

	ULONG WorkThreadCount = 2;

	HANDLE WorkThreadHandle = NULL;
	for (int i = 0; i < WorkThreadCount; i++)
	{
		WorkThreadHandle = (HANDLE)CreateThread(NULL,	             //���������߳�Ŀ���Ǵ���Ͷ�ݵ���ɶ˿��е�����			
			0,
			(LPTHREAD_START_ROUTINE)WorkThreadProcedure,
			(void*)this,
			0,
			NULL);
		if (WorkThreadHandle == NULL)
		{
			CloseHandle(m_CompletionPortHandle);
			return FALSE;
		}
		m_WorkThreadCount++;
		CloseHandle(WorkThreadHandle);
	}
	return TRUE;
}
DWORD CIOCPServer::WorkThreadProcedure(LPVOID ParameterData)
{


	CIOCPServer* This = (CIOCPServer*)(ParameterData);

	HANDLE   CompletionPortHandle = This->m_CompletionPortHandle;
	DWORD    TransferBufferLength = 0;
	PCONTEXT_OBJECT  ContextObject = NULL;
	LPOVERLAPPED     ol = NULL;   //������ǰͶ�ݵ�Overlapped�ĵ�ַ
	OVERLAPPEDPLUS*  OverlappedPlus = NULL;
	ULONG            BusyThread = 0;
	
	
	BOOL             v1 = FALSE;
	InterlockedIncrement(&This->m_CurrentThreadCount);   //ԭ����
	InterlockedIncrement(&This->m_BusyThreadCount);

	while (This->m_TimeToKill == FALSE)
	{

		InterlockedDecrement(&This->m_BusyThreadCount);     //0   16
		BOOL ReturnValue = GetQueuedCompletionStatus(
			CompletionPortHandle,
			&TransferBufferLength,
			(LPDWORD)&ContextObject,   //���Key
			&ol, 60000);

		/*
		OverlappedPlus = new Object[Pack_Type Overlapped]
		&OverlappedPlus->Overlapped
		ReturnValue = PostQueuedCompletionStatus(m_CompletionPortHandle,
		0, (ULONG_PTR)ContextObject, &OverlappedPlus->m_ol);     //  �����߳�  һ��֪ͨ
		*/



		DWORD LastError = GetLastError();
		OverlappedPlus = CONTAINING_RECORD(ol, OVERLAPPEDPLUS, m_ol);  //��Ҫ
		BusyThread = InterlockedIncrement(&This->m_BusyThreadCount);   //1

		if (!ReturnValue && LastError != WAIT_TIMEOUT)   //���Է����׻��Ʒ����˹ر�                    
		{
			if (ContextObject && This->m_TimeToKill == FALSE &&TransferBufferLength == 0)
			{
				
				This->RemoveStaleContext(ContextObject);
			}
			continue;
		}
		if (!v1)
		{
			//����һ���µ��̵߳��̵߳��̳߳�
			if (BusyThread == This->m_CurrentThreadCount)
			{

				if (BusyThread < This->m_ThreadPoolMax)
				{
					{

						if (ContextObject != NULL)
						{

							HANDLE ThreadHandle = (HANDLE)CreateThread(NULL,
								0,
								(LPTHREAD_START_ROUTINE)WorkThreadProcedure,
								(void*)This,
								0,
								NULL);
							InterlockedIncrement(&This->m_WorkThreadCount);  //16   17

							CloseHandle(ThreadHandle);
						}

					}
				}
			}

			if (!ReturnValue && LastError == WAIT_TIMEOUT)
			{
				if (ContextObject == NULL)
				{
					{
						if (This->m_CurrentThreadCount > This->m_ThreadPoolMin)
						{
							break;
						}
					}
					v1 = TRUE;
				}
			}
		}
		if (!v1)  //�ɹ� ����õ����(���� PostQueuedCompletionStatus �ͻ�WSARecv )
		{
			//ReturnValue  GetQueuedCompletionStatus����ֵ  
			if (ReturnValue && OverlappedPlus != NULL && ContextObject != NULL)
			{
				try
				{
					//����֮ǰͶ�ݵ�û���������
					This->PacketHandleIO(OverlappedPlus->m_PackType, ContextObject, TransferBufferLength);

					//û���ͷ��ڴ�
					ContextObject = NULL;

				}
				catch (...) {}
			}
		}

		if (OverlappedPlus)
		{
			delete OverlappedPlus;
			OverlappedPlus = NULL;
		}

	}
	InterlockedDecrement(&This->m_WorkThreadCount);
	InterlockedDecrement(&This->m_CurrentThreadCount);
	InterlockedDecrement(&This->m_BusyThreadCount);

	return 0;
}
BOOL CIOCPServer::PacketHandleIO(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject, DWORD TransferBufferLength)   //�ڹ����߳��б�����
{
	BOOL v1 = FALSE;

	if (IOInitialize == PacketType)
	{
		v1 = OnClientInitializing(ContextObject, TransferBufferLength);  //OnAccept ������Ͷ�ݵ�����
	}

	if (IORead == PacketType)   //WsaResv   ���ض˴��͵�����
	{
		v1 = OnClientReceiving(ContextObject, TransferBufferLength);  //
	}

	if (IOWrite == PacketType)  //WsaSend  ���ض��򱻿ض˴��͵�����
	{
		v1 = OnClientPostSending(ContextObject, TransferBufferLength);
	}
	return v1;
}
BOOL CIOCPServer::OnClientInitializing(PCONTEXT_OBJECT  ContextObject, DWORD TransferBufferLength)
{
/*	int i = 0;
	for (i=0;i<3;i++)
	{
	int ReturnValue = send(ContextObject->ClientSocket,"HelloWorld",strlen("HelloWorld"),0);
	}
*/
//	MessageBox(NULL,"HelloInit","HelloInit",0);

	return TRUE;
}
BOOL CIOCPServer::OnClientReceiving(PCONTEXT_OBJECT  ContextObject, DWORD TransferBufferLength)
{

	CLock Object(m_CriticalSection);
	try
	{

		if (TransferBufferLength == 0)    //�Է��ر����׽���
		{

			MessageBox(NULL, "�ر��׽���", "�ر��׽���", 0);
			RemoveStaleContext(ContextObject);
			return FALSE;
		}


		ContextObject->m_InCompressedBufferData.WriteArray((PBYTE)ContextObject->m_BufferData, TransferBufferLength);   //��ѹ����������
																													    //�����յ������ݿ����������Լ����ڴ���wsabuff    8192


		while (ContextObject->m_InCompressedBufferData.GetArrayLength() >PACKET_HEADER_LENGTH)          //�鿴���ݰ��������
		{
			char v10[PACKET_FLAG_LENGTH] = { 0 }; //Shine


			CopyMemory(v10, ContextObject->m_InCompressedBufferData.GetArray(), PACKET_FLAG_LENGTH);



			if (memcmp(m_PacketFlagData, v10, PACKET_FLAG_LENGTH) != 0)
			{
				throw "Bad Buffer";
			}

			ULONG PackTotalLength = 0;
			CopyMemory(&PackTotalLength, ContextObject->m_InCompressedBufferData.GetArray(PACKET_FLAG_LENGTH),  //[Shine[][]]
				sizeof(ULONG));

			//ȡ�����ݰ����ܳ�
			if (PackTotalLength && (ContextObject->m_InCompressedBufferData.GetArrayLength()) >= PackTotalLength)
			{
				ULONG DeCompressedLength = 0;  //[Shine][ѹ���ĳ���+13][û��ѹ���ĳ���][li]
				
				//[Shine][?1+13][?2][                      ]
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)v10, PACKET_FLAG_LENGTH);

				//[?1+13][?2][                      ]   v10[] = "Shine"
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)&PackTotalLength, sizeof(ULONG));

				//[?2][                      ]   v10[] = "Shine"  PackTotalLength = ?1 + 13
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)&DeCompressedLength, sizeof(ULONG));

				//[HelloWorld                ]   v10[] = "Shine"  PackTotalLength = ?1   DeCompressedLength = ?2
			
				
				ULONG CompressedLength = PackTotalLength - PACKET_HEADER_LENGTH;
				PBYTE CompressedData = new BYTE[CompressedLength];       //û�н�ѹ���ݳ���

				PBYTE DeCompressedData = new BYTE[DeCompressedLength];  //��ѹ�������ݳ��� 

				if (CompressedData == NULL || DeCompressedData == NULL)
				{
					throw "Bad Allocate";

				}
				ContextObject->m_InCompressedBufferData.ReadArray(CompressedData, CompressedLength); //�����ݰ���ǰ��Դ����û�н�ѹ��ȡ��pData   448


																						
				int	ReturnValue = uncompress(DeCompressedData, &DeCompressedLength, CompressedData, CompressedLength);     //��ѹ����

				if (ReturnValue == Z_OK)
				{
					ContextObject->m_InDeCompressedBufferData.ClearArray();
					ContextObject->m_InCompressedBufferData.ClearArray();
					ContextObject->m_InDeCompressedBufferData.WriteArray(DeCompressedData, DeCompressedLength);


					delete[] CompressedData;
					delete[] DeCompressedData;
					m_WindowNotifyProcedure(ContextObject);  //֪ͨ����   
				}
				else
				{
					delete[] CompressedData;
					delete[] DeCompressedData;
					throw "Bad Buffer";
				}



			}
			else
			{
				break;
			}
		}

		PostRecv(ContextObject);   //Ͷ���µĽ������ݵ�����
	}
	catch (...)
	{
		ContextObject->m_InCompressedBufferData.ClearArray();
		ContextObject->m_InDeCompressedBufferData.ClearArray();

		PostRecv(ContextObject);
	}

	return TRUE;

}
void CIOCPServer::OnAccept()
{
	int			Result = 0;	
	
	SOCKET		ClientSocket = INVALID_SOCKET;
	SOCKADDR_IN	ClientAddress = { 0 };                  //��������û���IP �˿�
	int			ClientAddressLength = sizeof(SOCKADDR_IN);;

	ClientSocket = accept(m_ListenSocket,               //ͬ������������Ϣ
		(sockaddr*)&ClientAddress,
		&ClientAddressLength);                         //ͨ�����ǵļ����׽���������һ����֮�ź�ͨ�ŵ��׽���
	

	if (ClientSocket == SOCKET_ERROR)
	{
		Result = WSAGetLastError();
		if (Result != WSAEWOULDBLOCK)             //����
		{
			return;
		}

		return;
	}

 
	//����������Ϊÿһ��������ź�ά����һ����֮���������ݽṹ������Ϊ�û������±�����
	PCONTEXT_OBJECT ContextObject = AllocateContext();

	if (ContextObject == NULL)
	{

		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
		return;
	}


	//��Ա��ֵ
	ContextObject->ClientSocket = ClientSocket;


	//�����ڴ�(wsaRecv ���ڽ��ܿͻ�����������)
	ContextObject->wsaInBuffer.buf = (char*)ContextObject->m_BufferData;
	ContextObject->wsaInBuffer.len = sizeof(ContextObject->m_BufferData);


	HANDLE Handle = CreateIoCompletionPort((HANDLE)ClientSocket, m_CompletionPortHandle, (ULONG_PTR)ContextObject, 0);   //4(ָ�� ֪ͨ)


	if (Handle != m_CompletionPortHandle)
	{

		delete ContextObject;
		ContextObject = NULL;

		if (ClientSocket != INVALID_SOCKET)
		{
			closesocket(ClientSocket);
			ClientSocket = INVALID_SOCKET;
		}

		return;
	}



	//�����׽��ֵ�ѡ� Set KeepAlive ����������� SO_KEEPALIVE 
	//�������Ӽ��Է������Ƿ�������2Сʱ���ڴ��׽ӿڵ���һ����û
	//�����ݽ�����TCP���Զ����Է� ��һ�����ִ��
/*	m_KeepLiveTime = 3;
	BOOL bOk = TRUE;
	if (setsockopt(ContextObject->ClientSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOk, sizeof(bOk)) != 0)
	{
	}
	//���ó�ʱ��ϸ��Ϣ
	tcp_keepalive	KeepAlive;
	KeepAlive.onoff = 1; // ���ñ���
	KeepAlive.keepalivetime = m_KeepLiveTime;       //����3����û�����ݣ��ͷ���̽���
	KeepAlive.keepaliveinterval = 1000 * 10;         //���Լ��Ϊ10�� Resend if No-Reply
	WSAIoctl
	(
		ContextObject->ClientSocket,
		SIO_KEEPALIVE_VALS,
		&KeepAlive,
		sizeof(KeepAlive),
		NULL,
		0,
		(unsigned long *)&bOk,
		0,
		NULL
	);
	*/
	//����������ʱ����������ͻ������߻�ϵ�ȷ������Ͽ����������������û������SO_KEEPALIVEѡ�
	//���һֱ���ر�SOCKET����Ϊ�ϵĵ�������Ĭ������Сʱʱ��̫�����������Ǿ��������ֵ
	CLock Object(m_CriticalSection);
	m_ContextConnectionList.AddTail(ContextObject);     //���뵽���ǵ��ڴ��б���
	
											
	OVERLAPPEDPLUS	*OverlappedPlus = new OVERLAPPEDPLUS(IOInitialize);   //ע��������ص�IO������ �û���������

	//[IOInitialize][Overlapped(Event)]

	//�Լ�����ɶ˿�Ͷ������
	BOOL ReturnValue = FALSE;
	ReturnValue = PostQueuedCompletionStatus(m_CompletionPortHandle,
		0, (ULONG_PTR)ContextObject, &OverlappedPlus->m_ol);     //  �����߳�  һ��֪ͨ

	//��Ϊ���ǽ��ܵ���һ���û����ߵ�������ô���Ǿͽ��������͸����ǵ���ɶ˿� �����ǵĹ����̴߳�����

	//ERROR_IO_PENDING  ��ȷ�Ĵ���
	if ((!ReturnValue && GetLastError() != ERROR_IO_PENDING))  //���Ͷ��ʧ��
	{
		RemoveStaleContext(ContextObject);
		return;
	}
    //Ͷ��WsaRecv����
	PostRecv(ContextObject);    //WSARecv
}
VOID CIOCPServer::PostRecv(CONTEXT_OBJECT* ContextObject)
{
	OVERLAPPEDPLUS * OverlappedPlus = new OVERLAPPEDPLUS(IORead);    //�����ǵĸ����ߵ��û���Ͷ��һ���������ݵ�����  ����û��ĵ�һ�����ݰ�����Ҳ�;��Ǳ��ض˵ĵ�½���󵽴����ǵĹ����߳̾�
																	 //����Ӧ	������ProcessIOMessage����
	DWORD			ReturnLength;
	ULONG			Flags = MSG_PARTIAL;
	int ReturnValue = WSARecv(ContextObject->ClientSocket,
		&ContextObject->wsaInBuffer,   //�������ݵ��ڴ�
		1,
		&ReturnLength,    // TransferBufferLength
		&Flags,
		&OverlappedPlus->m_ol,
		NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		RemoveStaleContext(ContextObject);
	}
}
VOID CIOCPServer::OnClientPreSending(CONTEXT_OBJECT* ContextObject, PBYTE BufferData, ULONG BufferLength)
{
	if (ContextObject == NULL)
	{
		return;
	}

	try
	{
		if (BufferLength > 0)
		{
			unsigned long	CompressedLength = (double)BufferLength * 1.001 + 12;
			LPBYTE			CompressedData = new BYTE[CompressedLength];
			int	ReturnValue = compress(CompressedData, &CompressedLength, (LPBYTE)BufferData, BufferLength);

			if (ReturnValue != Z_OK)
			{
				delete[] CompressedData;
				return;
			}
			ULONG PackTotalLength = CompressedLength + PACKET_HEADER_LENGTH;

			ContextObject->m_OutCompressedBufferData.WriteArray((LPBYTE)m_PacketFlagData, PACKET_FLAG_LENGTH);

			ContextObject->m_OutCompressedBufferData.WriteArray((PBYTE)&PackTotalLength, sizeof(ULONG));

			ContextObject->m_OutCompressedBufferData.WriteArray((PBYTE)&BufferLength, sizeof(ULONG));

			ContextObject->m_OutCompressedBufferData.WriteArray(CompressedData, CompressedLength);

			delete[] CompressedData;
		}
		OVERLAPPEDPLUS* OverlappedPlus = new OVERLAPPEDPLUS(IOWrite);
		PostQueuedCompletionStatus(m_CompletionPortHandle, 0, (DWORD)ContextObject, &OverlappedPlus->m_ol);
	}
	catch (...) {}
}
BOOL CIOCPServer::OnClientPostSending(CONTEXT_OBJECT* ContextObject, ULONG TransferBufferLength)
{
	try
	{
		DWORD Flags = MSG_PARTIAL;
		ContextObject->m_OutCompressedBufferData.RemoveComletedArray(TransferBufferLength);             //����ɵ����ݴ����ݽṹ��ȥ��
		if (ContextObject->m_OutCompressedBufferData.GetArrayLength() == 0)
		{
			ContextObject->m_OutCompressedBufferData.ClearArray();
			return true;		                             //�ߵ�����˵�����ǵ�����������ȫ����
		}
		else
		{
			OVERLAPPEDPLUS * OverlappedPlus = new OVERLAPPEDPLUS(IOWrite);           //����û�����  ���Ǽ���Ͷ�� ��������
			ContextObject->wsaOutBuffer.buf = (char*)ContextObject->m_OutCompressedBufferData.GetArray();
			ContextObject->wsaOutBuffer.len = ContextObject->m_OutCompressedBufferData.GetArrayLength();                 //���ʣ������ݺͳ���    
			int ReturnValue = WSASend(ContextObject->ClientSocket,
				&ContextObject->wsaOutBuffer,
				1,
				&ContextObject->wsaOutBuffer.len,
				Flags,
				&OverlappedPlus->m_ol,
				NULL);
			if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				RemoveStaleContext(ContextObject);
			}
		}
	}
	catch (...) {}
	return FALSE;
}
PCONTEXT_OBJECT CIOCPServer::AllocateContext()
{
	
	//�ڴ��
	PCONTEXT_OBJECT ContextObject = NULL;
	CLock Object(m_CriticalSection);   //�����ٽ���
	if (m_ContextFreePoolList.IsEmpty() == FALSE)
	{
		ContextObject = m_ContextFreePoolList.RemoveHead();   //�ڴ��ȡ�ڴ�
	}
	else
	{
		ContextObject = new CONTEXT_OBJECT;
	}
	if (ContextObject != NULL)
	{
		ContextObject->InitMember();   // ��ʼ����Ա����
	}
	return ContextObject;

}
VOID CIOCPServer::RemoveStaleContext(CONTEXT_OBJECT* ContextObject)
{
	CLock Object(m_CriticalSection);
	if (m_ContextConnectionList.Find(ContextObject))    //���ڴ��в��Ҹ��û������±��������ݽṹ
	{
		CancelIo((HANDLE)ContextObject->ClientSocket);  //ȡ���ڵ�ǰ�׽��ֵ��첽IO   ��ǰ��δ��ɵ��첽����ȫ������ȡ��   
		closesocket(ContextObject->ClientSocket);      //�ر��׽���
		ContextObject->ClientSocket = INVALID_SOCKET;
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)ContextObject))   //�жϻ���û���첽IO�����ڵ�ǰ�׽�����
		{
			Sleep(0);
		}
		MoveContextToFreePoolList(ContextObject);  //�����ڴ�ṹ�������ڴ��
	}
}
VOID CIOCPServer::MoveContextToFreePoolList(CONTEXT_OBJECT* ContextObject)
{

	CLock Object(m_CriticalSection);

	POSITION Pos = m_ContextConnectionList.Find(ContextObject);
	if (Pos)
	{

		ContextObject->m_InCompressedBufferData.ClearArray();
		ContextObject->m_InDeCompressedBufferData.ClearArray();
		ContextObject->m_OutCompressedBufferData.ClearArray();

		memset(ContextObject->m_BufferData, 0, PACKET_HEADER_LENGTH);
		m_ContextFreePoolList.AddTail(ContextObject);                         //�������ڴ��
		m_ContextConnectionList.RemoveAt(Pos);                                //���ڴ�ṹ���Ƴ�

	}
}