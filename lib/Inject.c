
#include "Inject.h"
#include "Constants.h"

#define Inject_detour_ptr		(0x006F9D81)
#define Inject_detour_return		(0x006F9D87)

struct InjectInfo {
	HANDLE hProcess;

	void *oldDetourCode;

	MemMgr_Ptr ptrNewcode;
	MemMgr_Ptr ptrCondition;
	MemMgr_Ptr ptrCode;

	MemMgr_Ptr func_DoString;
	MemMgr_Ptr func_RegisterFunction;
};

static CHAR Inject_detour_code[] = {
	0x68, 0, 0, 0, 0,		// PUSH xxxx
	0xC3,				// RETN
	0xC2, 0x04, 0x00		// RETN 4
};
static SIZE_T Inject_detour_size = sizeof(Inject_detour_code);
static SIZE_T Inject_detour_offset_ret = 1;

static CHAR Inject_newcode_code[] = {
	0x50,				// PUSH EAX
	0xBA, 0, 0, 0, 0,		// MOV EDX, xxxx
	0x8B, 0x02,			// MOV EAX, [EDX]
	0x33, 0xC9,			// XOR ECX, ECX
	0x3B, 0xC1,			// CMP EAX, ECX
	0x74, 0x09,			// JE SHORT (to POP EAX)
	0x89, 0x0A,			// MOV [EDX], ECX
	0xA1, 0, 0, 0, 0,		// MOV EAX, [xxxx]
	0xFF, 0xD0,			// CALL EAX
	0x58,				// POP EAX
	0x68, 0, 0, 0, 0,		// PUSH xxxx
	0xC3,				// RETN
};
static SIZE_T Inject_newcode_size = sizeof(Inject_newcode_code);
static SIZE_T Inject_newcode_offset_condition	= 2;
static SIZE_T Inject_newcode_offset_code	= 17;
static SIZE_T Inject_newcode_offset_ret		= 25;

static CHAR Inject_lua_code[] = {
	0xB9, 0, 0, 0, 0,		// MOV ECX, xxxx (lua string)
	0xB8, 0xD0, 0x4C, 0x70, 0x00,	// MOV EAX, DoString (00704CD0)
	0xFF, 0xD0,			// CALL EAX
	0xC3,				// RETN
};
static SIZE_T Inject_lua_size = sizeof(Inject_lua_code);
static SIZE_T Inject_lua_offset_string = 1;

static CHAR Inject_func_code[] = {
	0xB9, 0, 0, 0, 0,		// MOV ECX, xxxx (function name string)
	0xBA, 0, 0, 0, 0,		// MOV EDX, xxxx (function pointer)
	0xB8, 0x20, 0x41, 0x70, 0x00,	// MOV EAX, RegisterFunction (00704120)
	0xFF, 0xD0,			// CALL EAX
	0xC3,				// RETN
};
static SIZE_T Inject_func_size = sizeof(Inject_func_code);
static SIZE_T Inject_func_offset_name = 1;
static SIZE_T Inject_func_offset_func = 6;

struct InjectInfo* Inject_Initialize(HANDLE hProcess)
{
	struct InjectInfo *info = (struct InjectInfo*)calloc(1, sizeof(struct InjectInfo));
	info->hProcess = hProcess;

	if ((info->ptrNewcode = MemMgr_Allocate(hProcess, Inject_newcode_size)) == 0)
		goto init_error;

	if ((info->ptrCondition = MemMgr_Allocate(hProcess, 1)) == 0)
		goto init_error;

	if ((info->ptrCode = MemMgr_Allocate(hProcess, 4)) == 0)
		goto init_error;

	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_condition))	= info->ptrCondition;
	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_code))	= info->ptrCode;
	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_ret))	= Inject_detour_return;

	if (!MemMgr_Write(hProcess, info->ptrNewcode, Inject_newcode_code, Inject_newcode_size))
		goto init_error;

	*((MemMgr_Ptr*)(Inject_detour_code + Inject_detour_offset_ret)) = info->ptrNewcode;

	info->oldDetourCode = (void*)malloc(Inject_detour_size);
	if (!MemMgr_Read(hProcess, Inject_detour_ptr, info->oldDetourCode, Inject_detour_size))
		goto init_error;

	if (!MemMgr_Write(hProcess, Inject_detour_ptr, Inject_detour_code, Inject_detour_size))
		goto init_error;

	return info;

init_error:
	free(info);
	return NULL;
}
void Inject_Free(struct InjectInfo *info)
{
	if (info == NULL)		return;
	if (info->hProcess == NULL)	return;

	if (info->oldDetourCode)	MemMgr_Write(info->hProcess, Inject_detour_ptr, info->oldDetourCode, Inject_detour_size);

	if (info->ptrNewcode)		MemMgr_Free(info->hProcess, info->ptrNewcode);
	if (info->ptrCondition)		MemMgr_Free(info->hProcess, info->ptrCondition);
	if (info->ptrCode)		MemMgr_Free(info->hProcess, info->ptrCode);

	if (info->func_DoString)	MemMgr_Free(info->hProcess, info->func_DoString);
	if (info->func_RegisterFunction)MemMgr_Free(info->hProcess, info->func_RegisterFunction);

	free(info);
}
MemMgr_Ptr Inject_CodeCreate(struct InjectInfo *info, LPVOID code, SIZE_T len)
{
	MemMgr_Ptr ptr;

	if ((ptr = MemMgr_Allocate(info->hProcess, len)) == 0)
		return 0;

	if (!MemMgr_Write(info->hProcess, ptr, code, len))
		return 0;

	return ptr;
}
BOOL Inject_CodeExecute(struct InjectInfo *info, MemMgr_Ptr codePtr, DWORD msTick)
{
	static const INT32 cond = 1;

	if (!MemMgr_Write32(info->hProcess, info->ptrCode, codePtr))
		return FALSE;

	if (!MemMgr_Write32(info->hProcess, info->ptrCondition, cond))
		return FALSE;

	while (Inject_IsCodeExecuting(info))
		Sleep(msTick);

	return TRUE;
}
BOOL Inject_IsCodeExecuting(struct InjectInfo *info)
{
	return MemMgr_Read8(info->hProcess, info->ptrCondition) != 0;
}
void Inject_CodeFree(struct InjectInfo *info, MemMgr_Ptr codePtr)
{
	MemMgr_Free(info->hProcess, codePtr);
}

MemMgr_Ptr Inject_LuaCreate(struct InjectInfo *info, LPSTR stringLua, SIZE_T stringSize)
{
	MemMgr_Ptr stringPtr;

	if (info->func_DoString == NULL)
	{
		if ((info->func_DoString = Inject_CodeCreate(info, Inject_lua_code, Inject_lua_size)) == 0)
			return FALSE;
	}

	if ((stringPtr = Inject_CodeCreate(info, stringLua, stringSize)) == 0)
		return FALSE;

	return stringPtr;
}
BOOL Inject_LuaExecute(struct InjectInfo *info, MemMgr_Ptr stringPtr, DWORD msTick)
{
	if (!MemMgr_Write32(info->hProcess, info->func_DoString + Inject_lua_offset_string, stringPtr))
		return FALSE;

	if (!Inject_CodeExecute(info, info->func_DoString, msTick))
		return FALSE;

	return TRUE;
}
BOOL Inject_LuaFunction(struct InjectInfo *info, LPSTR name, SIZE_T nameSize, LPVOID func, SIZE_T funcSize, UINT32 paramNum, DWORD msTick)
{
	MemMgr_Ptr namePtr, funcPtr;
	SIZE_T pad = 4 - (nameSize % 4);
	SIZE_T memSize = nameSize + pad + funcSize;

	if ((namePtr = MemMgr_Allocate(info->hProcess, memSize)) == 0)
		return FALSE;

	if (!MemMgr_Write(info->hProcess, namePtr, name, nameSize))
		return FALSE;

	funcPtr = namePtr + pad + nameSize;

	if (!MemMgr_Write(info->hProcess, funcPtr, func, funcSize))
		return FALSE;

	if (info->func_RegisterFunction == NULL)
	{
		if ((info->func_RegisterFunction = Inject_CodeCreate(info, Inject_func_code, Inject_func_size)) == 0)
			return FALSE;
	}
	if (!MemMgr_Write32(info->hProcess, info->func_RegisterFunction + Inject_func_offset_name, namePtr))
		return FALSE;
	if (!MemMgr_Write32(info->hProcess, info->func_RegisterFunction + Inject_func_offset_func, funcPtr))
		return FALSE;

	if (!Inject_CodeExecute(info, info->func_RegisterFunction, msTick))
		return FALSE;

	return TRUE;
}