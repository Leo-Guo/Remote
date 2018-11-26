#pragma once
#include <WinSock2.h>
#include <Mstcpip.h>
#include "_CArray.h"

#pragma comment(lib,"ws2_32.lib")
#define PACKET_FLAG_LENGTH     5
#define PACKET_HEADER_LENGTH   13      //Shine[][]
#define PACKET_LENGTH    0x2000
#define MAX_RECV_BUFFER  0x2000
#define MAX_SEND_BUFFER  0x2000
enum PACKET_TYPE
{
	IOInitialize,
	IORead,
	IOWrite,
	IOIdle
};

class CLock
{
public:
	CLock(CRITICAL_SECTION& CriticalSection)
	{
		m_CriticalSection = &CriticalSection;
		Lock();
	}
	~CLock()
	{
		Unlock();

	}

	void Unlock()
	{
		LeaveCriticalSection(m_CriticalSection);
	}

	void Lock()
	{
		EnterCriticalSection(m_CriticalSection);
	}
private:
	CRITICAL_SECTION*	m_CriticalSection;
};



typedef struct _CONTEXT_OBJECT_
{
	SOCKET   ClientSocket;
	WSABUF   wsaInBuffer;    //接收用户到达的数据   没有内存   与m_BufferData关联
	WSABUF   wsaOutBuffer;   //向客户发送数据       没有内存   与m_OutCompressedBufferData 关联
	_CArray  m_InCompressedBufferData;	          // 接收到的压缩的数据m_BufferData 拷贝到该成员当中
	_CArray  m_InDeCompressedBufferData;          // 接收到的压缩的数据进行的解压后数据  m_InCompressedBufferData  ---》 m_InDeCompressedBufferData
	_CArray  m_OutCompressedBufferData;
	char     m_BufferData[PACKET_LENGTH];         // 是用户到达数据的内存 在OnAccept函数与wsaInBuffer关联
	VOID InitMember()
	{
		ClientSocket = INVALID_SOCKET;
		memset(m_BufferData, 0, sizeof(char)*PACKET_LENGTH);
		memset(&wsaInBuffer, 0, sizeof(WSABUF));
		memset(&wsaOutBuffer, 0, sizeof(WSABUF));
	}
	int				  DlgID;
	HANDLE            DlgHandle;

}CONTEXT_OBJECT, *PCONTEXT_OBJECT;


typedef void (CALLBACK *pfnWindowNotifyProcedure)(PCONTEXT_OBJECT ContextObject);
typedef CList<PCONTEXT_OBJECT> 	ContextObjectList;
class CIOCPServer
{
public:
	CIOCPServer();
	~CIOCPServer();
	BOOL CIOCPServer::StartServer(pfnWindowNotifyProcedure WindowNotifyProcedure,USHORT Port);
	static  DWORD WINAPI ListenThreadProcedure(LPVOID ParameterData);
	BOOL CIOCPServer::InitializeIOCP(VOID);
	void OnAccept();
	PCONTEXT_OBJECT AllocateContext();
	VOID CIOCPServer::RemoveStaleContext(CONTEXT_OBJECT* ContextObject);
	VOID CIOCPServer::MoveContextToFreePoolList(CONTEXT_OBJECT* ContextObject);
	static  DWORD WINAPI WorkThreadProcedure(LPVOID ParameterData);
	BOOL CIOCPServer::PacketHandleIO(PACKET_TYPE PacketType, PCONTEXT_OBJECT ContextObject, DWORD TransferBufferLength);
	BOOL CIOCPServer::OnClientInitializing(PCONTEXT_OBJECT  ContextObject, DWORD TransferBufferLength);
	VOID CIOCPServer::PostRecv(CONTEXT_OBJECT* ContextObject);
	BOOL CIOCPServer::OnClientReceiving(PCONTEXT_OBJECT  ContextObject, DWORD TransferBufferLength);
	VOID CIOCPServer::OnClientPreSending(CONTEXT_OBJECT* ContextObject, PBYTE BufferData, ULONG BufferLength);
	BOOL CIOCPServer::OnClientPostSending(CONTEXT_OBJECT* ContextObject, ULONG TransferBufferLength);
private:
	char     m_PacketFlagData[PACKET_FLAG_LENGTH];
	SOCKET m_ListenSocket;
	HANDLE m_ListenThreadHandle;
	HANDLE m_KillEventHandle;
	HANDLE m_ListenEventHandle;
	CRITICAL_SECTION     m_CriticalSection;
	ContextObjectList    m_ContextFreePoolList;           //   空闲池大哥          CList 构造函数
	ContextObjectList    m_ContextConnectionList;         //   上线用户list大哥
	pfnWindowNotifyProcedure  m_WindowNotifyProcedure;    //   回调窗口的函数指针
	HANDLE               m_CompletionPortHandle;
	ULONG	             m_ThreadPoolMin;
	ULONG				 m_ThreadPoolMax;
	ULONG				 m_ProcessorLowThreadsHold;
	ULONG				 m_ProcessorHighThreadsHold;
	ULONG                m_WorkThreadCount;
	ULONG	             m_CurrentThreadCount;
	ULONG	             m_BusyThreadCount;
	BOOL                 m_TimeToKill;
};




class OVERLAPPEDPLUS
{
public:

	OVERLAPPED			m_ol;  
	PACKET_TYPE			m_PackType;

	OVERLAPPEDPLUS(PACKET_TYPE PackType)
	{
		ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
		m_PackType = PackType;
	}
};