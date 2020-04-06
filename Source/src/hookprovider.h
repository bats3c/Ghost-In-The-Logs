#pragma once

#include "global.h"

BOOL ControlEtwHook(HANDLE hDevice, DWORD Method);
LPCSTR HookStatus();
INT LoadDriver();
INT CleanUp();