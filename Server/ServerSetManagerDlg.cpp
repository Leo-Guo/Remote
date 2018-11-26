// ServerSetManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerSetManagerDlg.h"
#include "afxdialogex.h"


// CServerSetManagerDlg 对话框

IMPLEMENT_DYNAMIC(CServerSetManagerDlg, CDialog)

CServerSetManager CServerSetManagerDlg::m_ConfigObject;  //静态成员初始化

CServerSetManagerDlg::CServerSetManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SERVER_SET_MANAGER, pParent)
	, m_CEdit_ServerSet_Max(0)
	, m_CEdit_ServerSet_Port(0)
{
	//读取InI文件中的数据  模块  Dll
	

	
}

CServerSetManagerDlg::~CServerSetManagerDlg()
{

	
}
void CServerSetManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_SET_MAX, m_CEdit_ServerSet_Max);
	DDV_MinMaxLong(pDX, m_CEdit_ServerSet_Max, 1, 100);
	DDX_Text(pDX, IDC_EDIT_SERVER_SET_PORT, m_CEdit_ServerSet_Port);
	DDV_MinMaxLong(pDX, m_CEdit_ServerSet_Port, 2048, 65536);
	DDX_Control(pDX, IDC_BUTTON_SERVER_SET_APPLY, m_CButton_ServerSet_Apply);
}

BEGIN_MESSAGE_MAP(CServerSetManagerDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_SET_APPLY, &CServerSetManagerDlg::OnBnClickedButtonServerSetApply)
	ON_EN_CHANGE(IDC_EDIT_SERVER_SET_MAX, &CServerSetManagerDlg::OnEnChangeEditServerSetMax)
	ON_EN_CHANGE(IDC_EDIT_SERVER_SET_PORT, &CServerSetManagerDlg::OnEnChangeEditServerSetPort)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CServerSetManagerDlg 消息处理程序


BOOL CServerSetManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	int Port = 0;
	int MaxConnections = 0;

	//将数据设置到Dlg
	Port = m_ConfigObject.GetInt("Settings", "Port");
	//读取ini 文件中的监听端口
	MaxConnections = m_ConfigObject.GetInt("Settings", "MaxConnections");


	m_CEdit_ServerSet_Port = Port;
	m_CEdit_ServerSet_Max = MaxConnections;

	//由于控件Value 所以要UpdateData   ——>False     v1     控件(内存)
	//        控制                      CListContrl   v1     操作v1就是操作控件
    UpdateData(FALSE);
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CServerSetManagerDlg::OnBnClickedButtonServerSetApply()
{
	// TODO: 在此添加控件通知处理程序代码

	//如果你更改数据 
	//将更改的数据写入到文件当中
	UpdateData(TRUE);


	//HomeWork

	m_ConfigObject.SetInt("Settings", "Port", m_CEdit_ServerSet_Port);
	//向ini文件中写入值
    m_ConfigObject.SetInt("Settings", "MaxConnections", m_CEdit_ServerSet_Max);

	m_CButton_ServerSet_Apply.EnableWindow(FALSE);
	m_CButton_ServerSet_Apply.ShowWindow(SW_HIDE);        //MFC




/*	CWnd* v1 = GetDlgItem(IDC_BUTTON_SERVER_SET_APPLY);   //SDK
	v1->EnableWindow(FALSE);
	v1->ShowWindow(SW_HIDE);
*/


}


void CServerSetManagerDlg::OnEnChangeEditServerSetMax()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_CButton_ServerSet_Apply.ShowWindow(SW_NORMAL);
	m_CButton_ServerSet_Apply.EnableWindow(TRUE);

}


void CServerSetManagerDlg::OnEnChangeEditServerSetPort()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	m_CButton_ServerSet_Apply.ShowWindow(SW_NORMAL);
	m_CButton_ServerSet_Apply.EnableWindow(TRUE);

}


void CServerSetManagerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}
