
#include "Misc.h"
#include "MemMgr.h"
#include "Constants.h"

BOOL Misc_EnableDebugPrivilege(BOOL bEnable)
{
    HANDLE hToken = NULL;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) return FALSE;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) return FALSE;

    TOKEN_PRIVILEGES tokenPriv;
    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Luid = luid;
    tokenPriv.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) return FALSE;

    return TRUE;
}

BOOL CALLBACK Misc_FindWoW_Proc(HWND hWnd, LONG lParam) {
    CHAR buff[255];

    if (IsWindowVisible(hWnd)) {
    	GetWindowTextA(hWnd, (LPSTR) buff, 254);
		if (strcmp(buff, "World of Warcraft") == 0) {
			((Misc_FindWoW_Callback)lParam)(hWnd);
		}
    }
    return TRUE;
}
BOOL Misc_FindWoW(Misc_FindWoW_Callback callback)
{
	EnumWindows(Misc_FindWoW_Proc, (LONG)callback);
}

BOOL Misc_UpdateAFK(HANDLE hProcess)
{
	int tick = GetTickCount();
	return MemMgr_Write(hProcess, O_last_hardware_action, &tick, sizeof(tick));
}
BOOL Misc_IsLoggedIn(HANDLE hProcess)
{
	return MemMgr_Read32(hProcess, O_is_ingame) != 0;
}