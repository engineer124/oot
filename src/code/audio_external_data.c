#include "ultra64.h"
#include "global.h"

// sSfxRequests ring buffer endpoints. read index <= write index, wrapping around mod 256.
u8 gSfxRequestWriteIndex = 0;
u8 gSfxRequestReadIndex = 0;

/**
 * Array of pointers to arrays of SfxBankEntry of sizes: 9, 12, 22, 20, 8, 3, 5
 *
 * 0 : Player Bank          size 9
 * 1 : Item Bank            size 12
 * 2 : Environment Bank     size 22
 * 3 : Enemy Bank           size 20
 * 4 : System Bank          size 8
 * 5 : Ocarina Bank         size 3
 * 6 : Voice Bank           size 5
 */
SfxBankEntry* gSfxBanks[7] = {
    gSfxPlayerBank, gSfxItemBank, gSfxEnvironmentBank, gSfxEnemyBank, gSfxSystemBank, gSfxOcarinaBank, gSfxVoiceBank,
};

u8 gSfxBankSizes[ARRAY_COUNT(gSfxBanks)] = {
    ARRAY_COUNT(gSfxPlayerBank), ARRAY_COUNT(gSfxItemBank),   ARRAY_COUNT(gSfxEnvironmentBank),
    ARRAY_COUNT(gSfxEnemyBank),  ARRAY_COUNT(gSfxSystemBank), ARRAY_COUNT(gSfxOcarinaBank),
    ARRAY_COUNT(gSfxVoiceBank),
};

u8 gSfxChannelLayout = 0;

u16 gSfxChannelLowVolumeFlag = 0;

// The center of the screen in projected coordinates.
// Gives the impression that the sfx has no specific location
Vec3f gSfxDefaultPos = { 0.0f, 0.0f, 0.0f };

// Reused as either frequency or volume multiplicative scaling factor
// Does not alter or change frequency or volume
f32 gSfxDefaultFreqAndVolScale = 1.0f;

s32 D_801333E4 = 0; // unused

// Adds no reverb to the existing reverb
s8 gSfxDefaultReverb = 0;

s32 D_801333EC = 0; // unused

u8 gAudioDebugPrintSfxRequest = 0;

u8 gAudioDebugSfxSwapOff = 0;

u8 gAudioDebugSfxSwapInitialized = false;

s32 D_801333FC = 0; // unused

u8 gSeqCmdWritePos = 0;
u8 gSeqCmdReadPos = 0;
u8 gStartSeqDisabled = false;
u8 gAudioDebugPrintSeqCmd = true;

u8 gSoundModeList[] = {
    SOUNDMODE_STEREO,
    SOUNDMODE_HEADSET,
    SOUNDMODE_SURROUND,
    SOUNDMODE_MONO,
};

u8 gAudioSpecId = 0;

u8 gAudioHeapResetState = AUDIO_HEAP_RESET_STATE_NONE;
