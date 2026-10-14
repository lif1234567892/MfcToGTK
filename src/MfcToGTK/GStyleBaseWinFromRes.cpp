#include "GStyleBaseWinFromRes.h"
#include <string>
#include <vector>
#include <algorithm>
#include "MFCGTKUtils.h"

using namespace std;

extern int ConvertControlID(const char *pIDSTR);

static int MyConvertControlID(const char *pIDSTR)
{
	if (0 == strcmp(pIDSTR, "IDC_STATIC"))
		return 99000001;
	return ConvertControlID(pIDSTR);
}

static void StringReplace(std::string &str, const std::string &old_str, const std::string &new_str)
{
	size_t pos = 0;
	while ((pos = str.find(old_str, pos)) != std::string::npos)
	{
		str.replace(pos, old_str.length(), new_str);
		pos += new_str.length();
	}
}

// ========== 解析MFC编辑框风格字符串 ==========
int GStyleBaseWinFromRes::ParseEditStyle(const std::string &styleStr)
{
	int style = EDIT_STYLE_NONE;
	std::string cleanStr = styleStr;
	// 去掉所有空格、引号、换行
	cleanStr.erase(remove_if(cleanStr.begin(), cleanStr.end(), ::isspace), cleanStr.end());
	StringReplace(cleanStr, "\"", "");
	// 按|拆分所有风格标志
	vector<string> flags;
	MFCUtils::StringSpiltEx(cleanStr, flags, "|", false, true);
	for (const auto &flag : flags)
	{
		if (flag == "ES_MULTILINE")
			style |= EDIT_STYLE_MULTILINE;
		else if (flag == "WS_VSCROLL" || flag == "ES_AUTOVSCROLL")
			style |= EDIT_STYLE_VSCROLL | EDIT_STYLE_AUTOVSCROLL;
		else if (flag == "WS_HSCROLL" || flag == "ES_AUTOHSCROLL")
			style |= EDIT_STYLE_HSCROLL | EDIT_STYLE_AUTOHSCROLL;
		else if (flag == "ES_PASSWORD")
			style |= EDIT_STYLE_PASSWORD;
		else if (flag == "ES_READONLY")
			style |= EDIT_STYLE_READONLY;
	}
	return style;
}

GStyleBaseWinFromRes::GStyleBaseWinFromRes()
{

}

GStyleBaseWinFromRes::~GStyleBaseWinFromRes()
{
	DestroyGTKInterface();
}

bool GStyleBaseWinFromRes::ParseResourceString(const std::string &strResDesc)
{
	vector<string> vetLines;
	MFCUtils::StringSpiltEx(strResDesc, vetLines, "\n", false, true);

	for (size_t iL = 0; iL < vetLines.size(); iL++)
	{
		std::string line = vetLines[iL];
		string tail;
		string strType = MFCUtils::StringFirstSplit(line, " ", tail);
		vector<string> atrs;
		MFCUtils::StringSpiltEx(tail, atrs, ",", false, true);

		int id = 0;
		int ctrl_left = 0, ctrl_top = 0;
		int ctrl_width = 0, ctrl_height = 0;
		string strCtrlText;
		int ctrlStyle = EDIT_STYLE_NONE;

		if (0 == strType.compare("DIALOG")) //DIALOG w,h
		{
			if (atrs.size() >= 2)
			{
				m_dialogWidth = m_Scale * atoi(atrs[0].c_str());
				m_dialogHeight = m_Scale * atoi(atrs[1].c_str());
			}
			if (atrs.size() >= 3)
			{
				m_dialogTitle = atrs[2];
			}
		}
		else if (0 == strType.compare("EDITTEXT")) // IDC_EDIT1,53,16,352,14,ES_AUTOHSCROLL
		{
			if (atrs.size() >= 5)
			{
				id = MyConvertControlID(atrs[0].c_str());
				ctrl_left = m_Scale * atoi(atrs[1].c_str());
				ctrl_top = m_Scale * atoi(atrs[2].c_str());
				ctrl_width = m_Scale * atoi(atrs[3].c_str());
				ctrl_height = m_Scale * atoi(atrs[4].c_str());
				// 有第6个参数则解析风格
				if (atrs.size() >= 6)
					ctrlStyle = ParseEditStyle(atrs[5]);
			}
		}
		else if (0 == strType.compare("LTEXT")) //text,id, l,t,w,h
		{
			if (atrs.size() >= 6)
			{
				strCtrlText = atrs[0];
				id = MyConvertControlID(atrs[1].c_str());
				ctrl_left = m_Scale * atoi(atrs[2].c_str());
				ctrl_top = m_Scale * atoi(atrs[3].c_str());
				ctrl_width = m_Scale * atoi(atrs[4].c_str());
				ctrl_height = m_Scale * atoi(atrs[5].c_str());
			}
		}
		else if (0 == strType.compare("PUSHBUTTON")) //text,id, l,t,w,h
		{
			if (atrs.size() >= 6)
			{
				strCtrlText = atrs[0];
				id = MyConvertControlID(atrs[1].c_str());
				ctrl_left = m_Scale * atoi(atrs[2].c_str());
				ctrl_top = m_Scale * atoi(atrs[3].c_str());
				ctrl_width = m_Scale * atoi(atrs[4].c_str());
				ctrl_height = m_Scale * atoi(atrs[5].c_str());
			}
		}
		else if (0 == strType.compare("CHECKBOX")) //text,id, "unuseType",unuseStyle,l,t,w,h
		{
			if (atrs.size() >= 8)
			{
				strCtrlText = atrs[0];
				id = MyConvertControlID(atrs[1].c_str());
				ctrl_left = m_Scale * atoi(atrs[4].c_str());
				ctrl_top = m_Scale * atoi(atrs[5].c_str());
				ctrl_width = m_Scale * atoi(atrs[6].c_str());
				ctrl_height = m_Scale * atoi(atrs[7].c_str());
			}
		}
		StringReplace(strCtrlText, "&comma;", ",");
		MFCUtils::TrimStr(strCtrlText, '\"');
		MFCUtils::TrimStr(m_dialogTitle, '\"');
		// 添加到控件列表
		if (id > 0)
		{
			ControlDef control;
			control.id = id;
			control.type = strType;
			control.x = ctrl_left;
			control.y = ctrl_top;
			control.width = ctrl_width;
			control.height = ctrl_height;
			control.text = strCtrlText;
			control.style = ctrlStyle; // 存储解析后的风格
			m_controls.push_back(control);
		}
	}

	return true;
}

std::string GStyleBaseWinFromRes::GetEditText(int ctrlId)
{
	auto it = m_controlMap.find(ctrlId);
	if (it == m_controlMap.end() || it->second._ty != WIDGET_T_Text || !it->second.pInnerWidget)
		return "";
	// 单行输入框
	if (GTK_IS_ENTRY(it->second.pInnerWidget))
		return gtk_entry_get_text(GTK_ENTRY(it->second.pInnerWidget));
	// 多行输入框
	else if (GTK_IS_TEXT_VIEW(it->second.pInnerWidget))
	{
		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(it->second.pInnerWidget));
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer, &start, &end);
		return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	}
	return "";
}

// ========== 新增：统一设置输入框内容 ==========
void GStyleBaseWinFromRes::SetEditText(int ctrlId, const std::string &text)
{
	auto it = m_controlMap.find(ctrlId);
	if (it == m_controlMap.end() || it->second._ty != WIDGET_T_Text || !it->second.pInnerWidget)
		return;
	if (GTK_IS_ENTRY(it->second.pInnerWidget))
		gtk_entry_set_text(GTK_ENTRY(it->second.pInnerWidget), text.c_str());
	else if (GTK_IS_TEXT_VIEW(it->second.pInnerWidget))
	{
		GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(it->second.pInnerWidget));
		gtk_text_buffer_set_text(buffer, text.c_str(), -1);
	}
}

// ========== 获取复选框状态 ==========
bool GStyleBaseWinFromRes::GetCheckBoxChecked(int ctrlId)
{
	auto it = m_controlMap.find(ctrlId);
	if (it == m_controlMap.end() || it->second._ty != WIDGET_T_CheckBox || !it->second.pInnerWidget)
		return false;
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(it->second.pInnerWidget));
}

// ========== 设置复选框状态 ==========
void GStyleBaseWinFromRes::SetCheckBoxChecked(int ctrlId, bool checked)
{
	auto it = m_controlMap.find(ctrlId);
	if (it == m_controlMap.end() || it->second._ty != WIDGET_T_CheckBox || !it->second.pInnerWidget)
		return;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(it->second.pInnerWidget), checked);
}

void GStyleBaseWinFromRes::OnControlClick(GtkWidget *widget)
{
	///g_object_set_data(G_OBJECT(pWid), "ctrl_id", GINT_TO_POINTER(control.id));
	gpointer gpID = g_object_get_data(G_OBJECT(widget), "ctrl_id");
	int ctrlId = GPOINTER_TO_INT(gpID);
	switch (ctrlId)
	{
	case 1: // 对应你定义的控件ID
		//g_print("选择文件按钮被点击\n");
		break;
	case 2:
		//g_print("选择目录按钮被点击\n");
		break;
		// 其他控件事件处理
	default:
		//g_print("控件ID:%d 触发事件\n", ctrlId);
		break;
	}
}

void G_OnControlClick(GtkWidget *widget, gpointer user_data)
{
	GStyleBaseWinFromRes *pThis = (GStyleBaseWinFromRes*) (user_data);
	pThis->OnControlClick(widget);
}

void GStyleBaseWinFromRes::OnDestroy()
{
	DestroyGTKInterface();
}

// 销毁整个界面
void GStyleBaseWinFromRes::DestroyGTKInterface()
{
	if (GTK_IS_WINDOW(m_hWnd_Widget))
	{
		// 销毁主窗口会自动递归销毁所有子控件、容器
		gtk_widget_destroy(m_hWnd_Widget);
	}
	// 所有指针置空、map清空，避免野指针
	m_hWnd_Widget = nullptr;
	m_controlMap.clear();
}

GtkWidget* GStyleBaseWinFromRes::GetWidgetByID(int id)
{
	map<int, WidgetAndType>::iterator ite = m_controlMap.find(id);
	if (ite != m_controlMap.end())
		return ite->second.pWidget;
	return 0;
}

WidgetType GStyleBaseWinFromRes::GetWidgetTypeByID(int id)
{
	map<int, WidgetAndType>::iterator ite = m_controlMap.find(id);
	if (ite != m_controlMap.end())
		return ite->second._ty;
	return WIDGET_T_None;
}

WidgetType GStyleBaseWinFromRes::GetWidgetType(const string &strT)
{
	if (strT == "EDITTEXT")
	{
		return WIDGET_T_Text;
	}
	else if (strT == "PUSHBUTTON")
	{
		return WIDGET_T_PushBtn;
	}
	else if (strT == "LTEXT")
	{
		return WIDGET_T_Static;
	}
	else if (strT == "CHECKBOX")
	{
		return WIDGET_T_CheckBox;
	}
	return WIDGET_T_None;
}

int GStyleBaseWinFromRes::GetWidgetID(GtkWidget *widget)
{
	gpointer gpID = g_object_get_data(G_OBJECT(widget), "ctrl_id");
	int ctrlId = GPOINTER_TO_INT(gpID);
	return ctrlId;
}

// 主窗口关闭回调：自动清理类内指针
void OnMainWindowDestroy(GtkWidget *widget, gpointer user_data)
{
	GStyleBaseWinFromRes *pParser = static_cast<GStyleBaseWinFromRes*>(user_data);
	pParser->OnDestroy();
}

// 窗口关闭回调：退出模态循环
gboolean OnModalWindowClose(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	GStyleBaseWinFromRes *pDlg = static_cast<GStyleBaseWinFromRes*>(user_data);
	pDlg->m_modalRet = IDCANCEL; // 默认关闭返回取消
	if (pDlg->m_modalLoop && g_main_loop_is_running(pDlg->m_modalLoop))
	{
		g_main_loop_quit(pDlg->m_modalLoop); // 退出嵌套循环
	}
	return FALSE;
}

// 创建GTK界面
void GStyleBaseWinFromRes::CreateGTKInterface()
{
	// 创建主窗口
	m_hWnd_Widget = gtk_window_new(GTK_WINDOW_TOPLEVEL); //GTK_WINDOW_POPUP
	if (m_hWnd_Widget)
		m_hWnd = GTK_WINDOW(m_hWnd_Widget);
	gtk_window_set_title(GTK_WINDOW(m_hWnd_Widget), m_dialogTitle.c_str());
	gtk_window_set_default_size(GTK_WINDOW(m_hWnd_Widget), m_dialogWidth, m_dialogHeight);
	// 固定窗口大小，避免用户拖动窗口导致布局错乱
	gtk_window_set_resizable(GTK_WINDOW(m_hWnd_Widget), FALSE);

	// ========== 新增模态配置 ==========
	// 设为模态：阻塞父窗口输入
	gtk_window_set_modal(GTK_WINDOW(m_hWnd_Widget), TRUE);
	// 可选：绑定父窗口，模态时父窗口自动灰掉，符合系统交互逻辑
	// gtk_window_set_transient_for(GTK_WINDOW(m_hWnd_Widget), 父窗口GTK_WINDOW指针);

	// 绑定窗口关闭信号：关闭时退出模态循环
	g_signal_connect(m_hWnd_Widget, "delete-event", G_CALLBACK(OnModalWindowClose), this);

	// 绑定窗口关闭信号，传当前类实例为参数
	g_signal_connect(m_hWnd_Widget, "destroy", G_CALLBACK(OnMainWindowDestroy), this);

	// 替换为GtkFixed绝对布局容器
	GtkWidget *fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(m_hWnd_Widget), fixed);

	// 清空之前的控件映射
	m_controlMap.clear();

	// 为每个控件创建GTK组件
	for (const auto &control : m_controls)
	{
		GtkWidget *pWid = CreateControlFromDefinition(control);
		if (pWid)
		{
			// 核心：按资源定义的x/y坐标把控件放到固定容器里
			gtk_fixed_put(GTK_FIXED(fixed), pWid, control.x, control.y);
			// 把控件ID和控件关联，方便后续查找
			g_object_set_data(G_OBJECT(pWid), "ctrl_id", GINT_TO_POINTER(control.id));

			WidgetAndType wtt;
			wtt.pWidget = pWid;
			wtt._ty = GetWidgetType(control.type);
			// 给InnerWidget赋值
			if (wtt._ty == WIDGET_T_Text)
			{
				// 多行输入框Inner是TextView，存在g_object_data里
				GtkWidget *pInner = (GtkWidget*) g_object_get_data(G_OBJECT(pWid), "inner_text_widget");
				wtt.pInnerWidget = pInner ? pInner : pWid;
			}
			else
			{
				wtt.pInnerWidget = pWid;
			}
			m_controlMap[control.id] = wtt;

			// 可选：绑定控件事件，比如按钮点击事件
			if (control.type == "PUSHBUTTON")
			{
				g_signal_connect(pWid, "clicked", G_CALLBACK(G_OnControlClick), GINT_TO_POINTER(this));
			}
			else if (control.type == "CHECKBOX")
			{
				g_signal_connect(pWid, "toggled", G_CALLBACK(G_OnControlClick), GINT_TO_POINTER(this));
			}
		}
	}
	// 5. CSS清除默认内边距，这一步是实现12x12的关键
	const char *css = "button, entry, checkbutton, radiobutton, combobox, label, scale { padding: 0px; margin: 0px; border-width: 1px; min-height: 0px; min-width: 0px; } \n"
			"checkbutton check, radiobutton radio {  padding: 0px; margin: 0px; min-height: 0px; min-width: 0px; } \n"
			"scrolledwindow { border-width: 1px; padding:0; margin:0; min-height:0px; } \n"
			"textview { padding: 2px; margin:0; min-height:0px; } \n"
			"combobox box { padding: 0px; margin: 0px; }";

	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(provider, css, -1, NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider),
	GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(provider); // 新增：释放CSS provider避免内存泄漏
}

// 获取控件列表
const std::vector<ControlDef>& GStyleBaseWinFromRes::GetControls() const
{
	return m_controls;
}

// 全局函数实现
GtkWidget* CreateControlFromDefinition(const ControlDef &control)
{
	if (control.type == "EDITTEXT")
	{
		return CreateEditBox(control);
	}
	else if (control.type == "PUSHBUTTON")
	{
		return CreateButton(control);
	}
	else if (control.type == "LTEXT")
	{
		return CreateLabel(control);
	}
	else if (control.type == "CHECKBOX")
	{
		return CreateCheckBox(control);
	}
	return 0;
}

GtkWidget* CreateEditBox(const ControlDef &control)
{
	// 单行编辑框
	if (!(control.style & EDIT_STYLE_MULTILINE))
	{
		GtkWidget *entry = gtk_entry_new();
		gtk_widget_set_size_request(entry, control.width, control.height);
		// 密码模式
		if (control.style & EDIT_STYLE_PASSWORD)
			gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
		// 只读
		if (control.style & EDIT_STYLE_READONLY)
			gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
		return entry;
	}
	// 多行编辑框：滚动容器套TextView
	else
	{
		GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_set_size_request(scrolled, control.width, control.height);
		// 滚动条策略对齐MFC行为
		GtkPolicyType vPolicy = (control.style & EDIT_STYLE_VSCROLL) ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER;
		GtkPolicyType hPolicy = (control.style & EDIT_STYLE_HSCROLL) ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER;
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), hPolicy, vPolicy);
		// 创建TextView
		GtkWidget *textView = gtk_text_view_new();
		gtk_container_add(GTK_CONTAINER(scrolled), textView);
		// 自动换行：没有水平滚动条时自动换行
		if (!(control.style & EDIT_STYLE_HSCROLL))
			gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_WORD_CHAR);
		// 只读
		if (control.style & EDIT_STYLE_READONLY)
			gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
		// 把实际TextView存在滚动容器的data里，后面取内容用
		g_object_set_data(G_OBJECT(scrolled), "inner_text_widget", textView);
		return scrolled;
	}
}

GtkWidget* CreateButton(const ControlDef &control)
{
	GtkWidget *button = gtk_button_new_with_label(control.text.c_str());
	gtk_widget_set_size_request(button, control.width, control.height);
	// 添加到容器中
	return button;
}

GtkWidget* CreateCheckBox(const ControlDef &control)
{
	GtkWidget *checkbox = gtk_check_button_new_with_label(control.text.c_str());
	gtk_widget_set_size_request(checkbox, control.width, control.height);
	// 添加到容器中
	return checkbox;
}

GtkWidget* CreateLabel(const ControlDef &control)
{
	GtkWidget *label = gtk_label_new(control.text.c_str());
	gtk_widget_set_size_request(label, control.width, control.height);
	// 添加到容器中
	return label;
}
