// WindowManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "WindowManagerDlg.h"
#include "afxdialogex.h"


// CWindowManagerDlg 对话框

IMPLEMENT_DYNAMIC(CWindowManagerDlg, CDialog)

CWindowManagerDlg::CWindowManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject, ULONG RunMode)
	: CDialog(IDD_DIALOG_WINDOW_MANAGER, pParent)
{
	m_RunMode = RunMode;

	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_WindowModuleBase = LoadLibrary("Window.dll");
	m_GetWindowList     = (pfnGetWindowList)GetProcAddress(m_WindowModuleBase, "GetWindowList");
	m_SetWindowStatus   = (pfnSetWindowStatus)GetProcAddress(m_WindowModuleBase, "SetWindowStatus");
	m_PostWindowMessage = (pfnPostWindowMessage)GetProcAddress(m_WindowModuleBase, "PostWindowMessage");
	m_RestartForbidWindow = (pfnRestartForbidWindow)GetProcAddress(m_WindowModuleBase, "RestartForbidWindow");
	m_FixWindowTitleBar = (pfnFixWindowTitleBar)GetProcAddress(m_WindowModuleBase, "FixWindowTitleBar");
}

CWindowManagerDlg::~CWindowManagerDlg()
{

	if (m_WindowModuleBase != NULL)
	{
		FreeLibrary(m_WindowModuleBase);
		m_WindowModuleBase = NULL;
	}
}

void CWindowManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WINDOW_SERVER, m_CListCtrl_Window_Server);
	DDX_Control(pDX, IDC_LIST_WINDOW_CLIENT, m_CListCtrl_Window_Client);
}


BEGIN_MESSAGE_MAP(CWindowManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOWSERVER_REFRESH, &CWindowManagerDlg::OnWindowserverRefresh)
	ON_COMMAND(ID_WINDOWSERVER_CLOSE, &CWindowManagerDlg::OnWindowserverClose)
	ON_COMMAND(ID_WINDOWSERVER_HIDE, &CWindowManagerDlg::OnWindowserverHide)
	ON_COMMAND(ID_WINDOWSERVER_RECOVER, &CWindowManagerDlg::OnWindowserverRecover)
	ON_COMMAND(ID_WINDOWSERVER_MAX, &CWindowManagerDlg::OnWindowserverMax)
	ON_COMMAND(ID_WINDOWSERVER_MIN, &CWindowManagerDlg::OnWindowserverMin)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WINDOW_SERVER, &CWindowManagerDlg::OnNMRClickListWindowServer)
	ON_COMMAND(ID_WINDOWSERVER_FORBID, &CWindowManagerDlg::OnWindowserverForbid)
	ON_COMMAND(ID_WINDOWSERVER_RESTART, &CWindowManagerDlg::OnWindowserverRestart)
	ON_COMMAND(ID_WINDOWSERVER_FIX, &CWindowManagerDlg::OnWindowserverFix)
END_MESSAGE_MAP()


// CWindowManagerDlg 消息处理程序
void CWindowManagerDlg::OnReceiveComplete(void)
{
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_WINDOW_MANAGER_REPLY:
	{

		ShowClientWindowList();
		break;
	}
/*	case  CLIENT_WINDOW_KILL_REPLY:
	{

		GetClientProcessList();
		break;
	}*/
	default:
	{
		break;
	}
	}
	return;
}

BOOL CWindowManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_IconHwnd, FALSE);   //设置忍者图标到Dialog
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);

	BOOL IsOk = INVALID_SOCKET;
	if (m_ContextObject != NULL)
	{
		IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
	}
	v1.Format("%s - 窗口管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
	SetWindowText(v1);//设置对话框标题
	LOGFONT  Logfont;//最好弄成类成员,全局变量,静态成员  
	CFont*   v3= m_CListCtrl_Window_Server.GetFont();
	v3->GetLogFont(&Logfont);
	Logfont.lfHeight = Logfont.lfHeight * 1.3;   //这里可以修改字体的高比例
	Logfont.lfWidth = Logfont.lfWidth * 1.3;   //这里可以修改字体的宽比例
	CFont  v4;
	v4.CreateFontIndirect(&Logfont);
	m_CListCtrl_Window_Server.SetFont(&v4);
	m_CListCtrl_Window_Client.SetFont(&v4);
	v4.Detach();
	switch (m_RunMode)
	{
		//Servrt Client
	case  WINDOW_MANAGER::CLIENT_MODE:
	{

		//设置显示ListCtrl属性
		m_CListCtrl_Window_Client.InsertColumn(0, "窗口句柄", LVCFMT_LEFT, 100);
		m_CListCtrl_Window_Client.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Client.InsertColumn(2, "窗口类名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Client.InsertColumn(3, "窗口状态", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Client.InsertColumn(4, "启用/禁用", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Client.InsertColumn(5, "线程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Client.InsertColumn(6, "进程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Client.InsertColumn(7, "占位", LVCFMT_LEFT, 120);
		m_CListCtrl_Window_Client.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);

		m_CListCtrl_Window_Server.InsertColumn(0, "窗口句柄", LVCFMT_LEFT, 100);
		m_CListCtrl_Window_Server.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Server.InsertColumn(2, "窗口类名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Server.InsertColumn(3, "窗口状态", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(4, "启用/禁用", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(5, "线程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(6, "进程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(7, "占位", LVCFMT_LEFT, 120);
		m_CListCtrl_Window_Server.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerWindowList();
		ShowClientWindowList();


		break;
	}
	//Server
	case WINDOW_MANAGER::SERVER_MODE:
	{
		m_CListCtrl_Window_Client.EnableWindow(FALSE);   //禁止功能
		m_CListCtrl_Window_Client.ShowWindow(FALSE);     //隐藏
		RECT Rect;
		GetWindowRect(&Rect);  //当前Dialog大小
		SetWindowPos(NULL, 0, 0, Rect.right, Rect.bottom/2, SWP_NOMOVE);

		//设置显示ListCtrl属性
		m_CListCtrl_Window_Server.InsertColumn(0, "窗口句柄", LVCFMT_LEFT, 100);
		m_CListCtrl_Window_Server.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Server.InsertColumn(2, "窗口类名称", LVCFMT_LEFT, 200);
		m_CListCtrl_Window_Server.InsertColumn(3, "窗口状态", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(4, "启用/禁用", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(5, "线程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(6, "进程ID", LVCFMT_LEFT, 80);
		m_CListCtrl_Window_Server.InsertColumn(7, "占位", LVCFMT_LEFT, 120);  
		m_CListCtrl_Window_Server.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerWindowList();
		break;
	}
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
BOOL CWindowManagerDlg::ShowServerWindowList()
{
	//枚举
	ULONG BufferLength = 0;
	char* BufferData = NULL;
	if (m_GetWindowList == NULL)
	{
		return FALSE;
	}
	BufferData = m_GetWindowList(BufferLength);
	if (BufferData == NULL)
	{
		return FALSE;
	}
	char* WindowName      = NULL;
	char* WindowClassName = NULL;
	char* WindowStatus    = NULL;
	char* WindowRestartForbid = NULL;
	DWORD Offset = 4;    //前面的4个字节是总长
	CString v1;
	CString v2;
	CString v3;
	m_CListCtrl_Window_Server.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	int i;
	for (i = 0; Offset < BufferLength; i++)
	{
		LPDWORD	Hwnd = LPDWORD(BufferData + Offset);        //这里得到窗口句柄
		Offset += sizeof(DWORD);
		WindowName = BufferData + Offset;
	
		Offset += strlen(WindowName)+1;
		WindowClassName = BufferData + Offset;
	    
		Offset+=strlen(WindowClassName)+1;
		WindowStatus = BufferData + Offset;
		
		Offset += strlen(WindowStatus) + 1;
		WindowRestartForbid = BufferData + Offset;
		Offset += lstrlen(WindowRestartForbid)+1;  
		
		DWORD	ThreadID = *((DWORD*)(BufferData + Offset));        //这里得到窗口句柄
		Offset += sizeof(DWORD);
		DWORD	ProcessID = *((DWORD*)(BufferData + Offset));   
		Offset += sizeof(DWORD);

		v1.Format("%5p", *Hwnd);

		m_CListCtrl_Window_Server.InsertItem(i, v1);       //将得到的数据加入到列表当中
		m_CListCtrl_Window_Server.SetItemText(i, 1, WindowName);
		m_CListCtrl_Window_Server.SetItemText(i, 2, WindowClassName);
		m_CListCtrl_Window_Server.SetItemText(i, 3, WindowStatus);
		m_CListCtrl_Window_Server.SetItemText(i, 4, WindowRestartForbid);
		v2.Format("%d", ThreadID);
		v3.Format("%d", ProcessID);
		m_CListCtrl_Window_Server.SetItemText(i, 5, v2);
		m_CListCtrl_Window_Server.SetItemText(i, 6, v3);
		m_CListCtrl_Window_Server.SetItemData(i, *Hwnd);   //窗口句柄 设置Hide
	}
	if (BufferData != NULL)
	{
		LocalFree(BufferData);
		BufferData = NULL;
	}

	v1.Format("窗口个数 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Window_Server.SetColumn(7, &lvc); //在列表中显示有多少个进程
	return  TRUE;
}
BOOL CWindowManagerDlg::ShowClientWindowList()
{
	char	*BufferData = (char *)(m_ContextObject->m_InDeCompressedBufferData.GetArray(0)); //xiaoxi[][][][][]

	char	*WindowName;  
	DWORD	Offset = 5;
	CString v1;
	m_CListCtrl_Window_Client.DeleteAllItems();
	//遍历发送来的每一个字符别忘了他的数据结构啊 Id+进程名+0+完整名+0
	int i;
	for (i = 0; Offset <m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1 - 4; i++)
	{
		LPDWORD	ProcessID = LPDWORD(BufferData + Offset);        //这里得到进程ID
		WindowName = BufferData + Offset + sizeof(DWORD);         //进程名就是ID之后的啦
		
																 //他的数据结构的构建很巧妙
		v1.Format("%5p", *ProcessID);
		m_CListCtrl_Window_Client.InsertItem(i, v1);       //将得到的数据加入到列表当中

		m_CListCtrl_Window_Client.SetItemText(i, 1, WindowName);
		m_CListCtrl_Window_Client.SetItemText(i, 2, "显示");
		// ItemData 为进程ID
		m_CListCtrl_Window_Client.SetItemData(i, *ProcessID);   //进程ID 设置Hide

		Offset += sizeof(DWORD) + lstrlen(WindowName) + 1;   //跳过这个数据结构 进入下一个循环
	}
	//4System.exe\0C:\windows\system.exe\0\08notepad.exe\0c:\windows\notepad.exe\0\0
	v1.Format("程序个数 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Window_Client.SetColumn(2, &lvc); //在列表中显示有多少个进程
	return TRUE;
}
void CWindowManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_ContextObject != NULL)
	{
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);

		//这个太重要了
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
	}

	CDialog::OnClose();
}
void CWindowManagerDlg::OnWindowserverRefresh()
{
	// TODO: 在此添加命令处理程序代码
	ShowServerWindowList();
}
void CWindowManagerDlg::OnWindowserverClose()
{
	// TODO: 在此添加命令处理程序代码
	if (m_PostWindowMessage==NULL)
	{
		return;
	}
	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = {0};
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		DWORD KeyValue = WM_CLOSE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
	    memcpy(BufferData+sizeof(DWORD), &KeyValue, sizeof(DWORD));   //1 4
		m_PostWindowMessage(BufferData);
	}
	ShowServerWindowList();
}
void CWindowManagerDlg::OnWindowserverHide()
{
	// TODO: 在此添加命令处理程序代码
	if (m_SetWindowStatus == NULL)
	{
		return;
	}
	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		DWORD KeyValue = SW_HIDE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &KeyValue, sizeof(DWORD));   //1 4	
		if (m_SetWindowStatus(BufferData)!=FALSE)
		{
			
		}	
	}
	ShowServerWindowList();
	
}
void CWindowManagerDlg::OnWindowserverRecover()
{
	// TODO: 在此添加命令处理程序代码
	if (m_SetWindowStatus == NULL)
	{
		return;
	}

	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		DWORD KeyValue = SW_NORMAL;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &KeyValue, sizeof(DWORD));   //1 4
		m_SetWindowStatus(BufferData);
	
	}
	ShowServerWindowList();

}
void CWindowManagerDlg::OnWindowserverMax()
{
	// TODO: 在此添加命令处理程序代码
	if (m_SetWindowStatus == NULL)
	{
		return;
	}

	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		DWORD KeyValue = SW_MAXIMIZE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &KeyValue, sizeof(DWORD));   //1 4
		if (m_SetWindowStatus(BufferData)!=FALSE)
		{
			
		}
	}
	ShowServerWindowList();
	
}
void CWindowManagerDlg::OnWindowserverMin()
{
	// TODO: 在此添加命令处理程序代码
	if (m_SetWindowStatus == NULL)
	{
		return;
	}

	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		DWORD KeyValue = SW_MINIMIZE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &KeyValue, sizeof(DWORD));   //1 4
		if (m_SetWindowStatus(BufferData) !=FALSE)
		{
		}
	}
	ShowServerWindowList();

}
void CWindowManagerDlg::OnWindowserverForbid()
{
	// TODO: 在此添加命令处理程序代码
	if (m_RestartForbidWindow == NULL)
	{
		return;
	}
	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		BOOL IsOk = FALSE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &IsOk, sizeof(BOOL));   //1 4
		if (m_RestartForbidWindow(BufferData) != FALSE)
		{
		}
	}

	ShowServerWindowList();
}
void CWindowManagerDlg::OnWindowserverFix()
{
	// TODO: 在此添加命令处理程序代码

	if (m_FixWindowTitleBar == NULL)
	{
		return;
	}
	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		BOOL IsOk = FALSE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD),"2018Remote", strlen("2018Remote")+1);   //1 4
		if (m_FixWindowTitleBar(BufferData) != FALSE)
		{

		}
	}
	ShowServerWindowList();
}
void CWindowManagerDlg::OnWindowserverRestart()
{
	// TODO: 在此添加命令处理程序代码

	if (m_RestartForbidWindow == NULL)
	{
		return;
	}

	CListCtrl	*v1 = NULL;
	v1 = &m_CListCtrl_Window_Server;
	char BufferData[20] = { 0 };
	int	Item = v1->GetSelectionMark();
	if (Item >= 0)
	{
		DWORD Hwnd = v1->GetItemData(Item); //得到窗口的句柄一同发送(隐藏数据)
		BOOL IsOk = TRUE;
		memcpy(BufferData, &Hwnd, sizeof(DWORD));   //1 4
		memcpy(BufferData + sizeof(DWORD), &IsOk, sizeof(BOOL));   //1 4
		if (m_RestartForbidWindow(BufferData) != FALSE)
		{

		}
	}
	ShowServerWindowList();

}
void CWindowManagerDlg::OnNMRClickListWindowServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_WINDOW_SERVER);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //设置当前窗口
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);
	*pResult = 0;
}



