#pragma once
#include "Common.h"
#include "IOCPServer.h"

// CRemoteControllDlg 对话框
extern "C" VOID CopyScreenData(PVOID FirstBufferData, PVOID NextBufferData, ULONG NextBufferLength);
class CRemoteControllDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoteControllDlg)

public:
	CRemoteControllDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL);   // 标准构造函数
	virtual ~CRemoteControllDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REMOTE_CONTROLL };
#endif
private:
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
	HDC    m_DesktopDCHandle;
	HDC    m_DesktopMemoryDCHandle;
	LPBITMAPINFO     m_BitmapInfo;
	HBITMAP	m_BitmapHandle;
	PVOID   m_BitmapData;
	POINT   m_ClientCursorPos;   //存储鼠标位置
	ULONG   m_HScrollPos;
	ULONG   m_VScrollPos;
	BOOL    m_IsTraceCursor = FALSE;
	BOOL 	m_IsControl = FALSE;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void CRemoteControllDlg::InitMMI(void);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	void OnReceiveComplete(void);
	VOID CRemoteControllDlg::DrawFirstScreen(void);
	VOID CRemoteControllDlg::DrawNextScreen(void);
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	VOID CRemoteControllDlg::SendCommand(MSG* Msg);
	BOOL CRemoteControllDlg::SaveSnapshot(void);
	VOID CRemoteControllDlg::SetClipboard(char *BufferData, ULONG BufferLength);
	VOID CRemoteControllDlg::SendClipboard();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
