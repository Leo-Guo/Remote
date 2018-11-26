// ServerSetManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServerSetManagerDlg.h"
#include "afxdialogex.h"


// CServerSetManagerDlg �Ի���

IMPLEMENT_DYNAMIC(CServerSetManagerDlg, CDialog)

CServerSetManager CServerSetManagerDlg::m_ConfigObject;  //��̬��Ա��ʼ��

CServerSetManagerDlg::CServerSetManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SERVER_SET_MANAGER, pParent)
	, m_CEdit_ServerSet_Max(0)
	, m_CEdit_ServerSet_Port(0)
{
	//��ȡInI�ļ��е�����  ģ��  Dll
	

	
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


// CServerSetManagerDlg ��Ϣ�������


BOOL CServerSetManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	int Port = 0;
	int MaxConnections = 0;

	//���������õ�Dlg
	Port = m_ConfigObject.GetInt("Settings", "Port");
	//��ȡini �ļ��еļ����˿�
	MaxConnections = m_ConfigObject.GetInt("Settings", "MaxConnections");


	m_CEdit_ServerSet_Port = Port;
	m_CEdit_ServerSet_Max = MaxConnections;

	//���ڿؼ�Value ����ҪUpdateData   ����>False     v1     �ؼ�(�ڴ�)
	//        ����                      CListContrl   v1     ����v1���ǲ����ؼ�
    UpdateData(FALSE);
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CServerSetManagerDlg::OnBnClickedButtonServerSetApply()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//������������ 
	//�����ĵ�����д�뵽�ļ�����
	UpdateData(TRUE);


	//HomeWork

	m_ConfigObject.SetInt("Settings", "Port", m_CEdit_ServerSet_Port);
	//��ini�ļ���д��ֵ
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
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_CButton_ServerSet_Apply.ShowWindow(SW_NORMAL);
	m_CButton_ServerSet_Apply.EnableWindow(TRUE);

}


void CServerSetManagerDlg::OnEnChangeEditServerSetPort()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	m_CButton_ServerSet_Apply.ShowWindow(SW_NORMAL);
	m_CButton_ServerSet_Apply.EnableWindow(TRUE);

}


void CServerSetManagerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}
