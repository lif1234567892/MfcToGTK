

#pragma once

#ifndef _WIN32
#include "MfcToGTK/MFC.h"
#include "MfcToGTK/CDialog.h"
#endif//_WIN32

#include "TSaveLoadCfg.h"
#include "Resource.h"


class CMFC_TestTool : public CDialog , public TSaveLoadCFG

{
// 构造
public:
	CMFC_TestTool(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	SaveLoad_CfgNode* GetSaveLoadCfgArray(int& OUT nArrayLen, bool& OUT bNeedFree);

// 实现
protected:

#ifdef _WIN32
	HICON m_hIcon;
#endif//_WIN32

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

#ifdef _WIN32
	afx_msg HCURSOR OnQueryDragIcon();
#endif//_WIN32

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBSelfile();
	afx_msg void OnBnClickedBSeldir();
	afx_msg void OnBnClickedBSellibdir();
	CString m_strSelFile;
	CString m_strSelDir;
	CString m_strSelLibDir;
	CString m_strClientID;



	void CallMethodSetResult(CString md, CString mdArg);
	CString m_strResult;
	CString GetCommonArgs_LTCD();
	CString GetCommonArgs_PrivCloud();
	CString GetCommonArgs_File_LTCD();
	CString GetCommonArgs_Dir_LTCD();
	CString GetCommonArgs_File();


};
