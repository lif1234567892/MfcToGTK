#include "CDialog.h"

CDialog::CDialog()
{
}

CDialog::~CDialog()
{
}

void CDialog::DoDataExchange(CDataExchange *pDX)
{
}

BOOL CDialog::OnInitDialog()
{
	return TRUE;
}

void CDialog::UpdateData(BOOL bToVar)
{
	CDataExchange exg;
	exg.bToVar = bToVar;
	DoDataExchange(&exg);
}

int CDialog::DoModal()
{
	if (ParseResourceString(m_strResString))
	{
		CreateGTKInterface();
		OnInitDialog();

		gtk_widget_show_all(m_hWnd_Widget);
		// 创建嵌套事件循环，阻塞直到窗口关闭
		m_modalLoop = g_main_loop_new(nullptr, FALSE);
		g_main_loop_run (m_modalLoop);
		g_main_loop_unref(m_modalLoop);
		m_modalLoop = nullptr;
		// 窗口关闭后销毁，返回结果
		DestroyGTKInterface();
		return m_modalRet;
	}
}

void CDialog::SetRsourceString(const char *pRes)
{
	m_strResString = pRes;
}

void CDialog::DDX_Text(CDataExchange *pDX, int ctrlID, CStringW &strVar)
{
	GtkWidget *widget = this->GetWidgetByID(ctrlID);
	WidgetType wt = this->GetWidgetTypeByID(ctrlID);
	if (pDX->bToVar)
	{
		if (wt == WIDGET_T_Static)
		{
			const char *text = gtk_label_get_text(GTK_LABEL(widget));
			strVar = (const wchar_t*) CA2W(text, CP_UTF8);
		}
		else if (wt == WIDGET_T_Text)
		{
			const char *text = gtk_entry_get_text(GTK_ENTRY(widget));
			strVar = (const wchar_t*) CA2W(text, CP_UTF8);
		}
		else if (wt == WIDGET_T_PushBtn)
		{
			const char *text = gtk_button_get_label(GTK_BUTTON(widget));
			strVar = (const wchar_t*) CA2W(text, CP_UTF8);
		}
		else if (wt == WIDGET_T_CheckBox)
		{
			const char *text = gtk_button_get_label(GTK_BUTTON(widget));
			strVar = (const wchar_t*) CA2W(text, CP_UTF8);
		}
	}
	else
	{
		if (wt == WIDGET_T_Static)
		{
			gtk_label_set_text(GTK_LABEL(widget), (const char*) CW2A(strVar, CP_UTF8));
		}
		else if (wt == WIDGET_T_Text)
		{
			gtk_entry_set_text(GTK_ENTRY(widget), (const char*) CW2A(strVar, CP_UTF8));
		}
		else if (wt == WIDGET_T_PushBtn)
		{
			gtk_button_set_label(GTK_BUTTON(widget), (const char*) CW2A(strVar, CP_UTF8));
		}
		else if (wt == WIDGET_T_CheckBox)
		{
			gtk_button_set_label(GTK_BUTTON(widget), (const char*) CW2A(strVar, CP_UTF8));
		}
	}
}

void CDialog::DDX_Check(CDataExchange *pDX, int ctrlID, BOOL &boolVar)
{
	GtkWidget *widget = this->GetWidgetByID(ctrlID);
	WidgetType wt = this->GetWidgetTypeByID(ctrlID);
	if (pDX->bToVar)
	{
		if (wt == WIDGET_T_CheckBox)
		{
			gboolean is_check = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
			boolVar = !!is_check;
		}
	}
	else
	{
		if (wt == WIDGET_T_CheckBox)
		{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), boolVar);
		}
	}
}

