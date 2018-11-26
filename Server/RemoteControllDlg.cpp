// RemoteControllDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "RemoteControllDlg.h"
#include "afxdialogex.h"


// CRemoteControllDlg �Ի���
enum
{
	IDM_CONTROL = 0x1010,
	IDM_SEND_CTRL_ALT_DEL,
	IDM_TRACE_CURSOR,	// ������ʾԶ�����
	IDM_BLOCK_INPUT,	// ����Զ�̼��������
	IDM_SAVE_DIB,		// ����ͼƬ
	IDM_GET_CLIPBOARD,	// ��ȡ������
	IDM_SET_CLIPBOARD,	// ���ü�����

};
IMPLEMENT_DYNAMIC(CRemoteControllDlg, CDialog)

CRemoteControllDlg::CRemoteControllDlg(CWnd* pParent, CIOCPServer*
	IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(IDD_DIALOG_REMOTE_CONTROLL, pParent)
{
	m_IOCPServer = IOCPServer;
	m_ContextObject = ContextObject;

	m_IconHwnd = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));


	m_HScrollPos = 0;
	m_VScrollPos = 0;
	m_IsControl = FALSE;
	m_IsTraceCursor = FALSE;
	if (m_ContextObject == NULL)
	{
		return;
	}
	ULONG	BufferLength = m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1;
	m_BitmapInfo = (BITMAPINFO *) new BYTE[BufferLength];

	if (m_BitmapInfo == NULL)
	{
		return;
	}

	memcpy(m_BitmapInfo, m_ContextObject->m_InDeCompressedBufferData.GetArray(1), BufferLength);

}

CRemoteControllDlg::~CRemoteControllDlg()
{


	::ReleaseDC(m_hWnd, m_DesktopDCHandle);   //GetDC
	::DeleteDC(m_DesktopMemoryDCHandle);                //Createƥ���ڴ�DC

	::DeleteObject(m_BitmapHandle);
	if (m_BitmapData != NULL)
	{
		m_BitmapData = NULL;
	}
}

void CRemoteControllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoteControllDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CRemoteControllDlg ��Ϣ�������


BOOL CRemoteControllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
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
	v1.Format("%s - Զ�̹���", IsOk != INVALID_SOCKET ? inet_ntoa(ClientAddress.sin_addr) : "����");
	SetWindowText(v1);//���öԻ������


	
	m_DesktopDCHandle = ::GetDC(m_hWnd);
	m_DesktopMemoryDCHandle = CreateCompatibleDC(m_DesktopDCHandle);

	m_BitmapHandle = CreateDIBSection(m_DesktopDCHandle, m_BitmapInfo,
		DIB_RGB_COLORS, &m_BitmapData, NULL, NULL);                     //����Ӧ�ó������ֱ��д��ġ����豸�޹ص�λͼ
	SelectObject(m_DesktopMemoryDCHandle, m_BitmapHandle);                    //��һ����ָ�����豸�����Ļ���


	SetScrollRange(SB_HORZ, 0, m_BitmapInfo->bmiHeader.biWidth);         //ָ����������Χ����Сֵ�����ֵ
	SetScrollRange(SB_VERT, 0, m_BitmapInfo->bmiHeader.biHeight);

	CMenu* SysMenu = GetSystemMenu(FALSE);
	if (SysMenu != NULL)
	{
		SysMenu->AppendMenu(MF_SEPARATOR);
		SysMenu->AppendMenu(MF_STRING, IDM_CONTROL, "������Ļ(&Y)");
		SysMenu->AppendMenu(MF_STRING, IDM_TRACE_CURSOR, "���ٱ��ض����(&T)");
		SysMenu->AppendMenu(MF_STRING, IDM_BLOCK_INPUT, "�������ض����ͼ���(&L)");
		SysMenu->AppendMenu(MF_STRING, IDM_SAVE_DIB, "�������(&S)");
		SysMenu->AppendMenu(MF_SEPARATOR);
		SysMenu->AppendMenu(MF_STRING, IDM_GET_CLIPBOARD, "��ȡ������(&R)");
		SysMenu->AppendMenu(MF_STRING, IDM_SET_CLIPBOARD, "���ü�����(&L)");
		SysMenu->AppendMenu(MF_SEPARATOR);

	}
	m_ClientCursorPos.x = 0;
	m_ClientCursorPos.y = 0;

	//�ش�����


	BYTE IsToken = CLIENT_GO_ON;
	m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(BYTE));



	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
void CRemoteControllDlg::OnReceiveComplete(void)
{
	if (m_ContextObject == NULL)
	{
		return;
	}
	switch (m_ContextObject->m_InDeCompressedBufferData.GetArray()[0])
	{
	case CLIENT_REMOTE_CONTROLL_FIRST_SCREEN:
	{
		DrawFirstScreen();            //������ʾ��һ֡ͼ�� һ��ת����������
		break;
	}
	case CLIENT_REMOTE_CONTROLL_NEXT_SCREEN:
	{
#define  ALGORITHM_DIFF 1

		if (m_ContextObject->m_InDeCompressedBufferData.GetArray(0)[1] == ALGORITHM_DIFF)
		{
			DrawNextScreen();
		}

		break;
	}
	case CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REPLY:
	{
		SetClipboard((char*)m_ContextObject->m_InDeCompressedBufferData.GetArray(1),
			m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - 1);
		break;
	}
	default:
	{
		break;
	}
	}

}
VOID CRemoteControllDlg::DrawFirstScreen(void)
{
	//m_bIsFirst = FALSE;

	//�õ����ض˷��������� ������������HBITMAP�Ļ������У�����һ��ͼ��ͳ�����
	memcpy(m_BitmapData, m_ContextObject->m_InDeCompressedBufferData.GetArray(1), m_BitmapInfo->bmiHeader.biSizeImage);

	PostMessage(WM_PAINT);//����WM_PAINT��Ϣ
}
VOID CRemoteControllDlg::DrawNextScreen(void)
{
	//�ú�������ֱ�ӻ�����Ļ�ϣ����Ǹ���һ�±仯���ֵ���Ļ����Ȼ�����
	//OnPaint����ȥ
	//��������Ƿ��ƶ�����Ļ�Ƿ�仯�ж��Ƿ��ػ���꣬��ֹ�����˸
	BOOL	IsChange = FALSE;
	ULONG	v1 = 1 + 1 + sizeof(POINT) + sizeof(BYTE); // ��ʶ + �㷨 + ���λ�� + �����������    [Pos][Length][Data]
	
	
	LPVOID	FirstScreenBufferData = m_BitmapData;
	LPVOID	BufferData = m_ContextObject->m_InDeCompressedBufferData.GetArray(v1);
	ULONG	BufferLength = m_ContextObject->m_InDeCompressedBufferData.GetArrayLength() - v1;

	POINT	FirstClientCursorPos;
	memcpy(&FirstClientCursorPos, &m_ClientCursorPos, sizeof(POINT));
	memcpy(&m_ClientCursorPos, m_ContextObject->m_InDeCompressedBufferData.GetArray(2), sizeof(POINT));

	// ����ƶ���
	if (memcmp(&FirstClientCursorPos, &m_ClientCursorPos, sizeof(POINT)) != 0)
	{
		IsChange = TRUE;
	}
	// ��Ļ�Ƿ�仯
	if (BufferLength > 0)
	{
		IsChange = TRUE;
	}

	//lodsdָ���ESIָ����ڴ�λ��4���ֽ����ݷ���EAX�в�������4
	//movsbָ���ֽڴ������ݣ�ͨ��SI��DI�������Ĵ��������ַ�����Դ��ַ��Ŀ���ַ  ecx
	CopyScreenData(FirstScreenBufferData, BufferData, BufferLength);
	if (IsChange)
	{
		PostMessage(WM_PAINT);
	}
}
void CRemoteControllDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_ContextObject->DlgID = 0;
	CancelIo((HANDLE)m_ContextObject->ClientSocket);
	closesocket(m_ContextObject->ClientSocket);
	CDialog::OnClose();
}


void CRemoteControllDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()


	BitBlt(m_DesktopDCHandle, 0, 0,
		m_BitmapInfo->bmiHeader.biWidth,
		m_BitmapInfo->bmiHeader.biHeight,
		m_DesktopMemoryDCHandle,
		m_HScrollPos,
		m_VScrollPos,
		SRCCOPY);

	if (m_IsTraceCursor)
	{
		DrawIconEx(
			m_DesktopDCHandle,
			m_ClientCursorPos.x - m_HScrollPos,
			m_ClientCursorPos.y - m_VScrollPos,
			m_IconHwnd,
			0, 0,
			0,
			NULL,
			DI_NORMAL | DI_COMPAT
		);
	}



}


void CRemoteControllDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CMenu* Menu = GetSystemMenu(FALSE);
	switch (nID)
	{
	case IDM_CONTROL:
	{
		m_IsControl = !m_IsControl;
		Menu->CheckMenuItem(IDM_CONTROL, m_IsControl ? MF_CHECKED : MF_UNCHECKED);   //�˵���ʽ
		break;
	}


	case IDM_BLOCK_INPUT: // ������������ͼ���
	{
		BOOL IsChecked = Menu->GetMenuState(IDM_BLOCK_INPUT, MF_BYCOMMAND) & MF_CHECKED;
		Menu->CheckMenuItem(IDM_BLOCK_INPUT, IsChecked ? MF_UNCHECKED : MF_CHECKED);

		BYTE	IsToken[2];
		IsToken[0] = CLIENT_REMOTE_CONTROLL_BLOCK_INPUT;
		IsToken[1] = !IsChecked;
		m_IOCPServer->OnClientPreSending(m_ContextObject, IsToken, sizeof(IsToken));

		break;
	}

	case IDM_TRACE_CURSOR: // ���ٱ��ض����
	{
		m_IsTraceCursor = !m_IsTraceCursor;	                               //�����ڸı�����
		Menu->CheckMenuItem(IDM_TRACE_CURSOR, m_IsTraceCursor ? MF_CHECKED : MF_UNCHECKED);    //�ڲ˵��򹳲���																							   // �ػ���������ʾ���
		OnPaint();

		break;
	}

	case IDM_SAVE_DIB:    // ���ձ���
	{
		SaveSnapshot();
		break;
	}

	case IDM_GET_CLIPBOARD:            //��ҪClient�ļ���������
	{
		BYTE	IsToken = CLIENT_REMOTE_CONTROLL_GET_CLIPBOARD_REQUIRE;
		m_IOCPServer->OnClientPreSending(m_ContextObject, &IsToken, sizeof(IsToken));

		break;
	}
	case IDM_SET_CLIPBOARD:              //����
	{
		SendClipboard();

		break;
	}
	}

	CDialog::OnSysCommand(nID, lParam);


}


BOOL CRemoteControllDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	#define MAKEDWORD(h,l)        (((unsigned long)h << 16) | l) 
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	{
		
		MSG	Msg;
		memcpy(&Msg, pMsg, sizeof(MSG));
		Msg.lParam = MAKEDWORD(HIWORD(pMsg->lParam) + m_VScrollPos, LOWORD(pMsg->lParam) + m_HScrollPos);
		Msg.pt.x += m_HScrollPos;
		Msg.pt.y += m_VScrollPos;
		SendCommand(&Msg);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{

		break;
	}

	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

VOID CRemoteControllDlg::SendCommand(MSG* Msg)
{
	if (!m_IsControl)
		return;

	LPBYTE BufferData = new BYTE[sizeof(MSG) + 1];
	BufferData[0] = CLIENT_REMOTE_CONTROLL_CONTROL;
	memcpy(BufferData + 1, Msg, sizeof(MSG));
	m_IOCPServer->OnClientPreSending(m_ContextObject, BufferData, sizeof(MSG) + 1);

	delete[] BufferData;
}

BOOL CRemoteControllDlg::SaveSnapshot(void)
{
	CString	FileFullPathData = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S.bmp");
	CFileDialog Dlg(FALSE, "bmp", FileFullPathData, OFN_OVERWRITEPROMPT, "λͼ�ļ�(*.bmp)|*.bmp|", this);
	if (Dlg.DoModal() != IDOK)
		return FALSE;


	BITMAPFILEHEADER	BitMapFileHeader;
	LPBITMAPINFO		BitMapInfor = m_BitmapInfo; //1920 1080  1  0000
	CFile	File;
	if (!File.Open(Dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}
	// BITMAPINFO��С
	int	v7 = sizeof(BITMAPINFO);     //�ļ���Ϣͷ  У��ֵ
	BitMapFileHeader.bfType = ((WORD)('M' << 8) | 'B');
	BitMapFileHeader.bfSize = BitMapInfor->bmiHeader.biSizeImage + sizeof(BitMapFileHeader);  //8421
	BitMapFileHeader.bfReserved1 = 0;                                          //8000
	BitMapFileHeader.bfReserved2 = 0;
	BitMapFileHeader.bfOffBits = sizeof(BitMapFileHeader) + v7;

	File.Write(&BitMapFileHeader, sizeof(BitMapFileHeader));
	File.Write(BitMapInfor, v7);

	File.Write(m_BitmapData, BitMapInfor->bmiHeader.biSizeImage);
	File.Close();
}

VOID CRemoteControllDlg::SetClipboard(char *BufferData, ULONG BufferLength)  //�õ��������õ�������
{
	if (!OpenClipboard())
		return;
	::EmptyClipboard();
	HGLOBAL GlobalHandle = GlobalAlloc(GMEM_DDESHARE, BufferLength);
	if (GlobalHandle != NULL) {

		LPTSTR v5 = (LPTSTR)GlobalLock(GlobalHandle);
		memcpy(v5, BufferData, BufferLength);
		GlobalUnlock(GlobalHandle);
		SetClipboardData(CF_TEXT, GlobalHandle);
		GlobalFree(GlobalHandle);
	}
	CloseClipboard();
}
VOID CRemoteControllDlg::SendClipboard(void)   //�����������ݷ��͵��ͻ���
{
	if (!::OpenClipboard(NULL))  //�򿪼��а��豸
		return;
	HGLOBAL GlobalHandle = GetClipboardData(CF_TEXT);   //������һ���ڴ�
	if (GlobalHandle == NULL)
	{
		::CloseClipboard();
		return;
	}
	int	  BufferLength = GlobalSize(GlobalHandle) + 1;
	char*   v5 = (LPSTR)GlobalLock(GlobalHandle);    //���� 
	LPBYTE	BufferData = new BYTE[BufferLength];


	BufferData[0] = CLIENT_REMOTE_CONTROLL_SET_CLIPBOARD_REQUIRE;

	memcpy(BufferData + 1, v5, BufferLength - 1);
	::GlobalUnlock(GlobalHandle);
	::CloseClipboard();
	m_IOCPServer->OnClientPreSending(m_ContextObject, (PBYTE)BufferData, BufferLength);
	delete[] BufferData;
}








void CRemoteControllDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &ScrollInfo);  //1920  1080

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;  //0-1
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);  //0      
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));//i = 0    ��ֹ����   //1080    900


	RECT ClientRect;
	GetClientRect(&ClientRect);


	if ((ClientRect.bottom + i) > m_BitmapInfo->bmiHeader.biHeight)  //1080
	{
		i = m_BitmapInfo->bmiHeader.biHeight - ClientRect.bottom;
	}

	InterlockedExchange((PLONG)&m_VScrollPos, i);  //m_VScrollPos = 0

	SetScrollPos(SB_VERT, i);
	OnPaint();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CRemoteControllDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO ScrollInfo;
	int	i;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &ScrollInfo);

	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = ScrollInfo.nTrackPos;
		break;
	default:
		return;
	}

	i = max(i, ScrollInfo.nMin);
	i = min(i, (int)(ScrollInfo.nMax - ScrollInfo.nPage + 1));

	RECT ClientRect;
	GetClientRect(&ClientRect);

	if ((ClientRect.right + i) > m_BitmapInfo->bmiHeader.biWidth)
		i = m_BitmapInfo->bmiHeader.biWidth - ClientRect.right;

	InterlockedExchange((PLONG)&m_HScrollPos, i);

	SetScrollPos(SB_HORZ, m_HScrollPos);

	OnPaint();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
