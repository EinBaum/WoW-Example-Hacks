
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Misc.h"
#include "lib/Constants.h"

FLOAT fovVal = 0;

INT WindowIndex;

BOOL FixFoV(HWND hWnd)
{
	HANDLE hProcess = NULL;
	DWORD processId = 0;

	printf("%d:\t", WindowIndex);
	WindowIndex++;

	GetWindowThreadProcessId(hWnd, &processId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (!hProcess)
	{
		printf("Cannot open process: Error %d...\n", GetLastError());
		return FALSE;
	}

	if (!Misc_IsLoggedIn(hProcess))
	{
		printf("Not logged in...\n");
		return FALSE;
	}

	INT32 off_cam = 0x00B4B2BC;
	INT32 off_camFoV = 0x000065B8;
	INT32 off_fovVal = 0x00000040;

	INT32 camPtr = MemMgr_ReadPtr(hProcess, off_cam);
	INT32 fovPtr = MemMgr_ReadPtr(hProcess, camPtr + off_camFoV);
	MemMgr_Write(hProcess, fovPtr + off_fovVal, (LPCVOID)&fovVal, sizeof(fovVal));

	printf("FoV Fix Enabled.\n");

	CloseHandle(hProcess);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, long lParam) {
    CHAR buff[255];

    if (IsWindowVisible(hWnd)) {
    	GetWindowTextA(hWnd, (LPSTR) buff, 254);
		if (strcmp(buff, "World of Warcraft") == 0) {
			FixFoV(hWnd);
		}
    }
    return TRUE;
}

int main(int argc, char* argv[])
{
	if (!Misc_EnableDebugPrivilege(TRUE))
	{
		printf("Run this program as Administrator.\n");
		return 1;
	}

	printf("Enter FoV value between 0 and 3.14: ");

	if (scanf("%f", &fovVal) != 1)
	{
		printf("Input error.\n");
		return 1;
	}

	for (;;) {
		system("cls");
		WindowIndex = 1;

		EnumWindows(EnumWindowsProc, 0);
		if (WindowIndex == 1)
		{
			printf("Cannot find any WoW windows...\n");
		}
		Sleep(10000);
	}
	return 0;
}