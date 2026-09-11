#pragma once
#include <vector>
using namespace std;


struct SaveLoad_CfgNode
{
	LPCWSTR lpININame;
	CString* spPtrs;
};
class TSaveLoadCFG
{
public:
	TSaveLoadCFG(LPCWSTR lpName);

	virtual SaveLoad_CfgNode* GetSaveLoadCfgArray(int& OUT nArrayLen, bool& OUT bNeedFree) = 0;
	void SaveOrLoadCfg(BOOL bSaveOrLoad);
	void  baseSaveOrLoadCfg(LPCWSTR lpFileName, BOOL bSaveOrLoad, SaveLoad_CfgNode* parray, int nArrayLen);

	CStringW __baseSaveOrLoadCfg_get_strInfo(CString*  spPtr);
	void __baseSaveOrLoadCfg_set_strInfo(CString*  spPtr, CString strInfo);
	SaveLoad_CfgNode* CopyCreateSaveLoadCfgArray(SaveLoad_CfgNode* pcfg, int len);


private:
	CString m_strCfgName;
};

CString Base64EncodeStr2Utf8(CStringW strIN);
CString Base64DecodeStr2Utf8(CStringW strIN);
