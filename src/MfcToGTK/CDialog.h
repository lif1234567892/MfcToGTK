#pragma once
#include "CDataExchange.h"
#include "wintypes.h"
#include "GStyleBaseWinFromRes.h"
#include "MFC.h"

class CDialog :public GStyleBaseWinFromRes
{
public:
	 CDialog();
	 virtual ~CDialog();

public:

	virtual void  DoDataExchange(CDataExchange* pDX);
	virtual BOOL  OnInitDialog();
	void          UpdateData(BOOL bToVar);

	void          DDX_Text(CDataExchange* pDX, int ctrlID, CStringW& strVar);
	void          DDX_Check(CDataExchange* pDX, int ctrlID, BOOL& boolVar);

	int           DoModal();
	void          SetRsourceString(const char* pRes);

	string m_strResString;

};
