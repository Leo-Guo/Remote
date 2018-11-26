#pragma once

#include "Common.h"
#include "IOCPServer.h"
#include "afxcmn.h"
// CWindowManagerDlg 对话框
typedef char* (*pfnGetWindowList)(ULONG&);
typedef BOOL(*pfnSetWindowStatus)(char*);
typedef BOOL(*pfnPostWindowMessage)(char*);
typedef BOOL(*pfnRestartForbidWindow)(char*);
typedef BOOL(*pfnFixWindowTitleBar)(char*);
class CWindowManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CWindowManagerDlg)

public:
	CWindowManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL, ULONG RunMode = WINDOW_MANAGER::SERVER_MODE);   // 标准构造函数
	virtual ~CWindowManagerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WINDOW_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void CWindowManagerDlg::OnReceiveComplete(void);
	BOOL CWindowManagerDlg::ShowServerWindowList();
	BOOL CWindowManagerDlg::ShowClientWindowList();
private:
	HMODULE m_WindowModuleBase;
	pfnGetWindowList		m_GetWindowList;
	pfnSetWindowStatus		m_SetWindowStatus;
	pfnPostWindowMessage	m_PostWindowMessage;
	pfnRestartForbidWindow  m_RestartForbidWindow;
	pfnFixWindowTitleBar    m_FixWindowTitleBar;
	HICON  m_IconHwnd;
	ULONG  m_RunMode;
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
public:
	CListCtrl m_CListCtrl_Window_Server;
	CListCtrl m_CListCtrl_Window_Client;
	afx_msg void OnWindowserverRefresh();
	afx_msg void OnWindowserverClose();
	afx_msg void OnWindowserverHide();
	afx_msg void OnWindowserverRecover();
	afx_msg void OnWindowserverMax();
	afx_msg void OnWindowserverMin();
	afx_msg void OnNMRClickListWindowServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnWindowserverForbid();
	afx_msg void OnWindowserverRestart();
	afx_msg void OnWindowserverFix();
};
