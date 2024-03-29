
// QueryDataDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "QueryData.h"
#include "QueryDataDlg.h"
#include "afxdialogex.h"
#include "ConfigAccount.h"
#include <vector>
#include "BasicExcel.hpp"
#include"TypeTurn.h"
using namespace YExcel;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CQueryDataDlg 对话框



CQueryDataDlg::CQueryDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QUERYDATA_DIALOG, pParent)
	, m_Sn(_T(""))
	, m_starttime(COleDateTime::GetCurrentTime())
	, m_endTime(COleDateTime::GetCurrentTime())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	//GetModuleFileDirectory(strExeFileDir, MAX_PATH_LEN);
	//catFilePath(strLanguageSettingPath, strExeFileDir, TEXT("setting.ini"));


	//if (!isFileExist(strLanguageSettingPath)) //不存在需要创建语言设置文件.默认选择中文
	//{
	//	::WritePrivateProfileString(L"Setting", L"Language", L"Chinese", strLanguageSettingPath);

	//}

	//LoadLanguage();
}

void CQueryDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SN, m_Sn);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_starttime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START2, m_endTime);
	DDX_Control(pDX, IDC_COMBO_NUM, m_cobNum);
	DDX_Control(pDX, IDC_LIST_RESPOT, m_listdata);
}

BEGIN_MESSAGE_MAP(CQueryDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CQueryDataDlg::OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CQueryDataDlg::OnBnClickedButtonSave)
	ON_COMMAND(ID_CONFIG_USERACCOUNT, &CQueryDataDlg::OnConfigUseraccount)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CQueryDataDlg::OnDtnDatetimechangeDatetimepickerStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START2, &CQueryDataDlg::OnDtnDatetimechangeDatetimepickerStart2)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_RESPOT, &CQueryDataDlg::OnLvnGetdispinfoListRespot)
	ON_BN_CLICKED(IDC_BUTTO_ENGLISH, &CQueryDataDlg::OnBnClickedButtoEnglish)
	ON_BN_CLICKED(IDC_BUTTON_CHINESE, &CQueryDataDlg::OnBnClickedButtonChinese)
END_MESSAGE_MAP()


// CQueryDataDlg 消息处理程序

BOOL CQueryDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_Menu.LoadMenu(IDR_MENU);
	SetMenu(&m_Menu);

	//设置按钮
	CheckDlgButton(IDC_RADIO_HK, 1);
	CheckDlgButton(IDC_CHECK_NORMAL, 1);

	//获取时间参数
	SYSTEMTIME st, se;
	m_starttime.GetAsSystemTime(st);
	m_starttime.GetAsSystemTime(se);
	m_strstarttime.Format(L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	m_strendtime.Format(L"%04d-%02d-%02d", se.wYear, se.wMonth, se.wDay);

	//设置combox选择框
	//int InsertString(int nIndex, LPCTSTR lpszString);
	m_cobNum.InsertString(0, TEXT("50"));
	m_cobNum.InsertString(1, TEXT("100"));
	m_cobNum.InsertString(2, TEXT("500"));
	m_cobNum.InsertString(3, TEXT("All Data"));
	m_cobNum.SetCurSel(0);
	//设置表头样式
	DWORD dwStyle = m_listdata.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_INFOTIP;
	m_listdata.SetExtendedStyle(dwStyle);

#pragma region 初始化列表头

	vectoehead.push_back("ID");
	vectoehead.push_back("InventerSN");
	vectoehead.push_back("DataLoggerSN");
	vectoehead.push_back("Vpv1");
	vectoehead.push_back("Vpv2");
	vectoehead.push_back("Ipv1");
	vectoehead.push_back("Ipv2");
	vectoehead.push_back("Vac1");
	vectoehead.push_back("Vac2");
	vectoehead.push_back("Vac3");
	vectoehead.push_back("Iac1");
	vectoehead.push_back("Iac2");
	vectoehead.push_back("Iac3");
	vectoehead.push_back("Fac1");
	vectoehead.push_back("Fac2");
	vectoehead.push_back("Fac3");
	vectoehead.push_back("Pac");
	vectoehead.push_back("WorkMode");
	vectoehead.push_back("WorkModeText");
	vectoehead.push_back("Tempperature");
	vectoehead.push_back("ErrorMessagebig");
	vectoehead.push_back("ETotal");
	vectoehead.push_back("HTotal");
	vectoehead.push_back("FirmwareVersion");
	vectoehead.push_back("WarningCode");
	vectoehead.push_back("PV2FaultValue");
	vectoehead.push_back("FunctionsBitValue");
	vectoehead.push_back("Line2VFaultValue");
	vectoehead.push_back("Line3VFaultValue");
	vectoehead.push_back("BUSVoltage");
	vectoehead.push_back("NBUSVoltage");
	vectoehead.push_back("Line3FFaultValue");
	vectoehead.push_back("GFCICheckValue_SafetyConutry");
	vectoehead.push_back("EDay");
	vectoehead.push_back("Vbattery1");
	vectoehead.push_back("PV_ETotal_H");
	vectoehead.push_back("Cbattery1");
	vectoehead.push_back("Ibattery1");
	vectoehead.push_back("PV_ETotal_L");
	vectoehead.push_back("ESWarning_BMSError");
	vectoehead.push_back("LoadPower");
	vectoehead.push_back("E_Load_Day");
	vectoehead.push_back("E_Total_Load");
	vectoehead.push_back("TotalPower");
	vectoehead.push_back("Vload");
	vectoehead.push_back("Iload");
	vectoehead.push_back("OperationModebig");
	vectoehead.push_back("CreationDate");
	vectoehead.push_back("CYear");
	vectoehead.push_back("CMonth");
	vectoehead.push_back("CDay");
	vectoehead.push_back("CHour");
	vectoehead.push_back("His");
	vectoehead.push_back("ResumeData");
	vectoehead.push_back("BMS_Alarm");
	vectoehead.push_back("BMS_Warning");
	vectoehead.push_back("SOH");
	vectoehead.push_back("BMS_Temperature");
	vectoehead.push_back("BMS_Charge_I_Max");
	vectoehead.push_back("BMS_Discharge_I_Max");
	vectoehead.push_back("Battary_Work_Mode");
	vectoehead.push_back("Pmeter");
	vectoehead.push_back("Pbackup");
	vectoehead.push_back("VdcForBP");
	vectoehead.push_back("IdcForBP");
	vectoehead.push_back("WorkModeForBP");
	vectoehead.push_back("Reserved1");
	vectoehead.push_back("Reserved2");
	vectoehead.push_back("Reserved3");
	vectoehead.push_back("Reserved4");
	vectoehead.push_back("Reserved5");
	vectoehead.push_back("Reserved6");
	vectoehead.push_back("Reserved7");
	vectoehead.push_back("Reserved8");
	vectoehead.push_back("Reserved9");
	vectoehead.push_back("Reserved10");
	vectoehead.push_back("Reserved11");
	vectoehead.push_back("Reserved12");
	vectoehead.push_back("Reserved13");
	vectoehead.push_back("Reserved14");
	vectoehead.push_back("Reserved15");
	vectoehead.push_back("Reserved16");
	vectoehead.push_back("Reserved17");
	vectoehead.push_back("Reserved18");
	vectoehead.push_back("Reserved19");
	vectoehead.push_back("Reserved20");
	vectoehead.push_back("Reserved21");
	vectoehead.push_back("Reserved22");
	vectoehead.push_back("Reserved23");
	vectoehead.push_back("Reserved24");
	vectoehead.push_back("Reserved25");
	vectoehead.push_back("Reserved26");
	vectoehead.push_back("GPRSBurninMode");
	vectoehead.push_back("PacH");
	vectoehead.push_back("Vpv3");
	vectoehead.push_back("Vpv4");
	vectoehead.push_back("Ipv3");
	vectoehead.push_back("Ipv4");
	vectoehead.push_back("Istr1");
	vectoehead.push_back("Istr2");
	vectoehead.push_back("Istr3");
	vectoehead.push_back("Istr4");
	vectoehead.push_back("Istr5");
	vectoehead.push_back("Istr6");
	vectoehead.push_back("Istr7");
	vectoehead.push_back("Istr8");
	vectoehead.push_back("Istr9");
	vectoehead.push_back("Istr10");
	vectoehead.push_back("Istr11");
	vectoehead.push_back("Istr12");
	vectoehead.push_back("Istr13");
	vectoehead.push_back("Istr14");
	vectoehead.push_back("Istr15");
	vectoehead.push_back("Istr16");
	vectoehead.push_back("Istr17");
	vectoehead.push_back("Istr18");
	vectoehead.push_back("Istr19");
	vectoehead.push_back("Istr20");
	vectoehead.push_back("PIDStatus");
#pragma endregion


	for (auto m = vectoehead.begin(); m != vectoehead.end(); m++)
	{
		std::string tmp = *m;
		const char* orig = tmp.c_str();
		size_t newsize = strlen(orig) + 1;
		wchar_t * wcstring = new wchar_t[newsize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
		std::wstring wstrtmpKey(wcstring);
		wvectoehead.push_back(wstrtmpKey);
		delete[] wcstring;
	}

	for (size_t i = 0; i < wvectoehead.size(); i++)
	{
		m_listdata.InsertColumn(i, wvectoehead[i].c_str());
		m_listdata.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}
	m_listdata.SetColumnWidth(0, 50);

	// 设置语言
	//ChangeMainPageLanguage();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQueryDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQueryDataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQueryDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//回调函数将数据写到字节流中
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	((std::string*)stream)->append((char*)ptr, 0, size* nmemb);
	return size * nmemb;
}

int CQueryDataDlg::postUserInfo()
{

	std::wstring name = m_Name.GetString();
	std::string name8 = TypeTurn::UnicodeToUTF8(name);

	const size_t newsizewp = (m_Password.GetLength() + 1) * 2;
	char *nstringwp = new char[newsizewp];
	size_t convertedCharswp = 0;
	wcstombs_s(&convertedCharswp, nstringwp, newsizewp, m_Password, _TRUNCATE);

	//构建Json风格的字符串
	std::string strRes = "";
	Json::Value root;
	Json::StreamWriterBuilder jsrocd;
	std::unique_ptr<Json::StreamWriter> write(jsrocd.newStreamWriter());
	std::ostringstream os;

	root["Account"] = Json::Value(name8);
	root["PassWord"] = Json::Value(nstringwp);

	write->write(root, &os);
	strRes = os.str();
	const char *data = strRes.c_str();

	//发送post请求
	CURL *easyhandle;
	CURLcode res;
	std::string result;

	curl_global_init(CURL_GLOBAL_ALL);
	easyhandle = curl_easy_init();

	curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(easyhandle, CURLOPT_URL, urlForUser);

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &result);

	res = curl_easy_perform(easyhandle); /* post away! */

	if (res != CURLE_OK)
		TRACE("curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	const char* resultA = result.c_str();
	wstring Returnback = TypeTurn::UTF8ToUnicode(resultA);
	//将字符串转化为utf-8
	int nlen = strlen(resultA);

	//读入json中
	Json::CharReaderBuilder jsreader;
	std::unique_ptr<Json::CharReader> const reader(jsreader.newCharReader());
	std::string err1;

	if (!reader->parse(resultA, resultA + nlen, &root, &err1))
	{
		size_t newsize = strlen(err1.c_str()) + 1;
		wchar_t * wcstring = new wchar_t[newsize];

		// Convert char* string to a wchar_t* string.  
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcstring, newsize, err1.c_str(), _TRUNCATE);
		MessageBox(L"parse error:%s", wcstring);
		delete[] wcstring;
	}

	int code = root["code"].asInt();
	std::string message = root["msg"].asString();
	messageW = TypeTurn::UTF8ToUnicode(message);
	if (code != 0)
	{
		return -1;
	}
	else
	{
		token = root["data"]["token"].asString();
	}

	/* always cleanup */
	delete[] nstringwp;
	curl_easy_cleanup(easyhandle);
	curl_slist_free_all(headers); /* free the header list */
	return 0;
}

//将CstringW转化为char *
char * Cstring2char(CString str)
{
	const size_t newsizew = (str.GetLength() + 1) * 2;
	char *nstringw = new char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, nstringw, newsizew, str, _TRUNCATE);
	return nstringw;
}


int CQueryDataDlg::postFordata()
{
	//构建Json风格的字符串
	std::string strRes = "";
	Json::Value root;
	Json::StreamWriterBuilder jsrocd;
	std::unique_ptr<Json::StreamWriter> write(jsrocd.newStreamWriter());
	std::ostringstream os;
	//Querybody = { "Sn": "8060KMTU179G0143",
	//	"StartTime" : "2018-06-01",
	//	"EndTime" : "2018-06-30",
	//	"Area" : 0,
	//	"Type" : 0, # 0为正常数据，1为脏数据
	//	"page_index": 1, # index从1开始
	//	"page_size": 2 }

	//Json::Value null_value; // null
	//Json::Value arr_value(Json::arrayValue); // []
	//Json::Value obj_value(Json::objectValue); // {}
	//将CstringW转化为char *,动态数组，需要delete
	char * sntmp = Cstring2char(m_Sn);
	char * starttime = Cstring2char(m_strstarttime);
	char * endtime = Cstring2char(m_strendtime);


	root["Sn"] = Json::Value(sntmp);
	root["StartTime"] = Json::Value(starttime);
	root["EndTime"] = Json::Value(endtime);
	root["Area"] = Json::Value(area);
	root["Type"] = Json::Value(type);
	root["page_index"] = Json::Value(1);
	root["page_size"] = Json::Value(querynum);

	write->write(root, &os);
	strRes = os.str();
	const char *data = strRes.c_str();

	//发送post请求
	CURL *easyhandle;
	CURLcode res;
	std::string resultdata;
	easyhandle = curl_easy_init();
	curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(easyhandle, CURLOPT_URL, urlForData);

	//curl_easy_perform(easyhandle); /* post away! */

	struct curl_slist *headers = NULL;
	// C++实现格式化转换
	std::stringstream ss;
	ss << "token: " << token;
	std::string headForQ = ss.str();
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, headForQ.c_str());

	curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &resultdata);
	
	res = curl_easy_perform(easyhandle); /* post away! */
	if (res != CURLE_OK)
		TRACE("curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	const char* resultA = resultdata.c_str();
	wstring Returnback = TypeTurn::UTF8ToUnicode(resultdata);
	TRACE(Returnback.c_str());
	//将字符串转化为utf-8
	int nlen = strlen(resultA);


	//读入json中
	Json::CharReaderBuilder jsreader;
	std::unique_ptr<Json::CharReader> const reader(jsreader.newCharReader());
	std::string err1;
	if (!reader->parse(resultA, resultA + nlen, &root, &err1))
	{
		TRACE(L"Can't parse the string from website, please query again");
	}

	std::string message = root["msg"].asString();
	messageD = TypeTurn::UTF8ToUnicode(message);

	int code = root["code"].asInt();
	if (code == 0)
	{
		totalData = root["data"]["Total"].asInt();
		SetDlgItemInt(IDC_EDIT_TOTAL_NUM, totalData);
		//OutInfo(L"%s have %d data", totalData);
		DataFromUrl = root["data"]["Data"].asString();
	}
	else
	{
		return -1;
	}

	/* always cleanup */
	delete[] sntmp;
	delete[] starttime;
	delete[] endtime;

	curl_easy_cleanup(easyhandle);
	curl_slist_free_all(headers); /* free the header list */

	return 0;
}

// sn:8060KMTU179G0143
DWORD   CQueryDataDlg::StartQuery()
{
	//置灰按钮
	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(FALSE);
	//清理数据
	vectordata.clear();

	if (-1 == postUserInfo())
	{
		wchar_t msg[100];
		//MessageBox(L"账户或密码不正确，请重新登录!");
		wcscpy_s(msg, _countof(msg), messageW.c_str());
		MessageBox(msg);
		m_hThread = NULL;
		GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
		return -1;
	}


	if (-1 == postFordata())
	{
		//MessageBox(L"不能解析数据，请重新查询试试");
		wchar_t msg[100];
		wcscpy_s(msg, _countof(msg), messageD.c_str());
		MessageBox(msg);
		m_hThread = NULL;
		GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
		return -1;
	}

	//postForData返回的DataFromUrl供插入和保存csv文档
	const char * resultA = DataFromUrl.c_str();
	int nlen = strlen(resultA);
	Json::Value val_array(Json::arrayValue);
	Json::CharReaderBuilder jsreader;
	std::unique_ptr<Json::CharReader> const reader(jsreader.newCharReader());
	std::string err1;
	//Todo:
	if (!reader->parse(resultA, resultA + nlen, &val_array, &err1))
	{
		MessageBox(L"There's no data, please query again");
		m_hThread = NULL;
		GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
		return -1;
	}


	// 查询到数据之后，使用jsoncpp解析，把之写到列表中,此方法会将Key排序
	//Json::Value::Members members;
	//members = val_array[0].getMemberNames();   // 获取所有key的值
	//for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++)   // 遍历每个key
	//{
	//	std::string strKey = *iterMember;
	//	//转化为wchar_t
	//	const char* orig = strKey.c_str();
	//	size_t newsize = strlen(orig) + 1;
	//	wchar_t * wcstring = new wchar_t[newsize];
	//	size_t convertedChars = 0;
	//	mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
	//	std::wstring wstrtmpKey(wcstring);
	//	vectoehead.push_back(wstrtmpKey);
	//	delete[] wcstring;
	//}

	size_t iSize = val_array.size();
	size_t cSize = vectoehead.size();
	for (size_t nIndex = 0; nIndex < iSize; ++nIndex)
	{
		std::vector<std::wstring> vectoeonedata;
		for (size_t i = 0; i < cSize; ++i)
		{
			std::string strVal;
			//读取数值并将之转化为std::string
			if (val_array[nIndex][vectoehead[i].c_str()].isString())
			{
				strVal = val_array[nIndex][vectoehead[i].c_str()].asString();
			}
			else if (val_array[nIndex][vectoehead[i].c_str()].isInt())
			{
				int iVal = val_array[nIndex][vectoehead[i].c_str()].asInt();

				strVal = std::to_string(iVal);
			}
			else if (val_array[nIndex][vectoehead[i].c_str()].isDouble())
			{
				double dVal = val_array[nIndex][vectoehead[i].c_str()].asDouble();
				char buf[20];
				std::sprintf(buf, "%.1f", dVal);
				strVal = buf;
			}
			else
			{

				int type = val_array[nIndex][vectoehead[i].c_str()].type();

				strVal = val_array[nIndex][vectoehead[i].c_str()].asString();
			}

			//转化为wchar_t,压入vector中
			const char* orig = strVal.c_str();
			size_t newsize = strlen(orig) + 1;
			wchar_t * wcstring = new wchar_t[newsize];
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
			std::wstring wstrtmpVal(wcstring);

			vectoeonedata.push_back(wstrtmpVal);
			delete[] wcstring;
		}
		vectordata.push_back(vectoeonedata);
		vectoeonedata.clear();
	}

	m_listdata.DeleteAllItems();

	// 使用虚拟列表只需要告诉行数就行
	m_listdata.SetItemCount(vectordata.size());
	//m_listdata.Invalidate();


	GetDlgItem(IDC_BUTTON_QUERY)->EnableWindow(TRUE);
	m_hThread = NULL;
	return 0;
}

void CQueryDataDlg::LoadLanguage(LPCTSTR szLangSel)
{

	if (szLangSel != L"")	//保存语言
	{
		wcscpy_s(m_szLang, MAX_PATH_LEN, szLangSel);
	}
	else	//读取语言设置
	{
		//获取设置的语言,getBuffer与releasebuffer成对使用
		GetPrivateProfileStringW(L"Setting", L"Language", L"English", m_szLang,
			100, strLanguageSettingPath);
	}

	swprintf_s(strLanguagePath, MAX_PATH_LEN, L"%s\\%s.ini", strExeFileDir, m_szLang);
	//保存语言设置
	WritePrivateProfileString(L"Setting", L"Language", m_szLang, strLanguageSettingPath);


}

void CQueryDataDlg::OnBnClickedButtonQuery()
{

	UpdateData(true);
	// 转换时间为Ctring形式
	// CString Format(DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT) const;
	// 没有login是需要提醒用户login
	if (m_Name.IsEmpty())
	{
		MessageBox(L"你还没有登录，请单击登录菜单登录！");
		return;
	}
	// 时间大于一个月则提示
	COleDateTimeSpan ts = m_endTime - m_starttime ;
	if (ts.GetDays() > 30)
	{
		MessageBox(L"查询数据目前支持时间区间小于30天！");
		return;
	}

	// 绑定查询数量
	int nIndex = m_cobNum.GetCurSel();
	//if (nIndex != CB_ERR)
	//{
	//	if (nIndex == 0)
	//		querynum = 50;
	//	else if (nIndex == 1)
	//		querynum = 100;
	//	else if (nIndex == 2)
	//		querynum = 500;
	//	else if (nIndex == 3)
	//		querynum = 200000;
	//}
	//else
	//{
	//	querynum = 50;
	//}
	querynum = 200000;

	if (1 == IsDlgButtonChecked(IDC_RADIO_HK))
	{
		//area = 0;
		//urlForUser = "http://192.168.90.30:7300/api/UserSearch/Login";
		//urlForData = "http://192.168.90.30:7300/api/UserSearch/QueryData";
		urlForUser = "http://hk.goodwe-power.com/datasearch/api/UserSearch/Login";
		urlForData = "http://hk.goodwe-power.com/datasearch/api/UserSearch/QueryData";
	}
	else if (1 == IsDlgButtonChecked(IDC_RADIO_AU))
	{
		urlForUser = "http://eu.goodwe-power.com/datasearch/api/UserSearch/Login";
		urlForData = "http://eu.goodwe-power.com/datasearch/api/UserSearch/QueryData";
	}
	else if (1 == IsDlgButtonChecked(IDC_RADIO_EU))
	{
		urlForUser = "http://au.goodwe-power.com/datasearch/api/UserSearch/Login";
		urlForData = "http://au.goodwe-power.com/datasearch/api/UserSearch/QueryData";
	}
	// 0为正常数据，1为脏数据
	if (1 == IsDlgButtonChecked(IDC_CHECK_NORMAL))
	{
		type = 0;
	}
	else if (1 == IsDlgButtonChecked(IDC_CHECK_INNORMAL))
	{
		type = 1;
	}

	m_Sn.Trim();

	 //创建线程
	if (!m_hThread)
	{
		// 创建线程
		m_hThread = ::CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
		if (!m_hThread)
		{

			return;
		}
	}

	//m_hThread = ::CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
	//WaitForSingleObject(m_hThread, INFINITE);
	//DWORD m_Exitcode;
	//GetExitCodeThread(m_hThread, &m_Exitcode);

}



void CQueryDataDlg::OnBnClickedButtonSave()
{
	// TODO: 以.csv文件格式保存
	if (m_Sn.IsEmpty())
	{
		return;
	}

	CString ExportFileName = m_Sn;
	ExportFileName.Append(_T(".xls"));

	//选择文件输出路径
	CFileDialog fileDlg(FALSE, _T("xls"), ExportFileName, \
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("EXCEL File(*.xls)|*.xls||"));
	if (IDOK != fileDlg.DoModal())
	{
		return;
	}


	CString szFilePathName = fileDlg.GetPathName();
	//创建excel表格

	size_t maxRows = vectordata.size();
	size_t maxCols = wvectoehead.size();
	size_t iSheetNumber = maxRows / 65535 + 1;

	BasicExcel e;
	e.New(iSheetNumber);
	for (size_t t = 0; t < iSheetNumber; t++)
	{
		BasicExcelWorksheet* sheet1 = e.GetWorksheet(t);

		BasicExcelCell * cell;

		for (size_t  i = 0; i < maxCols; i++)
		{
			cell = sheet1->Cell(0, i);
			cell->SetWString(wvectoehead[i].c_str());
		}
		// 1~3列使用字符串,19,48列使用字符串
		for (size_t i = 0; i < maxRows; i++)
		{
			for (size_t j = 0; j < maxCols; j++)
			{
				if (j == 18 || j == 0 || j == 1 || j == 2 || j == 47 )
				{
					cell = sheet1->Cell(i + 1, j);
					cell->SetWString(vectordata[i + t * 66536][j].c_str());
				}
				else
				{
					cell = sheet1->Cell(i + 1, j);
					double Value = _wtof(vectordata[i + t * 66536][j].c_str());
					cell->SetDouble(Value);
				}
			}
		}

	}

	char * filepath = Cstring2char(szFilePathName);

	e.SaveAs(filepath);

	delete[] filepath;
}


void CQueryDataDlg::OnConfigUseraccount()
{
	// TODO: 配置账户信息到注册表

	ConfigAccount dlg;
	INT_PTR nResponsed = dlg.DoModal();
	if (nResponsed == IDOK)
	{
		m_Name = dlg.m_Account;
		m_Password = dlg.m_Password;


	}

}


void CQueryDataDlg::OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	SYSTEMTIME st = pDTChange->st;
	m_strstarttime.Format(L"%04d-%02d-%02d 00:00:00", st.wYear, st.wMonth, st.wDay);
	*pResult = 0;
}


void CQueryDataDlg::OnDtnDatetimechangeDatetimepickerStart2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	SYSTEMTIME st = pDTChange->st;
	m_strendtime.Format(L"%04d-%02d-%02d 00:00:00", st.wYear, st.wMonth, st.wDay);
	*pResult = 0;
}


void CQueryDataDlg::OnLvnGetdispinfoListRespot(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LVITEM *pItem = &pDispInfo->item;

	//if (pItem->mask & LVIF_TEXT) //字符串缓冲区有效
	//{
	//	switch (pItem->iSubItem) {
	//	case 0: //填充数据项的名字
	//		lstrcpy(pItem->pszText, vectordata[iItemIndx].m_strItemText);
	//		break;
	//	case 1: //填充子项1
	//		lstrcpy(pItem->pszText, vectordata[iItemIndx].m_strSubItem1Text);
	//		break;
	//	case 2: //填充子项2
	//		lstrcpy(pItem->pszText, vectordata[iItemIndx].m_strSubItem2Text);
	//		break;
	//	}
	//}
	int iItemIndx = pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //字符串缓冲区有效
	{
		// Todo:现阶段不会转化成循环，先这样
		switch (pItem->iSubItem) {
		case 0:
		{
			//填充数据项
			lstrcpy(pItem->pszText, vectordata[iItemIndx][0].c_str());
			break;
		}
#pragma region caseForeverycolumn
		case 1:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][1].c_str());
			break;
		}
		case 2:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][2].c_str());
			break;
		}
		case 3:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][3].c_str());
			break;
		}
		case 4:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][4].c_str());
			break;
		}
		case 5:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][5].c_str());
			break;
		}
		case 6:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][6].c_str());
			break;
		}
		case 7:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][7].c_str());
			break;
		}
		case 8:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][8].c_str());
			break;
		}
		case 9:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][9].c_str());
			break;
		}
		case 10:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][10].c_str());
			break;
		}
		case 11:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][11].c_str());
			break;
		}
		case 12:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][12].c_str());
			break;
		}
		case 13:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][13].c_str());
			break;
		}
		case 14:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][14].c_str());
			break;
		}
		case 15:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][15].c_str());
			break;
		}
		case 16:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][16].c_str());
			break;
		}
		case 17:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][17].c_str());
			break;
		}
		case 18:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][18].c_str());
			break;
		}
		case 19:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][19].c_str());
			break;
		}
		case 20:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][20].c_str());
			break;
		}
		case 21:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][21].c_str());
			break;
		}
		case 22:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][22].c_str());
			break;
		}
		case 23:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][23].c_str());
			break;
		}
		case 24:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][24].c_str());
			break;
		}
		case 25:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][25].c_str());
			break;
		}
		case 26:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][26].c_str());
			break;
		}
		case 27:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][27].c_str());
			break;
		}
		case 28:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][28].c_str());
			break;
		}
		case 29:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][29].c_str());
			break;
		}
		case 30:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][30].c_str());
			break;
		}
		case 31:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][31].c_str());
			break;
		}
		case 32:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][32].c_str());
			break;
		}
		case 33:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][33].c_str());
			break;
		}
		case 34:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][34].c_str());
			break;
		}
		case 35:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][35].c_str());
			break;
		}
		case 36:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][36].c_str());
			break;
		}
		case 37:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][37].c_str());
			break;
		}
		case 38:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][38].c_str());
			break;
		}
		case 39:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][39].c_str());
			break;
		}
		case 40:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][40].c_str());
			break;
		}
		case 41:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][41].c_str());
			break;
		}
		case 42:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][42].c_str());
			break;
		}
		case 43:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][43].c_str());
			break;
		}
		case 44:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][44].c_str());
			break;
		}
		case 45:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][45].c_str());
			break;
		}
		case 46:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][46].c_str());
			break;
		}
		case 47:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][47].c_str());
			break;
		}
		case 48:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][48].c_str());
			break;
		}
		case 49:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][49].c_str());
			break;
		}
		case 50:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][50].c_str());
			break;
		}
		case 51:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][51].c_str());
			break;
		}
		case 52:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][52].c_str());
			break;
		}
		case 53:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][53].c_str());
			break;
		}
		case 54:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][54].c_str());
			break;
		}
		case 55:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][55].c_str());
			break;
		}
		case 56:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][56].c_str());
			break;
		}
		case 57:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][57].c_str());
			break;
		}
		case 58:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][58].c_str());
			break;
		}
		case 59:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][59].c_str());
			break;
		}
		case 60:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][60].c_str());
			break;
		}
		case 61:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][61].c_str());
			break;
		}
		case 62:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][62].c_str());
			break;
		}
		case 63:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][63].c_str());
			break;
		}
		case 64:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][64].c_str());
			break;
		}
		case 65:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][65].c_str());
			break;
		}
		case 66:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][66].c_str());
			break;
		}
		case 67:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][67].c_str());
			break;
		}
		case 68:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][68].c_str());
			break;
		}
		case 69:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][69].c_str());
			break;
		}
		case 70:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][70].c_str());
			break;
		}
		case 71:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][71].c_str());
			break;
		}
		case 72:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][72].c_str());
			break;
		}
		case 73:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][73].c_str());
			break;
		}
		case 74:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][74].c_str());
			break;
		}
		case 75:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][75].c_str());
			break;
		}
		case 76:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][76].c_str());
			break;
		}
		case 77:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][77].c_str());
			break;
		}
		case 78:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][78].c_str());
			break;
		}
		case 79:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][79].c_str());
			break;
		}
		case 80:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][80].c_str());
			break;
		}
		case 81:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][81].c_str());
			break;
		}
		case 82:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][82].c_str());
			break;
		}
		case 83:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][83].c_str());
			break;
		}
		case 84:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][84].c_str());
			break;
		}
		case 85:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][85].c_str());
			break;
		}
		case 86:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][86].c_str());
			break;
		}
		case 87:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][87].c_str());
			break;
		}
		case 88:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][88].c_str());
			break;
		}
		case 89:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][89].c_str());
			break;
		}
		case 90:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][90].c_str());
			break;
		}
		case 91:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][91].c_str());
			break;
		}
		case 92:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][92].c_str());
			break;
		}
		case 93:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][93].c_str());
			break;
		}
		case 94:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][94].c_str());
			break;
		}
		case 95:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][95].c_str());
			break;
		}
		case 96:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][96].c_str());
			break;
		}
		case 97:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][97].c_str());
			break;
		}
		case 98:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][98].c_str());
			break;
		}
		case 99:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][99].c_str());
			break;
		}
		case 100:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][100].c_str());
			break;
		}
		case 101:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][101].c_str());
			break;
		}
		case 102:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][102].c_str());
			break;
		}
		case 103:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][103].c_str());
			break;
		}
		case 104:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][104].c_str());
			break;
		}
		case 105:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][105].c_str());
			break;
		}
		case 106:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][106].c_str());
			break;
		}
		case 107:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][107].c_str());
			break;
		}
		case 108:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][108].c_str());
			break;
		}
		case 109:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][109].c_str());
			break;
		}
		case 110:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][110].c_str());
			break;
		}
		case 111:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][111].c_str());
			break;
		}
		case 112:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][112].c_str());
			break;
		}
		case 113:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][113].c_str());
			break;
		}
		case 114:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][114].c_str());
			break;
		}
		case 115:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][115].c_str());
			break;
		}
		case 116:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][116].c_str());
			break;
		}
		case 117:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][117].c_str());
			break;
		}
		case 118:
		{
			lstrcpy(pItem->pszText, vectordata[iItemIndx][118].c_str());
			break;
		}


#pragma endregion
		}


		//for (pItem->iSubItem = 0; pItem->iSubItem < (int)vectordata[0].size(); ++pItem->iSubItem)
		//{
		//	size_t column = pItem->iSubItem;
		//	//char转化为wchar_t
		//	const char* orig = vectordata[iItemIndx][column].c_str();
		//	size_t newsize = strlen(orig) + 1;
		//	wchar_t * wcstring = new wchar_t[newsize];
		//	size_t convertedChars = 0;
		//	mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);

		//	lstrcpy(pItem->pszText, wcstring);
		//	delete[] wcstring;
		//}
	}

	*pResult = 0;
}


void CQueryDataDlg::ChangeMainPageLanguage()
{
	// 读取控件字符
	GetDlgItem(IDC_STATIC_CONFIG)->SetWindowText(My_LoadString(L"IDC_STATIC_CONFIG="));
	GetDlgItem(IDC_STATIC_CONDITION)->SetWindowText(My_LoadString(L"IDC_STATIC_GET_FAULT_DATA"));
	GetDlgItem(IDC_STATIC_REPORT)->SetWindowText(My_LoadString(L"IDC_STATIC_REPORT"));

	GetDlgItem(IDC_RADIO_HK)->SetWindowText(My_LoadString(L"IDC_RADIO_HK"));
	GetDlgItem(IDC_RADIO_AU)->SetWindowText(My_LoadString(L"IDC_RADIO_AU"));
	GetDlgItem(IDC_RADIO_EU)->SetWindowText(My_LoadString(L"IDC_RADIO_EU"));

	GetDlgItem(IDC_STATIC_SN)->SetWindowText(My_LoadString(L"IDC_STATIC_SN"));
	GetDlgItem(IDC_STATIC_STARR_TTIME)->SetWindowText(My_LoadString(L"IDC_STATIC_STARR_TTIME"));
	GetDlgItem(IDC_STATIC_END_TTIME)->SetWindowText(My_LoadString(L"IDC_STATIC_END_TTIME"));
	GetDlgItem(IDC_CHECK_NORMAL)->SetWindowText(My_LoadString(L"IDC_CHECK_NORMAL"));
	GetDlgItem(IDC_CHECK_INNORMAL)->SetWindowText(My_LoadString(L"IDC_CHECK_INNORMAL"));
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(My_LoadString(L"IDC_STATIC_NUM"));

	GetDlgItem(IDC_BUTTON_QUERY)->SetWindowText(My_LoadString(L"IDC_BUTTON_QUERY"));
	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(My_LoadString(L"IDC_BUTTON_SAVE"));

}

void CQueryDataDlg::OnBnClickedButtoEnglish()
{
	LoadLanguage();
	ChangeMainPageLanguage();
}


void CQueryDataDlg::OnBnClickedButtonChinese()
{
	LoadLanguage(L"Chinese");
	ChangeMainPageLanguage();

}
