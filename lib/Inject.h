
#include <windows.h>
#include "MemMgr.h"

MemMgr_Ptr	Inject_CodeCreate(HANDLE hProcess, LPVOID code, SIZE_T len);
BOOL		Inject_CodeExecute(HANDLE hProcess, MemMgr_Ptr codePtr);
BOOL		Inject_IsCodeExecuting(HANDLE hProcess);

MemMgr_Ptr	Inject_LuaCreate(HANDLE hProcess, LPSTR lua, SIZE_T len);
BOOL		Inject_LuaExecute(HANDLE hProcess, MemMgr_Ptr luaPtr);