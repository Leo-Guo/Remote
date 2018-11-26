#pragma once
#include "Common.h"
#include "IOCPServer.h"
#include "afxwin.h"

// CCmdManagerDlg 对话框

class CCmdManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdManagerDlg)

public:
	CCmdManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL);   // 标准构造函数
	virtual ~CCmdManagerDlg();
private:
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CMD_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	void CCmdManagerDlg::OnReceiveComplete(void);
	VOID CCmdManagerDlg::ShowClientCmdData(void);
	CEdit m_CEdit_Cmd_Main;
private:
	ULONG           m_911;   //获得当前数据所在位置;
	ULONG           m_ShowDataLength;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

