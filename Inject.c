
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Inject.h"
#include "lib/Misc.h"
#include "lib/Constants.h"

VOID __stdcall test1()
{
	
}
VOID __stdcall test1_()
{
	
}

VOID DoInject(HWND hWnd)
{
	HANDLE hProcess = NULL;
	DWORD processId = 0;

	GetWindowThreadProcessId(hWnd, &processId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (!hProcess)
	{
		printf("Cannot open process: Error %d...\n", GetLastError());
		return FALSE;
	}

	if (!Inject_Initialize(hProcess))
	{
		printf("Initialize failed");
		return FALSE;
	}
	
	MemMgr_Ptr codePtr;
	
	if ((codePtr = Inject_CodeCreate(hProcess, test1, test1_-test1)) == 0)
	{
		printf("CodeCreate failed");
		return FALSE;
	}
	
	for (;;) {
		if (!Inject_CodeExecute(hProcess, codePtr))
		{
			printf("CodeExecute failed");
			return FALSE;
		}
		Sleep(50);
	}
	
	printf("Success.\n");

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