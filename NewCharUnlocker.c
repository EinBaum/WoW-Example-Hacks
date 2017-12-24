#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "lib/MemMgr.h"
#include "lib/Misc.h"

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

	/* Normally there is a function call that returns the maximum allowed
	 * skin ID. Return value is placed into EAX. Replace function call with a
	 * static value. */

	/* Replaces:	00470FA9   E8 92940000      CALL WoW.0047A440	*/
	/* With:		00470FA9   B8 1F000000      MOV EAX,1F			*/

	UINT8 mov = 0xB8;			// MOV EAX
	UINT32 val = 0x0000001F;	// 0x0000001F
	MemMgr_Write(hProcess, 0x470FA9, &mov, 1);
	MemMgr_Write(hProcess, 0x470FAA, &val, 4);

	/* Secret name unlocker */

	/* Old: 006C9A51     B8 06000000    MOV EAX,6	*/
	/* New: 006C9A51     B8 0D000000    MOV EAX,0D	*/

	UINT32 nameunlocker = 0x0000000D;
	MemMgr_Write(hProcess, 0x6C9A52, &nameunlocker, 4);


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
