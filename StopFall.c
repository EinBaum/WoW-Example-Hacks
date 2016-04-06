
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/ObjMgr.h"
#include "lib/Constants.h"

HWND		hWnd		= NULL;
HANDLE		hProcess	= NULL;
DWORD		processId	= 0;

INT32 val = 0;

BOOL DoStopFall(void)
{
	hWnd = FindWindowA(0, "World of Warcraft");

	if (hWnd == NULL) {
		printf("Cannot find WoW window...\n");
		return FALSE;
	}

	GetWindowThreadProcessId(hWnd, &processId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (!hProcess) {
		printf("Cannot open process: Error %d...\n", GetLastError());
		return FALSE;
	}

	if (!Misc_IsLoggedIn(hProcess)) {
		printf("Not logged in...\n");
		return FALSE;
	}

	struct ObjMgr_Obj* obj_last = ObjMgr_GetObjects(hProcess);
	ObjMgr_DelObjects(obj_last);

	if (!MemMgr_Write(hProcess, ObjMgr_TmpPlayerBase + O_wow_obj_movementflags, &val, sizeof(val)))
	{
		printf("Write Error: %d\n", GetLastError());
		return FALSE;
	}
	
	printf("Stopped!\n");
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (!Misc_EnableDebugPrivilege(TRUE))
	{
		printf("Run this program as Administrator.\n");
		return 1;
	}
	
	char hotkey;
	
	if (argc == 2)
	{
		hotkey = argv[1][0];
	}
	else
	{
		printf("YOU CAN RUN WITH PROGRAM WITH A PRE-ASSIGNED HOTKEY:\n");
		printf("Example: %s y\n", argv[0]);
		printf("will assign 'Y' as a hotkey.\n\n");
		printf("Enter the hotkey that you want to assign: ");
		
		if (scanf("%c", &hotkey) != 1)
		{
			printf("Input error.\n");
			return 1;
		}
	}
	
	UINT virtual = VkKeyScan(hotkey);
	
	if (!RegisterHotKey(NULL, 1, 0, virtual))
	{
		printf("Failed to register hotkey.\n");
		return 1;
	}

	printf("Hotkey '%c' registered.\n", hotkey);

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			DoStopFall();         
		}
	} 

	return 0;
}