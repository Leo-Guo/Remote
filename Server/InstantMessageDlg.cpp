// InstantMessageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "InstantMessageDlg.h"
#include "afxdialogex.h"
#include "Common.h"


// CInstantMessageDlg �Ի���

IMPLEMENT_DYNAMIC(CInstantMessageDlg, CDialog)

CInstantMessageDlg::CInstantMessageDlg(CWnd* pParent,CIOCPServer* 
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_INSTANT_MESSAGE, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;
}

CInstantMessageDlg::~CInstantMessageDlg()
{
}

void CInstantMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INSTANT_MESSAGE_MAIN, m_CEdit_Main);
}


BEGIN_MESSAGE_MAP(CInstantMessageDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CInstantMessageDlg ��Ϣ�������


BOOL CInstantMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CInstantMessageDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CancelIo((HANDLE)m_ContextObject->ClientSocket);
	closesocket(m_ContextObject->ClientSocket);
	CDialog::OnClose();
}


BOOL CInstantMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���


	if (pMsg->message == WM_KEYDOWN)
	{
		// ����VK_ESCAPE��VK_DELETE
		if (pMsg->wParam == VK_ESCAPE)
		{
			PostMessage(WM_CLOSE);
			return true;
		}
			
		//����ǿɱ༭��Ļس���
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Main.m_hWnd)
		{
			OnClientSending();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CInstantMessageDlg::OnClientSending()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int BufferLength = m_CEdit_Main.GetWindowTextLength();   //EditBox �ϻ�����ݳ���
	if (!BufferLength)
	{
		return;
	}
	CString v1;
	m_CEdit_Main.GetWindowText(v1);            //EditBox �ϻ������
	char* BufferData = (char*)VirtualAlloc(NULL, BufferLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memset(BufferData, 0, sizeof(char)*BufferLength);
	sprintf(BufferData, "%s", v1.GetBuffer(0));

	m_CEdit_Main.SetWindowText(NULL);          //EditBox �ϵ��������

	m_IOCPServer->OnClientPreSending(m_ContextObject, 
		(LPBYTE)BufferData, strlen(BufferData));   //���Լ��ڴ��е����ݷ���

	if (BufferData!=NULL)
	{
		VirtualFree(BufferData, BufferLength, MEM_RELEASE);
	}
}