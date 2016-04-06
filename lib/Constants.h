
#define	O_base				(0x400000)

#define	O_player_guid			(0x741E30)
#define	O_target_guid			(0x74E2d8)

#define	O_is_ingame			(0xB4B424)
#define	O_last_hardware_action	 	(0xCF0BC8)

#define	O_obj_mgr_curr			(0x00741414)
#define	O_obj_mgr_off			(0xAC)
#define	O_obj_mgr_next			(0x3C)
#define	O_obj_mgr_first			(0xAC)
#define	O_obj_mgr_local_guid		(0xC0)

#define	O_wow_obj_data_ptr		(0x8)
#define	O_wow_obj_type			(0x14)
#define	O_wow_obj_guid			(0x30)
#define	O_wow_obj_pos_y			(0x9B8)
#define	O_wow_obj_pos_x			(O_wow_obj_pos_y + 0x4)
#define	O_wow_obj_pos_z			(O_wow_obj_pos_y + 0x8)
#define	O_wow_obj_rotation		(O_wow_obj_pos_x + 0x10)
#define O_wow_obj_movementflags		(0x9E8)
#define	O_wow_obj_speed			(0xA34)

#define	O_wow_gameobj_pos_y		(0x2C4)
#define	O_wow_gameobj_pos_x		(O_wow_gameobj_pos_y + 0x4)
#define	O_wow_gameobj_pos_z		(O_wow_gameobj_pos_y + 0x8)

#define	O_wow_unit_charm		(0x18)
#define	O_wow_unit_summon		(0x20)
#define	O_wow_unit_charmedby		(0x28)
#define	O_wow_unit_summonedby		(0x30)
#define	O_wow_unit_createdby		(0x38)
#define	O_wow_unit_target		(0x40)
#define	O_wow_unit_channelobject	(0x50)
#define	O_wow_unit_hp_curr		(0x58)
#define	O_wow_unit_mana_curr		(0x5C)
#define	O_wow_unit_hp_max		(0x70)
#define	O_wow_unit_mana_max		(0x74)
#define	O_wow_unit_level		(0x88)

#define	O_wow_unitname_objname1		(0x214)
#define	O_wow_unitname_objname2		(0x8)
#define	O_wow_unitname_itemtype		(0x2DC)
#define	O_wow_unitname_unitname1	(0xB30)

enum {
	O_T_NONE = 0,
	O_T_ITEM = 1,
	O_T_CONTAINER = 2,
	O_T_UNIT = 3,
	O_T_PLAYER = 4,
	O_T_GAMEOBJ = 5,
	O_T_DYNOBJ = 6,
	O_T_CORPSE = 7,
	O_T_FORCEDWORD = 0xFFFFFFFF
};

enum
{
	O_M_None =          0x00000000,
	O_M_Forward =       0x00000001,
	O_M_Back =          0x00000002,
	O_M_TurnLeft =      0x00000010,
	O_M_TurnRight =     0x00000020,
	O_M_Stunned =       0x00001000,
	O_M_Swimming   =    0x00200000,
};