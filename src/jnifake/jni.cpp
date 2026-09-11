#include "jni.h"
#include <string.h>
#include <malloc.h>

const char * JNIEnv::GetStringUTFChars(jstring jstr, void * xx)
{
	return strdup(jstr.pstr);
}

void JNIEnv::ReleaseStringUTFChars(jstring jstr, const char * cstr)
{
	free((void*)cstr);
}

jstring JNIEnv::NewStringUTF(const char * p)
{
	return jstring(p);
}

void JNIEnv::DestroyJString(jstring jstr)
{
	free((void*)jstr.pstr);
	jstr.pstr = 0;
}

jstring::jstring(const char * x)
{
	this->pstr = strdup(x);
}

jstring::jstring(const jstring & dest)
{
	this->pstr = strdup(dest.pstr);
}

jstring::~jstring() {
	if (pstr) {
		free((void*)pstr);
		pstr = 0;
	} 
}
