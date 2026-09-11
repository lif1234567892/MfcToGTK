#ifdef _WIN32
#include "stdafx.h"
#else//!_WIN32
#include "MfcToGTK/MFC.h"
#include "MfcToGTK/APPConfig.h"
#endif//_WIN32

#include "TSaveLoadCfg.h"
#include "EncodeAndDecode.h"
#include <assert.h>
#include <string.h>



CString Base64EncodeStr2Utf8(CStringW strIN)
{
	CStringA strINA = (const char*)CW2A(strIN, CP_UTF8);
	vector<char> buffer;
	buffer.resize(strINA.GetLength() * 2 + 4 + 10, 0);
	unsigned int dwEncode = buffer.size() - 10;
	if (Base64Encode((unsigned char*)(const char*)strINA, strINA.GetLength(), &buffer[0], dwEncode))
	{
		return (LPCWSTR)CA2W(&buffer[0]);
	}
	return L"";
}

CString Base64DecodeStr2Utf8(CStringW strIN)
{
	CStringA strBase64A = (const char*)CW2A(strIN);
	vector<unsigned char> vetTemp;
	vetTemp.resize(strBase64A.GetLength() + 1, 0);
	int r = Base64_Decode((char*)(const char*)strBase64A, (char*)&vetTemp[0], strBase64A.GetLength(), vetTemp.size());
	return (LPCWSTR)CA2W((char*)&vetTemp[0], CP_UTF8);
}


TSaveLoadCFG::TSaveLoadCFG(LPCWSTR lpName)
{
	m_strCfgName = lpName;
}

void TSaveLoadCFG::SaveOrLoadCfg(BOOL bSaveOrLoad)
{
	int nArrayLen = 0;
	bool bNeedFree = false;
	SaveLoad_CfgNode* parray = ( this)->GetSaveLoadCfgArray(nArrayLen, bNeedFree);
	if (parray && nArrayLen > 0)
	{
		baseSaveOrLoadCfg(m_strCfgName, bSaveOrLoad, parray, nArrayLen);
		if (!bSaveOrLoad)
		{
			//((TBase*)this)->UpdateData(FALSE);
			//__if_exists(TBase::SaveLoadCfg_OnCfgLoaded)
			//{
			//	((TBase*)this)->SaveLoadCfg_OnCfgLoaded();
			//}
		}
		if (bNeedFree)
			delete[] parray;
	}
}

void TSaveLoadCFG::baseSaveOrLoadCfg(LPCWSTR lpFileName, BOOL bSaveOrLoad, SaveLoad_CfgNode* parray, int nArrayLen)
{
	if (parray && nArrayLen > 0 && lpFileName)
	{
		for (int i = 0; i < nArrayLen; i++)
		{
			if (bSaveOrLoad)
			{
				CStringW strTxt = __baseSaveOrLoadCfg_get_strInfo(parray[i].spPtrs);
				if (!WriteProfileStringW(lpFileName, parray[i].lpININame, Base64EncodeStr2Utf8(strTxt)))
				{
					//Debug_View(L"WritePrivateProfileString: err= %u, %s", GetLastError(), parray[i].lpININame);
				}
			}
			else
			{
				vector<wchar_t> szParam;
				szParam.resize(65535, 0);
				GetProfileStringW(lpFileName, parray[i].lpININame, L"", &szParam[0], (unsigned int)szParam.size());
				__baseSaveOrLoadCfg_set_strInfo(parray[i].spPtrs, Base64DecodeStr2Utf8(&szParam[0]));
			}
		}
	}
}

CStringW TSaveLoadCFG::__baseSaveOrLoadCfg_get_strInfo(CString* spPtr)
{
	CStringW strRet;
	return *((CStringW*)(spPtr));
}

void TSaveLoadCFG::__baseSaveOrLoadCfg_set_strInfo(CString* spPtr, CString strInfo)
{
	*((CStringW*)(spPtr)) = strInfo;
}

SaveLoad_CfgNode* TSaveLoadCFG::CopyCreateSaveLoadCfgArray(SaveLoad_CfgNode* pcfg, int len)
{
	if (len > 0 && pcfg) {
		SaveLoad_CfgNode* ret = new(nothrow)SaveLoad_CfgNode[len];
		if (ret)
		{
			for (int i = 0; i < len; i++)
				ret[i] = pcfg[i];
		}
		return ret;
	}
	return 0;
}
