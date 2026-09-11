
#ifdef _WIN32
#include "stdafx.h"
#include "MFC_TestTool.h"
#endif//_WIN32

#include "MFC_TestTool.h"

#ifdef _WIN32
#include "afxdialogex.h"
#endif//_WIN32

#include <jni.h>



#ifdef _WIN32
CString SelDir(HWND hwnd)
{
	CString str = L"select dir";
	CString strDir;
	TCHAR szBuffer[MAX_PATH * 2] = { 0 };

	LPITEMIDLIST pIDList = NULL;
	{
		//经过测试关不关重定向都一回事，那么 请写程序完成转换（1）Native->tem32(2)tem32->wow64
		BROWSEINFO bi;
		bi.hwndOwner = hwnd;
		bi.pidlRoot = NULL; //初始化制定的root目录很不容易
		bi.pszDisplayName = szBuffer;//此参数如为NULL则不能显示对话框
		bi.lpszTitle = str;
		bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = NULL;
		//初始化入口参数bi结束
		pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框

		if (pIDList)
		{
			SHGetPathFromIDList(pIDList, szBuffer);
			strDir = szBuffer;
		}
		IMalloc* piMalloc = 0;
		if (pIDList && SUCCEEDED(SHGetMalloc(&piMalloc)) && piMalloc)
		{
			piMalloc->Free(pIDList);
			piMalloc->Release();
		}
	}
	return strDir;
}
#endif//_WIN32

CMFC_TestTool::CMFC_TestTool(CWnd* pParent /*=nullptr*/)
	:
#ifdef _WIN32
			CDialog(IDD_MFC_TEST_DIALOG, pParent),
#endif//_WIN32

	m_strSelFile(L"")
	, m_strSelDir(L"")
	, m_strSelLibDir(L"")
	, m_strClientID(L"")
	, m_strResult(L"")
	, TSaveLoadCFG(L"CMFCtestDlg")

{
#ifdef _WIN32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#endif//_WIN32
}

void CMFC_TestTool::DoDataExchange(CDataExchange* pDX)
{
#ifdef _WIN32
	CDialog::DoDataExchange(pDX);
#endif//!_WIN32

	DDX_Text(pDX, IDC_EDIT1, m_strSelFile);
	DDX_Text(pDX, IDC_EDIT2, m_strSelDir);
	DDX_Text(pDX, IDC_EDIT3, m_strSelLibDir);
	DDX_Text(pDX, IDC_EDIT4, m_strClientID);
	DDX_Text(pDX, IDC_EDIT5, m_strResult);

}

SaveLoad_CfgNode * CMFC_TestTool::GetSaveLoadCfgArray(int &OUT nArrayLen, bool &OUT bNeedFree)
{
	SaveLoad_CfgNode cfg[] =
	{
		{L"m_strSelFile", &m_strSelFile},
		{L"m_strSelDir",   &m_strSelDir},
		{L"m_strSelLibDir",   &m_strSelLibDir},
		{L"m_strClientID",   &m_strClientID},

 
	};
	nArrayLen = _countof(cfg);
	bNeedFree = true;
	return CopyCreateSaveLoadCfgArray(cfg, nArrayLen);
}

BEGIN_MESSAGE_MAP(CMFC_TestTool, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_SelFile, &CMFC_TestTool::OnBnClickedBSelfile)
	ON_BN_CLICKED(IDC_B_SelDir, &CMFC_TestTool::OnBnClickedBSeldir)
	ON_BN_CLICKED(IDC_B_SelLibDir, &CMFC_TestTool::OnBnClickedBSellibdir)

END_MESSAGE_MAP()




BOOL CMFC_TestTool::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _WIN32
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
#endif//_WIN32
	// TODO: 在此添加额外的初始化代码
	SaveOrLoadCfg(FALSE);
	UpdateData(FALSE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFC_TestTool::OnPaint()
{
#ifdef _WIN32
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
		CDialog::OnPaint();
	}
#endif//_WIN32
}

#ifdef _WIN32
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFC_TestTool::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#endif//_WIN32

JNIEnv g_env;
jobject g_thiz;
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testtool_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz,
	jstring str_test_cmd, jstring args);

CString stringFromJNI(CString funName, CString args)
{
	jstring js_F = (const char*)CW2A(funName, CP_UTF8);
	jstring js_arg = (const char*)CW2A(args, CP_UTF8);
	jstring jret = Java_com_example_testtool_MainActivity_stringFromJNI(&g_env, g_thiz, js_F, js_arg);
	CString strRet = (LPCWSTR)CA2W(jret.pstr, CP_UTF8);
	return strRet;
}

void CMFC_TestTool::CallMethodSetResult(CString md, CString mdArg) {
	CString ret = stringFromJNI(md, mdArg);
	m_strResult = ret;
	UpdateData(FALSE);
}


CString get_tmp_dir()
{
#ifdef _WIN32
	wchar_t szTemp[MAX_PATH] = {};
	GetTempPathW(MAX_PATH, szTemp);
	CString str = szTemp;
	if (str.GetLength() > 0 && str[str.GetLength() - 1] == '\\')
		str = str.Left(str.GetLength() - 1);
	return str;
#else//!_WIN32
    const char* tmpEnv = getenv("TMPDIR");
    std::string tmpPath = (tmpEnv && *tmpEnv) ? tmpEnv : "/tmp";
    if (tmpPath.size() > 1 && tmpPath.back() == '/') {
        tmpPath.pop_back();
    }
    return (LPCWSTR)CA2W(tmpPath.c_str(),CP_UTF8);
#endif//_WIN32
}


CString CMFC_TestTool::GetCommonArgs_LTCD() {
	return m_strSelLibDir + "<<>>" +
		get_tmp_dir() + "<<>>" +
		m_strClientID + "<<>>" +
		m_strSelLibDir;
}



CString CMFC_TestTool::GetCommonArgs_File_LTCD() {
	return GetCommonArgs_File() + "<<>>" + GetCommonArgs_LTCD();
}

CString CMFC_TestTool::GetCommonArgs_Dir_LTCD() {
	return m_strSelDir + "<<>>" + GetCommonArgs_LTCD();
}

CString CMFC_TestTool::GetCommonArgs_File() {
	return m_strSelFile;
}





void CMFC_TestTool::OnBnClickedBSelfile()
{
	UpdateData(TRUE);

#ifdef _WIN32
	CFileDialog fd(TRUE);
	if (IDOK == fd.DoModal()) {
		m_strSelFile = fd.GetPathName();
		UpdateData(FALSE);
	}
#else//!_WIN32
	m_strSelFile = SelFile();
	UpdateData(FALSE);
#endif//_WIN32
}

void CMFC_TestTool::OnBnClickedBSeldir()
{
	UpdateData(TRUE);
	CString s = SelDir(m_hWnd);
	if (s.GetLength()) {
		m_strSelDir = s;
		UpdateData(FALSE);
	}
}

void CMFC_TestTool::OnBnClickedBSellibdir()
{
	UpdateData(TRUE);
	CString s = SelDir(m_hWnd);
	if (s.GetLength()) {
		m_strSelLibDir = s;
		UpdateData(FALSE);
	}
}


