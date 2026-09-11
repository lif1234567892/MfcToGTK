#ifndef __DLFCN_WIN_H__
#define __DLFCN_WIN_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#define RTLD_LAZY 0

	void * dlopen(const char* pDll, int flag);
	void* dlsym(void* hdll, const char* fnName);
	void dlclose(void* dll);
	const char* dlerror();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !__DLFCN_WIN_H__
