#include "dlfcnwin.h"

#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>


extern "C"
{

	void * dlopen(const char* pDll, int flag) {
		return LoadLibraryA(pDll);
	}

	void* dlsym(void* hdll, const char* fnName) {
		return (void*)GetProcAddress((HMODULE)hdll, fnName);
	}

	void dlclose(void* dll) {
		FreeLibrary((HMODULE)dll);
	}

	char buferr[100];
	const char * dlerror()
	{
		sprintf(buferr, "err=%d", GetLastError());
		return buferr;
	}

}
#endif//_WIN32



