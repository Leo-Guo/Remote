#pragma once
#include "Manager.h"
class CClientInstantManager :
	public CManager
{
public:
	CClientInstantManager(CIOCPClient* ClientObject);
	~CClientInstantManager();
	virtual void CClientInstantManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength);
};

int CALLBACK DialogProcedure(HWND DlgHwnd, unsigned int Msg,
	WPARAM wParam, LPARAM lParam);
VOID OnInitDialog(HWND DlgHwnd);
VOID OnTimerDialog(HWND DlgHwnd);