// ServiceManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServiceManagerDlg.h"
#include "afxdialogex.h"
#include "Common.h"

// CServiceManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CServiceManagerDlg, CDialog)

CServiceManagerDlg::CServiceManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject, ULONG RunMode)
	: CDialog(IDD_DIALOG_SERVICE_MANAGER, pParent)
{
	m_RunMode = RunMode;

	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;



	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_ServiceModuleBase = LoadLibrary("Service.dll");
	m_SeEnumServiceList = (LPFN_SEENUMSERVICELIST)GetProcAddress(m_ServiceModuleBase, "SeEnumServiceList");
}

CServiceManagerDlg::~CServiceManagerDlg()
{
	if (m_ServiceModuleBase != NULL)
	{
		FreeLibrary(m_ServiceModuleBase);
		m_ServiceModuleBase = NULL;

	}
}

void CServiceManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_SERVICE, m_CListCtrl_Client_Service);
	DDX_Control(pDX, IDC_LIST_SERVER_SERVICE, m_CListCtrl_Server_Service);
}


BEGIN_MESSAGE_MAP(CServiceManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_SERVICE, &CServiceManagerDlg::OnNMRClickListClientService)
	ON_COMMAND(ID_SERVICE_REFRESH, &CServiceManagerDlg::OnServiceRefresh)
	ON_COMMAND(ID_SERVICE_START, &CServiceManagerDlg::OnServiceStart)
	ON_COMMAND(ID_SERVICE_STOP, &CServiceManagerDlg::OnServiceStop)
	ON_COMMAND(ID_SERVICE_MANUAL_START, &CServiceManagerDlg::OnServiceManualStart)
	ON_COMMAND(ID_SERVICE_AUTO_START, &CServiceManagerDlg::OnServiceAutoStart)
END_MESSAGE_MAP()


// CServiceManagerDlg ��Ϣ�������
void CServiceManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case CLIENT_SERVICE_MANAGER_REPLY:
	{
			
		ShowClientServiceList();

		
		break;
	}
	case  2:
	{

	
		break;
	}
	default:
	{
		break;
	}
	}
	return;
}

BOOL CServiceManagerDlg::OnInitDialog()
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
	v1.Format("%s - �������", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "����");
	SetWindowText(v1);//���öԻ������



	LOGFONT  Logfont;//���Ū�����Ա,ȫ�ֱ���,��̬��Ա  
	CFont*   v3 = m_CListCtrl_Server_Service.GetFont();
	v3->GetLogFont(&Logfont);
	Logfont.lfHeight = Logfont.lfHeight * 1.3;   //��������޸�����ĸ߱���
	Logfont.lfWidth = Logfont.lfWidth * 1.3;     //��������޸�����Ŀ����
	CFont  v4;
	v4.CreateFontIndirect(&Logfont);
	m_CListCtrl_Server_Service.SetFont(&v4);
	m_CListCtrl_Client_Service.SetFont(&v4);
	v4.Detach();


	switch (m_RunMode)
	{
		
	case SERVICE_MANAGER::CLIENT_MODE:
	{
		//������ʾListCtrl����
		m_CListCtrl_Server_Service.InsertColumn(0, "��������", LVCFMT_LEFT, 200);  //ServiceName
		m_CListCtrl_Server_Service.InsertColumn(1, "��ʾ����", LVCFMT_LEFT, 200);  //DisplayName
		m_CListCtrl_Server_Service.InsertColumn(2, "��������", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Service.InsertColumn(3, "����״̬", LVCFMT_LEFT, 50);
		m_CListCtrl_Server_Service.InsertColumn(4, "��ִ���ļ�·��", LVCFMT_LEFT, 300);
		m_CListCtrl_Server_Service.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		m_CListCtrl_Client_Service.InsertColumn(0, "��������", LVCFMT_LEFT, 200);
		m_CListCtrl_Client_Service.InsertColumn(1, "��ʾ����", LVCFMT_LEFT, 200);
		m_CListCtrl_Client_Service.InsertColumn(2, "��������", LVCFMT_LEFT, 50);
		m_CListCtrl_Client_Service.InsertColumn(3, "����״̬", LVCFMT_LEFT, 50);
		m_CListCtrl_Client_Service.InsertColumn(4, "��ִ���ļ�·��", LVCFMT_LEFT, 300);
		m_CListCtrl_Client_Service.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerServiceList();
		ShowClientServiceList();
		break;
	}
	//Server
	case SERVICE_MANAGER::SERVER_MODE:
	{
		m_CListCtrl_Client_Service.EnableWindow(FALSE);   //��ֹ����
		m_CListCtrl_Client_Service.ShowWindow(FALSE);     //����
		RECT Rect;
		GetWindowRect(&Rect);  //��ǰDialog��С
		SetWindowPos(NULL, 0, 0, Rect.right / 2, Rect.bottom, SWP_NOMOVE);
		//������ʾListCtrl����
		m_CListCtrl_Server_Service.InsertColumn(0, "��ʵ����", LVCFMT_LEFT, 200);
		m_CListCtrl_Server_Service.InsertColumn(1, "��ʾ����", LVCFMT_LEFT, 200);
		m_CListCtrl_Server_Service.InsertColumn(2, "��������", LVCFMT_LEFT, 30);
		m_CListCtrl_Server_Service.InsertColumn(3, "����״̬", LVCFMT_LEFT, 30);
		m_CListCtrl_Server_Service.InsertColumn(4, "��ִ���ļ�·��", LVCFMT_LEFT, 300);
		m_CListCtrl_Server_Service.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
		ShowServerServiceList();
		break;
	}
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
BOOL CServiceManagerDlg::ShowServerServiceList()
{
	//ö��
	ULONG ItemCount = 0;
	vector<_SERVICE_INFORMATION_> ServiceInfo;
	if (m_SeEnumServiceList == NULL)
	{
		return FALSE;
	}
	ItemCount = m_SeEnumServiceList(ServiceInfo);
	if (ItemCount == 0)
	{
		return FALSE;
	}


	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Server_Service.DeleteAllItems();
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	vector<_SERVICE_INFORMATION_>::iterator i;
	int j = 0;
	for (i = ServiceInfo.begin(); i != ServiceInfo.end(); i++)
	{

	
		m_CListCtrl_Server_Service.InsertItem(j, i->ServiceName);       
		m_CListCtrl_Server_Service.SetItemText(j, 1, i->DisplayName);
		m_CListCtrl_Server_Service.SetItemText(j, 2, i->CurrentState);
		m_CListCtrl_Server_Service.SetItemText(j, 3, i->RunWay);
		m_CListCtrl_Server_Service.SetItemText(j, 4, i->BinaryPathName);
	

		j++;
	}

	v1.Format("������� / %d", j);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Server_Service.SetColumn(4, &lvc); //���б�����ʾ�ж��ٸ�����*/
	return  TRUE;
}
BOOL CServiceManagerDlg::ShowClientServiceList()
{

	char	*BufferData = (char *)(m_ContextObject->m_InDeCompressedBufferData.GetArray(1)); 
	DWORD	Offset = 0;
	CString v1;

	char*	ServiceName = NULL;  
	char*   DisplayName = NULL;
	char*   CurrentState = NULL;
	char*   RunWay = NULL;
	char*   BinaryPathName = NULL;

	m_CListCtrl_Client_Service.DeleteAllItems();
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	int i;
	for (i = 0; Offset < m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1; i++)
	{
		ServiceName = BufferData + Offset;       
		m_CListCtrl_Client_Service.InsertItem(i, ServiceName);      
		
		
		DisplayName = BufferData + Offset + lstrlen(ServiceName)+1;         
		m_CListCtrl_Client_Service.SetItemText(i, 1, DisplayName);

		
		CurrentState = BufferData + Offset + lstrlen(ServiceName)+lstrlen(DisplayName) + 2;
		m_CListCtrl_Client_Service.SetItemText(i, 2, CurrentState);


		RunWay = BufferData + Offset +
			lstrlen(ServiceName)+lstrlen(DisplayName) +  lstrlen(CurrentState)+3;
        	
		m_CListCtrl_Client_Service.SetItemText(i, 3, RunWay);

		BinaryPathName = BufferData + Offset +
			lstrlen(ServiceName) + lstrlen(DisplayName) + lstrlen(CurrentState)
			+ lstrlen(RunWay) + 4;
		m_CListCtrl_Client_Service.SetItemText(i, 4, BinaryPathName);

	
		Offset += lstrlen(ServiceName)+lstrlen(DisplayName)+
			lstrlen(CurrentState) + lstrlen(RunWay) + lstrlen(BinaryPathName) + 5;   
	}
	v1.Format("������� / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Client_Service.SetColumn(4, &lvc); //���б�����ʾ�ж��ٸ�����*/
	return TRUE;
}
void CServiceManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_ContextObject != NULL)
	{
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);

		//���̫��Ҫ��
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
	}
	CDialog::OnClose();

}

void CServiceManagerDlg::OnNMRClickListClientService(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_SERVICE);
	CMenu *SubMenu = Menu.GetSubMenu(0);
	CPoint	Point;
	GetCursorPos(&Point);
	SubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, Point.x, Point.y, this);

	*pResult = 0;
}


void CServiceManagerDlg::OnServiceRefresh()
{
	// TODO: �ڴ���������������
	if (m_RunMode==SERVICE_MANAGER::SERVER_MODE)
	{

	}
	else
	{
		BYTE IsToken = CLIENT_SERVICE_MANAGER_REQUIRE;   //ˢ��
		m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, 1);
	}
}


void CServiceManagerDlg::OnServiceStart()
{
	// TODO: �ڴ���������������
	if (m_RunMode == SERVICE_MANAGER::SERVER_MODE)
	{

	}
	else
	{
		ConfigClientService(1);
	}
}


void CServiceManagerDlg::OnServiceStop()
{
	// TODO: �ڴ���������������
	if (m_RunMode == SERVICE_MANAGER::SERVER_MODE)
	{

	}
	else
	{
		ConfigClientService(2);
	}
}

void CServiceManagerDlg::OnServiceAutoStart()
{
	if (m_RunMode == SERVICE_MANAGER::SERVER_MODE)
	{

	}
	else
	{
		ConfigClientService(3);
	}
}
void CServiceManagerDlg::OnServiceManualStart()
{
	// TODO: �ڴ���������������
	if (m_RunMode == SERVICE_MANAGER::SERVER_MODE)
	{

	}
	else
	{
		ConfigClientService(4);
	}
}




void CServiceManagerDlg::ConfigClientService(BYTE Method)
{
	DWORD	Offset = 2;
	POSITION Pos = m_CListCtrl_Client_Service.GetFirstSelectedItemPosition();
	int	Item = m_CListCtrl_Client_Service.GetNextSelectedItem(Pos);
	CString v1 = m_CListCtrl_Client_Service.GetItemText(Item, 0);

	char*	ServiceName = v1.GetBuffer(0);
	LPBYTE  BufferData = (LPBYTE)LocalAlloc(LPTR, 3 + lstrlen(ServiceName));  //[][][]\0
	BufferData[0] = CLIENT_SERVICE_CONFIG_REQUIRE;
	BufferData[1] = Method;
	memcpy(BufferData + Offset, ServiceName, lstrlen(ServiceName) + 1);
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, LocalSize(BufferData));
	LocalFree(BufferData);
}