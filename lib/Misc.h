
#include <windows.h>
#include "MemMgr.h"

typedef VOID (*Misc_FindWoW_Callback)(HANDLE hWnd);

BOOL Misc_EnableDebugPrivilege(BOOL bEnable);
BOOL Misc_FindWoW(Misc_FindWoW_Callback callback);

BOOL Misc_UpdateAFK(HANDLE hProcess);