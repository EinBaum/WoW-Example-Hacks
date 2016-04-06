
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/ObjMgr.h"
#include "lib/Constants.h"

HWND		hWnd		= NULL;
HANDLE		hProcess	= NULL;
DWORD		processId	= 0;

void DoTeleport(void)
{
	char in_str[100];
	FLOAT in;

	while (1)
	{
		system("cls");

		hWnd = FindWindowA(0, "World of Warcraft");

		if (hWnd == NULL) {
			printf("Cannot find WoW window...");

			Sleep(1000);
			continue;
		}

		GetWindowThreadProcessId(hWnd, &processId);
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

		if (!hProcess) {
			printf("Cannot open process: Error %d...", GetLastError());

			Sleep(1000);
			continue;
		}

		if (!Misc_IsLoggedIn(hProcess)) {
			printf("Not logged in...");

			Sleep(1000);
			continue;
		}

		struct ObjMgr_Obj* obj_last = ObjMgr_GetObjects(hProcess);
		ObjMgr_DelObjects(obj_last);

		FLOAT p_x = MemMgr_ReadFloat(hProcess, ObjMgr_TmpPlayerBase + O_wow_obj_pos_x);
		FLOAT p_y = MemMgr_ReadFloat(hProcess, ObjMgr_TmpPlayerBase + O_wow_obj_pos_y);
		FLOAT p_z = MemMgr_ReadFloat(hProcess, ObjMgr_TmpPlayerBase + O_wow_obj_pos_z);

		printf("Your coords: X %f Y %f Z %f\n", p_x, p_y, p_z);
		printf("Enter Z: ");

		if (fgets(in_str, sizeof(in_str), stdin) == NULL)
		{
			return;
		}

		if (sscanf(in_str, "%f", &in) != 1)
		{
			continue;
		}

		if (MemMgr_Write(hProcess, ObjMgr_TmpPlayerBase + O_wow_obj_pos_z, &in, sizeof(in)) != 0)
		{
			printf("Write Error: %d\n", GetLastError());
			continue;
		}

		CloseHandle(hProcess);
	}
}

int main(int argc, char* argv[])
{
	if (!Misc_EnableDebugPrivilege(TRUE))
	{
		printf("Run this program as Administrator.");
		return 1;
	}

	DoTeleport();
	return 0;
}