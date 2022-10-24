#ifndef Z_EN_OKARINA_TAG_H
#define Z_EN_OKARINA_TAG_H

#include "ultra64.h"
#include "global.h"

struct EnOkarinaTag;

typedef void (*EnOkarinaTagActionFunc)(struct EnOkarinaTag*, PlayState*);

typedef enum {
    /* 0 */ OCARINASPOT_TYPE_CHECK_ZORA_WATERFALL = 1,
    /* 2 */ OCARINASPOT_TYPE_CHECK_WINDMILL, // Child Only
    /* 4 */ OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME = 4,
    /* 5 */ OCARINASPOT_TYPE_LEARN_SUNS,
    /* 6 */ OCARINASPOT_TYPE_CHECK_ROYAL_TOMB,
    /* 7 */ OCARINASPOT_TYPE_GENERIC
} OcarinaSpotType;

typedef struct EnOkarinaTag {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnOkarinaTagActionFunc actionFunc;
    /* 0x0150 */ s16 type;
    /* 0x0152 */ s16 ocarinaSong;
    /* 0x0154 */ s16 switchFlag;
    /* 0x0156 */ char unk_156[0x2];
    /* 0x0158 */ s16 anySongAllowed;
    /* 0x015A */ s16 timer;
    /* 0x015C */ f32 interactRange; // xzDistance added onto 90.0f
} EnOkarinaTag; // size = 0x0160

#endif
