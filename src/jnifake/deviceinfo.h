#pragma once
#include <time.h>
#include <string>

using namespace std;

#define CLOCK_MONOTONIC 0

unsigned long GetTickCountMs();

string OH_GetOSFullName();
string OH_GetOsReleaseType();


void usleep(unsigned int us);

