// CreateClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "CreateClientDlg.h"
#include "afxdialogex.h"


// CCreateClientDlg �Ի���

IMPLEMENT_DYNAMIC(CCreateClientDlg, CDialog)



struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;
	char  ServerIP[20];  
	int   ServerPort;
}__ServerConnectInfo = {0x99999999,"",0 };

CCreateClientDlg::CCreateClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_CREATE_CLIENT, pParent)
	, m_CEdit_CreateClient_Port(_T("����һ���˿ں�"))
	, m_CEdit_CreateClient_IP(_T("����һ�����ӵ�IP��ַ"))
{

}

CCreateClientDlg::~CCreateClientDlg()
{
}

void CCreateClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CREATE_CLIENT_PORT, m_CEdit_CreateClient_Port);
	DDX_Text(pDX, IDC_EDIT_CREATE_CLIENT_IP, m_CEdit_CreateClient_IP);
}


BEGIN_MESSAGE_MAP(CCreateClientDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CLIENT_OK, &CCreateClientDlg::OnBnClickedButtonCreateClientOk)
END_MESSAGE_MAP()


// CCreateClientDlg ��Ϣ�������


void CCreateClientDlg::OnBnClickedButtonCreateClientOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFile FileObject;

	UpdateData(TRUE);   //�����ڿؼ��ϵ����ݸ��µ��ؼ�������
	USHORT  ServerPort = atoi(m_CEdit_CreateClient_Port);
	strcpy(__ServerConnectInfo.ServerIP,m_CEdit_CreateClient_IP);
	//127 
	if (ServerPort<0||ServerPort>65536)
	{
		__ServerConnectInfo.ServerPort = 2356;
	}
	else
	{
		__ServerConnectInfo.ServerPort = ServerPort;   //�˿�
	}

	
	char v1[MAX_PATH];
	ZeroMemory(v1, MAX_PATH);

	LONGLONG BufferLength = 0;
	BYTE* BufferData = NULL;
	CString  v3;
	CString  ClientFullPathData;
	try
	{
		//�˴��õ�δ����ǰ���ļ���
		GetModuleFileName(NULL, v1, MAX_PATH);     //�õ��ļ���
		v3 = v1;
		int Pos = v3.ReverseFind('\\');   //���ҵ����ѯ��һ�γ��ֵ�\\  C:...\\\\Debug\\

		v3 = v3.Left(Pos);


		ClientFullPathData = v3 + "\\Client.exe";   //�õ���ǰδ�����ļ���

																	//���ļ�
		FileObject.Open(ClientFullPathData, CFile::modeRead | CFile::typeBinary);

		BufferLength = FileObject.GetLength();
		BufferData = new BYTE[BufferLength];
		ZeroMemory(BufferData, BufferLength);
		//��ȡ�ļ�����

		FileObject.Read(BufferData, BufferLength);
		FileObject.Close();
		//д������IP�Ͷ˿� ��Ҫ��Ѱ��0x1234567�����ʶȻ��д�����λ��


		//ȫ�ֱ�������PE�ļ���Data����
		int Offset = MemoryFind((char*)BufferData, (char*)&__ServerConnectInfo.CheckFlag, 
			BufferLength, sizeof(DWORD));
		memcpy(BufferData + Offset, &__ServerConnectInfo, sizeof(__ServerConnectInfo));   //д����
		//���浽�ļ�
		FileObject.Open(ClientFullPathData, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite);
		FileObject.Write(BufferData, BufferLength);
		FileObject.Close();
		delete[] BufferData;
		MessageBox("���ɳɹ�");

	}
	catch (CMemoryException* e)
	{
		MessageBox("�ڴ治��");
	}
	catch (CFileException* e)
	{
		MessageBox("�ļ���������");
	}
	catch (CException* e)
	{
		MessageBox("δ֪����");
	}

	CDialog::OnCancel();   
}


int CCreateClientDlg::MemoryFind(const char *BufferData, const char *KeyValue, int BufferLength, int KeyLength)
{

	int i, j;
	if (KeyLength == 0 || BufferLength == 0)
	{
		return -1;
	}
	for (i = 0; i < BufferLength; i++)
	{
		for (j = 0; j < KeyLength; j++)
		{
			if (BufferData[i + j] != KeyValue[j])
			{
				break;
			}
		}
	    //0x12345678   78   56  34  12
		if (j == KeyLength)
		{
			return i;
		}
	}
	// BMK Hello
	return -1;
}
//