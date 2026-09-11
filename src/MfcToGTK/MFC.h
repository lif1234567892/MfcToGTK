#pragma once
#include "wintypes.h"

#include <string>
#include <locale>
#include <codecvt>

using namespace std;

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
#ifndef CP_ACP
#define CP_ACP 0
#endif

#define afx_msg
#define OUT
#define IN

class CWnd
{

};

class CW2A
{
public:
	explicit CW2A(const wchar_t *wstr, unsigned int codePage = CP_UTF8);
	operator const char*() const
	{
		return m_utf8Str.c_str();
	}
private:
	std::string m_utf8Str;
};

class CA2W
{
public:
	explicit CA2W(const char *str, unsigned int codePage = CP_UTF8);
	operator const wchar_t*() const
	{
		return m_wstr.c_str();
	}
private:
	std::wstring m_wstr;
};

////////////////////////////////////////////////////////////////////
//CStringW
///////////////////////////////////////////////////////////////////
class CStringW
{
public:
	CStringW();
	CStringW(const CStringW &other);
	CStringW(const wchar_t *pstr);
	~CStringW();

public:
	CStringW& operator=(const CStringW &other);
	operator const wchar_t*() const;
	void Format(const wchar_t *lpFmt, ...);
	int GetLength() const;
	wchar_t *pstr = 0;
};

////////////////////////////////////////////////////////////////////
//CStringA
///////////////////////////////////////////////////////////////////
class CStringA
{
public:
	CStringA();
	CStringA(const CStringA &other);
	CStringA(const char *pstr);
	~CStringA();

public:
	CStringA& operator=(const CStringA &other);
	operator const char*() const;
	void Format(const char *lpFmt, ...);
	int GetLength() const;
	char *pstr = 0;
};

////////////////////////////////////////////////////////////////////
//CString Utils
///////////////////////////////////////////////////////////////////
CStringW operator+(const CStringW &l, const char *r);
CStringW operator+(const CStringW &l, const CStringW &r);



typedef CStringW CString;
struct _GtkWindow;
typedef _GtkWindow GtkWindow;
CString SelDir(GtkWindow *parent);
CString SelFile(GtkWindow *parent = nullptr, const char *title = "选择文件", const char *filterStr = nullptr);



typedef const wchar_t *LPCWSTR;
typedef LPCWSTR LPCTSTR;

#define _countof(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

#define DECLARE_MESSAGE_MAP() virtual void OnControlClick(GtkWidget *widget);

#define BEGIN_MESSAGE_MAP(DLG,BASEDLG)\
		void DLG::OnControlClick(GtkWidget *widget){\
			int widtetid=GetWidgetID(widget);\

#define END_MESSAGE_MAP() }

#define ON_WM_PAINT()
#define ON_WM_QUERYDRAGICON()

#define ON_BN_CLICKED(IDC_BTN, fnptr) {\
		auto fn=fnptr;\
		if(widtetid==IDC_BTN){ (this->*fn)();}\
}

