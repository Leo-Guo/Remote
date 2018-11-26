// CreateClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "CreateClientDlg.h"
#include "afxdialogex.h"


// CCreateClientDlg 对话框

IMPLEMENT_DYNAMIC(CCreateClientDlg, CDialog)



struct _SERVER_CONNECT_INFO
{
	DWORD CheckFlag;
	char  ServerIP[20];  
	int   ServerPort;
}__ServerConnectInfo = {0x99999999,"",0 };

CCreateClientDlg::CCreateClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_CREATE_CLIENT, pParent)
	, m_CEdit_CreateClient_Port(_T("输入一个端口号"))
	, m_CEdit_CreateClient_IP(_T("输入一个连接的IP地址"))
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


// CCreateClientDlg 消息处理程序


void CCreateClientDlg::OnBnClickedButtonCreateClientOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile FileObject;

	UpdateData(TRUE);   //输入在控件上的数据更新到控件变量上
	USHORT  ServerPort = atoi(m_CEdit_CreateClient_Port);
	strcpy(__ServerConnectInfo.ServerIP,m_CEdit_CreateClient_IP);
	//127 
	if (ServerPort<0||ServerPort>65536)
	{
		__ServerConnectInfo.ServerPort = 2356;
	}
	else
	{
		__ServerConnectInfo.ServerPort = ServerPort;   //端口
	}

	
	char v1[MAX_PATH];
	ZeroMemory(v1, MAX_PATH);

	LONGLONG BufferLength = 0;
	BYTE* BufferData = NULL;
	CString  v3;
	CString  ClientFullPathData;
	try
	{
		//此处得到未处理前的文件名
		GetModuleFileName(NULL, v1, MAX_PATH);     //得到文件名
		v3 = v1;
		int Pos = v3.ReverseFind('\\');   //从右到左查询第一次出现的\\  C:...\\\\Debug\\

		v3 = v3.Left(Pos);


		ClientFullPathData = v3 + "\\Client.exe";   //得到当前未处理文件名

																	//打开文件
		FileObject.Open(ClientFullPathData, CFile::modeRead | CFile::typeBinary);

		BufferLength = FileObject.GetLength();
		BufferData = new BYTE[BufferLength];
		ZeroMemory(BufferData, BufferLength);
		//读取文件内容

		FileObject.Read(BufferData, BufferLength);
		FileObject.Close();
		//写入上线IP和端口 主要是寻找0x1234567这个标识然后写入这个位置


		//全局变量是在PE文件的Data节中
		int Offset = MemoryFind((char*)BufferData, (char*)&__ServerConnectInfo.CheckFlag, 
			BufferLength, sizeof(DWORD));
		memcpy(BufferData + Offset, &__ServerConnectInfo, sizeof(__ServerConnectInfo));   //写操作
		//保存到文件
		FileObject.Open(ClientFullPathData, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite);
		FileObject.Write(BufferData, BufferLength);
		FileObject.Close();
		delete[] BufferData;
		MessageBox("生成成功");

	}
	catch (CMemoryException* e)
	{
		MessageBox("内存不足");
	}
	catch (CFileException* e)
	{
		MessageBox("文件操作错误");
	}
	catch (CException* e)
	{
		MessageBox("未知错误");
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