#pragma once
#include <vector>
#include <string>

using namespace std;

namespace MFCUtils
{

void StringSpilt(const string &str, vector<string> &arr, const string &cut);
void TrimLeftStr(string &str, char chtr = 0);
void TrimRightStr(string &str, char chtr = 0);
void TrimStr(string &str, char chtr = 0);
void StringSpiltEx(const string &str, vector<string> &arr, const string &cut, bool bAllowEmpty, bool bTrim);
string StringFirstSplit(string in, const char *fp, string &tail);

}
;
