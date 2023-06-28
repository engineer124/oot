#ifndef Z64PLAYER_H
#define Z64PLAYER_H

#include "z64actor.h"
#include "alignment.h"

struct Player;

typedef enum {
    /* 0 */ PLAYER_SWORD_NONE,
    /* 1 */ PLAYER_SWORD_KOKIRI,
    /* 2 */ PLAYER_SWORD_MASTER,
    /* 3 */ PLAYER_SWORD_BIGGORON,
    /* 4 */ PLAYER_SWORD_MAX
} PlayerSword;

typedef enum {
    /* 0x00 */ PLAYER_SHIELD_NONE,
    /* 0x01 */ PLAYER_SHIELD_DEKU,
    /* 0x02 */ PLAYER_SHIELD_HYLIAN,
    /* 0x03 */ PLAYER_SHIELD_MIRROR,
    /* 0x04 */ PLAYER_SHIELD_MAX
} PlayerShield;

typedef enum {
    /* 0x00 */ PLAYER_TUNIC_KOKIRI,
    /* 0x01 */ PLAYER_TUNIC_GORON,
    /* 0x02 */ PLAYER_TUNIC_ZORA,
    /* 0x03 */ PLAYER_TUNIC_MAX
} PlayerTunic;

typedef enum {
    /* 0x00 */ PLAYER_BOOTS_KOKIRI,
    /* 0x01 */ PLAYER_BOOTS_IRON,
    /* 0x02 */ PLAYER_BOOTS_HOVER,
    /* Values below are only relevant when setting regs in Player_SetBootData */
    /* 0x03 */ PLAYER_BOOTS_INDOOR,
    /* 0x04 */ PLAYER_BOOTS_IRON_UNDERWATER,
    /* 0x05 */ PLAYER_BOOTS_KOKIRI_CHILD,
    /* 0x06 */ PLAYER_BOOTS_MAX
} PlayerBoots;

typedef enum {
    /* 0x00 */ PLAYER_STR_NONE,
    /* 0x01 */ PLAYER_STR_BRACELET,
    /* 0x02 */ PLAYER_STR_SILVER_G,
    /* 0x03 */ PLAYER_STR_GOLD_G,
    /* 0x04 */ PLAYER_STR_MAX
} PlayerStrength;

typedef enum {
    /* 0x00 */ PLAYER_MASK_NONE,
    /* 0x01 */ PLAYER_MASK_KEATON,
    /* 0x02 */ PLAYER_MASK_SKULL,
    /* 0x03 */ PLAYER_MASK_SPOOKY,
    /* 0x04 */ PLAYER_MASK_BUNNY,
    /* 0x05 */ PLAYER_MASK_GORON,
    /* 0x06 */ PLAYER_MASK_ZORA,
    /* 0x07 */ PLAYER_MASK_GERUDO,
    /* 0x08 */ PLAYER_MASK_TRUTH,
    /* 0x09 */ PLAYER_MASK_MAX
} PlayerMask;

typedef enum {
    /* 0x0 */ PLAYER_ENV_HAZARD_NONE,
    /* 0x1 */ PLAYER_ENV_HAZARD_HOTROOM,
    /* 0x2 */ PLAYER_ENV_HAZARD_UNDERWATER_FLOOR,
    /* 0x3 */ PLAYER_ENV_HAZARD_SWIMMING,
    /* 0x4 */ PLAYER_ENV_HAZARD_UNDERWATER_FREE
} PlayerEnvHazard;

typedef enum {
    /* 0x00 */ PLAYER_IA_NONE,
    /* 0x01 */ PLAYER_IA_LAST_USED,
    /* 0x02 */ PLAYER_IA_FISHING_POLE,
    /* 0x03 */ PLAYER_IA_SWORD_MASTER,
    /* 0x04 */ PLAYER_IA_SWORD_KOKIRI,
    /* 0x05 */ PLAYER_IA_SWORD_BIGGORON,
    /* 0x06 */ PLAYER_IA_DEKU_STICK,
    /* 0x07 */ PLAYER_IA_HAMMER,
    /* 0x08 */ PLAYER_IA_BOW,
    /* 0x09 */ PLAYER_IA_BOW_FIRE,
    /* 0x0A */ PLAYER_IA_BOW_ICE,
    /* 0x0B */ PLAYER_IA_BOW_LIGHT,
    /* 0x0C */ PLAYER_IA_BOW_0C,
    /* 0x0D */ PLAYER_IA_BOW_0D,
    /* 0x0E */ PLAYER_IA_BOW_0E,
    /* 0x0F */ PLAYER_IA_SLINGSHOT,
    /* 0x10 */ PLAYER_IA_HOOKSHOT,
    /* 0x11 */ PLAYER_IA_LONGSHOT,
    /* 0x12 */ PLAYER_IA_BOMB,
    /* 0x13 */ PLAYER_IA_BOMBCHU,
    /* 0x14 */ PLAYER_IA_BOOMERANG,
    /* 0x15 */ PLAYER_IA_MAGIC_SPELL_15,
    /* 0x16 */ PLAYER_IA_MAGIC_SPELL_16,
    /* 0x17 */ PLAYER_IA_MAGIC_SPELL_17,
    /* 0x18 */ PLAYER_IA_FARORES_WIND,
    /* 0x19 */ PLAYER_IA_NAYRUS_LOVE,
    /* 0x1A */ PLAYER_IA_DINS_FIRE,
    /* 0x1B */ PLAYER_IA_DEKU_NUT,
    /* 0x1C */ PLAYER_IA_OCARINA_FAIRY,
    /* 0x1D */ PLAYER_IA_OCARINA_OF_TIME,
    /* 0x1E */ PLAYER_IA_BOTTLE,
    /* 0x1F */ PLAYER_IA_BOTTLE_FISH,
    /* 0x20 */ PLAYER_IA_BOTTLE_FIRE,
    /* 0x21 */ PLAYER_IA_BOTTLE_BUG,
    /* 0x22 */ PLAYER_IA_BOTTLE_POE,
    /* 0x23 */ PLAYER_IA_BOTTLE_BIG_POE,
    /* 0x24 */ PLAYER_IA_BOTTLE_RUTOS_LETTER,
    /* 0x25 */ PLAYER_IA_BOTTLE_POTION_RED,
    /* 0x26 */ PLAYER_IA_BOTTLE_POTION_BLUE,
    /* 0x27 */ PLAYER_IA_BOTTLE_POTION_GREEN,
    /* 0x28 */ PLAYER_IA_BOTTLE_MILK_FULL,
    /* 0x29 */ PLAYER_IA_BOTTLE_MILK_HALF,
    /* 0x2A */ PLAYER_IA_BOTTLE_FAIRY,
    /* 0x2B */ PLAYER_IA_ZELDAS_LETTER,
    /* 0x2C */ PLAYER_IA_WEIRD_EGG,
    /* 0x2D */ PLAYER_IA_CHICKEN,
    /* 0x2E */ PLAYER_IA_MAGIC_BEAN,
    /* 0x2F */ PLAYER_IA_POCKET_EGG,
    /* 0x30 */ PLAYER_IA_POCKET_CUCCO,
    /* 0x31 */ PLAYER_IA_COJIRO,
    /* 0x32 */ PLAYER_IA_ODD_MUSHROOM,
    /* 0x33 */ PLAYER_IA_ODD_POTION,
    /* 0x34 */ PLAYER_IA_POACHERS_SAW,
    /* 0x35 */ PLAYER_IA_BROKEN_GORONS_SWORD,
    /* 0x36 */ PLAYER_IA_PRESCRIPTION,
    /* 0x37 */ PLAYER_IA_FROG,
    /* 0x38 */ PLAYER_IA_EYEDROPS,
    /* 0x39 */ PLAYER_IA_CLAIM_CHECK,
    /* 0x3A */ PLAYER_IA_MASK_KEATON,
    /* 0x3B */ PLAYER_IA_MASK_SKULL,
    /* 0x3C */ PLAYER_IA_MASK_SPOOKY,
    /* 0x3D */ PLAYER_IA_MASK_BUNNY_HOOD,
    /* 0x3E */ PLAYER_IA_MASK_GORON,
    /* 0x3F */ PLAYER_IA_MASK_ZORA,
    /* 0x40 */ PLAYER_IA_MASK_GERUDO,
    /* 0x41 */ PLAYER_IA_MASK_TRUTH,
    /* 0x42 */ PLAYER_IA_LENS_OF_TRUTH,
    /* 0x43 */ PLAYER_IA_MAX
} PlayerItemAction;

typedef enum {
    /* 0x00 */ PLAYER_LIMB_NONE,
    /* 0x01 */ PLAYER_LIMB_ROOT,
    /* 0x02 */ PLAYER_LIMB_WAIST,
    /* 0x03 */ PLAYER_LIMB_LOWER,
    /* 0x04 */ PLAYER_LIMB_R_THIGH,
    /* 0x05 */ PLAYER_LIMB_R_SHIN,
    /* 0x06 */ PLAYER_LIMB_R_FOOT,
    /* 0x07 */ PLAYER_LIMB_L_THIGH,
    /* 0x08 */ PLAYER_LIMB_L_SHIN,
    /* 0x09 */ PLAYER_LIMB_L_FOOT,
    /* 0x0A */ PLAYER_LIMB_UPPER,
    /* 0x0B */ PLAYER_LIMB_HEAD,
    /* 0x0C */ PLAYER_LIMB_HAT,
    /* 0x0D */ PLAYER_LIMB_COLLAR,
    /* 0x0E */ PLAYER_LIMB_L_SHOULDER,
    /* 0x0F */ PLAYER_LIMB_L_FOREARM,
    /* 0x10 */ PLAYER_LIMB_L_HAND,
    /* 0x11 */ PLAYER_LIMB_R_SHOULDER,
    /* 0x12 */ PLAYER_LIMB_R_FOREARM,
    /* 0x13 */ PLAYER_LIMB_R_HAND,
    /* 0x14 */ PLAYER_LIMB_SHEATH,
    /* 0x15 */ PLAYER_LIMB_TORSO,
    /* 0x16 */ PLAYER_LIMB_MAX
} PlayerLimb;

typedef enum {
    /* 0x00 */ PLAYER_BODYPART_WAIST,      // PLAYER_LIMB_WAIST
    /* 0x01 */ PLAYER_BODYPART_R_THIGH,    // PLAYER_LIMB_R_THIGH
    /* 0x02 */ PLAYER_BODYPART_R_SHIN,     // PLAYER_LIMB_R_SHIN
    /* 0x03 */ PLAYER_BODYPART_R_FOOT,     // PLAYER_LIMB_R_FOOT
    /* 0x04 */ PLAYER_BODYPART_L_THIGH,    // PLAYER_LIMB_L_THIGH
    /* 0x05 */ PLAYER_BODYPART_L_SHIN,     // PLAYER_LIMB_L_SHIN
    /* 0x06 */ PLAYER_BODYPART_L_FOOT,     // PLAYER_LIMB_L_FOOT
    /* 0x07 */ PLAYER_BODYPART_HEAD,       // PLAYER_LIMB_HEAD
    /* 0x08 */ PLAYER_BODYPART_HAT,        // PLAYER_LIMB_HAT
    /* 0x09 */ PLAYER_BODYPART_COLLAR,     // PLAYER_LIMB_COLLAR
    /* 0x0A */ PLAYER_BODYPART_L_SHOULDER, // PLAYER_LIMB_L_SHOULDER
    /* 0x0B */ PLAYER_BODYPART_L_FOREARM,  // PLAYER_LIMB_L_FOREARM
    /* 0x0C */ PLAYER_BODYPART_L_HAND,     // PLAYER_LIMB_L_HAND
    /* 0x0D */ PLAYER_BODYPART_R_SHOULDER, // PLAYER_LIMB_R_SHOULDER
    /* 0x0E */ PLAYER_BODYPART_R_FOREARM,  // PLAYER_LIMB_R_FOREARM
    /* 0x0F */ PLAYER_BODYPART_R_HAND,     // PLAYER_LIMB_R_HAND
    /* 0x10 */ PLAYER_BODYPART_SHEATH,     // PLAYER_LIMB_SHEATH
    /* 0x11 */ PLAYER_BODYPART_TORSO,      // PLAYER_LIMB_TORSO
    /* 0x12 */ PLAYER_BODYPART_MAX
} PlayerBodyPart;

typedef enum {
    /*  0 */ PLAYER_MWA_FORWARD_SLASH_1H,
    /*  1 */ PLAYER_MWA_FORWARD_SLASH_2H,
    /*  2 */ PLAYER_MWA_FORWARD_COMBO_1H,
    /*  3 */ PLAYER_MWA_FORWARD_COMBO_2H,
    /*  4 */ PLAYER_MWA_RIGHT_SLASH_1H,
    /*  5 */ PLAYER_MWA_RIGHT_SLASH_2H,
    /*  6 */ PLAYER_MWA_RIGHT_COMBO_1H,
    /*  7 */ PLAYER_MWA_RIGHT_COMBO_2H,
    /*  8 */ PLAYER_MWA_LEFT_SLASH_1H,
    /*  9 */ PLAYER_MWA_LEFT_SLASH_2H,
    /* 10 */ PLAYER_MWA_LEFT_COMBO_1H,
    /* 11 */ PLAYER_MWA_LEFT_COMBO_2H,
    /* 12 */ PLAYER_MWA_STAB_1H,
    /* 13 */ PLAYER_MWA_STAB_2H,
    /* 14 */ PLAYER_MWA_STAB_COMBO_1H,
    /* 15 */ PLAYER_MWA_STAB_COMBO_2H,
    /* 16 */ PLAYER_MWA_FLIPSLASH_START,
    /* 17 */ PLAYER_MWA_JUMPSLASH_START,
    /* 18 */ PLAYER_MWA_FLIPSLASH_FINISH,
    /* 19 */ PLAYER_MWA_JUMPSLASH_FINISH,
    /* 20 */ PLAYER_MWA_BACKSLASH_RIGHT,
    /* 21 */ PLAYER_MWA_BACKSLASH_LEFT,
    /* 22 */ PLAYER_MWA_HAMMER_FORWARD,
    /* 23 */ PLAYER_MWA_HAMMER_SIDE,
    /* 24 */ PLAYER_MWA_SPIN_ATTACK_1H,
    /* 25 */ PLAYER_MWA_SPIN_ATTACK_2H,
    /* 26 */ PLAYER_MWA_BIG_SPIN_1H,
    /* 27 */ PLAYER_MWA_BIG_SPIN_2H,
    /* 28 */ PLAYER_MWA_MAX
} PlayerMeleeWeaponAnimation;

typedef enum {
    /* -1 */ PLAYER_DOORTYPE_AJAR = -1,
    /*  0 */ PLAYER_DOORTYPE_NONE,
    /*  1 */ PLAYER_DOORTYPE_HANDLE,
    /*  2 */ PLAYER_DOORTYPE_SLIDING,
    /*  3 */ PLAYER_DOORTYPE_FAKE
} PlayerDoorType;

typedef enum {
    /* 0x00 */ PLAYER_MODELGROUP_0, // unused (except with the `func_80091880` bug)
    /* 0x01 */ PLAYER_MODELGROUP_CHILD_HYLIAN_SHIELD, // kokiri/master sword, shield not in hand
    /* 0x02 */ PLAYER_MODELGROUP_SWORD, // kokiri/master sword and possibly shield
    /* 0x03 */ PLAYER_MODELGROUP_DEFAULT, // non-specific models, for items that don't have particular link models
    /* 0x04 */ PLAYER_MODELGROUP_4, // unused, same as PLAYER_MODELGROUP_DEFAULT
    /* 0x05 */ PLAYER_MODELGROUP_BGS, // biggoron sword
    /* 0x06 */ PLAYER_MODELGROUP_BOW_SLINGSHOT, // bow/slingshot
    /* 0x07 */ PLAYER_MODELGROUP_EXPLOSIVES, // bombs, bombchus, same as PLAYER_MODELGROUP_DEFAULT
    /* 0x08 */ PLAYER_MODELGROUP_BOOMERANG,
    /* 0x09 */ PLAYER_MODELGROUP_HOOKSHOT,
    /* 0x0A */ PLAYER_MODELGROUP_10, // stick/fishing pole (which are drawn separately)
    /* 0x0B */ PLAYER_MODELGROUP_HAMMER,
    /* 0x0C */ PLAYER_MODELGROUP_OCARINA, // ocarina
    /* 0x0D */ PLAYER_MODELGROUP_OOT, // ocarina of time
    /* 0x0E */ PLAYER_MODELGROUP_BOTTLE, // bottles (drawn separately)
    /* 0x0F */ PLAYER_MODELGROUP_15, // "last used"
    /* 0x10 */ PLAYER_MODELGROUP_MAX
} PlayerModelGroup;

typedef enum {
    /* 0x00 */ PLAYER_MODELGROUPENTRY_ANIM,
    /* 0x01 */ PLAYER_MODELGROUPENTRY_LEFT_HAND,
    /* 0x02 */ PLAYER_MODELGROUPENTRY_RIGHT_HAND,
    /* 0x03 */ PLAYER_MODELGROUPENTRY_SHEATH,
    /* 0x04 */ PLAYER_MODELGROUPENTRY_WAIST,
    /* 0x05 */ PLAYER_MODELGROUPENTRY_MAX
} PlayerModelGroupEntry;

typedef enum {
    // left hand
    /* 0x00 */ PLAYER_MODELTYPE_LH_OPEN, // empty open hand
    /* 0x01 */ PLAYER_MODELTYPE_LH_CLOSED, // empty closed hand
    /* 0x02 */ PLAYER_MODELTYPE_LH_SWORD, // holding kokiri/master sword
    /* 0x03 */ PLAYER_MODELTYPE_LH_SWORD_2, // unused, same as PLAYER_MODELTYPE_LH_SWORD
    /* 0x04 */ PLAYER_MODELTYPE_LH_BGS, // holding bgs/broken giant knife (child: master sword)
    /* 0x05 */ PLAYER_MODELTYPE_LH_HAMMER, // holding hammer (child: empty hand)
    /* 0x06 */ PLAYER_MODELTYPE_LH_BOOMERANG, // holding boomerang (adult: empty hand)
    /* 0x07 */ PLAYER_MODELTYPE_LH_BOTTLE, // holding bottle (bottle drawn separately)
    // right hand
    /* 0x08 */ PLAYER_MODELTYPE_RH_OPEN, // empty open hand
    /* 0x09 */ PLAYER_MODELTYPE_RH_CLOSED, // empty closed hand
    /* 0x0A */ PLAYER_MODELTYPE_RH_SHIELD, // holding a shield (including no shield)
    /* 0x0B */ PLAYER_MODELTYPE_RH_BOW_SLINGSHOT, // holding bow/slingshot
    /* 0x0C */ PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2, // unused, same as PLAYER_MODELTYPE_RH_BOW_SLINGSHOT
    /* 0x0D */ PLAYER_MODELTYPE_RH_OCARINA, // holding ocarina (child: fairy ocarina, adult: OoT)
    /* 0x0E */ PLAYER_MODELTYPE_RH_OOT, // holding OoT
    /* 0x0F */ PLAYER_MODELTYPE_RH_HOOKSHOT, // holding hookshot (child: empty hand)
    // sheath
    /* 0x10 */ PLAYER_MODELTYPE_SHEATH_16, // sheathed kokiri/master sword?
    /* 0x11 */ PLAYER_MODELTYPE_SHEATH_17, // empty sheath?
    /* 0x12 */ PLAYER_MODELTYPE_SHEATH_18, // sword sheathed and shield on back?
    /* 0x13 */ PLAYER_MODELTYPE_SHEATH_19, // empty sheath and shield on back?
    // waist
    /* 0x14 */ PLAYER_MODELTYPE_WAIST,
    /* 0x15 */ PLAYER_MODELTYPE_MAX,
    /* 0xFF */ PLAYER_MODELTYPE_RH_FF = 0xFF // disable shield collider, cutscene-specific
} PlayerModelType;

// Some player animations are played at this reduced speed, for reasons yet unclear.
// Perhaps to compress animation data? Or to adjust from 30fps to 20fps?
// This is called "adjusted" for now.
// z_en_horse also has many instances of this adjusted speed
#define PLAYER_ANIM_ADJUSTED_SPEED (2.0f / 3.0f)
#define PLAYER_ANIM_NORMAL_SPEED   (3.0f / 3.0f)

typedef enum {
    /* 0x00 */ PLAYER_ANIMTYPE_0,
    /* 0x01 */ PLAYER_ANIMTYPE_1,
    /* 0x02 */ PLAYER_ANIMTYPE_2,
    /* 0x03 */ PLAYER_ANIMTYPE_3,
    /* 0x04 */ PLAYER_ANIMTYPE_4,
    /* 0x05 */ PLAYER_ANIMTYPE_5,
    /* 0x06 */ PLAYER_ANIMTYPE_MAX
} PlayerAnimType;

/**
 * Temporary names, derived from original animation names in `D_80853914`
 */
typedef enum {
    /* 0x00 */ PLAYER_ANIMGROUP_wait,
    /* 0x01 */ PLAYER_ANIMGROUP_walk,
    /* 0x02 */ PLAYER_ANIMGROUP_run,
    /* 0x03 */ PLAYER_ANIMGROUP_damage_run,
    /* 0x04 */ PLAYER_ANIMGROUP_heavy_run,
    /* 0x05 */ PLAYER_ANIMGROUP_waitL,
    /* 0x06 */ PLAYER_ANIMGROUP_waitR,
    /* 0x07 */ PLAYER_ANIMGROUP_wait2waitR,
    /* 0x08 */ PLAYER_ANIMGROUP_normal2fighter,
    /* 0x09 */ PLAYER_ANIMGROUP_doorA_free,
    /* 0x0A */ PLAYER_ANIMGROUP_doorA,
    /* 0x0B */ PLAYER_ANIMGROUP_doorB_free,
    /* 0x0C */ PLAYER_ANIMGROUP_doorB,
    /* 0x0D */ PLAYER_ANIMGROUP_carryB,
    /* 0x0E */ PLAYER_ANIMGROUP_landing,
    /* 0x0F */ PLAYER_ANIMGROUP_short_landing,
    /* 0x10 */ PLAYER_ANIMGROUP_landing_roll,
    /* 0x11 */ PLAYER_ANIMGROUP_hip_down,
    /* 0x12 */ PLAYER_ANIMGROUP_walk_endL,
    /* 0x13 */ PLAYER_ANIMGROUP_walk_endR,
    /* 0x14 */ PLAYER_ANIMGROUP_defense,
    /* 0x15 */ PLAYER_ANIMGROUP_defense_wait,
    /* 0x16 */ PLAYER_ANIMGROUP_defense_end,
    /* 0x17 */ PLAYER_ANIMGROUP_side_walk,
    /* 0x18 */ PLAYER_ANIMGROUP_side_walkL,
    /* 0x19 */ PLAYER_ANIMGROUP_side_walkR,
    /* 0x1A */ PLAYER_ANIMGROUP_45_turn,
    /* 0x1B */ PLAYER_ANIMGROUP_waitL2wait,
    /* 0x1C */ PLAYER_ANIMGROUP_waitR2wait,
    /* 0x1D */ PLAYER_ANIMGROUP_throw,
    /* 0x1E */ PLAYER_ANIMGROUP_put,
    /* 0x1F */ PLAYER_ANIMGROUP_back_walk,
    /* 0x20 */ PLAYER_ANIMGROUP_check,
    /* 0x21 */ PLAYER_ANIMGROUP_check_wait,
    /* 0x22 */ PLAYER_ANIMGROUP_check_end,
    /* 0x23 */ PLAYER_ANIMGROUP_pull_start,
    /* 0x24 */ PLAYER_ANIMGROUP_pulling,
    /* 0x25 */ PLAYER_ANIMGROUP_pull_end,
    /* 0x26 */ PLAYER_ANIMGROUP_fall_up,
    /* 0x27 */ PLAYER_ANIMGROUP_jump_climb_hold,
    /* 0x28 */ PLAYER_ANIMGROUP_jump_climb_wait,
    /* 0x29 */ PLAYER_ANIMGROUP_jump_climb_up,
    /* 0x2A */ PLAYER_ANIMGROUP_down_slope_slip_end,
    /* 0x2B */ PLAYER_ANIMGROUP_up_slope_slip_end,
    /* 0x2C */ PLAYER_ANIMGROUP_nwait,
    /* 0x2D */ PLAYER_ANIMGROUP_MAX
} PlayerAnimGroup;

#define LIMB_BUF_COUNT(limbCount) ((ALIGN16((limbCount) * sizeof(Vec3s)) + sizeof(Vec3s) - 1) / sizeof(Vec3s))
#define PLAYER_LIMB_BUF_COUNT LIMB_BUF_COUNT(PLAYER_LIMB_MAX)

typedef enum PlayerAttentionMode {
    /* 0 */ PLAYER_ATTENTIONMODE_NONE,
    /* 1 */ PLAYER_ATTENTIONMODE_C_UP,
    /* 2 */ PLAYER_ATTENTIONMODE_AIMING,
    /* 3 */ PLAYER_ATTENTIONMODE_CUTSCENE,
    /* 4 */ PLAYER_ATTENTIONMODE_ITEM_CUTSCENE,
    /* 5 */ PLAYER_ATTENTIONMODE_MAX
} PlayerAttentionMode;

typedef enum {
    /* 0x00 */ PLAYER_CSMODE_NONE,
    /* 0x01 */ PLAYER_CSMODE_1,
    /* 0x02 */ PLAYER_CSMODE_2,
    /* 0x03 */ PLAYER_CSMODE_3,
    /* 0x04 */ PLAYER_CSMODE_4,
    /* 0x05 */ PLAYER_CSMODE_5,
    /* 0x06 */ PLAYER_CSMODE_6,
    /* 0x07 */ PLAYER_CSMODE_7,
    /* 0x08 */ PLAYER_CSMODE_8,
    /* 0x09 */ PLAYER_CSMODE_9,
    /* 0x0A */ PLAYER_CSMODE_10,
    /* 0x0B */ PLAYER_CSMODE_11,
    /* 0x0C */ PLAYER_CSMODE_12,
    /* 0x0D */ PLAYER_CSMODE_13,
    /* 0x0E */ PLAYER_CSMODE_14,
    /* 0x0F */ PLAYER_CSMODE_15,
    /* 0x10 */ PLAYER_CSMODE_16,
    /* 0x11 */ PLAYER_CSMODE_17,
    /* 0x12 */ PLAYER_CSMODE_18,
    /* 0x13 */ PLAYER_CSMODE_19,
    /* 0x14 */ PLAYER_CSMODE_20,
    /* 0x15 */ PLAYER_CSMODE_21,
    /* 0x16 */ PLAYER_CSMODE_22,
    /* 0x17 */ PLAYER_CSMODE_23,
    /* 0x18 */ PLAYER_CSMODE_24,
    /* 0x19 */ PLAYER_CSMODE_25,
    /* 0x1A */ PLAYER_CSMODE_26,
    /* 0x1B */ PLAYER_CSMODE_27,
    /* 0x1C */ PLAYER_CSMODE_28,
    /* 0x1D */ PLAYER_CSMODE_29,
    /* 0x1E */ PLAYER_CSMODE_30,
    /* 0x1F */ PLAYER_CSMODE_31,
    /* 0x20 */ PLAYER_CSMODE_32,
    /* 0x21 */ PLAYER_CSMODE_33,
    /* 0x22 */ PLAYER_CSMODE_34,
    /* 0x23 */ PLAYER_CSMODE_35,
    /* 0x24 */ PLAYER_CSMODE_36,
    /* 0x25 */ PLAYER_CSMODE_37,
    /* 0x26 */ PLAYER_CSMODE_38,
    /* 0x27 */ PLAYER_CSMODE_39,
    /* 0x28 */ PLAYER_CSMODE_40,
    /* 0x29 */ PLAYER_CSMODE_41,
    /* 0x2A */ PLAYER_CSMODE_42,
    /* 0x2B */ PLAYER_CSMODE_43,
    /* 0x2C */ PLAYER_CSMODE_44,
    /* 0x2D */ PLAYER_CSMODE_45,
    /* 0x2E */ PLAYER_CSMODE_46,
    /* 0x2F */ PLAYER_CSMODE_47,
    /* 0x30 */ PLAYER_CSMODE_48,
    /* 0x31 */ PLAYER_CSMODE_49,
    /* 0x32 */ PLAYER_CSMODE_50,
    /* 0x33 */ PLAYER_CSMODE_51,
    /* 0x34 */ PLAYER_CSMODE_52,
    /* 0x35 */ PLAYER_CSMODE_53,
    /* 0x36 */ PLAYER_CSMODE_54,
    /* 0x37 */ PLAYER_CSMODE_55,
    /* 0x38 */ PLAYER_CSMODE_56,
    /* 0x39 */ PLAYER_CSMODE_57,
    /* 0x3A */ PLAYER_CSMODE_58,
    /* 0x3B */ PLAYER_CSMODE_59,
    /* 0x3C */ PLAYER_CSMODE_60,
    /* 0x3D */ PLAYER_CSMODE_61,
    /* 0x3E */ PLAYER_CSMODE_62,
    /* 0x3F */ PLAYER_CSMODE_63,
    /* 0x40 */ PLAYER_CSMODE_64,
    /* 0x41 */ PLAYER_CSMODE_65,
    /* 0x42 */ PLAYER_CSMODE_66,
    /* 0x43 */ PLAYER_CSMODE_67,
    /* 0x44 */ PLAYER_CSMODE_68,
    /* 0x45 */ PLAYER_CSMODE_69,
    /* 0x46 */ PLAYER_CSMODE_70,
    /* 0x47 */ PLAYER_CSMODE_71,
    /* 0x48 */ PLAYER_CSMODE_72,
    /* 0x49 */ PLAYER_CSMODE_73,
    /* 0x4A */ PLAYER_CSMODE_74,
    /* 0x4B */ PLAYER_CSMODE_75,
    /* 0x4C */ PLAYER_CSMODE_76,
    /* 0x4D */ PLAYER_CSMODE_77,
    /* 0x4E */ PLAYER_CSMODE_78,
    /* 0x4F */ PLAYER_CSMODE_79,
    /* 0x50 */ PLAYER_CSMODE_80,
    /* 0x51 */ PLAYER_CSMODE_81,
    /* 0x52 */ PLAYER_CSMODE_82,
    /* 0x53 */ PLAYER_CSMODE_83,
    /* 0x54 */ PLAYER_CSMODE_84,
    /* 0x55 */ PLAYER_CSMODE_85,
    /* 0x56 */ PLAYER_CSMODE_86,
    /* 0x57 */ PLAYER_CSMODE_87,
    /* 0x58 */ PLAYER_CSMODE_88,
    /* 0x59 */ PLAYER_CSMODE_89,
    /* 0x5A */ PLAYER_CSMODE_90,
    /* 0x5B */ PLAYER_CSMODE_91,
    /* 0x5C */ PLAYER_CSMODE_92,
    /* 0x5D */ PLAYER_CSMODE_93,
    /* 0x5E */ PLAYER_CSMODE_94,
    /* 0x5F */ PLAYER_CSMODE_95,
    /* 0x60 */ PLAYER_CSMODE_96,
    /* 0x61 */ PLAYER_CSMODE_97,
    /* 0x62 */ PLAYER_CSMODE_98,
    /* 0x63 */ PLAYER_CSMODE_99,
    /* 0x64 */ PLAYER_CSMODE_100,
    /* 0x65 */ PLAYER_CSMODE_101,
    /* 0x66 */ PLAYER_CSMODE_102,
    /* 0x67 */ PLAYER_CSMODE_MAX
} PlayerCutsceneMode;

typedef enum PlayerCueId {
    /* 0x00 */ PLAYER_CUEID_NONE,
    /* 0x01 */ PLAYER_CUEID_1,
    /* 0x02 */ PLAYER_CUEID_2,
    /* 0x03 */ PLAYER_CUEID_3,
    /* 0x04 */ PLAYER_CUEID_4,
    /* 0x05 */ PLAYER_CUEID_5,
    /* 0x06 */ PLAYER_CUEID_6,
    /* 0x07 */ PLAYER_CUEID_7,
    /* 0x08 */ PLAYER_CUEID_8,
    /* 0x09 */ PLAYER_CUEID_9,
    /* 0x0A */ PLAYER_CUEID_10,
    /* 0x0B */ PLAYER_CUEID_11,
    /* 0x0C */ PLAYER_CUEID_12,
    /* 0x0D */ PLAYER_CUEID_13,
    /* 0x0E */ PLAYER_CUEID_14,
    /* 0x0F */ PLAYER_CUEID_15,
    /* 0x10 */ PLAYER_CUEID_16,
    /* 0x11 */ PLAYER_CUEID_17,
    /* 0x12 */ PLAYER_CUEID_18,
    /* 0x13 */ PLAYER_CUEID_19,
    /* 0x14 */ PLAYER_CUEID_20,
    /* 0x15 */ PLAYER_CUEID_21,
    /* 0x16 */ PLAYER_CUEID_22,
    /* 0x17 */ PLAYER_CUEID_23,
    /* 0x18 */ PLAYER_CUEID_24,
    /* 0x19 */ PLAYER_CUEID_25,
    /* 0x1A */ PLAYER_CUEID_26,
    /* 0x1B */ PLAYER_CUEID_27,
    /* 0x1C */ PLAYER_CUEID_28,
    /* 0x1D */ PLAYER_CUEID_29,
    /* 0x1E */ PLAYER_CUEID_30,
    /* 0x1F */ PLAYER_CUEID_31,
    /* 0x20 */ PLAYER_CUEID_32,
    /* 0x21 */ PLAYER_CUEID_33,
    /* 0x22 */ PLAYER_CUEID_34,
    /* 0x23 */ PLAYER_CUEID_35,
    /* 0x24 */ PLAYER_CUEID_36,
    /* 0x25 */ PLAYER_CUEID_37,
    /* 0x26 */ PLAYER_CUEID_38,
    /* 0x27 */ PLAYER_CUEID_39,
    /* 0x28 */ PLAYER_CUEID_40,
    /* 0x29 */ PLAYER_CUEID_41,
    /* 0x2A */ PLAYER_CUEID_42,
    /* 0x2B */ PLAYER_CUEID_43,
    /* 0x2C */ PLAYER_CUEID_44,
    /* 0x2D */ PLAYER_CUEID_45,
    /* 0x2E */ PLAYER_CUEID_46,
    /* 0x2F */ PLAYER_CUEID_47,
    /* 0x30 */ PLAYER_CUEID_48,
    /* 0x31 */ PLAYER_CUEID_49,
    /* 0x32 */ PLAYER_CUEID_50,
    /* 0x33 */ PLAYER_CUEID_51,
    /* 0x34 */ PLAYER_CUEID_52,
    /* 0x35 */ PLAYER_CUEID_53,
    /* 0x36 */ PLAYER_CUEID_54,
    /* 0x37 */ PLAYER_CUEID_55,
    /* 0x38 */ PLAYER_CUEID_56,
    /* 0x39 */ PLAYER_CUEID_57,
    /* 0x3A */ PLAYER_CUEID_58,
    /* 0x3B */ PLAYER_CUEID_59,
    /* 0x3C */ PLAYER_CUEID_60,
    /* 0x3D */ PLAYER_CUEID_61,
    /* 0x3E */ PLAYER_CUEID_62,
    /* 0x3F */ PLAYER_CUEID_63,
    /* 0x40 */ PLAYER_CUEID_64,
    /* 0x41 */ PLAYER_CUEID_65,
    /* 0x42 */ PLAYER_CUEID_66,
    /* 0x43 */ PLAYER_CUEID_67,
    /* 0x44 */ PLAYER_CUEID_68,
    /* 0x45 */ PLAYER_CUEID_69,
    /* 0x46 */ PLAYER_CUEID_70,
    /* 0x47 */ PLAYER_CUEID_71,
    /* 0x48 */ PLAYER_CUEID_72,
    /* 0x49 */ PLAYER_CUEID_73,
    /* 0x4A */ PLAYER_CUEID_74,
    /* 0x4B */ PLAYER_CUEID_75,
    /* 0x4C */ PLAYER_CUEID_76,
    /* 0x4D */ PLAYER_CUEID_77,
    /* 0x4E */ PLAYER_CUEID_MAX
} PlayerCueId;

typedef enum {
    /* 0 */ PLAYER_LEDGE_CLIMB_NONE,
    /* 1 */ PLAYER_LEDGE_CLIMB_1,
    /* 2 */ PLAYER_LEDGE_CLIMB_2,
    /* 3 */ PLAYER_LEDGE_CLIMB_3,
    /* 4 */ PLAYER_LEDGE_CLIMB_4
} PlayerLedgeClimbType;

#define LEDGE_DIST_MAX 399.96002f

typedef enum {
    /* -1 */ PLAYER_ANALOG_NONE = -1,
    /*  0 */ PLAYER_ANALOG_UP,
    /*  1 */ PLAYER_ANALOG_RIGHT,
    /*  2 */ PLAYER_ANALOG_DOWN,
    /*  3 */ PLAYER_ANALOG_LEFT,
    /*  4 */ PLAYER_ANALOG_MAX
} PlayerAnalog;

typedef struct {
    /* 0x00 */ f32 ceilingCheckHeight;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 wallCheckRadius;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ Vec3s unk_44;
    /* 0x4A */ Vec3s unk_4A[4];
    /* 0x62 */ Vec3s unk_62[4];
    /* 0x7A */ Vec3s unk_7A[2];
    /* 0x86 */ Vec3s unk_86[2];
    /* 0x92 */ u16 voiceSfxIdOffset;
    /* 0x94 */ u16 surfaceSfxIdOffset;
    /* 0x98 */ PlayerAnimationHeader* unk_98;
    /* 0x9C */ PlayerAnimationHeader* unk_9C;
    /* 0xA0 */ PlayerAnimationHeader* unk_A0;
    /* 0xA4 */ PlayerAnimationHeader* unk_A4;
    /* 0xA8 */ PlayerAnimationHeader* unk_A8;
    /* 0xAC */ PlayerAnimationHeader* unk_AC[4];
    /* 0xBC */ PlayerAnimationHeader* unk_BC[2];
    /* 0xC4 */ PlayerAnimationHeader* unk_C4[2];
    /* 0xCC */ PlayerAnimationHeader* unk_CC[2];
} PlayerAgeProperties; // size = 0xD4

typedef struct {
    /* 0x00 */ s32 active;
    /* 0x04 */ Vec3f tip;
    /* 0x10 */ Vec3f base;
} WeaponInfo; // size = 0x1C

#define PLAYER_STATE1_EXITING_SCENE (1 << 0)
#define PLAYER_STATE1_SWINGING_BOTTLE (1 << 1)
#define PLAYER_STATE1_END_HOOKSHOT_MOVE (1 << 2)
#define PLAYER_STATE1_AIMING_FPS_ITEM (1 << 3)
#define PLAYER_STATE1_Z_TARGETING_UNFRIENDLY (1 << 4)
#define PLAYER_STATE1_INPUT_DISABLED (1 << 5)
#define PLAYER_STATE1_TALKING (1 << 6)
#define PLAYER_STATE1_IN_DEATH_CUTSCENE (1 << 7)
#define PLAYER_STATE1_START_CHANGE_ITEM (1 << 8)
#define PLAYER_STATE1_READY_TO_SHOOT (1 << 9)
#define PLAYER_STATE1_GETTING_ITEM (1 << 10)
#define PLAYER_STATE1_HOLDING_ACTOR (1 << 11)
#define PLAYER_STATE1_CHARGING_SPIN_ATTACK (1 << 12)
#define PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP (1 << 13)
#define PLAYER_STATE1_CLIMBING_ONTO_LEDGE (1 << 14)
#define PLAYER_STATE1_UNUSED_Z_TARGETING_FLAG (1 << 15)
#define PLAYER_STATE1_FORCE_STRAFING (1 << 16)
#define PLAYER_STATE1_Z_TARGETING_FRIENDLY (1 << 17)
#define PLAYER_STATE1_JUMPING (1 << 18)
#define PLAYER_STATE1_FREEFALLING (1 << 19)
#define PLAYER_STATE1_IN_FIRST_PERSON_MODE (1 << 20)
#define PLAYER_STATE1_CLIMBING (1 << 21)
#define PLAYER_STATE1_HOLDING_SHIELD (1 << 22)
#define PLAYER_STATE1_RIDING_HORSE (1 << 23)
#define PLAYER_STATE1_AIMING_BOOMERANG (1 << 24)
#define PLAYER_STATE1_AWAITING_THROWN_BOOMERANG (1 << 25)
#define PLAYER_STATE1_TAKING_DAMAGE (1 << 26)
#define PLAYER_STATE1_SWIMMING (1 << 27)
#define PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE (1 << 28)
#define PLAYER_STATE1_IN_CUTSCENE (1 << 29)
#define PLAYER_STATE1_30 (1 << 30)
#define PLAYER_STATE1_FALLING_INTO_GROTTO_OR_VOID (1 << 31)

#define PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL (1 << 0)
#define PLAYER_STATE2_CAN_SPEAK_OR_CHECK (1 << 1)
#define PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL (1 << 2)
#define PLAYER_STATE2_MAKING_REACTABLE_NOISE (1 << 3)
#define PLAYER_STATE2_MOVING_PUSH_PULL_WALL (1 << 4)
#define PLAYER_STATE2_DISABLE_MOVE_ROTATION_WHILE_Z_TARGETING (1 << 5)
#define PLAYER_STATE2_ALWAYS_DISABLE_MOVE_ROTATION (1 << 6)
#define PLAYER_STATE2_RESTRAINED_BY_ENEMY (1 << 7)
#define PLAYER_STATE2_ENABLE_PUSH_PULL_CAM (1 << 8)
#define PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND (1 << 9)
#define PLAYER_STATE2_DIVING (1 << 10)
#define PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER (1 << 11)
#define PLAYER_STATE2_IDLE_WHILE_CLIMBING (1 << 12)
#define PLAYER_STATE2_USING_SWITCH_Z_TARGET (1 << 13)
#define PLAYER_STATE2_FROZEN_IN_ICE (1 << 14)
#define PLAYER_STATE2_PAUSE_MOST_UPDATING (1 << 15)
#define PLAYER_STATE2_DO_ACTION_ENTER (1 << 16) // Turns on the "Enter On A" DoAction
#define PLAYER_STATE2_RELEASING_SPIN_ATTACK (1 << 17)
#define PLAYER_STATE2_CRAWLING (1 << 18) // Crawling through a crawlspace
#define PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING (1 << 19)
#define PLAYER_STATE2_NAVI_IS_ACTIVE (1 << 20)
#define PLAYER_STATE2_NAVI_REQUESTING_TALK (1 << 21)
#define PLAYER_STATE2_CAN_DISMOUNT_HORSE (1 << 22)
#define PLAYER_STATE2_OCARINA_START_OVERRIDE (1 << 23) // Indicate that the ocarina textbox will be started externally to player. If not set, default to free play ocarina textbox
#define PLAYER_STATE2_OCARINA_START_READY (1 << 24) // If ocarina-start is overridden or the ocarina is attached to an actor, then indicate that Player is ready to start playing ocarina
#define PLAYER_STATE2_OCARINA_ON_FOR_ACTOR (1 << 25) // Ocarina is being played to an actor
#define PLAYER_STATE2_DRAW_REFLECTION (1 << 26)
#define PLAYER_STATE2_OCARINA_ON (1 << 27) // The Action: `PlayOcarina` is being used
#define PLAYER_STATE2_IDLING (1 << 28)
#define PLAYER_STATE2_DISABLE_DRAW (1 << 29)
#define PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK (1 << 30)
#define PLAYER_STATE2_FORCE_VOID_OUT (1 << 31)

#define PLAYER_STATE3_IGNORE_CEILING_FLOOR_AND_WATER (1 << 0)
#define PLAYER_STATE3_MIDAIR (1 << 1)
#define PLAYER_STATE3_PAUSE_ACTION (1 << 2)
#define PLAYER_STATE3_ENDING_MELEE_ATTACK (1 << 3)
#define PLAYER_STATE3_CHECKING_FLOOR_AND_WATER_COLLISION (1 << 4)
#define PLAYER_STATE3_OCARINA_AFTER_TEXTBOX (1 << 5) // Makes player take out the ocarina after closing a textbox from either 1) talking to an actor 2) getItem textbox
#define PLAYER_STATE3_RESTORE_NAYRUS_LOVE (1 << 6) // Set by ocarina effects actors when destroyed to signal Nayru's Love may be restored (see `ACTOROVL_ALLOC_ABSOLUTE`)
#define PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH (1 << 7)

typedef void (*PlayerActionFunc)(struct Player*, struct PlayState*);
typedef s32 (*PlayerUpperActionFunc)(struct Player*, struct PlayState*);
typedef void (*PlayerCsIntoActionFunc)(struct PlayState*, struct Player*);

typedef struct Player {
    /* 0x0000 */ Actor      actor;
    /* 0x014C */ s8         currentTunic; // current tunic from `PlayerTunic`
    /* 0x014D */ s8         currentSwordItemId;
    /* 0x014E */ s8         currentShield; // current shield from `PlayerShield`
    /* 0x014F */ s8         currentBoots; // current boots from `PlayerBoots`
    /* 0x0150 */ s8         heldItemButton; // Button index for the item currently used
    /* 0x0151 */ s8         heldItemAction; // Item action for the item currently used
    /* 0x0152 */ u8         heldItemId; // Item id for the item currently used
    /* 0x0153 */ s8         prevBoots; // previous boots from `PlayerBoots`
    /* 0x0154 */ s8         itemAction; // the difference between this and heldItemAction is unclear
    /* 0x0155 */ char       unk_155[0x003];
    /* 0x0158 */ u8         modelGroup;
    /* 0x0159 */ u8         nextModelGroup;
    /* 0x015A */ s8         unk_15A;
    /* 0x015B */ u8         modelAnimType;
    /* 0x015C */ u8         leftHandType;
    /* 0x015D */ u8         rightHandType;
    /* 0x015E */ u8         sheathType;
    /* 0x015F */ u8         currentMask; // current mask equipped from `PlayerMask`
    /* 0x0160 */ Gfx**      rightHandDLists;
    /* 0x0164 */ Gfx**      leftHandDLists;
    /* 0x0168 */ Gfx**      sheathDLists;
    /* 0x016C */ Gfx**      waistDLists;
    /* 0x0170 */ u8         giObjectLoading;
    /* 0x0174 */ DmaRequest giObjectDmaRequest;
    /* 0x0194 */ OSMesgQueue giObjectLoadQueue;
    /* 0x01AC */ OSMesg     giObjectLoadMsg;
    /* 0x01B0 */ void*      giObjectSegment; // also used for title card textures
    /* 0x01B4 */ SkelAnime  skelAnime;
    /* 0x01F8 */ Vec3s      jointTable[PLAYER_LIMB_BUF_COUNT];
    /* 0x0288 */ Vec3s      morphTable[PLAYER_LIMB_BUF_COUNT];
    /* 0x0318 */ Vec3s      blendTable[PLAYER_LIMB_BUF_COUNT];
    /* 0x03A8 */ s16        unk_3A8[2];
    /* 0x03AC */ Actor*     heldActor;
    /* 0x03B0 */ Vec3f      leftHandPos;
    /* 0x03BC */ Vec3s      unk_3BC;
    /* 0x03C4 */ Actor*     unk_3C4;
    /* 0x03C8 */ Vec3f      unk_3C8;
    /* 0x03D4 */ char       unk_3D4[0x058];
    /* 0x042C */ s8         doorType;
    /* 0x042D */ s8         doorDirection;
    /* 0x042E */ s16        doorTimer;
    /* 0x0430 */ Actor*     doorActor;
    /* 0x0434 */ s8         getItemId;
    /* 0x0436 */ u16        getItemDirection;
    /* 0x0438 */ Actor*     interactRangeActor;
    /* 0x043C */ s8         mountSide;
    /* 0x043D */ char       unk_43D[0x003];
    /* 0x0440 */ Actor*     rideActor;
    /* 0x0444 */ u8         csMode;
    /* 0x0445 */ u8         prevCsMode;
    /* 0x0446 */ u8         cueId;
    /* 0x0447 */ u8         unk_447;
    /* 0x0448 */ Actor*     unk_448;
    /* 0x044C */ char       unk_44C[0x004];
    /* 0x0450 */ Vec3f      unk_450;
    /* 0x045C */ Vec3f      unk_45C;
    /* 0x0468 */ char       unk_468[0x002];
    /* 0x046A */ s16        doorBgCamIndex;
    /* 0x046C */ s16        subCamId;
    /* 0x046E */ char       unk_46E[0x02A];
    /* 0x0498 */ ColliderCylinder cylinder;
    /* 0x04E4 */ ColliderQuad meleeWeaponQuads[2];
    /* 0x05E4 */ ColliderQuad shieldQuad;
    /* 0x0664 */ Actor*     targetedActor;
    /* 0x0668 */ char       unk_668[0x004];
    /* 0x066C */ s32        zTargetSwitchTimer;
    /* 0x0670 */ s32        meleeWeaponEffectIndex;
    /* 0x0674 */ PlayerActionFunc actionFunc;
    /* 0x0678 */ PlayerAgeProperties* ageProperties;
    /* 0x067C */ u32        stateFlags1;
    /* 0x0680 */ u32        stateFlags2;
    /* 0x0684 */ Actor*     forcedTargetedActor;
    /* 0x0688 */ Actor*     boomerangActor;
    /* 0x068C */ Actor*     naviActor;
    /* 0x0690 */ s16        naviTextId;
    /* 0x0692 */ u8         stateFlags3;
    /* 0x0693 */ s8         exchangeItemId;
    /* 0x0694 */ Actor*     targetActor;
    /* 0x0698 */ f32        targetActorDistance;
    /* 0x069C */ char       unk_69C[0x004];
    /* 0x06A0 */ f32        secretRumbleCharge;
    /* 0x06A4 */ f32        closestSecretDistSq;
    /* 0x06A8 */ Actor*     ocarinaActor;
    /* 0x06AC */ s8         unk_6AC;
    /* 0x06AD */ u8         attentionMode;
    /* 0x06AE */ u16        rotOverrideFlags;
    /* 0x06B0 */ s16        unk_6B0;
    /* 0x06B2 */ char       unk_6B4[0x004];
    /* 0x06B6 */ Vec3s      headLimbRot;
    /* 0x06BC */ Vec3s      upperLimbRot;
    /* 0x06C2 */ s16        unk_6C2;
    /* 0x06C4 */ f32        shapeOffsetY;
    /* 0x06C8 */ SkelAnime  upperSkelAnime;
    /* 0x070C */ Vec3s      upperJointTable[PLAYER_LIMB_BUF_COUNT];
    /* 0x079C */ Vec3s      upperMorphTable[PLAYER_LIMB_BUF_COUNT];
    /* 0x082C */ PlayerUpperActionFunc upperActionFunc;
    /* 0x0830 */ f32        upperInterpWeight;
    /* 0x0834 */ s16        firstPersonItemTimer;
    /* 0x0836 */ s8         unk_836;
    /* 0x0837 */ u8         putAwayTimer;
    /* 0x0838 */ f32        speedXZ; // Controls horizontal speed, used for `actor.speed`. Current or target value depending on context.
    /* 0x083C */ s16        yaw; // General yaw value, used both for world and shape rotation. Current or target value depending on context.
    /* 0x083E */ s16        zTargetYaw; // yaw relating to Z targeting/"parallel" mode
    /* 0x0840 */ u16        underwaterTimer;
    /* 0x0842 */ s8         meleeWeaponAnimation;
    /* 0x0843 */ s8         meleeWeaponState;
    /* 0x0844 */ s8         unk_844;
    /* 0x0845 */ u8         slashCounter;
    /* 0x0846 */ u8         inputFrameCounter;
    /* 0x0847 */ s8         analogStickDirection128Parts[4];
    /* 0x084B */ s8         analogStickDirection4Parts[4];
    /* 0x084F */ union {
                    s8 actionVar8;
                    s8 miniCutsceneMoveVar8;
                    s8 spawnFromBlueWarpVar8;
                    s8 frozenInIceVar8;
                    s8 shieldAimCrouchedVar8;
                    s8 shieldDeflectAttackVar8;
                    s8 midairVar8;
                    s8 plantMagicBeansVar8; // Action: PlantMagicBeans
                };
    /* 0x0850 */ union {
                    s16 actionVar16;
                    s16 miniCutsceneMoveVar16; // Action: MiniCutsceneMove
                    s16 spawnFromBlueWarpVar16; // Action: SpawnFromBlueWarp
                    s16 startCutsceneDelayedVar16; // Action: StartCutscene
                    s16 midairVar16; // Action: Midair
                    s16 rollVar16; // Action: Roll
                    s16 chargeSpinAttackVar16; // Action: ChargeSpinAttack
                    s16 jumpToLedgeVar16; // Action: JumpToLedge
                    s16 IdleZTargetEnemyVar16; // Action: IdleZTargetEnemy
                    s16 knockbackFlyVar16; // Action: KnockbackFly
                    s16 knockbackDownVar16; // Action: KnockbackDown
                    s16 dieVar16; // Action: Die & SwimDrown
                    s16 playOcarinaVar16; // Action: PlayOcarina
                    s16 getItemVar16; // Action: GetItem
                    s16 spawnFromAgeSwapVar16; // Action: SpawnFromAgeSwap
                    s16 rideHorseVar16; // Action: RideHorse
                    s16 swimSpawnVar16; // Action: SwimSpawn
                    s16 idleVar16; // Action: Idle
                    s16 plantMagicBeansVar16; // Action: PlantMagicBeans
                    s16 aimFirstPersonVar16; // Action: AimFirstPerson
                    s16 pushVar16; // Action: Push
                    s16 shieldAimCrouchedVar16; // Action: ShieldAimCrouched
                };
    /* 0x0854 */ f32        unk_854;
    /* 0x0858 */ f32        spinAttackTimer;
    /* 0x085C */ f32        dekuStickLength; // stick length among other things
    /* 0x0860 */ s16        stickFlameTimer; // stick flame timer among other things
    /* 0x0862 */ s8         unk_862; // get item draw ID + 1
    /* 0x0864 */ f32        unk_864;
    /* 0x0868 */ f32        unk_868;
    /* 0x086C */ f32        unk_86C;
    /* 0x0870 */ f32        unk_870;
    /* 0x0874 */ f32        unk_874;
    /* 0x0878 */ f32        unk_878;
    /* 0x087C */ s16        unk_87C;
    /* 0x087E */ s16        unk_87E;
    /* 0x0880 */ f32        speedLimit;
    /* 0x0884 */ f32        yDistToLedge; // y distance to ground above an interact wall. LEDGE_DIST_MAX if no ground if found
    /* 0x0888 */ f32        distToInteractWall; // xyz distance to the interact wall
    /* 0x088C */ u8         ledgeClimbType;
    /* 0x088D */ u8         ledgeClimbDelayTimer;
    /* 0x088E */ u8         unk_88E;
    /* 0x088F */ u8         damageFlashTimer;
    /* 0x0890 */ u8         unk_890;
    /* 0x0891 */ u8         shockTimer;
    /* 0x0892 */ u8         unk_892;
    /* 0x0893 */ u8         hoverBootsTimer;
    /* 0x0894 */ s16        fallStartHeight; // last truncated Y position before falling
    /* 0x0896 */ s16        fallDistance; // truncated Y distance the player has fallen so far (positive is down)
    /* 0x0898 */ s16        floorPitch; // angle of the floor slope in the direction of current world yaw (positive for ascending slope)
    /* 0x089A */ s16        floorPitchAlt; // the calculation for this value is bugged and doesn't represent anything meaningful
    /* 0x089C */ s16        unk_89C;
    /* 0x089E */ u16        floorSfxOffset;
    /* 0x08A0 */ u8         unk_8A0;
    /* 0x08A1 */ u8         unk_8A1;
    /* 0x08A2 */ s16        unk_8A2;
    /* 0x08A4 */ f32        unk_8A4;
    /* 0x08A8 */ f32        unk_8A8;
    /* 0x08AC */ f32        pushedSpeed; // Pushing player, examples include water currents, floor conveyors, climbing sloped surfaces
    /* 0x08B0 */ s16        pushedYaw; // Yaw direction of player being pushed
    /* 0x08B4 */ WeaponInfo meleeWeaponInfo[3];
    /* 0x0908 */ Vec3f      bodyPartsPos[PLAYER_BODYPART_MAX];
    /* 0x09E0 */ MtxF       mf_9E0;
    /* 0x0A20 */ MtxF       shieldMf;
    /* 0x0A60 */ u8         isBurning;
    /* 0x0A61 */ u8         flameTimers[PLAYER_BODYPART_MAX]; // one flame per body part
    /* 0x0A73 */ u8         unk_A73;
    /* 0x0A74 */ PlayerCsIntoActionFunc csIntoActionFunc;
    /* 0x0A78 */ s8         invincibilityTimer; // prevents damage when nonzero (positive = visible, counts towards zero each frame)
    /* 0x0A79 */ u8         floorTypeTimer;
    /* 0x0A7A */ u8         floorProperty;
    /* 0x0A7B */ u8         prevFloorType;
    /* 0x0A7C */ f32        analogStickDistance;
    /* 0x0A80 */ s16        analogStickAngle;
    /* 0x0A82 */ u16        prevFloorSfxOffset;
    /* 0x0A84 */ s16        sceneExitPosY;
    /* 0x0A86 */ s8         unk_A86;
    /* 0x0A87 */ u8         unk_A87;
    /* 0x0A88 */ Vec3f      unk_A88; // previous body part 0 position
} Player; // size = 0xA94

#endif
