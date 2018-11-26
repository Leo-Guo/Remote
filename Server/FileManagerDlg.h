#pragma once
#include "Common.h"
#include "IOCPServer.h"
#include "afxcmn.h"
#include "_CVolume.h"
#include "TrueColorToolBar.h"
#include "afxwin.h"
using namespace std;
// CFileManagerDlg 对话框

class CFileManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileManagerDlg)

public:
	CFileManagerDlg(CWnd* pParent = NULL, CIOCPServer* IOCPServer = NULL,
		CONTEXT_OBJECT *ContextObject = NULL, ULONG RunMode = PROCESS_MANAGER::SERVER_MODE);   // 标准构造函数
	virtual ~CFileManagerDlg();

private:
	CIOCPServer*    m_IOCPServer;
	PCONTEXT_OBJECT m_ContextObject;
	HICON  m_IconHwnd;
	ULONG  m_RunMode;

	
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	void OnReceiveComplete(void);
	BOOL CFileManagerDlg::InitServerUSN();
	BOOL CFileManagerDlg::GetVolumeFileSystem(CHAR * VolumeName, CString& FileSystem);
	static UINT ThreadProcedure(LPVOID ParameterData);
	UINT CFileManagerDlg::RealThreadProcedure(LPVOID ParameterData);
	VOID CFileManagerDlg::FixedServerVolumeInfo();
	VOID CFileManagerDlg::FixedServerFileInfoUSN(CString FileDirectory = "");
	int	CFileManagerDlg::GetServerIconInfo(LPCTSTR VolumeValue, LPCTSTR FileFullPathData,
		DWORD FileAttributes);
	BOOL CFileManagerDlg::GetParentDirectory(CString& DestinationString,
		CString& SourceString);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE_MANAGER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	CTrueColorToolBar m_ToolBar_Server_File; //两个工具栏
	CTrueColorToolBar m_ToolBar_Client_File;
	CListCtrl m_CListCtrl_Server_File;
	CListCtrl m_CListCtrl_Client_File;
	CProgressCtrl* m_ProgressCtrl_File;
	CStatusBar     m_StatusBar_File; // 带进度条的状态栏

	CImageList* m_CImageList_Large;
	CImageList* m_CImageList_Small;


	vector<_CVolume> m_Volumes;
	vector<_CVolume>::iterator m_Volume;	// 迭代器
	
private:

	HMODULE  m_FileModuleBase;
	LPFN_GETLOCALHARDDISKINFO m_GetLocalHardDiskInfo;



	HANDLE m_MonitorThreadHandle[26];
	CString m_ParentDirectoryFileServer;
	ULONG64 m_CurrentPathReferenceFileServer;			    // 记录当前目录的索引
	CString m_ParentDirectoryFileClient;
public:
	CStatic m_ServerFileBarPos;
	CStatic m_ClientFileBarPos;
	CString m_ServerFilePath;
	CString m_ClientFilePath;
	VOID CFileManagerDlg::FixedServerFileInfo();
	VOID CFileManagerDlg::FixedClientFileInfo(BYTE* BufferData,ULONG BufferLength);
	VOID CFileManagerDlg::GetClientFileInfo(CString Directory=NULL);
	VOID CFileManagerDlg::FixedClientFileList(BYTE *BufferData, ULONG BufferLength);
	afx_msg void OnNMDblclkListServerFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListClientFile(NMHDR *pNMHDR, LRESULT *pResult);
	int	CFileManagerDlg::GetServerIconIndex(LPCTSTR FileFullPathData, DWORD FileAttributes);

};
static DWORD MonitorProcedure(LPVOID ParameterData);