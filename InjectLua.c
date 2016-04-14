
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Inject.h"
#include "lib/Misc.h"
#include "lib/Constants.h"

LPSTR luaCode = "DEFAULT_CHAT_FRAME:AddMessage(\"Hello world!\")";

VOID DoInject(HWND hWnd)
{
	HANDLE hProcess = NULL;
	DWORD processId = 0;
	struct InjectInfo *info;
	MemMgr_Ptr stringPtr;

	GetWindowThreadProcessId(hWnd, &processId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (!hProcess)
	{
		printf("Cannot open process: Error %d...\n", GetLastError());
		return FALSE;
	}

	if ((info = Inject_Initialize(hProcess)) == NULL)
	{
		printf("Initialize failed");
		return FALSE;
	}

	if ((stringPtr = Inject_LuaCreate(info, luaCode, strlen(luaCode))) == 0)
	{
		printf("LuaCreate failed");
		return FALSE;
	}

	for (;;) {
		if (!Inject_LuaExecute(info, stringPtr))
		{
			printf("LuaExecute failed");
			return FALSE;
		}
		Sleep(500);
	}

	printf("Success.\n");

	Inject_Free(info);
	CloseHandle(hProcess);
}

int main(int argc, char* argv[])
{
	if (!Misc_EnableDebugPrivilege(TRUE))
	{
		printf("Run this program as Administrator.\n");
		return 1;
	}
	Misc_FindWoW((Misc_FindWoW_Callback)DoInject);
	return 0;
}