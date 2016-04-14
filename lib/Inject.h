
#include <windows.h>
#include "MemMgr.h"

struct InjectInfo;


struct InjectInfo*	Inject_Initialize(HANDLE hProcess);
void			Inject_Free(struct InjectInfo *info);

MemMgr_Ptr		Inject_CodeCreate(struct InjectInfo *info, LPVOID code, SIZE_T len);
BOOL			Inject_CodeExecute(struct InjectInfo *info, MemMgr_Ptr codePtr);
BOOL			Inject_CodeExecuteWait(struct InjectInfo *info, MemMgr_Ptr codePtr, DWORD msTick);
BOOL			Inject_IsCodeExecuting(struct InjectInfo *info);

MemMgr_Ptr		Inject_LuaCreate(struct InjectInfo *info, LPSTR lua, SIZE_T len);
BOOL			Inject_LuaExecute(struct InjectInfo *info, MemMgr_Ptr luaPtr);