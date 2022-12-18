/**
 * @file code_800F9280.c
 *
 * This file implements a set of high-level audio sequence commands that allow sequences to be modified in real-time.
 * These commands are intended to interface external to the audio library.
 *
 * These commands are generated using `AudioSeq_QueueSeqCmd`, and a user-friendly interface for this function
 * can be found in `seqcmd.h`
 *
 * These commands change sequences by generating internal audio commands `AudioThread_QueueCmd` which allows these
 * sequence requests to be passed onto the audio thread. It is worth noting all functions in this file are
 * called from the graph thread.
 *
 * These commands are not to be confused with the sequence instructions used by the sequences themselves
 * which are a midi-based scripting language.
 *
 * Nor are these commands to be confused with the internal audio commands used to transfer requests from
 * the graph thread to the audio thread.
 */
#include "ultra64.h"
#include "global.h"
#include "ultra64/abi.h"

// Direct audio command (skips the queueing system)
#define SEQCMD_SET_PLAYER_VOLUME_NOW(seqPlayerIndex, duration, volume)                          \
    AudioSeq_ProcessSeqCmd((SEQCMD_OP_SET_PLAYER_VOLUME << 28) | ((u8)(seqPlayerIndex) << 24) | \
                           ((u8)(duration) << 16) | ((u8)((volume)*127.0f)));

typedef struct {
    /* 0x0 */ u8 seqId;
    /* 0x1 */ u8 priority; // higher values have higher priority
} SequenceRequest;         // size = 0x2

SequenceRequest sSeqRequests[4][5];
u8 sNumSeqRequests[4];
u32 sAudioSeqCmds[0x100];
SequenceController gSeqController[4];

void AudioSeq_StartSequence(u8 seqPlayerIndex, u8 seqId, u8 seqArgs, u16 fadeInDuration) {
    u8 channelIndex;
    u16 skipTicks;
    s32 pad;

    if (!gStartSeqDisabled || (seqPlayerIndex == SEQ_PLAYER_SFX)) {
        seqArgs &= 0x7F;
        if (seqArgs == 0x7F) {
            // `fadeInDuration` is interpreted as seconds
            skipTicks = (fadeInDuration >> 3) * 60 * gAudioCtx.audioBufParams.updatesPerFrame;
            AUDIOCMD_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS((u32)seqPlayerIndex, (u32)seqId, skipTicks);
        } else {
            // `fadeInDuration` is interpreted as (1/30th) of a second
            AUDIOCMD_GLOBAL_INIT_SEQPLAYER((u32)seqPlayerIndex, (u32)seqId,
                                           (fadeInDuration * (u16)gAudioCtx.audioBufParams.updatesPerFrame) / 4);
        }

        gSeqController[seqPlayerIndex].seqId = seqId | (seqArgs << 8);
        gSeqController[seqPlayerIndex].prevSeqId = seqId | (seqArgs << 8);

        if (gSeqController[seqPlayerIndex].volCur != 1.0f) {
            AUDIOCMD_SEQPLAYER_FADE_VOLUME_SCALE((u32)seqPlayerIndex, gSeqController[seqPlayerIndex].volCur);
        }

        gSeqController[seqPlayerIndex].tempoTimer = 0;
        gSeqController[seqPlayerIndex].tempoOriginal = 0;
        gSeqController[seqPlayerIndex].tempoCmd = 0;

        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            gSeqController[seqPlayerIndex].channelController[channelIndex].volCur = 1.0f;
            gSeqController[seqPlayerIndex].channelController[channelIndex].volTimer = 0;
            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleCur = 1.0f;
            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTimer = 0;
        }

        gSeqController[seqPlayerIndex].freqScaleChannelFlags = 0;
        gSeqController[seqPlayerIndex].volChannelFlags = 0;
    }
}

void AudioSeq_StopSequence(u8 seqPlayerIndex, u16 fadeOutDuration) {
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(seqPlayerIndex,
                                      (fadeOutDuration * (u16)gAudioCtx.audioBufParams.updatesPerFrame) / 4);
    gSeqController[seqPlayerIndex].seqId = NA_BGM_DISABLED;
}

void AudioSeq_ProcessSeqCmd(u32 cmd) {
    s32 priority;
    s32 channelMaskEnable;
    u16 channelMaskDisable;
    u16 fadeTimer;
    u16 val;
    u8 oldSpec;
    u8 spec;
    u8 op;
    u8 subOp;
    u8 seqPlayerIndex;
    u8 seqId;
    u8 seqArgs;
    u8 found;
    u8 ioPort;
    u8 duration;
    u8 channelIndex;
    u8 i;
    f32 freqScaleTarget;
    s32 pad;

    if (gAudioDebugPrintSeqCmd && (cmd & SEQCMD_OP_MASK) != (SEQCMD_OP_SET_PLAYER_IO << 28)) {
        AudioDebug_ScrPrt("SEQ H", (cmd >> 16) & 0xFFFF);
        AudioDebug_ScrPrt("    L", cmd & 0xFFFF);
    }

    op = cmd >> 28;
    seqPlayerIndex = (cmd & 0xF000000) >> 24;

    switch (op) {
        case SEQCMD_OP_PLAY_SEQUENCE:
            // Play a new sequence
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            // `fadeTimer` is only shifted 13 bits instead of 16 bits.
            // `fadeTimer` continues to be scaled in `AudioSeq_StartSequence`
            fadeTimer = (cmd & 0xFF0000) >> 13;
            if (!gSeqController[seqPlayerIndex].isWaitingForFonts && (seqArgs < 0x80)) {
                AudioSeq_StartSequence(seqPlayerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQCMD_OP_STOP_SEQUENCE:
            // Stop a sequence and disable the sequence player
            fadeTimer = (cmd & 0xFF0000) >> 13;
            AudioSeq_StopSequence(seqPlayerIndex, fadeTimer);
            break;

        case SEQCMD_OP_QUEUE_SEQUENCE:
            // Queue a sequence into `sSeqRequests`
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            fadeTimer = (cmd & 0xFF0000) >> 13;
            priority = seqArgs;

            // Checks if the requested sequence is first in the list of requests
            // If it is already queued and first in the list, then play the sequence immediately
            for (i = 0; i < sNumSeqRequests[seqPlayerIndex]; i++) {
                if (sSeqRequests[seqPlayerIndex][i].seqId == seqId) {
                    if (i == 0) {
                        AudioSeq_StartSequence(seqPlayerIndex, seqId, seqArgs, fadeTimer);
                    }
                    return;
                }
            }

            // Searches the sequence requests for the first request that does not have a higher priority
            // than the current incoming request
            found = sNumSeqRequests[seqPlayerIndex];
            for (i = 0; i < sNumSeqRequests[seqPlayerIndex]; i++) {
                if (priority >= sSeqRequests[seqPlayerIndex][i].priority) {
                    found = i;
                    i = sNumSeqRequests[seqPlayerIndex]; // "break;"
                }
            }

            // Check if the queue is full
            if (sNumSeqRequests[seqPlayerIndex] < ARRAY_COUNT(sSeqRequests[seqPlayerIndex])) {
                sNumSeqRequests[seqPlayerIndex]++;
            }

            for (i = sNumSeqRequests[seqPlayerIndex] - 1; i != found; i--) {
                // Move all requests of lower priority backwards 1 place in the queue
                // If the queue is full, overwrite the entry with the lowest priority
                sSeqRequests[seqPlayerIndex][i].priority = sSeqRequests[seqPlayerIndex][i - 1].priority;
                sSeqRequests[seqPlayerIndex][i].seqId = sSeqRequests[seqPlayerIndex][i - 1].seqId;
            }

            // Fill the newly freed space in the queue with the new request
            sSeqRequests[seqPlayerIndex][found].priority = seqArgs;
            sSeqRequests[seqPlayerIndex][found].seqId = seqId;

            // The sequence is first in queue, so start playing.
            if (found == 0) {
                AudioSeq_StartSequence(seqPlayerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQCMD_OP_UNQUEUE_SEQUENCE:
            // Unqueue sequence
            fadeTimer = (cmd & 0xFF0000) >> 13;

            found = sNumSeqRequests[seqPlayerIndex];
            for (i = 0; i < sNumSeqRequests[seqPlayerIndex]; i++) {
                seqId = cmd & 0xFF;
                if (sSeqRequests[seqPlayerIndex][i].seqId == seqId) {
                    found = i;
                    i = sNumSeqRequests[seqPlayerIndex]; // "break;"
                }
            }

            if (found != sNumSeqRequests[seqPlayerIndex]) {
                // Move all requests of lower priority forward 1 place in the queue
                for (i = found; i < sNumSeqRequests[seqPlayerIndex] - 1; i++) {
                    sSeqRequests[seqPlayerIndex][i].priority = sSeqRequests[seqPlayerIndex][i + 1].priority;
                    sSeqRequests[seqPlayerIndex][i].seqId = sSeqRequests[seqPlayerIndex][i + 1].seqId;
                }
                sNumSeqRequests[seqPlayerIndex]--;
            }

            // If the sequence was first in queue (it is currently playing),
            // Then stop the sequence and play the next sequence in the queue.
            if (found == 0) {
                AudioSeq_StopSequence(seqPlayerIndex, fadeTimer);
                if (sNumSeqRequests[seqPlayerIndex] != 0) {
                    AudioSeq_StartSequence(seqPlayerIndex, sSeqRequests[seqPlayerIndex][0].seqId,
                                           sSeqRequests[seqPlayerIndex][0].priority, fadeTimer);
                }
            }
            break;

        case SEQCMD_OP_SET_PLAYER_VOLUME:
            // Transition volume to a target volume for an entire player
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            gSeqController[seqPlayerIndex].volTarget = (f32)val / 127.0f;
            if (gSeqController[seqPlayerIndex].volCur != gSeqController[seqPlayerIndex].volTarget) {
                gSeqController[seqPlayerIndex].volStep =
                    (gSeqController[seqPlayerIndex].volCur - gSeqController[seqPlayerIndex].volTarget) / (f32)duration;
                gSeqController[seqPlayerIndex].volTimer = duration;
            }
            break;

        case SEQCMD_OP_SET_PLAYER_FREQ:
            // Transition freq scale to a target freq for all channels
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 1000
            freqScaleTarget = (f32)val / 1000.0f;
            for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
                gSeqController[seqPlayerIndex].channelController[i].freqScaleTarget = freqScaleTarget;
                gSeqController[seqPlayerIndex].channelController[i].freqScaleTimer = duration;
                gSeqController[seqPlayerIndex].channelController[i].freqScaleStep =
                    (gSeqController[seqPlayerIndex].channelController[i].freqScaleCur - freqScaleTarget) /
                    (f32)duration;
            }
            gSeqController[seqPlayerIndex].freqScaleChannelFlags = 0xFFFF;
            break;

        case SEQCMD_OP_SET_CHANNEL_FREQ:
            // Transition freq scale to a target for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF000) >> 12;
            val = cmd & 0xFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 1000
            freqScaleTarget = (f32)val / 1000.0f;
            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTarget = freqScaleTarget;
            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleStep =
                (gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleCur - freqScaleTarget) /
                (f32)duration;
            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTimer = duration;
            gSeqController[seqPlayerIndex].freqScaleChannelFlags |= 1 << channelIndex;
            break;

        case SEQCMD_OP_SET_CHANNEL_VOLUME:
            // Transition volume to a target volume for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            gSeqController[seqPlayerIndex].channelController[channelIndex].volTarget = (f32)val / 127.0f;
            if (gSeqController[seqPlayerIndex].channelController[channelIndex].volCur !=
                gSeqController[seqPlayerIndex].channelController[channelIndex].volTarget) {
                gSeqController[seqPlayerIndex].channelController[channelIndex].volStep =
                    (gSeqController[seqPlayerIndex].channelController[channelIndex].volCur -
                     gSeqController[seqPlayerIndex].channelController[channelIndex].volTarget) /
                    (f32)duration;
                gSeqController[seqPlayerIndex].channelController[channelIndex].volTimer = duration;
                gSeqController[seqPlayerIndex].volChannelFlags |= 1 << channelIndex;
            }
            break;

        case SEQCMD_OP_SET_PLAYER_IO:
            // Set global io port
            ioPort = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            AUDIOCMD_SEQPLAYER_SET_IO(seqPlayerIndex, ioPort, val);
            break;

        case SEQCMD_OP_SET_CHANNEL_IO:
            // Set io port if channel masked
            channelIndex = (cmd & 0xF00) >> 8;
            ioPort = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            if (!(gSeqController[seqPlayerIndex].channelPortMask & (1 << channelIndex))) {
                AUDIOCMD_CHANNEL_SET_IO(seqPlayerIndex, (u32)channelIndex, ioPort, val);
            }
            break;

        case SEQCMD_OP_SET_CHANNEL_IO_DISABLE_MASK:
            // Disable channel io specifically for
            // `SEQCMD_OP_SET_CHANNEL_IO` This can be bypassed by setting channel io through `AudioThread_QueueCmdS8`
            // 0x6 directly. This is accomplished by setting a channel mask.
            gSeqController[seqPlayerIndex].channelPortMask = cmd & 0xFFFF;
            break;

        case SEQCMD_OP_SET_CHANNEL_DISABLE_MASK:
            // Disable or Reenable channels

            // Disable channels
            channelMaskDisable = cmd & 0xFFFF;
            if (channelMaskDisable != 0) {
                // Apply channel mask `channelMaskDisable`
                AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, channelMaskDisable);
                // Disable channels
                AUDIOCMD_CHANNEL_SET_MUTE(seqPlayerIndex, SEQ_ALL_CHANNELS, true);
            }

            // Reenable channels
            channelMaskEnable = (channelMaskDisable ^ 0xFFFF);
            if (channelMaskEnable != 0) {
                // Apply channel mask `channelMaskEnable`
                AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, channelMaskEnable);
                // Enable channels
                AUDIOCMD_CHANNEL_SET_MUTE(seqPlayerIndex, SEQ_ALL_CHANNELS, false);
            }
            break;

        case SEQCMD_OP_TEMPO_CMD:
            // Update a tempo using a sub-command system.
            // Stores the cmd for processing elsewhere.
            gSeqController[seqPlayerIndex].tempoCmd = cmd;
            break;

        case SEQCMD_OP_SETUP_CMD:
            // Queue a sub-command to execute once the sequence is finished playing
            subOp = (cmd & 0xF00000) >> 20;
            if (subOp != SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS) {
                // Ensure the maximum number of setup commands is not exceeded
                if (gSeqController[seqPlayerIndex].setupCmdNum <
                    (ARRAY_COUNT(gSeqController[seqPlayerIndex].setupCmd) - 1)) {
                    found = gSeqController[seqPlayerIndex].setupCmdNum++;
                    if (found < ARRAY_COUNT(gSeqController[seqPlayerIndex].setupCmd)) {
                        gSeqController[seqPlayerIndex].setupCmd[found] = cmd;
                        // Adds a delay of 2 frames before executing any setup commands.
                        // This allows setup commands to be requested along with a new sequence on a seqPlayerIndex.
                        // This 2 frame delay ensures the player is enabled before its state is checked for
                        // the purpose of deciding if the setup commands should be run.
                        // Otherwise, the setup commands will be executed before the sequence starts,
                        // when the player is still disabled, instead of when the newly played sequence ends.
                        gSeqController[seqPlayerIndex].setupCmdTimer = 2;
                    }
                }
            } else {
                // `SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS`
                // Discard all setup command requests on `seqPlayerIndex`
                gSeqController[seqPlayerIndex].setupCmdNum = 0;
            }
            break;

        case SEQCMD_OP_GLOBAL_CMD:
            // Apply a command that applies to all sequence players
            subOp = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            switch (subOp) {
                case SEQCMD_SUB_OP_GLOBAL_SET_SOUND_MODE:
                    // Set sound mode
                    AUDIOCMD_GLOBAL_SET_SOUND_MODE(gSoundModeList[val]);
                    break;

                case SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES:
                    // Disable the starting of new sequences (except for the sfx player)
                    gStartSeqDisabled = val & 1;
                    break;
            }
            break;

        case SEQCMD_OP_RESET_AUDIO_HEAP:
            // Resets the audio heap based on the audio specifications and sfx channel layout
            spec = cmd & 0xFF;
            gSfxChannelLayout = (cmd & 0xFF00) >> 8;
            oldSpec = gAudioSpecId;
            gAudioSpecId = spec;
            AudioThread_ResetAudioHeap(spec);
            Audio_ResetForAudioHeapStep1(oldSpec);
            AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
            break;
    }
}

/**
 * Add the sequence cmd to the `sAudioSeqCmds` queue
 */
void AudioSeq_QueueSeqCmd(u32 cmd) {
    sAudioSeqCmds[gSeqCmdWritePos++] = cmd;
}

void AudioSeq_ProcessSeqCmds(void) {
    while (gSeqCmdWritePos != gSeqCmdReadPos) {
        AudioSeq_ProcessSeqCmd(sAudioSeqCmds[gSeqCmdReadPos++]);
    }
}

u16 AudioSeq_GetActiveSeqId(u8 seqPlayerIndex) {
    if (!gAudioCtx.seqPlayers[seqPlayerIndex].enabled) {
        return NA_BGM_DISABLED;
    }
    return gSeqController[seqPlayerIndex].seqId;
}

s32 AudioSeq_IsSeqCmdNotQueued(u32 cmdVal, u32 cmdMask) {
    u8 i;

    for (i = gSeqCmdReadPos; i != gSeqCmdWritePos; i++) {
        if ((sAudioSeqCmds[i] & cmdMask) == cmdVal) {
            return false;
        }
    }

    return true;
}

// Unused
void AudioSeq_ResetSequenceRequests(u8 seqPlayerIndex) {
    sNumSeqRequests[seqPlayerIndex] = 0;
}

/**
 * Check if the setup command is queued. If it is, then replace the command
 * with `SEQCMD_SUB_OP_SETUP_RESTORE_VOLUME`.
 * Unused
 */
void AudioSeq_ReplaceSeqCmdSetupOpVolRestore(u8 seqPlayerIndex, u8 setupOpDisabled) {
    u8 i;

    for (i = 0; i < gSeqController[seqPlayerIndex].setupCmdNum; i++) {
        u8 setupOp = (gSeqController[seqPlayerIndex].setupCmd[i] & 0xF00000) >> 20;

        if (setupOp == setupOpDisabled) {
            gSeqController[seqPlayerIndex].setupCmd[i] = 0xFF000000;
        }
    }
}

void AudioSeq_SetVolumeScale(u8 seqPlayerIndex, u8 scaleIndex, u8 targetVol, u8 volFadeTimer) {
    f32 volScale;
    u8 i;

    gSeqController[seqPlayerIndex].volScales[scaleIndex] = targetVol & 0x7F;

    if (volFadeTimer != 0) {
        gSeqController[seqPlayerIndex].fadeVolUpdate = true;
        gSeqController[seqPlayerIndex].volFadeTimer = volFadeTimer;
    } else {
        for (i = 0, volScale = 1.0f; i < VOL_SCALE_INDEX_MAX; i++) {
            volScale *= gSeqController[seqPlayerIndex].volScales[i] / 127.0f;
        }

        SEQCMD_SET_PLAYER_VOLUME_NOW(seqPlayerIndex, volFadeTimer, volScale);
    }
}

/**
 * Update different commands and requests for active sequences
 */
void AudioSeq_UpdateActiveSequences(void) {
    u32 tempoCmd;
    u8 tempoOp;
    u16 tempoPrev;
    u16 tempoTarget;
    u8 setupOp;
    u8 targetSeqPlayerIndex;
    u8 setupVal2;
    u8 setupVal1;
    u16 seqId;
    s32 pad[2];
    u16 channelMask;
    u32 retMsg;
    f32 volume;
    u8 tempoTimer;
    u8 seqPlayerIndex;
    u8 j;
    u8 channelIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < SEQ_PLAYER_MAX; seqPlayerIndex++) {

        // The setup for this block of code was not fully implemented until Majora's Mask.
        // The intent was to load soundfonts asynchronously before playing a
        // sequence in `AudioSeq_StartSequence` using `(seqArgs & 0x80)`.
        // Checks if the requested sequence is finished loading fonts
        if (gSeqController[seqPlayerIndex].isWaitingForFonts) {
            switch (func_800E5E20(&retMsg)) {
                case SEQ_PLAYER_BGM_MAIN + 1:
                case SEQ_PLAYER_FANFARE + 1:
                case SEQ_PLAYER_SFX + 1:
                case SEQ_PLAYER_BGM_SUB + 1:
                    // The fonts have been loaded successfully.
                    gSeqController[seqPlayerIndex].isWaitingForFonts = false;
                    // Queue the same command that was stored previously
                    // The code to store this command is missing in OoT, so no command is executed
                    AudioSeq_ProcessSeqCmd(gSeqController[seqPlayerIndex].startSeqCmd);
                    break;
            }
        }

        // Update global volume
        if (gSeqController[seqPlayerIndex].fadeVolUpdate) {
            volume = 1.0f;
            for (j = 0; j < VOL_SCALE_INDEX_MAX; j++) {
                volume *= (gSeqController[seqPlayerIndex].volScales[j] / 127.0f);
            }
            SEQCMD_SET_PLAYER_VOLUME(seqPlayerIndex, gSeqController[seqPlayerIndex].volFadeTimer,
                                     (u8)(volume * 127.0f));
            gSeqController[seqPlayerIndex].fadeVolUpdate = false;
        }

        if (gSeqController[seqPlayerIndex].volTimer != 0) {
            gSeqController[seqPlayerIndex].volTimer--;

            if (gSeqController[seqPlayerIndex].volTimer != 0) {
                gSeqController[seqPlayerIndex].volCur -= gSeqController[seqPlayerIndex].volStep;
            } else {
                gSeqController[seqPlayerIndex].volCur = gSeqController[seqPlayerIndex].volTarget;
            }

            AUDIOCMD_SEQPLAYER_FADE_VOLUME_SCALE((u32)seqPlayerIndex, gSeqController[seqPlayerIndex].volCur);
        }

        // Process tempo
        if (gSeqController[seqPlayerIndex].tempoCmd != 0) {
            tempoCmd = gSeqController[seqPlayerIndex].tempoCmd;
            tempoTimer = (tempoCmd & 0xFF0000) >> 15;
            tempoTarget = tempoCmd & 0xFFF;
            if (tempoTimer == 0) {
                tempoTimer++;
            }

            // Process tempo commands
            if (gAudioCtx.seqPlayers[seqPlayerIndex].enabled) {
                tempoPrev = gAudioCtx.seqPlayers[seqPlayerIndex].tempo / TICKS_PER_BEAT;
                tempoOp = (tempoCmd & 0xF000) >> 12;
                switch (tempoOp) {
                    case SEQCMD_SUB_OP_TEMPO_SPEED_UP:
                        // Speed up tempo by `tempoTarget` amount
                        tempoTarget += tempoPrev;
                        break;

                    case SEQCMD_SUB_OP_TEMPO_SLOW_DOWN:
                        // Slow down tempo by `tempoTarget` amount
                        if (tempoTarget < tempoPrev) {
                            tempoTarget = tempoPrev - tempoTarget;
                        }
                        break;

                    case SEQCMD_SUB_OP_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        tempoTarget = tempoPrev * (tempoTarget / 100.0f);
                        break;

                    case SEQCMD_SUB_OP_TEMPO_RESET:
                        // Reset tempo to original tempo
                        tempoTarget = (gSeqController[seqPlayerIndex].tempoOriginal != 0)
                                          ? gSeqController[seqPlayerIndex].tempoOriginal
                                          : tempoPrev;
                        break;

                    default: // `SEQCMD_SUB_OP_TEMPO_SET`
                        // `tempoTarget` is the new tempo
                        break;
                }

                if (tempoTarget > 300) {
                    tempoTarget = 300;
                }

                if (gSeqController[seqPlayerIndex].tempoOriginal == 0) {
                    gSeqController[seqPlayerIndex].tempoOriginal = tempoPrev;
                }

                gSeqController[seqPlayerIndex].tempoTarget = tempoTarget;
                gSeqController[seqPlayerIndex].tempoCur = gAudioCtx.seqPlayers[seqPlayerIndex].tempo / TICKS_PER_BEAT;
                gSeqController[seqPlayerIndex].tempoStep =
                    (gSeqController[seqPlayerIndex].tempoCur - gSeqController[seqPlayerIndex].tempoTarget) / tempoTimer;
                gSeqController[seqPlayerIndex].tempoTimer = tempoTimer;
                gSeqController[seqPlayerIndex].tempoCmd = 0;
            }
        }

        // Step tempo to target
        if (gSeqController[seqPlayerIndex].tempoTimer != 0) {
            gSeqController[seqPlayerIndex].tempoTimer--;
            if (gSeqController[seqPlayerIndex].tempoTimer != 0) {
                gSeqController[seqPlayerIndex].tempoCur -= gSeqController[seqPlayerIndex].tempoStep;
            } else {
                gSeqController[seqPlayerIndex].tempoCur = gSeqController[seqPlayerIndex].tempoTarget;
            }

            AUDIOCMD_SEQPLAYER_SET_TEMPO((u32)seqPlayerIndex, gSeqController[seqPlayerIndex].tempoCur);
        }

        // Update channel volumes
        if (gSeqController[seqPlayerIndex].volChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (gSeqController[seqPlayerIndex].channelController[channelIndex].volTimer != 0) {
                    gSeqController[seqPlayerIndex].channelController[channelIndex].volTimer--;
                    if (gSeqController[seqPlayerIndex].channelController[channelIndex].volTimer != 0) {
                        gSeqController[seqPlayerIndex].channelController[channelIndex].volCur -=
                            gSeqController[seqPlayerIndex].channelController[channelIndex].volStep;
                    } else {
                        gSeqController[seqPlayerIndex].channelController[channelIndex].volCur =
                            gSeqController[seqPlayerIndex].channelController[channelIndex].volTarget;
                        gSeqController[seqPlayerIndex].volChannelFlags ^= (1 << channelIndex);
                    }

                    AUDIOCMD_CHANNEL_SET_VOL_SCALE(
                        seqPlayerIndex, (u32)channelIndex,
                        gSeqController[seqPlayerIndex].channelController[channelIndex].volCur);
                }
            }
        }

        // Update frequencies
        if (gSeqController[seqPlayerIndex].freqScaleChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTimer != 0) {
                    gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTimer--;
                    if (gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTimer != 0) {
                        gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleCur -=
                            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleStep;
                    } else {
                        gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleCur =
                            gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleTarget;
                        gSeqController[seqPlayerIndex].freqScaleChannelFlags ^= (1 << channelIndex);
                    }

                    AUDIOCMD_CHANNEL_SET_FREQ_SCALE(
                        seqPlayerIndex, (u32)channelIndex,
                        gSeqController[seqPlayerIndex].channelController[channelIndex].freqScaleCur);
                }
            }
        }

        // Process setup commands
        if (gSeqController[seqPlayerIndex].setupCmdNum != 0) {
            // If there is a SeqCmd to reset the audio heap queued, then drop all setup commands
            if (!AudioSeq_IsSeqCmdNotQueued(SEQCMD_OP_RESET_AUDIO_HEAP << 28, SEQCMD_OP_MASK)) {
                gSeqController[seqPlayerIndex].setupCmdNum = 0;
                return;
            }

            // Only process setup commands once the timer reaches zero
            if (gSeqController[seqPlayerIndex].setupCmdTimer != 0) {
                gSeqController[seqPlayerIndex].setupCmdTimer--;
                continue;
            }

            // Only process setup commands if `seqPlayerIndex` if no longer playing
            // i.e. the `seqPlayer` is no longer enabled
            if (gAudioCtx.seqPlayers[seqPlayerIndex].enabled) {
                continue;
            }

            for (j = 0; j < gSeqController[seqPlayerIndex].setupCmdNum; j++) {
                setupOp = (gSeqController[seqPlayerIndex].setupCmd[j] & 0x00F00000) >> 20;
                targetSeqPlayerIndex = (gSeqController[seqPlayerIndex].setupCmd[j] & 0x000F0000) >> 16;
                setupVal2 = (gSeqController[seqPlayerIndex].setupCmd[j] & 0xFF00) >> 8;
                setupVal1 = gSeqController[seqPlayerIndex].setupCmd[j] & 0xFF;

                switch (setupOp) {
                    case SEQCMD_SUB_OP_SETUP_RESTORE_VOLUME:
                        // Restore `targetSeqPlayerIndex` volume back to normal levels
                        AudioSeq_SetVolumeScale(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTORE_VOLUME_IF_QUEUED:
                        // Restore `targetSeqPlayerIndex` volume back to normal levels,
                        // but only if the number of sequence queue requests from `sSeqRequests`
                        // exactly matches the argument to the command
                        if (setupVal1 == sNumSeqRequests[seqPlayerIndex]) {
                            AudioSeq_SetVolumeScale(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal2);
                        }
                        break;

                    case SEQCMD_SUB_OP_SETUP_SEQ_UNQUEUE:
                        // Unqueue `seqPlayerIndex` from sSeqRequests
                        //! @bug this command does not work as intended as unqueueing
                        //! the sequence relies on `gSeqController[seqPlayerIndex].seqId`
                        //! However, `gSeqController[seqPlayerIndex].seqId` is reset before the sequence on
                        //! `seqPlayerIndex` is requested to stop, i.e. before the sequence is disabled and setup
                        //! commands (including this command) can run. A simple fix would have been to unqueue based on
                        //! `gSeqController[seqPlayerIndex].prevSeqId` instead
                        SEQCMD_UNQUEUE_SEQUENCE(seqPlayerIndex, 0, gSeqController[seqPlayerIndex].seqId);
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTART_SEQ:
                        // Restart the currently active sequence on `targetSeqPlayerIndex` with full volume.
                        // Sequence on `targetSeqPlayerIndex` must still be active to play (can be muted)
                        SEQCMD_PLAY_SEQUENCE(targetSeqPlayerIndex, 1, 0, gSeqController[targetSeqPlayerIndex].seqId);
                        gSeqController[targetSeqPlayerIndex].fadeVolUpdate = true;
                        gSeqController[targetSeqPlayerIndex].volScales[VOL_SCALE_INDEX_FANFARE] = 0x7F;
                        break;

                    case SEQCMD_SUB_OP_SETUP_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        SEQCMD_SCALE_TEMPO(targetSeqPlayerIndex, setupVal2, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_TEMPO_RESET:
                        // Reset tempo to previous tempo
                        SEQCMD_RESET_TEMPO(targetSeqPlayerIndex, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_PLAY_SEQ:
                        // Play the requested sequence
                        // Uses the fade timer set by `SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER`
                        seqId = gSeqController[seqPlayerIndex].setupCmd[j] & 0xFFFF;
                        SEQCMD_PLAY_SEQUENCE(targetSeqPlayerIndex, gSeqController[targetSeqPlayerIndex].setupFadeTimer,
                                             0, seqId);
                        AudioSeq_SetVolumeScale(targetSeqPlayerIndex, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
                        gSeqController[targetSeqPlayerIndex].setupFadeTimer = 0;
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER:
                        // A command specifically to support `SEQCMD_SUB_OP_SETUP_PLAY_SEQ`
                        // Sets the fade timer for the sequence requested in `SEQCMD_SUB_OP_SETUP_PLAY_SEQ`
                        gSeqController[seqPlayerIndex].setupFadeTimer = setupVal2;
                        break;

                    case SEQCMD_SUB_OP_SETUP_RESTORE_VOLUME_WITH_SCALE_INDEX:
                        // Restore the volume back to default levels
                        // Allows a `scaleIndex` to be specified.
                        AudioSeq_SetVolumeScale(targetSeqPlayerIndex, setupVal2, 0x7F, setupVal1);
                        break;

                    case SEQCMD_SUB_OP_SETUP_POP_PERSISTENT_CACHE:
                        // Discard audio data by popping one more audio caches from the audio heap
                        if (setupVal1 & (1 << SEQUENCE_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SEQUENCE_TABLE);
                        }
                        if (setupVal1 & (1 << FONT_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(FONT_TABLE);
                        }
                        if (setupVal1 & (1 << SAMPLE_TABLE)) {
                            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SAMPLE_TABLE);
                        }
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_CHANNEL_DISABLE_MASK:
                        // Disable (or reenable) specific channels of `targetSeqPlayerIndex`
                        channelMask = gSeqController[seqPlayerIndex].setupCmd[j] & 0xFFFF;
                        SEQCMD_SET_CHANNEL_DISABLE_MASK(targetSeqPlayerIndex, channelMask);
                        break;

                    case SEQCMD_SUB_OP_SETUP_SET_PLAYER_FREQ:
                        // Scale all channels of `targetSeqPlayerIndex`
                        SEQCMD_SET_PLAYER_FREQ(targetSeqPlayerIndex, setupVal2, (setupVal1 * 10) & 0xFFFF);
                        break;
                }
            }

            gSeqController[seqPlayerIndex].setupCmdNum = 0;
        }
    }
}

u8 AudioSeq_UpdateAudioHeapReset(void) {
    if (gAudioHeapResetState != AUDIO_HEAP_RESET_STATE_NONE) {
        if (gAudioHeapResetState == AUDIO_HEAP_RESET_STATE_RESETTING) {
            if (func_800E5EDC() == 1) {
                gAudioHeapResetState = AUDIO_HEAP_RESET_STATE_NONE;
                AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_SFX, 0, gSfxChannelLayout);
                Audio_ResetForAudioHeapStep2();
            }
        } else if (gAudioHeapResetState == AUDIO_HEAP_RESET_STATE_RESETTING_ALT) {
            while (func_800E5EDC() != 1) {}
            gAudioHeapResetState = AUDIO_HEAP_RESET_STATE_NONE;
            AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_SFX, 0, gSfxChannelLayout);
            Audio_ResetForAudioHeapStep2();
        }
    }

    return gAudioHeapResetState;
}

void AudioSeq_ResetActiveSequences(void) {
    u8 seqPlayerIndex;
    u8 scaleIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < SEQ_PLAYER_MAX; seqPlayerIndex++) {
        sNumSeqRequests[seqPlayerIndex] = 0;

        gSeqController[seqPlayerIndex].seqId = NA_BGM_DISABLED;
        gSeqController[seqPlayerIndex].prevSeqId = NA_BGM_DISABLED;
        gSeqController[seqPlayerIndex].tempoTimer = 0;
        gSeqController[seqPlayerIndex].tempoOriginal = 0;
        gSeqController[seqPlayerIndex].tempoCmd = 0;
        gSeqController[seqPlayerIndex].channelPortMask = 0;
        gSeqController[seqPlayerIndex].setupCmdNum = 0;
        gSeqController[seqPlayerIndex].setupFadeTimer = 0;
        gSeqController[seqPlayerIndex].freqScaleChannelFlags = 0;
        gSeqController[seqPlayerIndex].volChannelFlags = 0;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            gSeqController[seqPlayerIndex].volScales[scaleIndex] = 0x7F;
        }

        gSeqController[seqPlayerIndex].volFadeTimer = 1;
        gSeqController[seqPlayerIndex].fadeVolUpdate = true;
    }
}

void AudioSeq_ResetActiveSequencesAndVolume(void) {
    u8 seqPlayerIndex;
    u8 scaleIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < SEQ_PLAYER_MAX; seqPlayerIndex++) {
        gSeqController[seqPlayerIndex].volCur = 1.0f;
        gSeqController[seqPlayerIndex].volTimer = 0;
        gSeqController[seqPlayerIndex].fadeVolUpdate = false;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            gSeqController[seqPlayerIndex].volScales[scaleIndex] = 0x7F;
        }
    }
    AudioSeq_ResetActiveSequences();
}
