#pragma once
#include "Common.h"
#include "IOCPServer.h"
#include "afxwin.h"

// CCmdManagerDlg �Ի���

class CCmdManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdManagerDlg)

public:
	CCmdManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL);   // ��׼���캯��
	virtual ~CCmdManagerDlg();
private:
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CMD_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	void CCmdManagerDlg::OnReceiveComplete(void);
	VOID CCmdManagerDlg::ShowClientCmdData(void);
	CEdit m_CEdit_Cmd_Main;
private:
	ULONG           m_911;   //��õ�ǰ��������λ��;
	ULONG           m_ShowDataLength;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

