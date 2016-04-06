
#include "Inject.h"
#include "Constants.h"

#define Inject_detour_ptr		(0x6F9D81)
#define Inject_detour_return		(0x6F9D87)
#define Inject_detour_test		(0x7F0740)


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

static BOOL bInitialized = FALSE;
static MemMgr_Ptr ptrNewcode;
static MemMgr_Ptr ptrCondition;
static MemMgr_Ptr ptrCode;

BOOL Inject_IsInitialized(HANDLE hProcess)
{
	return bInitialized;
}
BOOL Inject_Initialize(HANDLE hProcess)
{
	if (Inject_IsInitialized(hProcess))
		return TRUE;

	if ((ptrNewcode = MemMgr_Allocate(hProcess, Inject_newcode_size)) == 0)
		return FALSE;

	if ((ptrCondition = MemMgr_Allocate(hProcess, 1)) == 0)
		return FALSE;

	if ((ptrCode = MemMgr_Allocate(hProcess, 4)) == 0)
		return FALSE;

	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_condition))	= ptrCondition;
	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_code))	= ptrCode;
	*((MemMgr_Ptr*)(Inject_newcode_code + Inject_newcode_offset_ret))	= Inject_detour_return;

	if (!MemMgr_Write(hProcess, ptrNewcode, Inject_newcode_code, Inject_newcode_size))
		return FALSE;

	*((MemMgr_Ptr*)(Inject_detour_code + Inject_detour_offset_ret)) = ptrNewcode;

	if (!MemMgr_Write(hProcess, Inject_detour_ptr, Inject_detour_code, Inject_detour_size))
		return FALSE;

	bInitialized = TRUE;
	return TRUE;
}
MemMgr_Ptr Inject_CodeCreate(HANDLE hProcess, LPVOID code, SIZE_T len)
{
	MemMgr_Ptr ptr;

	if ((ptr = MemMgr_Allocate(hProcess, len)) == 0)
		return 0;

	if (!MemMgr_Write(hProcess, ptr, code, len))
		return 0;

	return ptr;
}
BOOL Inject_CodeExecute(HANDLE hProcess, MemMgr_Ptr codePtr)
{
	static CHAR cond = 1;
	
	if (!Inject_Initialize(hProcess))
		return FALSE;

	if (!MemMgr_Write(hProcess, ptrCode, &codePtr, sizeof(codePtr)))
		return FALSE;

	if (!MemMgr_Write(hProcess, ptrCondition, &cond, sizeof(cond)))
		return FALSE;

	return TRUE;
}
BOOL Inject_IsCodeExecuting(HANDLE hProcess)
{
	return Inject_IsInitialized(hProcess)
		&& (MemMgr_Read8(hProcess, ptrCondition) != 0);
}

MemMgr_Ptr Inject_LuaCreate(HANDLE hProcess, LPSTR lua, SIZE_T len)
{
	return Inject_CodeCreate(hProcess, lua, len);
}
BOOL Inject_LuaExecute(HANDLE hProcess, MemMgr_Ptr luaPtr)
{
	return FALSE;
}