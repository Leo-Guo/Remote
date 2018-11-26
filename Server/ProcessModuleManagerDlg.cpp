// ProcessModuleManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ProcessModuleManagerDlg.h"
#include "afxdialogex.h"


// CProcessModuleManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CProcessModuleManagerDlg, CDialog)

CProcessModuleManagerDlg::CProcessModuleManagerDlg(CWnd* pParent,ULONG ProcessID,ULONG RunMode)
	: CDialog(IDD_DIALOG_PROCESS_MODULE, pParent)
{
	m_RunMode = RunMode;
	m_ProcessID = ProcessID;
	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_ProcessModuleBase = GetModuleHandle("Process.dll");
	m_SeEnumProcessModuleList = (LPFN_SEENUMPROCESSMODULELIST)GetProcAddress(m_ProcessModuleBase, "SeEnumProcessModuleList");
}

CProcessModuleManagerDlg::~CProcessModuleManagerDlg()
{
}

void CProcessModuleManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_MODULE_MAIN, m_CListCtrl_Process_Module);
}


BEGIN_MESSAGE_MAP(CProcessModuleManagerDlg, CDialog)
END_MESSAGE_MAP()


// CProcessModuleManagerDlg ��Ϣ�������


BOOL CProcessModuleManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString v1;
	SetIcon(m_IconHwnd, FALSE);   //��������ͼ�굽Dialog
	
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_CListCtrl_Process_Module.InsertColumn(0, "ģ���ַ", LVCFMT_LEFT, 100);
	m_CListCtrl_Process_Module.InsertColumn(1, "ģ���С", LVCFMT_LEFT, 100);
	m_CListCtrl_Process_Module.InsertColumn(2, "ģ������·��", LVCFMT_LEFT, 300);
	m_CListCtrl_Process_Module.InsertColumn(2, "ģ������", LVCFMT_LEFT, 50);
	m_CListCtrl_Process_Module.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	switch (m_RunMode)   //���洦��
	{
	case PROCESS_MANAGER::SERVER_MODE:
	{
		v1.Format("%s - ���̹���","����");
		ShowServerProcessModuleList();

		break;
	}
	case PROCESS_MANAGER::CLIENT_MODE:
	{
		v1.Format("%s - ���̹���","Զ��");
		
		break;
	}
	default: 
		break;
	}
	SetWindowText(v1);//���öԻ������

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
BOOL CProcessModuleManagerDlg::ShowServerProcessModuleList()
{
	//ö��
	ULONG ItemCount = 0;
	vector<_PROCESS_MODULE_INFORMATION_> ProcessModuleInfo;
	if (m_SeEnumProcessModuleList == NULL)
	{
		return FALSE;
	}
	ItemCount = m_SeEnumProcessModuleList(m_ProcessID,ProcessModuleInfo);
	if (ItemCount == 0)
	{
		return FALSE;
	}
	CHAR* ModuleFullPathData = NULL;
	DWORD Offset = 0;
	CString v1;
	m_CListCtrl_Process_Module.DeleteAllItems();
	//������������ÿһ���ַ��������������ݽṹ�� Id+������+0+������+0
	vector<_PROCESS_MODULE_INFORMATION_>::iterator i;
	int j = 0;
	for (i = ProcessModuleInfo.begin(); i != ProcessModuleInfo.end(); i++)
	{
		ULONG64 ModuleBase = i->ModuleBase;
		v1.Format("0x%llx", ModuleBase);
		m_CListCtrl_Process_Module.InsertItem(j, v1);       //���õ������ݼ��뵽�б���
		size_t ModuleSize = i->ModuleSize;
		v1.Format("0x%x", ModuleSize);
		m_CListCtrl_Process_Module.SetItemText(j, 1, v1);
		WcharToChar(&ModuleFullPathData, i->ModuleFullPathData, sizeof(i->ModuleFullPathData));
		if (ModuleFullPathData != NULL)
		{
			m_CListCtrl_Process_Module.SetItemText(j, 3,ModuleFullPathData);
			delete[] ModuleFullPathData;
			ModuleFullPathData = NULL;
		}
		if (i->ModuleType==MODULE_X64)
		{
			m_CListCtrl_Process_Module.SetItemText(j, 2, "X64");
		}
		else
		{
			m_CListCtrl_Process_Module.SetItemText(j, 2, "X86");
		}
		j++;
	}
	v1.Format("ģ����� / %d", j);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = v1.GetBuffer(0);
	lvc.cchTextMax = v1.GetLength();
	m_CListCtrl_Process_Module.SetColumn(3, &lvc); //���б�����ʾ�ж��ٸ�ģ��
	return  TRUE;
}