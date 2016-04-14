
#include "ObjMgr.h"

INT8 MemMgr_Read8(HANDLE hProcess, MemMgr_Ptr offset)
{
	INT8 r;
	if (!ReadProcessMemory(hProcess, (LPVOID)offset, &r, sizeof(r), NULL))
	{

	}
	return r;
}
INT16 MemMgr_Read16(HANDLE hProcess, MemMgr_Ptr offset)
{
	INT16 r;
	if (!ReadProcessMemory(hProcess, (LPVOID)offset, &r, sizeof(r), NULL))
	{

	}
	return r;
}
INT32 MemMgr_Read32(HANDLE hProcess, MemMgr_Ptr offset)
{
	INT32 r;
	if (!ReadProcessMemory(hProcess, (LPVOID)offset, &r, sizeof(r), NULL))
	{

	}
	return r;
}
INT64 MemMgr_Read64(HANDLE hProcess, MemMgr_Ptr offset)
{
	INT64 r;
	if (!ReadProcessMemory(hProcess, (LPVOID)offset, &r, sizeof(r), NULL))
	{

	}
	return r;
}
FLOAT MemMgr_ReadFloat(HANDLE hProcess, MemMgr_Ptr offset)
{
	FLOAT r;
	if (!ReadProcessMemory(hProcess, (LPVOID)offset, &r, sizeof(r), NULL))
	{

	}
	return r;
}
MemMgr_Ptr MemMgr_ReadPtr(HANDLE hProcess, MemMgr_Ptr offset)
{
	return (MemMgr_Ptr)MemMgr_Read32(hProcess, offset);
}
BOOL MemMgr_Write(HANDLE hProcess, MemMgr_Ptr offset, LPCVOID data, SIZE_T size)
{
	BOOL success = WriteProcessMemory(
		hProcess,
		(LPVOID)offset,
		data, size,
		NULL);

	return success;
}
BOOL MemMgr_Write32(HANDLE hProcess, MemMgr_Ptr offset, INT32 data)
{
	return MemMgr_Write(hProcess, offset, &data, sizeof(data));
}

MemMgr_Ptr MemMgr_Allocate(HANDLE hProcess, SIZE_T size)
{
	LPVOID ptr = VirtualAllocEx(
		hProcess,
		NULL,
		size,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);

	return (MemMgr_Ptr)ptr;
}
void MemMgr_Free(HANDLE hProcess, MemMgr_Ptr addr)
{
	VirtualFreeEx(hProcess, (LPVOID)addr, 0, MEM_RELEASE);
}