#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "gitl.h"
#include "global.h"

BOOL HandleHook(BOOL Enable) {

	INT Value;
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	hDevice = CreateFileW(DeviceName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (Enable)
	{
		Value = ControlEtwHook(hDevice, METHOD_ENABLE_HOOK);
		printf_s("[+] Enabled Hook (events will be dropped)");
	}
	else {
		Value = ControlEtwHook(hDevice, METHOD_DISABLE_HOOK);
		printf_s("[+] Disabled Hook (events will be reported)");
	}

	if (Value == 1 || Value == 0) {
		return TRUE;
	}



	return FALSE;
}

BOOL HandleArg(CHAR* Arg) {
	INT Value;

	if (strncmp(Arg, "enable", 6) == 0) {
		Value = HandleHook(TRUE);
	}
	else if (strncmp(Arg, "disable", 7) == 0) {
		Value = HandleHook(FALSE);
	}
	else if (strncmp(Arg, "status", 6) == 0) {
		printf("[+] %s\n", HookStatus());
	}
	else if (strncmp(Arg, "load", 4) == 0) {
		printf("[i] Attempting to load kernel hook\n");
		Value = LoadDriver();
		if (strcmp(HookStatus(), "ERROR: Drive returned unknown value") == 0) {
			printf("[-] Driver failed to load\n");
		}
	}
	else if (strncmp(Arg, "clean", 5) == 0) {
		Value = CleanUp();
	}

	return Value;
}

VOID Usage() {
	printf_s("Usage: gitl.exe <arg>\n\n");
	printf_s("Arguments:\n\n");
	printf_s("enable\t-\tEnable the kernel hook, this will disable etw\n");
	printf_s("disable\t-\tDisable the kernel hook, this will enable etw\n");
	printf_s("load\t-\tUse KDU to load the kernel driver\n");
	printf_s("clean\t-\tClean up any left over files\n");
	return;
}

int main(ULONG argc, PCHAR argv[])
{
	printf_s(banner);

	if (argc != 2) {
		Usage();
		exit(1);
	}

	HandleArg(argv[1]);

	return 1;
}