#include <stdio.h>
#include "Resource.h"
#include <string.h>
#include "MFC_TestTool.h"
#include <jni.h>

#define MAKE_ID(XXX) if(strcmp(pIDSTR,#XXX)==0)return XXX;
int ConvertControlID(const char *pIDSTR)
{
	MAKE_ID(IDC_EDIT1);
	MAKE_ID(IDC_EDIT2);
	MAKE_ID(IDC_EDIT3);
	MAKE_ID(IDC_B_SelLibDir);
	MAKE_ID(IDC_B_SelFile);
	MAKE_ID(IDC_B_SelDir);
	MAKE_ID(IDC_B_CID);

	return 0;
}
#undef MAKE_ID


int main(int argc, char* argv[]) {
	printf("hello\n");
    gtk_init(&argc, &argv);
	CMFC_TestTool dlg;
	dlg.SetRsourceString(
			"DIALOG           435, 295, \"TestTool\" \r\n"
			"EDITTEXT        IDC_EDIT1,53,16,352,14,ES_AUTOHSCROLL\r\n"
			"LTEXT           \"选择文件\",IDC_STATIC,7,19,33,8\r\n"
			"EDITTEXT        IDC_EDIT2,53,35,352,14,ES_AUTOHSCROLL\r\n"
			"LTEXT           \"选择目录\",IDC_STATIC,7,39,33,8\r\n"
			"EDITTEXT        IDC_EDIT3,53,53,352,14,ES_AUTOHSCROLL\r\n"
			"LTEXT           \"选择库目录\",IDC_STATIC,7,57,41,8\r\n"
			"PUSHBUTTON      \"...\",IDC_B_SelLibDir,408,53,20,14\r\n"
			"PUSHBUTTON      \"...\",IDC_B_SelFile,408,16,20,14\r\n"
			"PUSHBUTTON      \"...\",IDC_B_SelDir,408,35,20,14\r\n"
			"PUSHBUTTON      \"ClientID\",IDC_B_CID,7,91,50,14\r\n"
	);
	dlg.DoModal();


	return 0;
}

std::string jstringToString(JNIEnv *env, jstring jstr) {
    if (jstr == nullptr) {
        return ""; // 处理空字符串
    }
    // 获取UTF-8编码的字符串指针
    const char *cstr = env->GetStringUTFChars(jstr, nullptr);
    if (cstr == nullptr) {
        return ""; // 内存分配失败
    }
    // 转换为std::string
    std::string str(cstr);
    // 释放字符串指针
    env->ReleaseStringUTFChars(jstr, cstr);
    return str;
}

extern "C" JNIEXPORT jstring
JNICALL Java_com_example_testtool_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz,
                                                           jstring str_test_cmd, jstring args) {
    std::string cmd = jstringToString(env, str_test_cmd);
    std::string cmdArgs = jstringToString(env, args);

    std::string
            hello = "Call"+cmd;
    printf("invoke %s\n", hello.c_str());

    return env->NewStringUTF(hello.c_str());
}





