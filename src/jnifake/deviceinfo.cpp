#include "deviceinfo.h"

#ifdef _WIN32
#include <Windows.h>
#endif//_WIN32

#ifdef _WIN32
unsigned long GetTickCountMs()
{
	return (unsigned long)GetTickCount();
}
#else/_WIN32
unsigned longGetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#endif//_WIN32

string OH_GetOSFullName()
{
#ifdef _WIN32
	return "Windows";
#else//!_WIN32
	return "Linux";
#endif//_WIN32
}

string OH_GetOsReleaseType()
{
#ifdef _WIN32
	return "Workstation";
#else//!_WIN32
	return "Ubuntu";
#endif//_WIN32
}

#ifdef _WIN32
void usleep(unsigned int us)
{
	int ms = us / 1000;
	if (ms == 0)
		ms = 1;
	Sleep(ms);
}
#endif//_WIN32


