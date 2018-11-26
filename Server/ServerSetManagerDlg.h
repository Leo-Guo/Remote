#pragma once
#include "ServerSetManager.h"
#include "afxwin.h"

// CServerSetManagerDlg �Ի���

class CServerSetManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CServerSetManagerDlg)

public:
	CServerSetManagerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CServerSetManagerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SERVER_SET_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

	static CServerSetManager m_ConfigObject;
	   //���캯��
	long m_CEdit_ServerSet_Max;
	long m_CEdit_ServerSet_Port;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonServerSetApply();
	CButton m_CButton_ServerSet_Apply;
	afx_msg void OnEnChangeEditServerSetMax();
	afx_msg void OnEnChangeEditServerSetPort();
	afx_msg void OnClose();
};

