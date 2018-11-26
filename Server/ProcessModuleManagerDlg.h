#pragma once

#include "afxcmn.h"
#include "Common.h"
// CProcessModuleManagerDlg �Ի���



class CProcessModuleManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessModuleManagerDlg)

public:
	CProcessModuleManagerDlg(CWnd* pParent,ULONG ProcessID,ULONG RunMode);   // ��׼���캯��
	virtual ~CProcessModuleManagerDlg();
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS_MODULE };
#endif
private:
	HMODULE m_ProcessModuleBase;
	ULONG   m_ProcessID;
	ULONG  m_RunMode;
	HICON  m_IconHwnd;

	LPFN_SEENUMPROCESSMODULELIST m_SeEnumProcessModuleList = NULL;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL CProcessModuleManagerDlg::ShowServerProcessModuleList();
	CListCtrl m_CListCtrl_Process_Module;
};
