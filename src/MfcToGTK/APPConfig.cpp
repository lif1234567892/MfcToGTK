#include "APPConfig.h"

#include <mutex>
#include <string>
#include "MFC.h"
#include <vector>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

static std::mutex g_iniMutex;

static string get_defaultIniPath()
{
	// 1. 读取当前执行模块的完整路径，/proc/self/exe是Linux内核提供的当前进程符号链接
	char path[1024];
	ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (len == -1)
	{
		strcpy(path, "appconfig.ini");
		return path;
	}
	path[len] = '\0';

	// 2. 提取模块所在目录
	char *dir = dirname(path);
	// 重新分配空间保存最终路径
	char *fullPath = (char*) malloc(strlen(dir) + strlen("/appconfig.ini") + 1);
	strcpy(fullPath, dir);
	strcat(fullPath, "/appconfig.ini");

	return fullPath;
}


/********************************************************************
 * 对齐Windows WritePrivateProfileStringW：第一个参数为节名
 * 特性：
 * 1. lpKeyName=NULL => 删除整个lpSectionName节
 * 2. lpString=NULL => 删除lpSectionName下的lpKeyName键
 * 3. 自动保存到默认INI路径（程序目录/app_config.ini）
 ********************************************************************/
bool WriteProfileStringW(const wchar_t* lpSectionName, const wchar_t* lpKeyName, const wchar_t* lpString)
{
	bool keyExist = false;
	if (!lpSectionName)
		return false;
	std::lock_guard < std::mutex > lock(g_iniMutex);

	// 宽字符转UTF8
	std::string utf8Section = (const char*)CW2A(lpSectionName, CP_UTF8);
	std::string utf8Key = lpKeyName ? CW2A(lpKeyName, CP_UTF8) : "";
	std::string utf8Value = lpString ? CW2A(lpString, CP_UTF8) : "";
	std::string utf8IniPath = get_defaultIniPath().c_str();

	// 读取所有行
	std::vector < std::string > allLines;
	std::ifstream inFile(utf8IniPath);
	if (inFile.is_open())
	{
		std::string line;
		while (std::getline(inFile, line))
		{
			allLines.push_back(line);
		}
		inFile.close();
	}

	// 定位目标节位置
	int sectionStart = -1, sectionEnd = allLines.size();
	for (int i = 0; i < allLines.size(); i++)
	{
		const std::string &line = allLines[i];
		if (line.empty() || line[0] != '[')
			continue;
		size_t endBracket = line.find(']');
		if (endBracket == std::string::npos)
			continue;
		// 提取节名，去掉前后空格
		std::string curSection = line.substr(1, endBracket - 1);
		curSection.erase(0, curSection.find_first_not_of(" \t"));
		curSection.erase(curSection.find_last_not_of(" \t") + 1);

		if (curSection == utf8Section)
		{
			sectionStart = i;
		}
		else if (sectionStart != -1)
		{
			sectionEnd = i; // 找到下一个节，当前节结束
			break;
		}
	}

	// 情况1：删除整个节
	if (!lpKeyName)
	{
		if (sectionStart != -1)
		{
			allLines.erase(allLines.begin() + sectionStart, allLines.begin() + sectionEnd);
		}
		goto WRITE_BACK;
	}

	// 情况2：节不存在，新增节
	if (sectionStart == -1)
	{
		allLines.push_back("");
		allLines.push_back("[" + utf8Section + "]");
		sectionStart = allLines.size() - 1;
		sectionEnd = allLines.size();
	}

	// 情况3：节内找key替换/删除/新增
	keyExist = false;
	for (int i = sectionStart + 1; i < sectionEnd; i++)
	{
		const std::string &line = allLines[i];
		if (line.empty() || line[0] == '#' || line[0] == ';')
			continue; // 跳过注释/空行
		size_t eqPos = line.find('=');
		if (eqPos == std::string::npos)
			continue;
		std::string curKey = line.substr(0, eqPos);
		curKey.erase(0, curKey.find_first_not_of(" \t"));
		curKey.erase(curKey.find_last_not_of(" \t") + 1);
		if (curKey == utf8Key)
		{
			keyExist = true;
			if (lpString)
			{
				allLines[i] = utf8Key + "=" + utf8Value;
			}
			else
			{
				allLines.erase(allLines.begin() + i); // 删除key
			}
			break;
		}
	}
	// 新增key到节末尾
	if (!keyExist && lpString)
	{
		allLines.insert(allLines.begin() + sectionEnd, utf8Key + "=" + utf8Value);
	}

	WRITE_BACK:
	// 写回文件
	std::ofstream outFile(utf8IniPath, std::ios::trunc);
	if (!outFile.is_open())
		return FALSE;
	for (const auto &line : allLines)
	{
		outFile << line << "\n";
	}
	outFile.flush();
	outFile.close();
	return TRUE;
}

/********************************************************************
 * 对齐Windows GetPrivateProfileStringW：第一个参数为节名
 * 返回：实际读到的宽字符数，找不到返回默认值
 ********************************************************************/
unsigned int GetProfileStringW(const wchar_t *lpSectionName, const wchar_t *lpKeyName, const wchar_t *lpDefault, wchar_t* lpReturnedString, unsigned nSize)
{
	if (!lpSectionName || !lpKeyName || !lpReturnedString || nSize == 0)
		return 0;
	std::lock_guard < std::mutex > lock(g_iniMutex);

	// 初始化默认值
	const wchar_t *defVal = lpDefault ? lpDefault : L"";
	wcsncpy(lpReturnedString, defVal, nSize - 1);
	lpReturnedString[nSize - 1] = L'\0';
	unsigned int retLen = wcslen(lpReturnedString);

	// 转UTF8
	std::string utf8Section = (const char*)CW2A(lpSectionName, CP_UTF8);
	std::string utf8Key = (const char*)CW2A(lpKeyName, CP_UTF8);
	std::string utf8IniPath = get_defaultIniPath().c_str();

	std::ifstream inFile(utf8IniPath);
	if (!inFile.is_open())
		return retLen;

	bool inTargetSection = false;
	std::string line;
	while (std::getline(inFile, line))
	{
		if (line.empty())
			continue;
		// 匹配节头
		if (line[0] == '[')
		{
			size_t endBracket = line.find(']');
			if (endBracket == std::string::npos)
				continue;
			std::string curSection = line.substr(1, endBracket - 1);
			curSection.erase(0, curSection.find_first_not_of(" \t"));
			curSection.erase(curSection.find_last_not_of(" \t") + 1);
			inTargetSection = (curSection == utf8Section);
			continue;
		}
		// 不在目标节直接跳过
		if (!inTargetSection)
			continue;
		// 跳过注释
		if (line[0] == '#' || line[0] == ';')
			continue;
		size_t eqPos = line.find('=');
		if (eqPos == std::string::npos)
			continue;
		std::string curKey = line.substr(0, eqPos);
		curKey.erase(0, curKey.find_first_not_of(" \t"));
		curKey.erase(curKey.find_last_not_of(" \t") + 1);
		if (curKey == utf8Key)
		{
			std::string utf8Value = line.substr(eqPos + 1);
			CA2W wValue(utf8Value.c_str(), CP_UTF8);
			const wchar_t *wStr = (const wchar_t*) wValue;
			retLen = wcslen(wStr);
			wcsncpy(lpReturnedString, wStr, nSize - 1);
			lpReturnedString[nSize - 1] = L'\0';
			if (retLen >= nSize)
				retLen = nSize - 1;
			break;
		}
	}
	inFile.close();
	return retLen;
}

