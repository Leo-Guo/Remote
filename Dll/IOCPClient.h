#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <MSTcpIP.h>
#include <iostream>
#include "_CArray.h"
#include "Manager.h"
#pragma comment(lib,"ws2_32.lib")

#define PACKET_FLAG_LENGTH     5
#define PACKET_HEADER_LENGTH   13      //Shine[][]
#define MAX_RECV_BUFFER  0x2000
#define MAX_SEND_BUFFER  0x2000
class CIOCPClient
{
public:
	CIOCPClient();
	BOOL ConnectServer(char * ServerIP, unsigned short Port);
	~CIOCPClient();
	BOOL IsReceiving()
	{
		return m_IsReceiving;
	}
	VOID CIOCPClient::Disconnect();
	static DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);
	int CIOCPClient::OnServerSending(char* BufferData, ULONG BufferLength);
	VOID CIOCPClient::OnServerReceiving(char* BufferData, ULONG BufferLength);
	BOOL CIOCPClient::SendWithSplit(char* BufferData, ULONG BufferLength, ULONG SplitLength);
	VOID CIOCPClient::SetManagerObject(class CManager* Manager)
	{
		m_ManagerObject = Manager;
	}
	VOID CIOCPClient::WaitForEvent()
	{
		WaitForSingleObject(m_EventHandle, INFINITE);
	}
	SOCKET m_ClientSocket;
	HANDLE m_EventHandle;
private:
	BOOL   m_IsReceiving;
	HANDLE m_WorkThreadHandle;

	char     m_PacketFlagData[PACKET_FLAG_LENGTH];
	_CArray  m_OutCompressedBufferData;  //压缩后发送数据
	_CArray  m_InCompressedBufferData;
	_CArray  m_InDeCompressedBufferData;

	CManager* m_ManagerObject;


};

