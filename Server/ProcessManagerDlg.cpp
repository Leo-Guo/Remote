// ProcessManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ProcessManagerDlg.h"
#include "ProcessModuleManagerDlg.h"
#include "afxdialogex.h"
#include "Common.h"



// CProcessManagerDlg �Ի���




IMPLEMENT_DYNAMIC(CProcessManagerDlg, CDialog)
CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject,ULONG RunMode)
	: CDialog(IDD_DIALOG_PROCESS_MANAGER, pParent)
{
	m_RunMode = RunMode;

	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_ProcessModuleBase = LoadLibrary("Process.dll");
	m_SeEnumProcessList = (LPFN_SEENUMPROCESSLIST)GetProcAddress(m_ProcessModuleBase, "SeEnumProcessList");
	m_Ring3KillProcess = (pfnRing3KillProcess)GetProcAddress(m_ProcessModuleBase, "Ring3KillProcess");
	
	Wow64EnableWow64FsRedirection(TRUE);  

}
CProcessManagerDlg::~CProcessManagerDlg()
{
	if (m_ProcessModuleBase!=NULL)
	{
		FreeLibrary(m_ProcessModuleBase);
		m_ProcessModuleBase = NULL;

		m_SeEnumProcessList = NULL;
		m_Ring3KillProcess = NULL;

	}
}
void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_PROCESS, m_CListCtrl_Client_Process);
	DDX_Control(pDX, IDC_LIST_SERVER_PROCESS, m_CListCtrl_Server_Process);
}
BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SERVER_PROCESS, &CProcessManagerDlg::OnNMCustomdrawListServerProcess)
	ON_COMMAND(ID_SERVERPROCESS_REFRESH, &CProcessManagerDlg::OnServerProcessRefresh)
	ON_COMMAND(ID_SERVERPROCESS_LOCATION, &CProcessManagerDlg::OnServerProcessLocation)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER_PROCESS, &CProcessManagerDlg::OnNMRClickListServerProcess)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_PROCESS, &CProcessManagerDlg::OnNMRClickListClientProcess)
	ON_COMMAND(ID_CLIENTPROCESS_REFRESH, &CProcessManagerDlg::OnClientProcessRefresh)
	ON_COMMAND(ID_CLIENTPROCESS_RING3_KILL, &CProcessManagerDlg::OnClientProcessRing3Kill)
	ON_COMMAND(ID_SERVERPROCESS_RING3_KILL, &CProcessManagerDlg::OnServerProcessRing3Kill)
	ON_COMMAND(ID_SERVERPROCESS_MODULE, &CProcessManagerDlg::OnServerProcessModule)
END_MESSAGE_MAP()
// CProcessManagerDlg ��Ϣ�������
void CProcessManagerDlg::OnReceiveComplete(void)
{
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_PROCESS_MANAGER_REPLY:
	{

		ShowClientProcessList();
		break;
	}
	case  CLIENT_PROCESS_KILL_REPLY:
	{

		GetClientProcessList();
		break;
	}
	default:
	{
		break;
	}
	}
	return;
}
BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_IconHwnd, FALSE);   //��������ͼ�굽Dialog
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);

	BOOL IsOk = INVALID_SOCKET;
	if (m_ContextObject != NULL)
	{
		IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //�õ����ӵ�ip 
	}
	v1.Format("%s - ���̹���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "����");
	SetWindowText(v1);//���öԻ������



	LOGFONT  Logfont;//���Ū�����Ա,ȫ�ֱ���,��̬��Ա  
	CFont*   v3 = m_CListCtrl_Server_Process.GetFont();
	v3->GetLogFont(&Logfont);
	Logfont.lfHeight = Logfont.lfHeight * 1.3;   //��������޸�����ĸ߱���
	Logfont.lfWidth = Logfont.lfWidth * 1.3;     //��������޸�����Ŀ����
	CFont  v4;
	v4.CreateFontIndirect(&Logfont);
	m_CListCtrl_Server_Process.SetFont(&v4);
	m_CListCtrl_Client_Process.SetFont(&v4);
	v4.Detach();


	switch (m_RunMode)
	{
	//Servrt Client
	case  PROCESS_MANAGER::CLIENT_MODE:
	{
	
		//������ʾListCtrl����
		m_CListCtrl_Server_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Process.InsertColumn(1, "���̾���", LVCFMT_LEFT, 100);
		m_CListCtrl_Server_Process.InsertColumn(2, "����·��", LVCFMT_LEFT, 700);
		m_CListCtrl_Server_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);

		m_CListCtrl_Client_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Client_Process.InsertColumn(1, "���̾���", LVCFMT_LEFT, 100);
		m_CListCtrl_Client_Process.InsertColumn(2, "����·��", LVCFMT_LEFT, 700);
		m_CListCtrl_Client_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerProcessList();
		ShowClientProcessList();


		break;
	}
    //Server
	case PROCESS_MANAGER::SERVER_MODE:
	{
		m_CListCtrl_Client_Process.EnableWindow(FALSE);   //��ֹ����
		m_CListCtrl_Client_Process.ShowWindow(FALSE);     //����
		RECT Rect;  
		GetWindowRect(&Rect);  //��ǰDialog��С
		SetWindowPos(NULL, 0, 0, Rect.right / 2, Rect.bottom, SWP_NOMOVE);

		//������ʾListCtrl����
		m_CListCtrl_Server_Process.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Process.InsertColumn(1, "���̾���", LVCFMT_LEFT, 100);
		m_CListCtrl_Server_Process.InsertColumn(2, "����·��", LVCFMT_LEFT, 700);
		m_CListCtrl_Server_Process.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerProcessList();
		break;
	}
	default:
		break;
	}
	//C:\Windows\System32\calc.exe mspaint.exe 
	//C:\Windows\SysWow64\calc.exe

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
BOOL CProcessManagerDlg::ShowServerProcessList()
{
	//ö��
	ULONG ItemCount = 0;
	vector<_PROCESS_INFORMATION_> ProcessInfo;
	if (m_SeEnumProcessList==NULL)
	{
		return FALSE;
	}
	ItemCount = m_SeEnumProcessList(ProcessInfo);
	if (ItemCount==0)
	{
		return FALSE;
	}

	char* ImageNameData;   //notepad.exe  
	char* ProcessFullPathData;
	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Server_Process.DeleteAllItems();
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	vector<_PROCESS_INFORMATION_>::iterator i;
	int j = 0;
	for (i = ProcessInfo.begin(); i!=ProcessInfo.end(); i++)
	{
		ULONG32 ProcessID = i->ProcessID;
		v1.Format("%5u", ProcessID);
		ImageNameData = i->ImageNameData;
		ProcessFullPathData = i->ProcessFullPathData;
		
		m_CListCtrl_Server_Process.InsertItem(j,v1);       //���õ������ݼ��뵽�б���
		 
		m_CListCtrl_Server_Process.SetItemText(j, 1, ImageNameData);
		m_CListCtrl_Server_Process.SetItemText(j, 2, ProcessFullPathData);
		// ItemData Ϊ����ID
		m_CListCtrl_Server_Process.SetItemData(j, ProcessID);   //����ID ����Hide
	
		j++;
	}

	v1.Format("������� / %d", j);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Server_Process.SetColumn(2, &lvc); //���б�����ʾ�ж��ٸ�����
	return  TRUE;
}
void CProcessManagerDlg::OnNMCustomdrawListServerProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.  
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint  
	// stage, then tell Windows we want messages for every item.  
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		// This is the notification message for an item.  We'll request  
		// notifications before each subitem's prepaint stage.  
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{

		COLORREF clrNewTextColor, clrNewBkColor;

		int    iItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CString ProcessImageName = m_CListCtrl_Server_Process.GetItemText(iItem, 1);
		if (strcmp(ProcessImageName, "demo.exe") == 0)
		{
			clrNewTextColor = RGB(0, 0, 0);       //Set the text   
			clrNewBkColor = RGB(0, 255, 255);     //��ɫ  
		}
		else if (strcmp(ProcessImageName, "Taskmgr.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text   
			clrNewBkColor = RGB(255, 255, 0);     //��ɫ  
		}
		else if (strcmp(ProcessImageName, "Calculator.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 153, 0);     //����ɫ  
		}
		else if (strcmp(ProcessImageName, "mspaint.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 66, 255);            //�ۺ�ɫ  
		}
		else if (strcmp(ProcessImageName, "explorer.exe") == 0) {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 0, 0);           //��ɫ  
		}
		else {
			clrNewTextColor = RGB(0, 0, 0);       //text  
			clrNewBkColor = RGB(255, 255, 255);           //��ɫ  
		}
		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;
		// Tell Windows to paint the control itself.  
		*pResult = CDRF_DODEFAULT;

	}

}
VOID CProcessManagerDlg::LocationExplorer(CString ProcessFullPath)
{
	if (!ProcessFullPath.IsEmpty() && PathFileExists(ProcessFullPath))
	{
		CString v1;
		v1.Format("/select,%s", ProcessFullPath);
		ShellExecuteA(NULL, "open", "explorer.exe", v1, NULL, SW_SHOW);
	}
	else
	{
		::MessageBox(m_hWnd,"�ļ�·������", NULL, MB_OK | MB_ICONWARNING);
	}
}
void CProcessManagerDlg::OnServerProcessRefresh()
{
	ShowServerProcessList();
}
void CProcessManagerDlg::OnServerProcessLocation()
{
	int iItem = m_CListCtrl_Server_Process.GetSelectionMark();  //�ź�
	if (iItem == -1)
	{
		return;
	}
	CHAR ProcessFullPath[MAX_PATH] = { 0 };
	m_CListCtrl_Server_Process.GetItemText(iItem,2,ProcessFullPath, MAX_PATH);
	CString v1(ProcessFullPath);
	LocationExplorer(v1);
}
void CProcessManagerDlg::OnNMRClickListServerProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_PROCESS_SERVER);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //���õ�ǰ����
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);

	*pResult = 0;
}
BOOL CProcessManagerDlg::ShowClientProcessList()
{

	char	*BufferData = (char *)(m_ContextObject->m_InDeCompressedBufferData.GetArray(1)); //xiaoxi[][][][][]
	char	*ImageNameData;   //notepad.exe  
	char	*ProcessFullPathData;
	DWORD	Offset = 0;
	CString v1;
	m_CListCtrl_Client_Process.DeleteAllItems();
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	int i;
	for (i = 0; Offset < m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1; i++)
	{
		LPDWORD	ProcessID = LPDWORD(BufferData + Offset);        //����õ�����ID
		ImageNameData = BufferData + Offset + sizeof(DWORD);         //����������ID֮�����
		ProcessFullPathData = ImageNameData + lstrlen(ImageNameData) + 1;    //���������ǽ�����֮�����
																 //�������ݽṹ�Ĺ���������
		v1.Format("%5u", *ProcessID);
		m_CListCtrl_Client_Process.InsertItem(i, v1);       //���õ������ݼ��뵽�б���

		m_CListCtrl_Client_Process.SetItemText(i, 1, ImageNameData);
		m_CListCtrl_Client_Process.SetItemText(i, 2, ProcessFullPathData);
		// ItemData Ϊ����ID
		m_CListCtrl_Client_Process.SetItemData(i, *ProcessID);   //����ID ����Hide
		Offset += sizeof(DWORD) + lstrlen(ImageNameData) + lstrlen(ProcessFullPathData) + 2;   //����������ݽṹ ������һ��ѭ��
	}

	//4System.exe\0C:\windows\system.exe\0\08notepad.exe\0c:\windows\notepad.exe\0\0
	v1.Format("������� / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Client_Process.SetColumn(2, &lvc); //���б�����ʾ�ж��ٸ�����
	return TRUE;
}
void CProcessManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_ContextObject!=NULL)
	{
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);

		//���̫��Ҫ��
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
	}
	CDialog::OnClose();
	
}
void CProcessManagerDlg::OnNMRClickListClientProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_PROCESS_CLIENT);
	CPoint Point;
	GetCursorPos(&Point);
	SetForegroundWindow();   //���õ�ǰ����
	CMenu*	SubMenu = Menu.GetSubMenu(0);
	SubMenu->EnableMenuItem(1,MF_REMOVE);
	Menu.GetSubMenu(0)->TrackPopupMenu(
		TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		Point.x, Point.y, this, NULL);

	*pResult = 0;
}
void CProcessManagerDlg::GetClientProcessList()
{
	BYTE IsToken = CLIENT_PROCESS_REFRESH_REQUIRE;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, 1);
}
void CProcessManagerDlg::OnClientProcessRefresh()
{
	// TODO: �ڴ���������������
	GetClientProcessList();
}
void CProcessManagerDlg::OnClientProcessRing3Kill()
{
	CListCtrl	*v1 = &m_CListCtrl_Client_Process;
	LPBYTE BufferData = (LPBYTE)LocalAlloc(LPTR, 1 + (v1->GetSelectedCount() * 4));
		//����������̵�����ͷ
	BufferData[0] = CLIENT_PROCESS_RING3_KILL_REQUIRE;
	//��ʾ������Ϣ
	char *Tips = "����: ��ֹ���̻ᵼ�²�ϣ�������Ľ����\n"
		"�������ݶ�ʧ��ϵͳ���ȶ����ڱ���ֹǰ��\n"
		"���̽�û�л��ᱣ����״̬�����ݡ�";
	CString v2;
	if (v1->GetSelectedCount() > 1)
	{
		v2.Format("%sȷʵ\n����ֹ��%d�������?", Tips, v1->GetSelectedCount());
	}
	else
	{
		v2.Format("%sȷʵ\n����ֹ���������?", Tips);
	}
	if (::MessageBox(m_hWnd, v2, "���̽�������", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	DWORD	Offset = 1;
	POSITION Pos = v1->GetFirstSelectedItemPosition();
	//�õ�Ҫ�����ĸ�����
	while (Pos)
	{
		int	Item = v1->GetNextSelectedItem(Pos);
		DWORD ProcessID = v1->GetItemData(Item);
		memcpy(BufferData + Offset, &ProcessID, sizeof(DWORD));  //sdkfj101112
		Offset += sizeof(DWORD);
	}
	//�������ݵ����ض��ڱ��ض��в���COMMAND_KILLPROCESS�������ͷ
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, LocalSize(BufferData));
	LocalFree(BufferData);
}
void CProcessManagerDlg::OnServerProcessRing3Kill()
{
	CListCtrl	*v1 = &m_CListCtrl_Server_Process;
	PULONG ProcessIDArray = (PULONG)LocalAlloc(LPTR, (v1->GetSelectedCount() * 4));
	//��ʾ������Ϣ
	char *Tips = "����: ��ֹ���̻ᵼ�²�ϣ�������Ľ����\n"
		"�������ݶ�ʧ��ϵͳ���ȶ����ڱ���ֹǰ��\n"
		"���̽�û�л��ᱣ����״̬�����ݡ�";
	CString v2;
	if (v1->GetSelectedCount() > 1)
	{
		v2.Format("%sȷʵ\n����ֹ��%d�������?", Tips, v1->GetSelectedCount());
	}
	else
	{
		v2.Format("%sȷʵ\n����ֹ���������?", Tips);
	}
	if (::MessageBox(m_hWnd, v2, "���̽�������", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;
	DWORD	Offset = 0;
	POSITION Pos = v1->GetFirstSelectedItemPosition();
	//�õ�Ҫ�����ĸ�����
	while (Pos)
	{
		int	Item = v1->GetNextSelectedItem(Pos);
		DWORD ProcessID = v1->GetItemData(Item);
		memcpy(ProcessIDArray + Offset, &ProcessID, sizeof(DWORD));  //sdkfj101112
		Offset += sizeof(DWORD);
	}
	//�������ݵ����ض��ڱ��ض��в���COMMAND_KILLPROCESS�������ͷ
	m_Ring3KillProcess(ProcessIDArray, v1->GetSelectedCount());
	LocalFree(ProcessIDArray);

	ShowServerProcessList();

}


void CProcessManagerDlg::OnServerProcessModule()
{
	// TODO: �ڴ���������������
	int Item = m_CListCtrl_Server_Process.GetSelectionMark();
	if (Item == -1)
	{
		return;
	}
	ULONG ProcessID = (ULONG)m_CListCtrl_Server_Process.GetItemData(Item);   //�����������

	if (OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,ProcessID)==NULL)
	{
		ShowErrorMessage("Error", "OpenProcess()");
		return;
	}
	
	CProcessModuleManagerDlg Dlg(this, ProcessID, PROCESS_MANAGER::SERVER_MODE);
	Dlg.DoModal();
}
