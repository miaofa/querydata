// ConfigAccount.cpp: 实现文件
//

#include "stdafx.h"
#include "QueryData.h"
#include "ConfigAccount.h"
#include "afxdialogex.h"


// ConfigAccount 对话框

IMPLEMENT_DYNAMIC(ConfigAccount, CDialogEx)

ConfigAccount::ConfigAccount(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_USER, pParent)
	, m_Account(_T(""))
	, m_Password(_T(""))
{

}

ConfigAccount::~ConfigAccount()
{
}

void ConfigAccount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_Account);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
}


BEGIN_MESSAGE_MAP(ConfigAccount, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &ConfigAccount::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &ConfigAccount::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// ConfigAccount 消息处理程序


void ConfigAccount::OnBnClickedButtonOk()
{
	UpdateData(TRUE);
	return OnOK();
}


void ConfigAccount::OnBnClickedButtonCancel()
{
	return OnCancel();
}
