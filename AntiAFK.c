
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Misc.h"
#include "lib/Constants.h"

INT WindowIndex;
BOOL Success;

BOOL DoAntiAFK(HWND hWnd)
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

	if (!Misc_UpdateAFK(hProcess))
	{
		printf("Write Error: %d\n", GetLastError());
		return FALSE;
	}
	
	Success = TRUE;
	printf("Anti-AFK Enabled.\n");

	CloseHandle(hProcess);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, long lParam) {
    CHAR buff[255];

    if (IsWindowVisible(hWnd)) {
    	GetWindowTextA(hWnd, (LPSTR) buff, 254);
		if (strcmp(buff, "World of Warcraft") == 0) {
			DoAntiAFK(hWnd);
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
	for (;;) {
		system("cls");
		WindowIndex = 1;
		Success = FALSE;
		
		EnumWindows(EnumWindowsProc, 0);
		if (!Success)
		{
			if (WindowIndex == 1)
			{
				printf("Cannot find any WoW windows...\n");
			}
			Sleep(1000);
		}
		else
		{
			Sleep(5000 + rand());
		}
		
	}
	return 0;
}