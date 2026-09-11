#include "MFC.h"
#include <gtk/gtk.h>

// 等价于你Windows下的SelDir函数
// 参数parent: 父窗口指针（对应原函数的HWND，传nullptr则为全局模态）
// 返回值：选中的目录路径（UTF8编码，用户点取消则返回空串）
CString SelDir(GtkWindow *parent)
{
	std::string strDir;
	const char *dialogTitle = "select dir";

	// 1. 创建目录选择对话框（优先用Native原生对话框，系统风格更统一）
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	GtkFileChooserNative *dialog = gtk_file_chooser_native_new(dialogTitle, parent, action, "选择", // 确定按钮文本
			"取消"); // 取消按钮文本

	// 2. 对应你原BROWSEINFO的flag配置
	// BIF_NONEWFOLDERBUTTON: 禁止新建文件夹
	gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(dialog), FALSE);
	// BIF_RETURNONLYFSDIRS: 只允许选择真实文件系统路径（默认开启）
	// BIF_NEWDIALOGSTYLE: GTK默认就是新样式对话框

	// 3. 阻塞运行对话框，等用户操作
	gint res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT) // 用户点击了确定按钮
	{
		// 获取选中的路径，返回的字符串需要手动释放
		gchar *szPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if (szPath)
		{
			strDir = szPath;
			g_free(szPath);
		}
	}

	// 4. 销毁对话框释放内存
	g_object_unref(dialog);

	return (LPCWSTR) CA2W(strDir.c_str(), CP_UTF8);
}

// 直接返回选中的文件路径，用户点取消返回空串
// 参数1: 父窗口指针，传nullptr则是全局模态
// 参数2: 对话框标题，可选
// 参数3: 文件过滤规则，可选，格式: "描述|*.ext1;*.ext2" 比如 "可执行文件|*.exe;*.dll"
CStringW SelFile(GtkWindow *parent, const char *title, const char *filterStr)
{
	std::string retPath;

	GtkFileChooserNative *dialog = gtk_file_chooser_native_new(title, parent, GTK_FILE_CHOOSER_ACTION_OPEN, "确定", "取消");
	if (!dialog)
		return L"";

	// 可选：设置文件过滤
	if (filterStr && *filterStr)
	{
		std::string filterStrCopy = filterStr;
		size_t splitPos = filterStrCopy.find('|');
		if (splitPos != std::string::npos)
		{
			std::string filterName = filterStrCopy.substr(0, splitPos);
			std::string filterPatterns = filterStrCopy.substr(splitPos + 1);

			GtkFileFilter *filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter, filterName.c_str());

			// 拆分多个后缀
			size_t pos = 0;
			while (pos != std::string::npos)
			{
				size_t nextPos = filterPatterns.find(';', pos);
				std::string pattern = filterPatterns.substr(pos, nextPos - pos);
				gtk_file_filter_add_pattern(filter, pattern.c_str());
				pos = nextPos == std::string::npos ? nextPos : nextPos + 1;
			}
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
		}
	}

	// 运行对话框
	gint res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if (path)
		{
			retPath = path;
			g_free(path);
		}
	}

	g_object_unref(dialog);
	return (LPCWSTR) CA2W(retPath.c_str(), CP_UTF8);
}

CW2A::CW2A(const wchar_t *wstr, unsigned int codePage)
{
	if (!wstr || codePage != CP_UTF8)
		return;
	try
	{
#if __SIZEOF_WCHAR_T__ == 2
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
#endif
		m_utf8Str = conv.to_bytes(wstr);
	}
	catch (...)
	{
		m_utf8Str.clear();
	}
}

CA2W::CA2W(const char *str, unsigned int codePage)
{
	if (!str || (codePage != CP_UTF8 && codePage != CP_ACP))
		return;
	try
	{
#if __SIZEOF_WCHAR_T__ == 2
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
#endif
		m_wstr = conv.from_bytes(str);
	}
	catch (...)
	{
		m_wstr.clear();
	}
}

////////////////////////////////////////////////////////////////////
//CStringW
///////////////////////////////////////////////////////////////////
CStringW::CStringW()
{
	pstr = wcsdup(L"");
}

// 拷贝构造：深拷贝
CStringW::CStringW(const CStringW &other)
{
	pstr = wcsdup(other.pstr);
}

CStringW::CStringW(const wchar_t *pstr)
{
	if (pstr)
		this->pstr = wcsdup(pstr);
}

CStringW::~CStringW()
{
	if (pstr)
	{
		free (pstr);
		pstr = 0;
	}
}

// 赋值运算符：深拷贝，避免自赋值
CStringW& CStringW::operator=(const CStringW &other)
{
	if (this == &other)
		return *this;
	free (pstr);
	pstr = wcsdup(other.pstr);
	return *this;
}

CStringW::operator const wchar_t*() const
{
	return pstr;
}

// 可变参数格式化，自动计算缓冲区大小无溢出
void CStringW::Format(const wchar_t *lpFmt, ...)
{
	if (!lpFmt)
	{
		free (pstr);
		pstr = wcsdup(L"");
		return;
	}
	va_list args;
	va_start(args, lpFmt);
	// 先计算需要的缓冲区长度
	int reqLen = vswprintf(nullptr, 0, lpFmt, args);
	va_end(args);
	if (reqLen <= 0)
	{
		free (pstr);
		pstr = wcsdup(L"");
		return;
	}
	// 分配内存后格式化
	free (pstr);
	pstr = (wchar_t*) malloc((reqLen + 1) * sizeof(wchar_t));
	va_start(args, lpFmt);
	vswprintf(pstr, reqLen + 1, lpFmt, args);
	va_end(args);
}

int CStringW::GetLength() const
{
	return wcslen(pstr);
}

////////////////////////////////////////////////////////////////////
//CStringA
///////////////////////////////////////////////////////////////////
CStringA::CStringA()
{
	pstr = strdup("");
}

// 拷贝构造
CStringA::CStringA(const CStringA &other)
{
	pstr = strdup(other.pstr);
}

CStringA::CStringA(const char *pstr)
{
	if (pstr)
		this->pstr = strdup(pstr);
}

CStringA::~CStringA()
{
	if (pstr)
	{
		free (pstr);
		pstr = 0;
	}
}

// 赋值运算符
CStringA& CStringA::operator=(const CStringA &other)
{
	if (this == &other)
		return *this;
	free (pstr);
	pstr = strdup(other.pstr);
	return *this;
}

CStringA::operator const char*() const
{
	return pstr;
}

// 格式化
void CStringA::Format(const char *lpFmt, ...)
{
	if (!lpFmt)
	{
		free (pstr);
		pstr = strdup("");
		return;
	}
	va_list args;
	va_start(args, lpFmt);
	int reqLen = vsnprintf(nullptr, 0, lpFmt, args);
	va_end(args);
	if (reqLen <= 0)
	{
		free (pstr);
		pstr = strdup("");
		return;
	}
	free (pstr);
	pstr = (char*) malloc(reqLen + 1);
	va_start(args, lpFmt);
	vsnprintf(pstr, reqLen + 1, lpFmt, args);
	va_end(args);
}
int CStringA::GetLength() const
{
	return strlen(pstr);
}

////////////////////////////////////////////////////////////////////
//CString Utils
///////////////////////////////////////////////////////////////////

// 重载+：CStringW + UTF8 char*，自动转码拼接
CStringW operator+(const CStringW &l, const char *r)
{
	if (!r || !*r)
		return l;
	CA2W wR(r, CP_UTF8); // 复用之前的UTF8转宽字符工具
	size_t lenL = l.GetLength();
	size_t lenR = wcslen((const wchar_t*) wR);
	CStringW res;
	free(res.pstr);
	res.pstr = (wchar_t*) malloc(sizeof(wchar_t) * (lenL + lenR + 1));
	wcscpy(res.pstr, l.pstr);
	wcscat(res.pstr, (const wchar_t*) wR);
	return res;
}

// 重载+：两个CStringW拼接
CStringW operator+(const CStringW &l, const CStringW &r)
{
	size_t lenL = l.GetLength();
	size_t lenR = r.GetLength();
	CStringW res;
	free(res.pstr);
	res.pstr = (wchar_t*) malloc(sizeof(wchar_t) * (lenL + lenR + 1));
	wcscpy(res.pstr, l.pstr);
	wcscat(res.pstr, r.pstr);
	return res;
}

