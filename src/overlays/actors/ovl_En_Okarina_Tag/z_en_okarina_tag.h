#ifndef Z_EN_OKARINA_TAG_H
#define Z_EN_OKARINA_TAG_H

#include "ultra64.h"
#include "global.h"

struct EnOkarinaTag;

typedef void (*EnOkarinaTagActionFunc)(struct EnOkarinaTag*, PlayState*);

#define OCARINASPOT_GET_TYPE(thisx) (((thisx)->params >> 10) & 0x3F)
#define OCARINASPOT_GET_SONG(thisx) (((thisx)->params >> 6) & 0xF)
#define OCARINASPOT_GET_SWITCHFLAGS(thisx) (((thisx)->params & 0x3F))

typedef enum {
    /* 1 */ OCARINASPOT_TYPE_CHECK_ZORA_WATERFALL = 1, // Open Zora's waterfall
    /* 2 */ OCARINASPOT_TYPE_CHECK_WINDMILL, // Trigger the well to open. Child only
    /* 4 */ OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME = 4, // Door in the temple of time
    /* 5 */ OCARINASPOT_TYPE_LEARN_SUNS, // Engraving on the wall to teach suns song
    /* 6 */ OCARINASPOT_TYPE_CHECK_ROYAL_TOMB, // Entrance to the royal tomb
    /* 7 */ OCARINASPOT_TYPE_SWITCHFLAG // Set a switch flag
} OcarinaSpotType;

typedef enum {
    /* 0x0 */ OCARINASPOT_SONG_SARIA,     // OCARINA_ACTION_CHECK_SARIA
    /* 0x1 */ OCARINASPOT_SONG_EPONA,     // OCARINA_ACTION_CHECK_EPONA
    /* 0x2 */ OCARINASPOT_SONG_LULLABY,   // OCARINA_ACTION_CHECK_LULLABY
    /* 0x3 */ OCARINASPOT_SONG_SUNS,      // OCARINA_ACTION_CHECK_SUNS
    /* 0x4 */ OCARINASPOT_SONG_TIME,      // OCARINA_ACTION_CHECK_TIME
    /* 0x5 */ OCARINASPOT_SONG_STORMS,    // OCARINA_ACTION_CHECK_STORMS
    /* 0x6 */ OCARINASPOT_SONG_SCARECROW, // OCARINA_ACTION_SCARECROW_SPAWN_RECORDING
    /* 0xF */ OCARINASPOT_SONG_ANY = 0xF
} OcarinaSpotSong;

typedef struct EnOkarinaTag {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnOkarinaTagActionFunc actionFunc;
    /* 0x0150 */ s16 type;
    /* 0x0152 */ s16 ocarinaSong;
    /* 0x0154 */ s16 switchFlag;
    /* 0x0156 */ char unk_156[0x2];
    /* 0x0158 */ s16 anySongAllowed;
    /* 0x015A */ s16 debugDisplayTimer;
    /* 0x015C */ f32 xzRange; // xzDistance added onto 90.0f
} EnOkarinaTag; // size = 0x0160

#endif
