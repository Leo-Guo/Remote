// RegisterManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "RegisterManagerDlg.h"
#include "afxdialogex.h"


// CRegisterManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CRegisterManagerDlg, CDialog)

CRegisterManagerDlg::CRegisterManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject, ULONG RunMode)
	: CDialog(IDD_DIALOG_REGISTER_MANAGER, pParent)
{
	m_RunMode = RunMode;

	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;



	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_RegisterModuleBase = LoadLibrary("Register.dll");
	m_SeEnumRegisterDirList = (LPFN_SEENUMREGISTERDIRLIST)GetProcAddress(m_RegisterModuleBase, "SeEnumRegisterDirList");
    m_SeEnumRegisterItemList = (LPFN_SEENUMREGISTERITEMLIST)GetProcAddress(m_RegisterModuleBase, "SeEnumRegisterItemList");
}

CRegisterManagerDlg::~CRegisterManagerDlg()
{

	if (m_RegisterModuleBase != NULL)
	{
		FreeLibrary(m_RegisterModuleBase);
		m_RegisterModuleBase = NULL;

	}
}

void CRegisterManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SERVER_REGISTER_MAIN, m_CTreeCtrl_Server_Register);
	DDX_Control(pDX, IDC_LIST_SERVER_REGISTER_MAIN, m_CListCtrl_Server_Register);
	DDX_Control(pDX, IDC_TREE_CLIENT_REGISTER_MAIN, m_CTreeCtrl_Client_Register);
	DDX_Control(pDX, IDC_LIST_CLIENT_REGISTER_MAIN, m_CListCtrl_Client_Register);
}


BEGIN_MESSAGE_MAP(CRegisterManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SERVER_REGISTER_MAIN, &CRegisterManagerDlg::OnTvnSelchangedTreeServerRegisterMain)
END_MESSAGE_MAP()


// CRegisterManagerDlg ��Ϣ�������
void CRegisterManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[0])
	{
	case 1:
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

BOOL CRegisterManagerDlg::OnInitDialog()
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
	v1.Format("%s - ע������", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "����");
	SetWindowText(v1);//���öԻ������
	
	switch (m_RunMode)
	{

	case SERVICE_MANAGER::CLIENT_MODE:
	{
		//������ʾListCtrl����
	/*	m_CListCtrl_Server_Service.InsertColumn(0, "��������", LVCFMT_LEFT, 200);  //ServiceName
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
		ShowClientServiceList();*/
		break;
	}
	//Server
	case SERVICE_MANAGER::SERVER_MODE:
	{
		m_CTreeCtrl_Client_Register.EnableWindow(FALSE);   //��ֹ����
		m_CTreeCtrl_Client_Register.ShowWindow(FALSE);     //����
		m_CListCtrl_Client_Register.EnableWindow(FALSE);   //��ֹ����
		m_CListCtrl_Client_Register.ShowWindow(FALSE);     //����
		RECT Rect;
		GetWindowRect(&Rect);  //��ǰDialog��С
		SetWindowPos(NULL, 0, 0, Rect.right, Rect.bottom/2, SWP_NOMOVE);
		//������ʾ������

		m_ImageListTree.Create(18, 18, ILC_COLOR16, 10, 0);   //���� ���ؼ��ϵ�ͼ��
		m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_ROOT), IMAGE_ICON, 18, 18, 0);
		m_ImageListTree.Add(m_IconHwnd);
		m_IconHwnd = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_DIRECTORY), IMAGE_ICON, 18, 18, 0);
		m_ImageListTree.Add(m_IconHwnd);


		m_CTreeCtrl_Server_Register.SetImageList(&m_ImageListTree, TVSIL_NORMAL);

		m_RootTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("�����", 0, 0, 0, 0);      //0
		m_ClassRootTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("HKEY_CLASSES_ROOT", 1, 1, m_RootTreeItem[0], 0);
		m_CurrentUserTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("HKEY_CURRENT_USER", 1, 1, m_RootTreeItem[0], 0); //1
		m_LocalMachineTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("HKEY_LOCAL_MACHINE", 1, 1, m_RootTreeItem[0], 0);
		m_UsersTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("HKEY_USERS", 1, 1, m_RootTreeItem[0], 0);
		m_CurrentConfigTreeItem[0] = m_CTreeCtrl_Server_Register.InsertItem("HKEY_CURRENT_CONFIG", 1, 1, m_RootTreeItem[0], 0);

		m_CTreeCtrl_Server_Register.Expand(m_RootTreeItem[0], TVE_EXPAND);

		
		//�����б�����
		
		
		
		m_CListCtrl_Server_Register.InsertColumn(0, "����", LVCFMT_LEFT, 150, -1);
		m_CListCtrl_Server_Register.InsertColumn(1, "����", LVCFMT_LEFT, 60, -1);
		m_CListCtrl_Server_Register.InsertColumn(2, "����", LVCFMT_LEFT, 300, -1);
		m_CListCtrl_Server_Register.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		//////���ͼ��//////
		m_ImageListCtrl.Create(16, 16, TRUE, 2, 2);
		m_ImageListCtrl.Add(AfxGetApp()->LoadIcon(IDI_ICON_STRING));
		m_ImageListCtrl.Add(AfxGetApp()->LoadIcon(IDI_ICON_DWORD));
		m_CListCtrl_Server_Register.SetImageList(&m_ImageListCtrl, LVSIL_SMALL);

		break;
	}
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CRegisterManagerDlg::OnClose()
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


void CRegisterManagerDlg::OnTvnSelchangedTreeServerRegisterMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	TVITEM Item = pNMTreeView->itemNew;

	if (m_SeEnumRegisterDirList == NULL||m_SeEnumRegisterItemList==NULL)
	{
		return;
	}

	if (m_IsWorking==TRUE)
	{
		return;
	}


	m_IsWorking = TRUE;
	if (Item.hItem == m_RootTreeItem[0])
	{
		m_IsWorking = FALSE;
		return;
	}
	m_SelectedTreeItem[0] = Item.hItem;		
	
	m_CListCtrl_Server_Register.DeleteAllItems();
	
	CString FullPathData = GetFullPathData(m_CTreeCtrl_Server_Register,
		m_RootTreeItem[0],m_SelectedTreeItem[0]);    //��ü�ֵ·��  


	//HKEY_USERS\\


	char IsToken = GetFiveFatherPath(FullPathData);       //IsToke����Common.h ��ö��


	int i = m_CListCtrl_Server_Register.InsertItem(0, "(Ĭ��)", 0);
	m_CListCtrl_Server_Register.SetItemText(i, 1, "REG_SZ");
	m_CListCtrl_Server_Register.SetItemText(i, 2, "(����δ����ֵ)");



	FullPathData.Insert(0,IsToken);
	//ö�����
	vector<string> RegisterInfoDir;
	vector<REGISTER_ITEM_INFORMATION>       RegisterInfoItem;

	//
	ULONG ItemCount = 	
	m_SeEnumRegisterDirList(RegisterInfoDir, FullPathData.GetBuffer(0),FullPathData.GetLength());
	ShowServerRegisterDir(RegisterInfoDir);

	m_SeEnumRegisterItemList(RegisterInfoItem, FullPathData.GetBuffer(0),FullPathData.GetLength());
	ShowServerRegisterItem(RegisterInfoItem);



	m_IsWorking = FALSE;




	*pResult = 0;
}
void CRegisterManagerDlg::ShowServerRegisterDir(vector<string>& RegisterInfoDir)
{
	if (RegisterInfoDir.size() == 0) return;

	vector<string>::iterator i;
	for (i=RegisterInfoDir.begin();i!=RegisterInfoDir.end();i++)
	{
		m_CTreeCtrl_Server_Register.InsertItem(i->c_str(), 1, 1,m_SelectedTreeItem[0], 0);//�����Ӽ�����
		m_CTreeCtrl_Server_Register.Expand(m_SelectedTreeItem[0], TVE_EXPAND);
	}


	
	
}
void CRegisterManagerDlg::ShowServerRegisterItem(vector<REGISTER_ITEM_INFORMATION>& RegisterInfoItem)
{
	if (RegisterInfoItem.size() == 0) return;

	vector<REGISTER_ITEM_INFORMATION>::iterator i;
	for (i = RegisterInfoItem.begin(); i != RegisterInfoItem.end(); i++)
	{
		if (i->ItemType == REG_SZ)
		{
			int Item = m_CListCtrl_Server_Register.InsertItem(0,i->ItemName, 0);
			m_CListCtrl_Server_Register.SetItemText(Item, 1, "REG_SZ");
			m_CListCtrl_Server_Register.SetItemText(Item, 2, (char*)i->ItemData);



		}
		if (i->ItemType == REG_DWORD)
		{
			
			DWORD v1= 0;
			memcpy((void*)&v1, i->ItemData, sizeof(DWORD));
		
			CString v16;
			char v10[MAX_PATH];
			v16.Format("0x%lx", v1);
			sprintf(v10,"  (%d)",v1);
			v16 += " ";
			v16 += v10;
			
			int Item = m_CListCtrl_Server_Register.InsertItem(0, i->ItemName, 1);
			m_CListCtrl_Server_Register.SetItemText(Item, 1, "REG_DWORD");
			m_CListCtrl_Server_Register.SetItemText(Item, 2, v16);

		}
		if (i->ItemType == REG_QWORD)
		{

		}
		if (i->ItemType == REG_BINARY)
		{
			CString v2;
			v2.Format("%d",i->ItemData);

			int Item = m_CListCtrl_Server_Register.InsertItem(0, i->ItemName, 1);
			m_CListCtrl_Server_Register.SetItemText(Item, 1, "REG_BINARY");
			m_CListCtrl_Server_Register.SetItemText(Item, 2, v2);
		}
		if (i->ItemType == REG_EXPAND_SZ)
		{
			int iItem = m_CListCtrl_Server_Register.InsertItem(0, i->ItemName, 0);
			m_CListCtrl_Server_Register.SetItemText(iItem, 1, "REG_EXPAND_SZ");
			m_CListCtrl_Server_Register.SetItemText(iItem, 2, (char*)i->ItemData);
		}
	}

}
	

CString GetFullPathData(CTreeCtrl& TreeCtrl,HTREEITEM RootTreeItem,HTREEITEM SelectedTreeItem)
{
	CString v1;
	CString FullPathData = "";
	while (1)
	{
		if (SelectedTreeItem == RootTreeItem)
		{
			return FullPathData;
		}
		v1 = TreeCtrl.GetItemText(SelectedTreeItem);
		if (v1.Right(1) != "\\")
			v1 += "\\";
		FullPathData = v1 + FullPathData;
		SelectedTreeItem = TreeCtrl.GetParentItem(SelectedTreeItem);   //�õ�����

	}
	return FullPathData;
}
char    GetFiveFatherPath(CString& FullPathData)  
{
	char IsToken;
	if (!FullPathData.Find("HKEY_CLASSES_ROOT"))	//�ж�����
	{

		IsToken = MHKEY_CLASSES_ROOT;
		FullPathData.Delete(0, sizeof("HKEY_CLASSES_ROOT"));
	}
	else if (!FullPathData.Find("HKEY_CURRENT_USER"))
	{
		IsToken = MHKEY_CURRENT_USER;
		FullPathData.Delete(0, sizeof("HKEY_CURRENT_USER"));

	}
	else if (!FullPathData.Find("HKEY_LOCAL_MACHINE"))
	{
		IsToken = MHKEY_LOCAL_MACHINE;
		FullPathData.Delete(0, sizeof("HKEY_LOCAL_MACHINE"));

	}
	else if (!FullPathData.Find("HKEY_USERS"))
	{
		IsToken = MHKEY_USERS;
		FullPathData.Delete(0, sizeof("HKEY_USERS"));

	}
	else if (!FullPathData.Find("HKEY_CURRENT_CONFIG"))
	{
		IsToken = MHKEY_CURRENT_CONFIG;
		FullPathData.Delete(0, sizeof("HKEY_CURRENT_CONFIG"));

	}
	return IsToken;
}


