
#include <windows.h>
#include "MemMgr.h"

extern MemMgr_Ptr ObjMgr_TmpPlayerBase;
extern UINT64 ObjMgr_TmpPlayerGUID;

BOOL ObjMgr_IsLoggedIn(HANDLE hProcess);

struct ObjMgr_Obj;
struct ObjMgr_Obj {
	UINT64 guid;
	UINT64 summoned_by;
	FLOAT pos_x;
	FLOAT pos_y;
	FLOAT pos_z;
	FLOAT rotation;
	MemMgr_Ptr base_addr;
	MemMgr_Ptr unitfields_addr;
	UINT16 type;
	char name[50];

	UINT32 hp_curr;
	UINT32 hp_max;
	UINT32 mana_curr;
	UINT32 mana_max;
	UINT32 level;

	UINT32 obj_type;

	struct ObjMgr_Obj* prev;
};

struct ObjMgr_Obj* ObjMgr_GetObjects(HANDLE hProcess);
void ObjMgr_DelObjects(struct ObjMgr_Obj* obj);