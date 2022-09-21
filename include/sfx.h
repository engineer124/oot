#ifndef SFX_H
#define SFX_H

typedef enum {
    /* 0 */ BANK_PLAYER,
    /* 1 */ BANK_ITEM,
    /* 2 */ BANK_ENV,
    /* 3 */ BANK_ENEMY,
    /* 4 */ BANK_SYSTEM,
    /* 5 */ BANK_OCARINA,
    /* 6 */ BANK_VOICE
} SfxBankType;

typedef enum {
    /* 0 */ SFX_STATE_EMPTY,
    /* 1 */ SFX_STATE_QUEUED,
    /* 2 */ SFX_STATE_READY,
    /* 3 */ SFX_STATE_PLAYING_REFRESH,
    /* 4 */ SFX_STATE_PLAYING,
    /* 5 */ SFX_STATE_PLAYING_ONE_FRAME
} SfxState;

typedef struct {
    /* 0x00 */ f32* posX;
    /* 0x04 */ f32* posY;
    /* 0x08 */ f32* posZ;
    /* 0x0C */ u8   token;
    /* 0x10 */ f32* freqScale;
    /* 0x14 */ f32* vol;
    /* 0x18 */ s8*  reverbAdd;
    /* 0x1C */ f32  dist;
    /* 0x20 */ u32  priority; // lower is more prioritized
    /* 0x24 */ u8   sfxImportance;
    /* 0x26 */ u16  sfxParams;
    /* 0x28 */ u16  sfxId;
    /* 0x2A */ u8   state; // uses SfxState enum
    /* 0x2B */ u8   freshness;
    /* 0x2C */ u8   prev;
    /* 0x2D */ u8   next;
    /* 0x2E */ u8   channelIndex;
    /* 0x2F */ u8   randFreq;
} SfxBankEntry; // size = 0x30

/*
 * SfxId Bitbacking:
 *
 * & 0x1FF    0000000111111111    index
 * & 0x200    0000001000000000    unused bit
 * & 0x400    0000010000000000    unused bit
 * & 0x800    0000100000000000    SFX_FLAG
 * & 0xC00    0000110000000000    Flag Mask
 * & 0xF000   1111000000000000    bankId
 */

#define DEFINE_SFX(enum, _1, _2, _3, _4) enum,

typedef enum {
    SFX_ID_NONE,
    SFX_ID_PLAYER_BASE = 0x7FF,
    #include "tables/sfx/playerbank_table.h"
    SFX_ID_ITEM_BASE = 0x17FF,
    #include "tables/sfx/itembank_table.h"
    SFX_ID_ENVIRONMENT_BASE = 0x27FF,
    #include "tables/sfx/environmentbank_table.h"
    SFX_ID_ENEMY_BASE = 0x37FF,
    #include "tables/sfx/enemybank_table.h"
    SFX_ID_SYSTEM_BASE = 0x47FF,
    #include "tables/sfx/systembank_table.h"
    SFX_ID_OCARINA_BASE = 0x57FF,
    #include "tables/sfx/ocarinabank_table.h"
    SFX_ID_VOICE_BASE = 0x67FF,
    #include "tables/sfx/voicebank_table.h"
    SFX_ID_MAX
} SfxId;

#undef DEFINE_SFX

#define SFX_BANK_SHIFT(sfxId)   (((sfxId) >> 12) & 0xFF)

#define SFX_BANK_MASK(sfxId)    ((sfxId) & 0xF000)

#define SFX_INDEX(sfxId)    ((sfxId) & 0x01FF)
#define SFX_BANK(sfxId)     SFX_BANK_SHIFT(SFX_BANK_MASK(sfxId))

/**
 * With `SFX_FLAG` on, play the entire sfx audio clip.
 * Requesting the sfx while playing will restart the sfx from the beginning.
 * i.e. sfx only needs to be requested once.
 * 
 * With `SFX_FLAG` off, play the sfx for only one frame.
 * Requesting the sfx while playing will allow the sfx to continue to the next frame.
 * i.e. sfx needs to be requested every frame.
 * 
 * By default, `SFX_FLAG` is on.
 * Use `- SFX_FLAG` to turn `SFX_FLAG` off.
 */
#define SFX_FLAG_MASK 0xC00
#define SFX_FLAG 0x800

typedef struct {
    /* 0x0 */ u32 priority; // lower is more prioritized
    /* 0x4 */ u8 entryIndex;
} ActiveSfx; // size = 0x8

// SfxParams bit-packing

// Slows the decay of volume with distance (a 2-bit number ranging from 0-3)
#define SFX_PARAM_DIST_RANGE_SHIFT 0
#define SFX_PARAM_DIST_RANGE_MASK (3 << SFX_PARAM_DIST_RANGE_SHIFT)

// Force the sfx to reset from the beginning when requested again
#define SFX_FLAG_FORCE_RESET (1 << 2)

// Lower SEQ_PLAYER_BGM_MAIN and SEQ_PLAYER_BGM_SUB while the sfx is playing
#define SFX_FLAG_LOWER_VOLUME_BGM (1 << 3)

// Sfx priority is not raised with distance (making it more likely to be ejected)
#define SFX_FLAG_PRIORITY_NO_DIST (1 << 4)

// If a new sfx is requested at both the same position with the same importance,
// Block that new sfx from replacing the current sfx
// Note: Only 1 sfx can be played at a specific position at once
#define SFX_FLAG_BLOCK_EQUAL_IMPORTANCE (1 << 5)

// Applies increasingly random offsets to frequency (a 2-bit number ranging from 0-3)
#define SFX_PARAM_RAND_FREQ_RAISE_SHIFT 6
#define SFX_PARAM_RAND_FREQ_RAISE_MASK (3 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT)

// Use lowpass filter on surround sound
#define SFX_FLAG_SURROUND_LOWPASS_FILTER (1 << 9)

#define SFX_FLAG_BEHIND_SCREEN_Z_INDEX_SHIFT 10
#define SFX_FLAG_BEHIND_SCREEN_Z_INDEX (1 << SFX_FLAG_BEHIND_SCREEN_Z_INDEX_SHIFT)

// Randomly scale base frequency each frame through multiplicative offset
#define SFX_PARAM_RAND_FREQ_SCALE (1 << 11)

// Sfx reverb is not raised with distance
#define SFX_FLAG_REVERB_NO_DIST (1 << 12)

// Sfx volume is not lowered with distance
#define SFX_FLAG_VOLUME_NO_DIST (1 << 13)

// Randomly lower base frequency each frame through additive offset
#define SFX_PARAM_RAND_FREQ_LOWER (1 << 14)

// Sfx frequency is not raised with distance
#define SFX_FLAG_FREQ_NO_DIST (1 << 15)

typedef struct {
    /* 0x0 */ u8 importance;
    /* 0x2 */ u16 params;
} SfxParams; // size = 0x4

#endif
