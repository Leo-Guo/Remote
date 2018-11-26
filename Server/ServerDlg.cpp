
// ServerDlg.cpp : ʵ���ļ�
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
	{ "����",			50 },
	{ "�������/��ע",	160 },
	{ "����ϵͳ",		128 },
	{ "CPU",			180 },
	{ "����ͷ",		81 },
	{ "PING",			151 }
};
COLUMN_DATA __Column_Data_Message[] =
{
	{ "��Ϣ����",		200 },
	{ "ʱ��",			200 },
	{ "��Ϣ����",	    490 }
};



//���캯��   InitDialog  OnPaint  OnSize
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

CIOCPServer*  __IOCPServer = NULL;
CServerDlg*   __ServerDlg   = NULL;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CServerDlg �Ի���



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	
	__ServerDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//��ʼ�����Ա����
	m_Count = 0;

	//���ܷ��ʴ�������Դ
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


	//�û�����
	ON_MESSAGE(UM_CLIENT_LOGIN, OnClientLogin)

	//��ʱ��Ϣ
	ON_MESSAGE(UM_OPEN_INSTANT_MESSAGE_DIALOG, OpenInstantMessageDialog)

	//CMD����
	ON_COMMAND(IDM_SERVER_CMD_MANAGER, &CServerDlg::OnServerCmdManager)
	ON_MESSAGE(UM_OPEN_CMD_MANAGER_DIALOG, OnOpenCmdManagerDialog)

	//���̹���
	ON_COMMAND(IDM_SERVER_PROCESS_MANAGER, &CServerDlg::OnServerProcessManager)
	ON_MESSAGE(UM_OPEN_PROCESS_MANAGER_DIALOG, OnOpenProcessManagerDialog)

	//���ڹ���
	ON_COMMAND(IDM_SERVER_WINDOW_MANAGER, &CServerDlg::OnServerWindowManager)
	ON_MESSAGE(UM_OPEN_WINDOW_MANAGER_DIALOG, OnOpenWindowManagerDialog)

	//�ļ�����
	ON_COMMAND(IDM_SERVER_FILE_MANAGER, &CServerDlg::OnServerFileManager)
	ON_MESSAGE(UM_OPEN_FILE_MANAGER_DIALOG, OnOpenFileManagerDialog)


	//Զ�ع���
	ON_COMMAND(IDM_SERVER_REMOTE_CONTROLL, &CServerDlg::OnServerRemoteControll)
	ON_MESSAGE(UM_OPEN_REMOTE_CONTROLL_DIALOG, OnOpenRemoteControllDialog)

	

	//��Ƶ
	ON_COMMAND(IDM_SERVER_VIDEO_MANAGER, &CServerDlg::OnServerVideoManager)
	ON_MESSAGE(UM_OPEN_VIDEO_MANAGER_DIALOG, OnOpenVideoManagerDialog)

	//��Ƶ
	ON_COMMAND(IDM_SERVER_AUDIO_MANAGER, &CServerDlg::OnServerAudioManager)
	ON_MESSAGE(WM_OPEN_AUDIO_MANAGER_DIALOG, OnOpenAudioManagerDialog)



	//����
	ON_COMMAND(IDM_SERVER_SERVICE_MANAGER, &CServerDlg::OnServerServiceManager)
	ON_MESSAGE(UM_OPEN_SERVICE_MANAGER_DIALOG, OnOpenServiceManagerDialog)
	
	
	
	//ע���
	ON_COMMAND(IDM_SERVER_REGISTER_MANAGER, &CServerDlg::OnServerRegisterManager)
	ON_MESSAGE(UM_OPEN_REGISTER_MANAGER_DIALOG, OnOpenRegisterManagerDialog)



	//���ɿͻ���
	ON_COMMAND(IDM_SERVER_CREATE_CLIENT, &CServerDlg::OnServerCreateClient)

	
	
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER_ONLINE, &CServerDlg::OnNMRClickListServerOnline)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_DELETE, &CServerDlg::OnListserveronlinemainDelete)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_MESSAGE, &CServerDlg::OnListserveronlinemainMessage)
	ON_COMMAND(ID_LISTSERVERONLINEMAIN_SHUTDOWN, &CServerDlg::OnListserveronlinemainShutdown)
END_MESSAGE_MAP()

// CServerDlg ��Ϣ�������

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitTrueToolBarMain();
	InitListControl();
	InitSolidMenu();				   //������̬�˵�
	InitNotifyIconData();			   //����
	InitStatusBar();


	//ͨ�Ŷ���
	Activate(2356, 100);




	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


VOID CServerDlg::Activate(int Port, int MaxConnections)
{
	__IOCPServer = new CIOCPServer;                                   //��̬�������ǵ������

	if (__IOCPServer == NULL)
	{
		return;
	}
	if (__IOCPServer->StartServer(WindowNotifyProcedure, Port) == TRUE)    //����ָ��
	{

	}
	CString v1;
	v1.Format("�����˿�: %d�ɹ�", Port);
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
	case CLIENT_LOGIN: // ���߰�  shine
	{

		__ServerDlg->PostMessageA(UM_CLIENT_LOGIN,(WPARAM)(PROCESS_MANAGER::CLIENT_MODE),(LPARAM)ContextObject);   //ʹ���Զ�����Ϣ

		break;
	}
	case CLIENT_GET_OUT:
	{
		CancelIo((HANDLE)ContextObject->ClientSocket);  //�����ڵ�ǰ�����ϵ��첽����
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
		CancelIo((HANDLE)ContextObject->ClientSocket);  //�����ڵ�ǰ�����ϵ��첽����
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
		// ���ļ�����������
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
 
	CTime Time = CTime::GetCurrentTime();          //����һ��CTime ���һ������ t ʹ�����еĳ�Ա����GetCurrentTime() ������ SDK����
	v2 = Time.Format("%H:%M:%S");
	if (IsOk)
	{
		v1 = L"ִ�гɹ�";
	}
	else
	{
		v1 = L"ִ��ʧ��";
	}

	m_CListCtrl_Server_Message.InsertItem(0, v1);    //��ؼ�����������
	m_CListCtrl_Server_Message.SetItemText(0, 1, v2);
	m_CListCtrl_Server_Message.SetItemText(0, 2, Message);


	
	if (Message.Find("����") > 0)         //�������߻���������Ϣ
	{
		m_Count++;
	}
	else if (Message.Find("����") > 0)
	{
		m_Count--;
	}
	else if (Message.Find("�Ͽ�") > 0)
	{
		m_Count--;
	}

	m_Count = (m_Count <= 0 ? 0 : m_Count);         //��ֹiCount ��-1�����
	v3.Format("��%d����������", m_Count);
	m_StatusBar.SetPaneText(0, v3);   //��״̬������ʾ����
}


HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CServerDlg::OnClose()
{	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	if (::MessageBox(NULL, "��ȷ��?", "2018Remote", MB_OKCANCEL) == IDOK)
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// ����ͼ��
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

	// TODO: �ڴ˴������Ϣ����������
	if (m_StatusBar.m_hWnd != NULL)
	{    //���Ի����С�ı�ʱ ״̬����СҲ��֮�ı�
		CRect Rect;
		Rect.top = cy - 20;
		Rect.left = 0;
		Rect.right = cx;
		Rect.bottom = cy;
		m_StatusBar.MoveWindow(Rect);
		m_StatusBar.SetPaneInfo(0, m_StatusBar.GetItemID(0), SBPS_POPOUT, cx - 10);
	}


	if (m_CTrueColorToolBar_Server_Main.m_hWnd != NULL)                  //������
	{
		CRect Rect;
		Rect.top = Rect.left = 0;
		Rect.right = cx;
		Rect.bottom = 80;
		m_CTrueColorToolBar_Server_Main.MoveWindow(Rect);             //���ù�������Сλ��
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
	Menu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_SERVER_MAIN));        //����˵���Դ
	::SetMenu(this->GetSafeHwnd(), Menu);                                //Ϊ�������ò˵�
	::DrawMenuBar(this->GetSafeHwnd());                                  //��ʾ�˵�
}
void CServerDlg::OnServerMainSet()
{
	//����Dlg(ģ̬ !ģ̬)
	CServerSetManagerDlg Dlg;    //���캯��
	Dlg.DoModal();
	
	



}
void CServerDlg::OnServerMainExit()
{


	
	//CDialogEx::OnClose();   
	SendMessage(WM_CLOSE);	
}
VOID CServerDlg::InitNotifyIconData()
{
	CString Tip = "2018RemoteԶ��Э�����.........";       
	m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);                //��С��ֵ	
	m_NotifyIconData.hWnd   = m_hWnd;                                //������    �Ǳ������ڸ���CWnd����
	m_NotifyIconData.uID    = IDR_MAINFRAME;
	m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;     //������ӵ�е�״̬  ������ʾ
	m_NotifyIconData.hIcon  = m_hIcon;  
	m_NotifyIconData.uCallbackMessage = UM_ICONNOTIFY;              //�ص���Ϣ(�Զ���)
                                                                    //icon ����
	                                                                //������ʾ
	lstrcpyn(m_NotifyIconData.szTip, (LPCSTR)Tip, sizeof(m_NotifyIconData.szTip) / sizeof(m_NotifyIconData.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);                  //��ʾ����
	//��Ϣ (��Ϣ����  ��Ϣ�뺯������  ����������  ������ʵ��)
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
			//���ڲ���ʾ
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
		SetForegroundWindow();   //���õ�ǰ����
		Menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
			Point.x, Point.y, this, NULL);

		break;
	}
	}
}
void CServerDlg::OnNotifyicondataExit()
{
	// TODO: �ڴ���������������
}
void CServerDlg::OnNotifyicondataShow()
{
	// TODO: �ڴ���������������
}
VOID CServerDlg::InitStatusBar()
{
	if (!m_StatusBar.Create(this) ||
		!m_StatusBar.SetIndicators(__Indicators,
			sizeof(__Indicators) / sizeof(UINT)))                    //����״̬���������ַ���Դ��ID
	{
		return;
	}
	CRect v1;
	GetWindowRect(&v1); //Top Left Bottom Rigth
	v1.bottom += 1;   //û���κ�����  ����OnSize����ִ��
	MoveWindow(v1);
}
VOID CServerDlg::InitTrueToolBarMain()
{
	if (!m_CTrueColorToolBar_Server_Main.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_CTrueColorToolBar_Server_Main.LoadToolBar(IDR_TOOLBAR_SERVER_MAIN))  //����һ��������  ������Դ
	{

		return;
	}
	m_CTrueColorToolBar_Server_Main.LoadTrueColorToolBar
	(
		48,    //������ʹ�����
		IDB_BITMAP_SERVER_MAIN,
		IDB_BITMAP_SERVER_MAIN,
		IDB_BITMAP_SERVER_MAIN
	);  //�����ǵ�λͼ��Դ�����

	RECT v1, v2;
	GetWindowRect(&v2);   //�õ��������ڵĴ�С
	v1.left = 0;
	v1.top = 0;
	v1.bottom = 80;
	v1.right = v2.right - v2.left + 10;
	m_CTrueColorToolBar_Server_Main.MoveWindow(&v1, TRUE);


	m_CTrueColorToolBar_Server_Main.SetButtonText(0, "�ն˹���");     //��λͼ����������ļ�
	m_CTrueColorToolBar_Server_Main.SetButtonText(1, "���̹���");
	m_CTrueColorToolBar_Server_Main.SetButtonText(2, "���ڹ���");
	m_CTrueColorToolBar_Server_Main.SetButtonText(3, "�������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(4, "�ļ�����");
	m_CTrueColorToolBar_Server_Main.SetButtonText(5, "��������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(6, "ϵͳ����");
	m_CTrueColorToolBar_Server_Main.SetButtonText(7, "��Ƶ����");
	m_CTrueColorToolBar_Server_Main.SetButtonText(8, "�������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(9, "ע������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(10, "���������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(11, "�ͻ�������");
	m_CTrueColorToolBar_Server_Main.SetButtonText(12, "����");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //��ʾ
}
//CtlList �ؼ��ϵ��Ҽ���Ϣ
void CServerDlg::OnNMRClickListServerOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMenu	Menu;
	Menu.LoadMenu(IDR_MENU_LIST_SERVER_ONLINE_MAIN);               //���ز˵���Դ   ��Դ����������  Online
	CMenu*	SubMenu = Menu.GetSubMenu(0);
	CPoint	Point;
	GetCursorPos(&Point);


	int	v1 = SubMenu->GetMenuItemCount();    //�˵��ϵ�����
	if (m_CListCtrl_Server_Online.GetSelectedCount() == 0)         //���û��ѡ��
	{
		for (int i = 0; i < v1; i++)
		{
			SubMenu->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);          //�˵�ȫ�����
		}

	}
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_DELETE, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_MESSAGE, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	Menu.SetMenuItemBitmaps(ID_LISTSERVERONLINEMAIN_SHUTDOWN, MF_BYCOMMAND, &m_Bitmap[0], &m_Bitmap[0]);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Point.x, Point.y, this);
	*pResult = 0;
}
//ɾ���û�
void CServerDlg::OnListserveronlinemainDelete()
{
	// TODO: �ڴ���������������
	BYTE IsToken = CLIENT_GET_OUT;   //�򱻿ض˷���һ��COMMAND_SYSTEM
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
		ClientAddressData += "ǿ�ƶϿ�";
		WindowShowMessage(true, ClientAddressData);
	}
}
//��ʱ��Ϣ
void CServerDlg::OnListserveronlinemainMessage()
{

	//������Ϣ���͵��ͻ��� 	CLIENT_INSTANT_MESSAGE_REQUEST = 2,
	//�ͻ��˻ش���Ϣ CLIENT_INSTANT_MESSAGE_REPLY = 3 MessageHandleIO
	//MessageHandleIO  SendMessage(�Զ�����Ϣ  ��Ϣ����)
	//��Ϣ���� �����Ի���


	BYTE IsToken = CLIENT_INSTANT_MESSAGE_REQUEST;   
	SendingSelectedCommand(&IsToken, sizeof(BYTE));
/*
*/
	// TODO: �ڴ���������������
}
LRESULT CServerDlg::OpenInstantMessageDialog(WPARAM wParam, LPARAM lParam)
{
	PCONTEXT_OBJECT ContextObject = (CONTEXT_OBJECT*)lParam;

	CInstantMessageDlg *Dlg = new CInstantMessageDlg(this, __IOCPServer, ContextObject);
	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_INSTANT_MESSAGE, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	return 0;
}

//�ػ�
void CServerDlg::OnListserveronlinemainShutdown()
{
	// TODO: �ڴ���������������

	BYTE IsToken = CLIENT_SHUT_DOWN_REQUEST;
	SendingSelectedCommand(&IsToken, sizeof(BYTE));  //��Ⱥ�ػ�


	CString  ClientAddressData;
	int SelectedCount = m_CListCtrl_Server_Online.GetSelectedCount();
	int i = 0;
	for (i = 0; i < SelectedCount; i++)
	{
		POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
		int Item = m_CListCtrl_Server_Online.GetNextSelectedItem(Pos);
		ClientAddressData = m_CListCtrl_Server_Online.GetItemText(Item, 0);
		m_CListCtrl_Server_Online.DeleteItem(Item);
		ClientAddressData += "ǿ�ƶϿ�";
		WindowShowMessage(true, ClientAddressData);
	}
}
//�������ݵ��ͻ���

VOID CServerDlg::SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength)
{

	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();   //1[pcontext client]  2  3   //1    2
	while (Pos)
	{
		int	i = m_CListCtrl_Server_Online.GetNextSelectedItem(Pos);
		CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)m_CListCtrl_Server_Online.GetItemData(i); //																						// ���ͻ���������б����ݰ�                                                 //�鿴ClientContext�ṹ��
		__IOCPServer->OnClientPreSending(ContextObject, BufferData, BufferLength);         //Cleint   Context

	}
}

//�û�����
LRESULT CServerDlg::OnClientLogin(WPARAM wParam, LPARAM lParam)
{
	CString ClientAddressData, ClientPosition, HostNameData, ProcessorNameStringData, IsWebCameraExist, WebSpeed,OSNameData;
	CONTEXT_OBJECT* ContextObject = (CONTEXT_OBJECT*)lParam;         //ע�������  ClientContext  ���Ƿ�������ʱ���б���ȡ��������
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

		//��������
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
		//����
		WebSpeed.Format("%d", li->WebSpeed);
		IsWebCameraExist = li->IsWebCameraExist ? "��" : "��";

		//��ؼ��������
		AddCtrlListServerOnline(ClientAddressData, ClientPosition, HostNameData, OSNameData, ProcessorNameStringData,
			IsWebCameraExist, WebSpeed, ContextObject);   //Context ���׽���
	}
	catch (...) {}

}
VOID CServerDlg::AddCtrlListServerOnline(CString ClientAddressData, CString ClientPosition, CString HostNameData,
	CString OSNameData, CString ProcessorNameStringData, CString IsWebCameraExist, CString WebSpeed, CONTEXT_OBJECT* ContextObject)
{
	//Ĭ��Ϊ0��  �������в�������ж���������
	int i = m_CListCtrl_Server_Online.InsertItem(m_CListCtrl_Server_Online.GetItemCount(), ClientAddressData);
	m_CListCtrl_Server_Online.SetItemText(i, 1, ClientPosition);
	m_CListCtrl_Server_Online.SetItemText(i, 2, HostNameData);
	m_CListCtrl_Server_Online.SetItemText(i, 3, OSNameData);
	m_CListCtrl_Server_Online.SetItemText(i, 4, ProcessorNameStringData);
	m_CListCtrl_Server_Online.SetItemText(i, 5, IsWebCameraExist);
	m_CListCtrl_Server_Online.SetItemText(i, 6, WebSpeed);

	m_CListCtrl_Server_Online.SetItemData(i, (ULONG_PTR)ContextObject);  //���뵽������
	WindowShowMessage(TRUE, ClientAddressData + "��������");
}
//�ն˹���
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
	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_CMD_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);
	if (ContextObject != NULL)
	{
		ContextObject->DlgID = CMD_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}
//���̹���
VOID CServerDlg::OnServerProcessManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_PROCESS_MANAGER_DIALOG, PROCESS_MANAGER::SERVER_MODE, NULL);   //û�пͻ���
	}
	else
	{

		BYTE	IsToken = CLIENT_PROCESS_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));

	}
}
LRESULT CServerDlg::OnOpenProcessManagerDialog(WPARAM wParam, LPARAM lParam)  //ע���������
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CProcessManagerDlg	*Dlg = new CProcessManagerDlg(this, __IOCPServer, ContextObject, RunMode);
	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_PROCESS_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = PROCESS_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}


	return 0;
}
//���ڹ���
VOID CServerDlg::OnServerWindowManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_WINDOW_MANAGER_DIALOG, WINDOW_MANAGER::SERVER_MODE, NULL);   //û�пͻ���
	}
	else
	{

		BYTE	IsToken = CLIENT_WINDOW_MANAGER_REQUIRE;
		SendingSelectedCommand(&IsToken, sizeof(BYTE));	
	}
}
LRESULT CServerDlg::OnOpenWindowManagerDialog(WPARAM wParam, LPARAM lParam)  //ע���������
{
	ULONG RunMode = (ULONG)wParam;
	CONTEXT_OBJECT *ContextObject = (CONTEXT_OBJECT*)lParam;
	CWindowManagerDlg	*Dlg = new CWindowManagerDlg(this, __IOCPServer, ContextObject, RunMode);

	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_WINDOW_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = WINDOW_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}


	return 0;
}

//Զ�̿���
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
	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_REMOTE_CONTROLL, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);
	if (ContextObject != NULL)
	{
		ContextObject->DlgID = REMOTE_CONTROLL_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}
//�ļ�����
VOID CServerDlg::OnServerFileManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_FILE_MANAGER_DIALOG, FILE_MANAGER::SERVER_MODE, NULL);   //û�пͻ���
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

	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_FILE_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = FILE_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;

}

//��Ƶ����
VOID CServerDlg::OnServerAudioManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
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

//��Ƶ����
VOID CServerDlg::OnServerVideoManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
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


//ע������
VOID CServerDlg::OnServerRegisterManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_REGISTER_MANAGER_DIALOG, REGISTER_MANAGER::SERVER_MODE, NULL);   //û�пͻ���
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

	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_REGISTER_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = REGISTER_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}

	return 0;
}

//�������
VOID CServerDlg::OnServerServiceManager()
{
	// �ж��Ƿ���ѡ���� - û��ѡ���ֱ�����Լ�Ͷ����Ϣ�����ļ�������s
	POSITION Pos = m_CListCtrl_Server_Online.GetFirstSelectedItemPosition();
	if (!Pos)
	{
		PostMessage(UM_OPEN_SERVICE_MANAGER_DIALOG, SERVICE_MANAGER::SERVER_MODE, NULL);   //û�пͻ���
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

	// ���ø�����Ϊ׿��
	Dlg->Create(IDD_DIALOG_SERVICE_MANAGER, GetDesktopWindow());    //������������Dlg
	Dlg->ShowWindow(SW_SHOW);

	if (ContextObject != NULL)
	{
		ContextObject->DlgID = SERVICE_MANAGER_DIALOG;
		ContextObject->DlgHandle = Dlg;
	}
	return 0;
}







//���ɿͻ���
VOID CServerDlg::OnServerCreateClient()
{
	CCreateClientDlg Dlg;
	Dlg.DoModal();
}
void CServerDlg::OnServermainTestAddUser()
{
	// TODO: �ڴ���������������
	int i = m_CListCtrl_Server_Online.InsertItem(m_CListCtrl_Server_Online.GetItemCount(), "�ŷ�");
	m_CListCtrl_Server_Online.SetItemText(i, 1, "23");
	m_CListCtrl_Server_Online.SetItemText(i, 2, "���ｫ��");
	m_CListCtrl_Server_Online.SetItemText(i, 3, "��");


}










