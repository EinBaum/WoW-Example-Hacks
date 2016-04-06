
#include "ObjMgr.h"
#include "Constants.h"

MemMgr_Ptr ObjMgr_TmpPlayerBase	= (MemMgr_Ptr)0;
UINT64 ObjMgr_TmpPlayerGUID	= 0;


struct ObjMgr_Obj* ObjMgr_GetObjects(HANDLE hProcess)
{

	ObjMgr_TmpPlayerGUID = MemMgr_Read64(hProcess, O_base + O_player_guid);

	MemMgr_Ptr ptr_obj_first = MemMgr_ReadPtr(hProcess, MemMgr_ReadPtr(hProcess, O_base + O_obj_mgr_curr) + O_obj_mgr_off);

	MemMgr_Ptr obj_base_addr = ptr_obj_first;
	struct ObjMgr_Obj* obj_cur = NULL;
	while (obj_base_addr != 0 && (obj_base_addr % 2 == 0)) {

		struct ObjMgr_Obj* obj_new = (struct ObjMgr_Obj*)calloc(1, sizeof(struct ObjMgr_Obj));
		obj_new->prev = obj_cur;
		obj_cur = obj_new;

		obj_cur->base_addr = obj_base_addr;
		obj_cur->guid = MemMgr_Read64(hProcess, obj_cur->base_addr + O_wow_obj_guid);
		obj_cur->type = MemMgr_Read16(hProcess, obj_cur->base_addr + O_wow_obj_type);

		switch (obj_cur->type) {
		case O_T_UNIT:
		case O_T_PLAYER:
			obj_cur->unitfields_addr	= MemMgr_ReadPtr(hProcess, obj_cur->base_addr + O_wow_obj_data_ptr);
			obj_cur->hp_curr		= MemMgr_Read32(hProcess, obj_cur->unitfields_addr + O_wow_unit_hp_curr);
			obj_cur->hp_max			= MemMgr_Read32(hProcess, obj_cur->unitfields_addr + O_wow_unit_hp_max);
			obj_cur->summoned_by		= MemMgr_Read64(hProcess, obj_cur->unitfields_addr + O_wow_unit_summonedby);

			obj_cur->pos_x			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_obj_pos_x);
			obj_cur->pos_y			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_obj_pos_y);
			obj_cur->pos_z			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_obj_pos_z);
			obj_cur->rotation		= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_obj_rotation);
			switch (obj_cur->type) {
			case O_T_UNIT:
				ReadProcessMemory(hProcess,
					(LPCVOID)MemMgr_ReadPtr(hProcess, MemMgr_ReadPtr(hProcess, obj_cur->base_addr + O_wow_unitname_unitname1)),
					obj_cur->name,
					sizeof(obj_cur->name),
					NULL);
				break;
			case O_T_PLAYER:
				if (ObjMgr_TmpPlayerGUID == obj_cur->guid) {
					ObjMgr_TmpPlayerBase = obj_cur->base_addr;
					ObjMgr_TmpPlayerGUID = obj_cur->guid;
				}
				//obj_cur->name = (const char*)MemMgr_ReadPtr(hProcess, MemMgr_ReadPtr(hProcess, obj_cur->base_addr + O_wow_unitname_unitname1));
				break;
			}

			break;

		case O_T_GAMEOBJ:
			obj_cur->pos_x			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_gameobj_pos_x);
			obj_cur->pos_y			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_gameobj_pos_y);
			obj_cur->pos_z			= MemMgr_ReadFloat(hProcess, obj_cur->base_addr + O_wow_gameobj_pos_z);

			obj_cur->unitfields_addr	= MemMgr_ReadPtr(hProcess, obj_cur->base_addr + O_wow_obj_data_ptr);
			ReadProcessMemory(hProcess,
				(LPCVOID)(MemMgr_Read32(hProcess, (MemMgr_Read32(hProcess, (obj_cur->base_addr + O_wow_unitname_objname1)) + O_wow_unitname_objname2))),
				obj_cur->name,
				sizeof(obj_cur->name),
				NULL);
			obj_cur->type = MemMgr_Read32(hProcess, obj_cur->base_addr + O_wow_unitname_itemtype);

			break;
		}

		obj_base_addr = MemMgr_ReadPtr(hProcess, obj_cur->base_addr + O_obj_mgr_next);
	}

	return obj_cur;
}
void ObjMgr_DelObjects(struct ObjMgr_Obj* obj) {

	while (obj != NULL) {
		struct ObjMgr_Obj* obj_prev = obj->prev;
		free(obj);
		obj = obj_prev;
	}
}