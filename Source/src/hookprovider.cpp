#include "gitl.h"
#include "global.h"

BOOL ControlEtwHook(HANDLE hDevice, DWORD Method) {

	PVOID Value;
	BOOL Result;
	DWORD BytesReturned = 0;

	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("[!] Error: Unable to communicate with the driver, have you loaded it?\n");
		return -1;
	}

	Result = DeviceIoControl(hDevice, Method, NULL, 0, Value, sizeof(Value), &BytesReturned, NULL);

	CloseHandle(hDevice);

	return (INT)Value;
}

LPCSTR HookStatus() {
	DWORD Value;
	BOOL Result;
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	hDevice = CreateFileW(DeviceName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("[!] Error: Unable to communicate with the driver, have you loaded it?\n");
		return "ERROR: Drive returned unknown value";
	}

	Result = DeviceIoControl(hDevice, METHOD_STATUS_HOOK, NULL, 0, NULL, 0, &Value, NULL);

	if (Value == 1) {
		return "Enabled (events not being logged)";
	}
	else if (Value == 0) {
		return "Disabled (all events are being logged)";
	}

	return "ERROR: Drive returned unknown value";
}

INT LoadDriver() {
	int ret;

	__try {
		ret = KDUMain();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf_s("[!] Unhandled exception 0x%lx\r\n", GetExceptionCode());
	}
	return ret;
}

INT UnloadDriver() {
	return 1;
}