#pragma once
#include <stddef.h>

struct jstring
{
	const char* pstr = 0;

	//funcs
	jstring(const char* x);
	jstring(const jstring& dest);
	~jstring();

	bool operator==(nullptr_t nt) { return pstr == 0; }
};

struct jobject
{
	void* pThis = 0;
};

struct JNIEnv
{
	const char* GetStringUTFChars(jstring jstr, void* xx);
	void ReleaseStringUTFChars(jstring jstr, const char* cstr);
	jstring NewStringUTF(const char* p);
	void    DestroyJString(jstring jstr);
};

#ifdef _WIN32
#define JNIEXPORT __declspec(dllexport)
#else//!_WIN32
#define JNIEXPORT  __attribute__((visibility("default")))
#endif//_WIN32
#define JNICALL





