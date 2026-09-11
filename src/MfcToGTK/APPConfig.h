#pragma once



bool WriteProfileStringW(const wchar_t* lpSectionName, const wchar_t* lpKeyName, const wchar_t* lpString);

unsigned int GetProfileStringW(const wchar_t* lpSectionName, const wchar_t* lpKeyName, const wchar_t* lpDefault,  wchar_t* lpReturnedString, unsigned int nSize);

