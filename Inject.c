
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Inject.h"
#include "lib/Misc.h"
#include "lib/Constants.h"

LPSTR func1Name = "__read";
CHAR func1[] = {
	0x51,				// PUSH ECX (save it for lua_pushnumber)
	0x33, 0xD2,			// XOR EDX, EDX
	0x42,				// INC EDX
	0xB8, 0x20, 0x36, 0x6F, 0x00,	// MOV EAX, 006F3620 (lua_tonumber)
	0xFF, 0xD0,			// CALL EAX
	0xB8, 0xB0, 0xA2, 0x40, 0x00,	// MOV EAX, 0040A2B0 (ftol)
	0xFF, 0xD0,			// CALL EAX
	0x59,				// POP ECX (restore ECX)
	0xDB, 0x00,			// FILD DWORD PTR DS:[EAX]
	0x83, 0xEC, 0x08,		// SUB ESP, 8
	0xDD, 0x1C, 0x24,		// FSTP QWORD PTR SS:[ESP]
	0xB8, 0x10, 0x38, 0x6F, 0x00,	// MOV EAX, lua_pushnumber (006F3810)
	0xFF, 0xD0,			// CALL EAX

	0x33, 0xC0,			// XOR EAX, EAX
	0x40,				// INC EAX
	0xC3,				// RETN
};

LPSTR func2Name = "__write";
CHAR func2[] = {
	0x56,				// PUSH ESI

	0x51,				// PUSH ECX (save it for second call)
	0x33, 0xD2,			// XOR EDX, EDX
	0x42,				// INC EDX
	0xB8, 0x20, 0x36, 0x6F, 0x00,	// MOV EAX, 006F3620 (lua_tonumber)
	0xFF, 0xD0,			// CALL EAX
	0xB8, 0xB0, 0xA2, 0x40, 0x00,	// MOV EAX, 0040A2B0 (ftol)
	0xFF, 0xD0,			// CALL EAX
	0x8B, 0xF0,			// MOV ESI, EAX (save ftol result)

	0x59,				// POP ECX (restore ECX)
	0x33, 0xD2,			// XOR EDX, EDX
	0x42,				// INC EDX
	0x42,				// INC EDX
	0xB8, 0x20, 0x36, 0x6F, 0x00,	// MOV EAX, 006F3620 (lua_tonumber)
	0xFF, 0xD0,			// CALL EAX
	0xB8, 0xB0, 0xA2, 0x40, 0x00,	// MOV EAX, 0040A2B0 (ftol)
	0xFF, 0xD0,			// CALL EAX

	0x89, 0x06,			// MOV DWORD PTR DS:[ESI],EAX

	0x33, 0xC0,			// XOR EAX, EAX
	0x5E,				// POP ESI
	0xC3,				// RETN
};

LPSTR func3Name = "__call";
CHAR func3[] = {
	0x33, 0xD2,			// XOR EDX, EDX
	0x42,				// INC EDX
	0xB8, 0x20, 0x36, 0x6F, 0x00,	// MOV EAX, 006F3620 (lua_tonumber)
	0xFF, 0xD0,			// CALL EAX
	0xB8, 0xB0, 0xA2, 0x40, 0x00,	// MOV EAX, 0040A2B0 (ftol)
	0xFF, 0xD0,			// CALL EAX

	0xFF, 0xD0,			// CALL EAX

	0x33, 0xC0,			// XOR EAX, EAX
	0xC3,				// RETN
};

VOID DoInject(HWND hWnd)
{
	HANDLE hProcess = NULL;
	DWORD processId = 0;
	struct InjectInfo *info;
	MemMgr_Ptr codePtr;

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

	if (!Inject_LuaFunction(info, func1Name, strlen(func1Name) + 1, func1, sizeof(func1), 1, 10))
		return FALSE;

	if (!Inject_LuaFunction(info, func2Name, strlen(func2Name) + 1, func2, sizeof(func2), 2, 10))
		return FALSE;

	if (!Inject_LuaFunction(info, func3Name, strlen(func3Name) + 1, func3, sizeof(func3), 1, 10))
		return FALSE;
	
	/* Game checks if lua function is in the WoW module. Disable the check */
	CHAR retn = 0xC3;
	MemMgr_Write(hProcess, 0x42A320, &retn, 1);
	
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