#pragma once


// ConfigAccount 对话框

class ConfigAccount : public CDialogEx
{
	DECLARE_DYNAMIC(ConfigAccount)

public:
	ConfigAccount(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ConfigAccount();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_USER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Account;
	CString m_Password;
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
