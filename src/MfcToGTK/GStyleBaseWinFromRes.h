#pragma once
#include <string>
#include <vector>
#include <map>
#include <gtk/gtk.h>

using namespace std;

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7

// ========== 新增：编辑框风格标志位，对应MFC RC属性 ==========
enum EditStyleFlags {
    EDIT_STYLE_NONE         = 0,
    EDIT_STYLE_MULTILINE    = 1 << 0,  // ES_MULTILINE 多行
    EDIT_STYLE_VSCROLL      = 1 << 1,  // WS_VSCROLL 垂直滚动条
    EDIT_STYLE_HSCROLL      = 1 << 2,  // WS_HSCROLL 水平滚动条
    EDIT_STYLE_PASSWORD     = 1 << 3,  // ES_PASSWORD 密码输入
    EDIT_STYLE_READONLY     = 1 << 4,  // ES_READONLY 只读
    EDIT_STYLE_AUTOHSCROLL  = 1 << 5,  // ES_AUTOHSCROLL 水平自动滚动
    EDIT_STYLE_AUTOVSCROLL  = 1 << 6,  // ES_AUTOVSCROLL 垂直自动滚动
};

// 控件定义结构
struct ControlDef {
    int id;
    std::string type;
    std::string text;
    int x, y, width, height;
    int style = EDIT_STYLE_NONE; // 新增：存储控件风格标志位
};

enum WidgetType
{
	WIDGET_T_None,
	WIDGET_T_Static,
	WIDGET_T_Text,
	WIDGET_T_PushBtn,
	WIDGET_T_CheckBox,
};
struct WidgetAndType
{
	WidgetType _ty=WIDGET_T_None;
	GtkWidget * pWidget=0;         // 外层控件（多行编辑时是滚动容器）
	GtkWidget * pInnerWidget=0;    // 新增：实际输入控件（单行Entry/多行TextView，其他控件和pWidget一致）
};

// 资源解析器类
class GStyleBaseWinFromRes 
{
public:
	GStyleBaseWinFromRes();
	~GStyleBaseWinFromRes();
    // 解析 注意 字符串不要有英文逗号 用“&comma;” 代替
	/*
	DIALOG w,h,title
	EDITTEXT     id,l,t,w,h
	LTEXT        text,id, l,t,w,h
	PUSHBUTTON   text,id, l,t,w,h
	CHECKBOX     text,id, "unuseType",unuseStyle,l,t,w,h

    EDITTEXT        IDC_EDIT1,53,16,352,14,ES_AUTOHSCROLL
    LTEXT           "选择文件",IDC_STATIC,7,19,33,8
    EDITTEXT        IDC_EDIT2,53,35,352,14,ES_AUTOHSCROLL
    LTEXT           "选择目录",IDC_STATIC,7,39,33,8
    EDITTEXT        IDC_EDIT3,53,53,352,14,ES_AUTOHSCROLL
    LTEXT           "选择库目录",IDC_STATIC,7,57,41,8
    PUSHBUTTON      "...",IDC_B_SelLibDir,408,53,20,14
    PUSHBUTTON      "...",IDC_B_SelFile,408,16,20,14
    PUSHBUTTON      "...",IDC_B_SelDir,408,35,20,14
    PUSHBUTTON      "ClientID",IDC_B_CID,7,91,50,14
    EDITTEXT        IDC_EDIT4,53,70,352,14,ES_AUTOHSCROLL
    LTEXT           "ClientID",IDC_STATIC,7,74,33,8
    PUSHBUTTON      "PreInit",IDC_B_PreInit,58,91,50,14
    PUSHBUTTON      "Init",IDC_B_Init,109,91,50,14
    PUSHBUTTON      "查云",IDC_B_CloudScan,7,110,50,14
    PUSHBUTTON      "扫目录+LOG",IDC_B_ScanDirLog,60,110,56,14
    EDITTEXT        IDC_EDIT5,7,163,421,14,ES_AUTOHSCROLL
    LTEXT           "Result",IDC_STATIC,7,153,25,8
    PUSHBUTTON      "无本地引擎",IDC_B_NoLocalEng,7,129,58,14
    PUSHBUTTON      "UnInit",IDC_B_Uninit,160,91,50,14
    PUSHBUTTON      "开云log",IDC_B_OpenLog,68,129,43,14
    PUSHBUTTON      "关云log",IDC_B_OffLog,114,129,43,14
    PUSHBUTTON      "NoCC",IDC_B_NoCC,160,129,37,14
    PUSHBUTTON      "NewVD",IDC_B_NewVD,211,91,50,14
    CHECKBOX         "UsePrivateCloud&comma;hostport=",IDC_Chk_UsePrivCloud,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,127,112,116,10
    EDITTEXT        IDC_E_PRIVHOSTPORT,245,110,156,14,ES_AUTOHSCROLL
    PUSHBUTTON      "私有云设置",IDC_BSetPrivateCloud,202,129,50,14
	 * */


    bool         ParseResourceString(const std::string &strResDesc);

	// 创建GTK界面
	void         CreateGTKInterface();

	virtual void OnControlClick(GtkWidget *widget);
	virtual void OnDestroy();

	// 销毁整个界面
	void         DestroyGTKInterface();

	// 获取控件列表
	const std::vector<ControlDef>& GetControls() const;
	int          GetWidgetID(GtkWidget *widget);
	GtkWidget *  GetWidgetByID(int id);
	WidgetType   GetWidgetType(const string& strT);
  	WidgetType   GetWidgetTypeByID(int id);
    int          m_modalRet = 0;// 存储模态返回值：IDOK/IDCANCEL
    GMainLoop*   m_modalLoop = nullptr;// 模态嵌套事件循环

    // ========== 统一的输入框内容操作接口，自动适配单行/多行 ==========
    std::string  GetEditText(int ctrlId);
    void         SetEditText(int ctrlId, const std::string& text);
    // ========== 复选框状态操作接口 ==========
    bool         GetCheckBoxChecked(int ctrlId);
    void         SetCheckBoxChecked(int ctrlId, bool checked);


    float m_Scale=1.75f;

protected:
	std::vector<ControlDef> m_controls;
	int                     m_dialogWidth = 0;
	int                     m_dialogHeight = 0;
	std::string             m_dialogTitle;
	map<int, WidgetAndType> m_controlMap;
	GtkWidget *             m_hWnd_Widget=0;
	GtkWindow*              m_hWnd=0;

private:
    //解析MFC RC风格字符串
    int                     ParseEditStyle(const std::string& styleStr);
};

// 全局函数声明
GtkWidget* CreateControlFromDefinition(const ControlDef& control);
GtkWidget* CreateEditBox(const ControlDef& control);
GtkWidget* CreateButton(const ControlDef& control);
GtkWidget* CreateCheckBox(const ControlDef& control);
GtkWidget* CreateLabel(const ControlDef& control);


