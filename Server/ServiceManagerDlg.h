#pragma once
#include "Common.h"
#include "IOCPServer.h"

// CServiceManagerDlg 对话框

class CServiceManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CServiceManagerDlg)

public:
	CServiceManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL, ULONG RunMode = SERVICE_MANAGER::SERVER_MODE);   // 标准构造函数
	virtual ~CServiceManagerDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SERVICE_MANAGER };
#endif
private:
	HICON  m_IconHwnd;

	ULONG  m_RunMode;
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HMODULE         m_ServiceModuleBase;
	LPFN_SEENUMSERVICELIST m_SeEnumServiceList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_CListCtrl_Server_Service;
	CListCtrl m_CListCtrl_Client_Service;
	BOOL ShowServerServiceList();
	BOOL ShowClientServiceList();
	afx_msg void OnClose();
	void OnReceiveComplete(void);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRClickListClientService(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnServiceRefresh();
	afx_msg void OnServiceStart();
	afx_msg void OnServiceStop();
	afx_msg void OnServiceManualStart();
	afx_msg void OnServiceAutoStart();

	void CServiceManagerDlg::ConfigClientService(BYTE Method);
};
