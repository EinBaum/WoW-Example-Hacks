
#include <windows.h>

typedef UINT32 MemMgr_Ptr;

BOOL		MemMgr_Read(HANDLE hProcess, MemMgr_Ptr offset, LPVOID data, SIZE_T size);
INT8		MemMgr_Read8(HANDLE hProcess, MemMgr_Ptr offset);
INT16		MemMgr_Read16(HANDLE hProcess, MemMgr_Ptr offset);
INT32		MemMgr_Read32(HANDLE hProcess, MemMgr_Ptr offset);
INT64		MemMgr_Read64(HANDLE hProcess, MemMgr_Ptr offset);
FLOAT		MemMgr_ReadFloat(HANDLE hProcess, MemMgr_Ptr offset);
BOOL		MemMgr_Write(HANDLE hProcess, MemMgr_Ptr, LPCVOID data, SIZE_T size);
BOOL		MemMgr_Write32(HANDLE hProcess, MemMgr_Ptr offset, INT32 data);

MemMgr_Ptr	MemMgr_Allocate(HANDLE hProcess, SIZE_T size);
void		MemMgr_Free(HANDLE hProcess, MemMgr_Ptr addr);