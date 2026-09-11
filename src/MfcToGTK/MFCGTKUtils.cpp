#include "MFCGTKUtils.h"
#include <string.h>

void MFCUtils::StringSpilt(const string &str, vector<string> &arr, const string &cut)
{
	StringSpiltEx(str, arr, cut, true, false);
}

//判断非空白字符
#define IsNonSpaceChar(ch) ((ch) != ' ' && (ch) != '\t' && (ch) != '\r' && (ch) != '\n')
void MFCUtils::TrimLeftStr(string &str, char chtr)
{
	size_t len = str.length();
	for (size_t i = 0; i < len; i++) //find first non-space
	{
		char ch = str[i];
		if ((0 == chtr && IsNonSpaceChar(ch)) || (chtr != 0 && ch != chtr)) //left i shift
		{
			size_t nShiftLeft = i;
			if (nShiftLeft > 0)
			{
				size_t nRemain = len - i;
				for (size_t j = 0; j < nRemain; j++)
				{ //shift left all char
					str[j] = str[nShiftLeft + j];
				}
				str.erase(len - nShiftLeft, nShiftLeft);
			}
			return;
		}
	}
	//not find non-space means all is space
	str = "";
}

void MFCUtils::TrimRightStr(string &str, char chtr)
{
	int len = (int) str.length();
	for (int i = (len - 1); i >= 0; i--) //find last non-space
	{
		char ch = str[i];
		if ((0 == chtr && IsNonSpaceChar(ch)) || (chtr != 0 && ch != chtr)) //left i shift
		{
			int nChopTail = (len - 1) - i;
			if (nChopTail > 0)
			{
				str.erase(len - nChopTail, nChopTail);
			}
			return;
		}
	}
	//not find non-space means all is space
	str = "";
}

void MFCUtils::TrimStr(string &str, char chtr)
{
	TrimLeftStr(str,chtr);
	TrimRightStr(str,chtr);
}

void MFCUtils::StringSpiltEx(const string &str, vector<string> &arr, const string &cut, bool bAllowEmpty, bool bTrim)
{
	// str: 需要切割的字符串
	// arr: 切割后的字符串数组存放位置
	// cut：需要切割的符号
	string::size_type pos1, pos2;
	pos2 = str.find(cut);
	pos1 = 0;
	while (string::npos != pos2)
	{
		string sToPush = str.substr(pos1, pos2 - pos1);
		if (bTrim)
			TrimStr(sToPush);
		if (bAllowEmpty || sToPush.length())
		{
			arr.push_back(sToPush);
		}
		pos1 = pos2 + cut.size();
		pos2 = str.find(cut, pos1);
	}
	if (pos1 != str.length())
	{
		string sToPush = str.substr(pos1);
		if (bTrim)
			TrimStr(sToPush);
		if (bAllowEmpty || sToPush.length())
		{
			arr.push_back(sToPush);
		}
	}
}

string MFCUtils::StringFirstSplit(string in, const char *fp, string &tail)
{
	size_t f = in.find(fp);
	if (f != string::npos)
	{
		tail = in.substr(f + strlen(fp));
		TrimStr(tail);
		string h = in.substr(0, f);
		TrimStr(h);
		return h;
	}
	return "";
}

