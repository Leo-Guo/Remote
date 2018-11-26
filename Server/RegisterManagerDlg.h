#pragma once
#include "Common.h"
#include "IOCPServer.h"
#include "afxcmn.h"

// CRegisterManagerDlg 对话框

class CRegisterManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterManagerDlg)

public:
	CRegisterManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL, ULONG RunMode = REGISTER_MANAGER::SERVER_MODE);   // 标准构造函数
	virtual ~CRegisterManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER_MANAGER };
#endif
private:
	HICON  m_IconHwnd;

	ULONG  m_RunMode;
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HMODULE         m_RegisterModuleBase;
	HTREEITEM	m_RootTreeItem[2];
	HTREEITEM   m_ClassRootTreeItem[2];
	HTREEITEM	m_CurrentUserTreeItem[2];
	HTREEITEM	m_LocalMachineTreeItem[2];
	HTREEITEM	m_UsersTreeItem[2];
	HTREEITEM   m_CurrentConfigTreeItem[2];

	HTREEITEM   m_SelectedTreeItem[2];

	BOOL        m_IsWorking = FALSE;

	LPFN_SEENUMREGISTERDIRLIST  m_SeEnumRegisterDirList;
	LPFN_SEENUMREGISTERITEMLIST m_SeEnumRegisterItemList;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void OnReceiveComplete(void);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	CImageList	m_ImageListTree;              //树控件上的图标	
	CImageList	m_ImageListCtrl; 
	CTreeCtrl m_CTreeCtrl_Server_Register;
	CListCtrl m_CListCtrl_Server_Register;
	CTreeCtrl m_CTreeCtrl_Client_Register;
	CListCtrl m_CListCtrl_Client_Register;
	afx_msg void OnTvnSelchangedTreeServerRegisterMain(NMHDR *pNMHDR, LRESULT *pResult);
	void CRegisterManagerDlg::ShowServerRegisterDir(vector<string>& RegisterInfoDir);
	void CRegisterManagerDlg::ShowServerRegisterItem(vector<REGISTER_ITEM_INFORMATION>& RegisterInfoItem);
};
CString GetFullPathData(CTreeCtrl& TreeCtrl, HTREEITEM RootTreeItem, HTREEITEM SelectedTreeItem);
char    GetFiveFatherPath(CString& FullPathData);