#pragma once
#include "afxcmn.h"


// CCreateClientDlg �Ի���

class CCreateClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateClientDlg)

public:
	CCreateClientDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateClientDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CREATE_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_CEdit_CreateClient_Port;
	int CCreateClientDlg::MemoryFind(const char *BufferData, const char *KeyValue, int BufferLength, int KeyLength);
	afx_msg void OnBnClickedButtonCreateClientOk();

	CString m_CEdit_CreateClient_IP;
};
