/**
 * @file audio_seqcmd.c
 *
 * This file implements a set of high-level audio sequence commands that allow sequences to be modified in real-time.
 * These commands are intended to interface external to the audio library.
 *
 * These commands are generated using (Audio_QueueSeqCmd), and a user-friendly interface for this function
 * can be found in (seqcmd.h)
 *
 * These commands change sequences by generating internal audio commands (Audio_QueueCmd) which allows these
 * sequence requests to be passed onto the audio thread. It is worth noting all functions in this file are
 * called from the graph thread.
 *
 * These commands are not to be confused with the sequence commands used by the sequences themselves
 * which are a midi-based scripting language.
 *
 * Nor are these commands to be confused with the internal audio commands used to transfer requests from
 * the graph thread to the audio thread.
 */
#include "ultra64.h"
#include "global.h"
#include "ultra64/abi.h"

// Direct audio command (skips the queueing system)
#define Audio_SetVolumeScaleNow(playerIndex, volFadeTimer, volScale)                                          \
    Audio_ProcessSeqCmd((SEQ_CMD_SET_PLAYER_VOL << 28) | ((u8)playerIndex << 24) | ((u8)volFadeTimer << 16) | \
                        ((u8)(volScale * 127.0f)));

typedef struct {
    u8 seqId;
    u8 priority; // higher values have higher priority
} SeqRequest;

SeqRequest sSeqRequests[4][5];
u8 sNumSeqRequests[4];
u32 sAudioSeqCmds[0x100];
ActiveSeq gActiveSeqs[4];

u8 sSeqCmdWritePos = 0;
u8 sSeqCmdReadPos = 0;
u8 gNewSeqDisabled = false;
u8 gAudioDebugPrintSeqCmd = true;
u8 sSoundModes[] = { 0, 1, 2, 3 };
u8 gAudioSpecId = 0;
u8 D_80133418 = 0;

void Audio_StartSequence(u8 playerIndex, u8 seqId, u8 seqArgs, u16 fadeTimer) {
    u8 channelIndex;
    u16 duration;
    s32 pad;

    if (!gNewSeqDisabled || (playerIndex == SEQ_PLAYER_SFX)) {
        seqArgs &= 0x7F;
        if (seqArgs == 0x7F) {
            // fadeTimer is interpreted as skip ticks
            duration = (fadeTimer >> 3) * 60 * gAudioContext.audioBufferParameters.updatesPerFrame;
            Audio_QueueCmdS32(0x85000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(seqId, 8, 8), duration);
        } else {
            // fadeTimer is interpreated as number of frames at 30 fps
            Audio_QueueCmdS32(0x82000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(seqId, 8, 8),
                              (fadeTimer * (u16)gAudioContext.audioBufferParameters.updatesPerFrame) / 4);
        }

        gActiveSeqs[playerIndex].seqId = seqId | (seqArgs << 8);
        gActiveSeqs[playerIndex].prevSeqId = seqId | (seqArgs << 8);

        if (gActiveSeqs[playerIndex].volCur != 1.0f) {
            Audio_QueueCmdF32(0x41000000 | _SHIFTL(playerIndex, 16, 8), gActiveSeqs[playerIndex].volCur);
        }

        gActiveSeqs[playerIndex].tempoDuration = 0;
        gActiveSeqs[playerIndex].tempoOriginal = 0;
        gActiveSeqs[playerIndex].tempoCmd = 0;

        for (channelIndex = 0; channelIndex < 16; channelIndex++) {
            gActiveSeqs[playerIndex].channelData[channelIndex].volCur = 1.0f;
            gActiveSeqs[playerIndex].channelData[channelIndex].volDuration = 0;
            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleCur = 1.0f;
            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleDuration = 0;
        }

        gActiveSeqs[playerIndex].freqScaleChannelFlags = 0;
        gActiveSeqs[playerIndex].volChannelFlags = 0;
    }
}

void Audio_StopSequenceNow(u8 playerIndex, u16 fadeTimer) {
    Audio_QueueCmdS32(0x83000000 | ((u8)playerIndex << 16),
                      (fadeTimer * (u16)gAudioContext.audioBufferParameters.updatesPerFrame) / 4);
    gActiveSeqs[playerIndex].seqId = NA_BGM_DISABLED;
}

void Audio_ProcessSeqCmd(u32 cmd) {
    s32 priority;
    s32 channelMaskEnable;
    u16 channelMaskDisable;
    u16 fadeTimer;
    u16 val;
    u8 oldSpec;
    u8 spec;
    u8 op;
    u8 subOp;
    u8 playerIndex;
    u8 seqId;
    u8 seqArgs;
    u8 found;
    u8 port;
    u8 duration;
    u8 channelIndex;
    u8 i;
    f32 freqScaleTarget;
    s32 pad;

    if (gAudioDebugPrintSeqCmd && ((cmd & SEQ_CMD_MASK) != (SEQ_CMD_SET_PLAYER_IO << 28))) {
        // print cmd high-bits, "SEQ H"
        AudioDebug_ScrPrt((const s8*)gAudioDebugTextSeqCmdHighBits, (cmd >> 16) & 0xFFFF);
        // pring cmd low-bits, "    L"
        AudioDebug_ScrPrt((const s8*)gAudioDebugTextSeqCmdLowBits, cmd & 0xFFFF);
    }

    op = cmd >> 28;
    playerIndex = (cmd & 0xF000000) >> 24;

    switch (op) {
        case SEQ_CMD_PLAY:
            // Play a new sequence
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            // fadeTimer is only shifted 13 bytes instead of 16 bytes.
            // fadeTimer continues to be scaled in Audio_StartSequence
            fadeTimer = (cmd & 0xFF0000) >> 13;
            if (!gActiveSeqs[playerIndex].isWaitingForFonts && (seqArgs < 0x80)) {
                Audio_StartSequence(playerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQ_CMD_STOP:
            // Stop a sequence and disable the sequence player
            fadeTimer = (cmd & 0xFF0000) >> 13;
            Audio_StopSequenceNow(playerIndex, fadeTimer);
            break;

        case SEQ_CMD_QUEUE:
            // Queue a sequence into sSeqRequests
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            fadeTimer = (cmd & 0xFF0000) >> 13;
            priority = seqArgs;

            // Checks if the requested sequence is first in the list of requests
            // If it is already queued and first in the list, then play the sequence immediately
            for (i = 0; i < sNumSeqRequests[playerIndex]; i++) {
                if (sSeqRequests[playerIndex][i].seqId == seqId) {
                    if (i == 0) {
                        Audio_StartSequence(playerIndex, seqId, seqArgs, fadeTimer);
                    }
                    return;
                }
            }

            // Searches the sequence requests for the first request that does not have a higher priority
            // than the current incoming request
            found = sNumSeqRequests[playerIndex];
            for (i = 0; i < sNumSeqRequests[playerIndex]; i++) {
                if (priority >= sSeqRequests[playerIndex][i].priority) {
                    found = i;
                    i = sNumSeqRequests[playerIndex]; // "break;"
                }
            }

            // Check if the queue is full
            if (sNumSeqRequests[playerIndex] < 5) {
                sNumSeqRequests[playerIndex]++;
            }

            for (i = sNumSeqRequests[playerIndex] - 1; i != found; i--) {
                // Move all requests of lower priority backwards 1 place in the queue
                sSeqRequests[playerIndex][i].priority = sSeqRequests[playerIndex][i - 1].priority;
                sSeqRequests[playerIndex][i].seqId = sSeqRequests[playerIndex][i - 1].seqId;
            }

            // Fill the newly freed space in the queue with the new request
            sSeqRequests[playerIndex][found].priority = seqArgs;
            sSeqRequests[playerIndex][found].seqId = seqId;

            // The sequence is first in queue, so start playing.
            if (found == 0) {
                Audio_StartSequence(playerIndex, seqId, seqArgs, fadeTimer);
            }
            break;

        case SEQ_CMD_UNQUEUE:
            // Unqueue sequence
            fadeTimer = (cmd & 0xFF0000) >> 13;

            found = sNumSeqRequests[playerIndex];
            for (i = 0; i < sNumSeqRequests[playerIndex]; i++) {
                seqId = cmd & 0xFF;
                if (sSeqRequests[playerIndex][i].seqId == seqId) {
                    found = i;
                    i = sNumSeqRequests[playerIndex]; // "break;"
                }
            }

            if (found != sNumSeqRequests[playerIndex]) {
                // Move all requests of lower priority forward 1 place in the queue
                for (i = found; i < sNumSeqRequests[playerIndex] - 1; i++) {
                    sSeqRequests[playerIndex][i].priority = sSeqRequests[playerIndex][i + 1].priority;
                    sSeqRequests[playerIndex][i].seqId = sSeqRequests[playerIndex][i + 1].seqId;
                }
                sNumSeqRequests[playerIndex]--;
            }

            // If the sequence was first in queue (it is currently playing),
            // Then stop the sequence and play the next sequence in the queue.
            if (found == 0) {
                Audio_StopSequenceNow(playerIndex, fadeTimer);
                if (sNumSeqRequests[playerIndex] != 0) {
                    Audio_StartSequence(playerIndex, sSeqRequests[playerIndex][0].seqId,
                                        sSeqRequests[playerIndex][0].priority, fadeTimer);
                }
            }
            break;

        case SEQ_CMD_SET_PLAYER_VOL:
            // Transition volume to a target volume for an entire player
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            gActiveSeqs[playerIndex].volTarget = (f32)val / 127.0f;
            if (gActiveSeqs[playerIndex].volCur != gActiveSeqs[playerIndex].volTarget) {
                gActiveSeqs[playerIndex].volVelocity =
                    (gActiveSeqs[playerIndex].volCur - gActiveSeqs[playerIndex].volTarget) / (f32)duration;
                gActiveSeqs[playerIndex].volDuration = duration;
            }
            break;

        case SEQ_CMD_SET_PLAYER_FREQ:
            // Transition freq scale to a target freq for all channels
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 1000
            freqScaleTarget = (f32)val / 1000.0f;
            for (i = 0; i < 16; i++) {
                gActiveSeqs[playerIndex].channelData[i].freqScaleTarget = freqScaleTarget;
                gActiveSeqs[playerIndex].channelData[i].freqScaleDuration = duration;
                gActiveSeqs[playerIndex].channelData[i].freqScaleVelocity =
                    (gActiveSeqs[playerIndex].channelData[i].freqScaleCur - freqScaleTarget) / (f32)duration;
            }
            gActiveSeqs[playerIndex].freqScaleChannelFlags = 0xFFFF;
            break;

        case SEQ_CMD_SET_CHANNEL_FREQ:
            // Transition freq scale to a target for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF000) >> 12;
            val = cmd & 0xFFF;
            if (duration == 0) {
                duration++;
            }
            // Frequency is scaled relative to 100
            freqScaleTarget = (f32)val / 1000.0f;
            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleTarget = freqScaleTarget;
            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleVelocity =
                (gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleCur - freqScaleTarget) / (f32)duration;
            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleDuration = duration;
            gActiveSeqs[playerIndex].freqScaleChannelFlags |= 1 << channelIndex;
            break;

        case SEQ_CMD_SET_CHANNEL_VOL:
            // Transition volume to a target volume for a specific channel
            duration = (cmd & 0xFF0000) >> 15;
            channelIndex = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            // Volume is scaled relative to 127
            gActiveSeqs[playerIndex].channelData[channelIndex].volTarget = (f32)val / 127.0f;
            if (gActiveSeqs[playerIndex].channelData[channelIndex].volCur !=
                gActiveSeqs[playerIndex].channelData[channelIndex].volTarget) {
                gActiveSeqs[playerIndex].channelData[channelIndex].volVelocity =
                    (gActiveSeqs[playerIndex].channelData[channelIndex].volCur -
                     gActiveSeqs[playerIndex].channelData[channelIndex].volTarget) /
                    (f32)duration;
                gActiveSeqs[playerIndex].channelData[channelIndex].volDuration = duration;
                gActiveSeqs[playerIndex].volChannelFlags |= 1 << channelIndex;
            }
            break;

        case SEQ_CMD_SET_PLAYER_IO:
            // Set global io port
            port = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            Audio_QueueCmdS8(0x46000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(port, 0, 8), val);
            break;

        case SEQ_CMD_SET_CHANNEL_IO:
            // Set io port if channel masked
            channelIndex = (cmd & 0xF00) >> 8;
            port = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            if (!(gActiveSeqs[playerIndex].channelPortMask & (1 << channelIndex))) {
                Audio_QueueCmdS8(
                    0x06000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(channelIndex, 8, 8) | _SHIFTL(port, 0, 8), val);
            }
            break;

        case SEQ_CMD_DISABLE_CHANNEL_IO:
            // Disable channel io specifically for SEQ_CMD_SET_CHANNEL_IO
            // This can be bypassed by setting channel io through Audio_QueueCmdS8 0x6 directly.
            // This is accomplished by setting a channel mask.
            gActiveSeqs[playerIndex].channelPortMask = cmd & 0xFFFF;
            break;

        case SEQ_CMD_DISABLE_CHANNELS:
            // Disable or Reenable channels
            channelMaskDisable = cmd & 0xFFFF;
            if (channelMaskDisable != 0) {
                // Apply channel mask channelMaskDisable
                Audio_QueueCmdU16(0x90000000 | _SHIFTL(playerIndex, 16, 8), channelMaskDisable);
                // Disable channels
                Audio_QueueCmdS8(0x08000000 | _SHIFTL(playerIndex, 16, 8) | 0xFF00, 1);
            }

            // Reenable channels
            channelMaskEnable = (channelMaskDisable ^ 0xFFFF);
            if (channelMaskEnable != 0) {
                // Apply channel mask channelMaskEnable
                Audio_QueueCmdU16(0x90000000 | _SHIFTL(playerIndex, 16, 8), channelMaskEnable);
                // Enable channels
                Audio_QueueCmdS8(0x08000000 | _SHIFTL(playerIndex, 16, 8) | 0xFF00, 0);
            }
            break;

        case SEQ_CMD_TEMPO_CMD:
            // Update a tempo using a sub-command system.
            // Stores the cmd for processing elsewhere.
            gActiveSeqs[playerIndex].tempoCmd = cmd;
            break;

        case SEQ_CMD_SETUP_CMD:
            // Queue a sub-command to execute once the sequence is finished playing
            subOp = (cmd & 0xF00000) >> 20;
            if (subOp != SEQ_SUB_CMD_SETUP_RESET_SETUP_CMDS) {
                // Ensure the maximum number of setup commands is not exceeded
                if (gActiveSeqs[playerIndex].setupCmdNum < 7) {
                    found = gActiveSeqs[playerIndex].setupCmdNum++;
                    if (found < 8) {
                        gActiveSeqs[playerIndex].setupCmd[found] = cmd;
                        // Adds a delay of 2 frames before executing any setup commands.
                        // This allows setup commands to be requested along with a new sequence on a playerIndex.
                        // This 2 frame delay ensures the player is enabled before attempting to execute commands.
                        // Otherwise, the setup commands will execute before the sequence starts instead of when
                        // the sequence ends.
                        gActiveSeqs[playerIndex].setupCmdTimer = 2;
                    }
                }
            } else {
                // SEQ_SUB_CMD_SETUP_RESET_SETUP_CMDS
                // Discard all setup command requests on playerIndex
                gActiveSeqs[playerIndex].setupCmdNum = 0;
            }
            break;

        case SEQ_CMD_GLOBAL_CMD:
            // Apply a command that applies to all sequence players
            subOp = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            switch (subOp) {
                case SEQ_SUB_CMD_GLOBAL_SET_SOUND_MODE:
                    // Set sound mode
                    Audio_QueueCmdS32(0xF0000000, sSoundModes[val]);
                    break;

                case SEQ_SUB_CMD_GLOBAL_DISABLE_NEW_SEQUENCES:
                    // Disable the starting of new sequences (except for the sfx player)
                    gNewSeqDisabled = val & 1;
                    break;
            }
            break;

        case SEQ_CMD_RESET_HEAP:
            // Resets the audio heap based on the audio specifications and sfx channel layout
            spec = cmd & 0xFF;
            gSfxChannelLayout = (cmd & 0xFF00) >> 8;
            oldSpec = gAudioSpecId;
            gAudioSpecId = spec;
            func_800E5F88(spec);
            func_800F71BC(oldSpec);
            Audio_QueueCmdS32(0xF8000000, 0);
            break;
    }
}

/**
 * Add the sequence cmd to the sAudioSeqCmds queue
 */
void Audio_QueueSeqCmd(u32 cmd) {
    sAudioSeqCmds[sSeqCmdWritePos++] = cmd;
}

void Audio_ProcessSeqCmds(void) {
    while (sSeqCmdWritePos != sSeqCmdReadPos) {
        Audio_ProcessSeqCmd(sAudioSeqCmds[sSeqCmdReadPos++]);
    }
}

u16 Audio_GetActiveSeqId(u8 playerIndex) {
    if (!gAudioContext.seqPlayers[playerIndex].enabled) {
        return NA_BGM_DISABLED;
    }
    return gActiveSeqs[playerIndex].seqId;
}

s32 Audio_IsSeqCmdNotQueued(u32 cmdVal, u32 cmdMask) {
    u8 i;

    for (i = sSeqCmdReadPos; i != sSeqCmdWritePos; i++) {
        if ((sAudioSeqCmds[i] & cmdMask) == cmdVal) {
            return false;
        }
    }

    return true;
}

// Unused
void Audio_ResetSequenceRequests(u8 playerIndex) {
    sNumSeqRequests[playerIndex] = 0;
}

/**
 * Check if the setup command is queued. If it is, then remove the command from the queue.
 * Unused
 */
void Audio_DisableSeqCmdSetupOp(u8 playerIndex, u8 setupOpDisabled) {
    u8 i;

    for (i = 0; i < gActiveSeqs[playerIndex].setupCmdNum; i++) {
        u8 setupOp = (gActiveSeqs[playerIndex].setupCmd[i] & 0xF00000) >> 20;

        if (setupOp == setupOpDisabled) {
            gActiveSeqs[playerIndex].setupCmd[i] = 0xFF000000;
        }
    }
}

void Audio_SetVolumeScale(u8 playerIndex, u8 scaleIndex, u8 targetVol, u8 volFadeTimer) {
    f32 volScale;
    u8 i;

    gActiveSeqs[playerIndex].volScales[scaleIndex] = targetVol & 0x7F;

    if (volFadeTimer != 0) {
        gActiveSeqs[playerIndex].fadeVolUpdate = 1;
        gActiveSeqs[playerIndex].volFadeTimer = volFadeTimer;
    } else {
        for (i = 0, volScale = 1.0f; i < VOL_SCALE_INDEX_MAX; i++) {
            volScale *= gActiveSeqs[playerIndex].volScales[i] / 127.0f;
        }

        Audio_SetVolumeScaleNow(playerIndex, volFadeTimer, volScale);
    }
}

/**
 * Update different commands and requests for active sequences
 */
void Audio_UpdateActiveSequences(void) {
    u32 tempoCmd;
    u8 tempoOp;
    u16 tempoPrev;
    u16 tempoTarget;
    u8 setupOp;
    u8 playerIndexTarget;
    u8 setupVal2;
    u8 setupVal1;
    u16 seqId;
    s32 pad[2];
    u16 channelMask;
    u32 retMsg;
    f32 volume;
    u8 tempoDuration;
    u8 playerIndex;
    u8 j;
    u8 channelIndex;

    for (playerIndex = 0; playerIndex < 4; playerIndex++) {

        // The setup for this block of code was not fully implemented until Majora's Mask.
        // The intent was to load soundfonts asyncronously before playing a
        // sequence in Audio_StartSequence using (seqArgs & 0x80).
        // Checks if the requested sequence is finished loading fonts
        if (gActiveSeqs[playerIndex].isWaitingForFonts) {
            switch (func_800E5E20(&retMsg)) {
                case SEQ_PLAYER_BGM_MAIN + 1:
                case SEQ_PLAYER_FANFARE + 1:
                case SEQ_PLAYER_SFX + 1:
                case SEQ_PLAYER_BGM_SUB + 1:
                    // The fonts have been loaded successfully.
                    gActiveSeqs[playerIndex].isWaitingForFonts = false;
                    // Queue the same command that was stored previously
                    // The code to store this command is missing in OoT, so no command is executed
                    Audio_ProcessSeqCmd(gActiveSeqs[playerIndex].startSeqCmd);
                    break;
            }
        }

        // Update global volume
        if (gActiveSeqs[playerIndex].fadeVolUpdate) {
            volume = 1.0f;
            for (j = 0; j < VOL_SCALE_INDEX_MAX; j++) {
                volume *= (gActiveSeqs[playerIndex].volScales[j] / 127.0f);
            }
            AudioSeqCmd_SetPlayerVol(playerIndex, gActiveSeqs[playerIndex].volFadeTimer, (u8)(volume * 127.0f));
            gActiveSeqs[playerIndex].fadeVolUpdate = 0;
        }

        if (gActiveSeqs[playerIndex].volDuration != 0) {
            gActiveSeqs[playerIndex].volDuration--;

            if (gActiveSeqs[playerIndex].volDuration != 0) {
                gActiveSeqs[playerIndex].volCur -= gActiveSeqs[playerIndex].volVelocity;
            } else {
                gActiveSeqs[playerIndex].volCur = gActiveSeqs[playerIndex].volTarget;
            }

            Audio_QueueCmdF32(0x41000000 | _SHIFTL(playerIndex, 16, 8), gActiveSeqs[playerIndex].volCur);
        }

        // Process tempo commands
        if (gActiveSeqs[playerIndex].tempoCmd != 0) {
            tempoCmd = gActiveSeqs[playerIndex].tempoCmd;
            tempoDuration = (tempoCmd & 0xFF0000) >> 15;
            tempoTarget = tempoCmd & 0xFFF;
            if (tempoDuration == 0) {
                tempoDuration++;
            }

            // Process tempo commands
            if (gAudioContext.seqPlayers[playerIndex].enabled) {
                tempoPrev = gAudioContext.seqPlayers[playerIndex].tempo / TATUMS_PER_BEAT;
                tempoOp = (tempoCmd & 0xF000) >> 12;
                switch (tempoOp) {
                    case SEQ_SUB_CMD_TEMPO_SPEED_UP:
                        // Speed up tempo by "tempoTarget" amount
                        tempoTarget += tempoPrev;
                        break;

                    case SEQ_SUB_CMD_TEMPO_SLOW_DOWN:
                        // Slow down tempo by "tempoTarget" amount
                        if (tempoTarget < tempoPrev) {
                            tempoTarget = tempoPrev - tempoTarget;
                        }
                        break;

                    case SEQ_SUB_CMD_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        tempoTarget = tempoPrev * (tempoTarget / 100.0f);
                        break;

                    case SEQ_SUB_CMD_TEMPO_RESET:
                        // Reset tempo to original tempo
                        tempoTarget = (gActiveSeqs[playerIndex].tempoOriginal != 0)
                                          ? gActiveSeqs[playerIndex].tempoOriginal
                                          : tempoPrev;
                        break;

                    default: // SEQ_SUB_CMD_TEMPO_SET
                        // tempoTarget is the new tempo
                        break;
                }

                if (tempoTarget > 300) {
                    tempoTarget = 300;
                }

                if (gActiveSeqs[playerIndex].tempoOriginal == 0) {
                    gActiveSeqs[playerIndex].tempoOriginal = tempoPrev;
                }

                gActiveSeqs[playerIndex].tempoTarget = tempoTarget;
                gActiveSeqs[playerIndex].tempoCur = gAudioContext.seqPlayers[playerIndex].tempo / TATUMS_PER_BEAT;
                gActiveSeqs[playerIndex].tempoVelocity =
                    (gActiveSeqs[playerIndex].tempoCur - gActiveSeqs[playerIndex].tempoTarget) / tempoDuration;
                gActiveSeqs[playerIndex].tempoDuration = tempoDuration;
                gActiveSeqs[playerIndex].tempoCmd = 0;
            }
        }

        // Step tempo to target
        if (gActiveSeqs[playerIndex].tempoDuration != 0) {
            gActiveSeqs[playerIndex].tempoDuration--;
            if (gActiveSeqs[playerIndex].tempoDuration != 0) {
                gActiveSeqs[playerIndex].tempoCur -= gActiveSeqs[playerIndex].tempoVelocity;
            } else {
                gActiveSeqs[playerIndex].tempoCur = gActiveSeqs[playerIndex].tempoTarget;
            }
            // Set tempo
            Audio_QueueCmdS32(0x47000000 | _SHIFTL(playerIndex, 16, 8), gActiveSeqs[playerIndex].tempoCur);
        }

        // Update channel volumes
        if (gActiveSeqs[playerIndex].volChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < 16; channelIndex++) {
                if (gActiveSeqs[playerIndex].channelData[channelIndex].volDuration != 0) {
                    gActiveSeqs[playerIndex].channelData[channelIndex].volDuration--;
                    if (gActiveSeqs[playerIndex].channelData[channelIndex].volDuration != 0) {
                        gActiveSeqs[playerIndex].channelData[channelIndex].volCur -=
                            gActiveSeqs[playerIndex].channelData[channelIndex].volVelocity;
                    } else {
                        gActiveSeqs[playerIndex].channelData[channelIndex].volCur =
                            gActiveSeqs[playerIndex].channelData[channelIndex].volTarget;
                        gActiveSeqs[playerIndex].volChannelFlags ^= (1 << channelIndex);
                    }
                    // CHAN_UPD_VOL_SCALE
                    Audio_QueueCmdF32(0x01000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(channelIndex, 8, 8),
                                      gActiveSeqs[playerIndex].channelData[channelIndex].volCur);
                }
            }
        }

        // Update frequencies
        if (gActiveSeqs[playerIndex].freqScaleChannelFlags != 0) {
            for (channelIndex = 0; channelIndex < 16; channelIndex++) {
                if (gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleDuration != 0) {
                    gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleDuration--;
                    if (gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleDuration != 0) {
                        gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleCur -=
                            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleVelocity;
                    } else {
                        gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleCur =
                            gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleTarget;
                        gActiveSeqs[playerIndex].freqScaleChannelFlags ^= (1 << channelIndex);
                    }
                    // CHAN_UPD_FREQ_SCALE
                    Audio_QueueCmdF32(0x04000000 | _SHIFTL(playerIndex, 16, 8) | _SHIFTL(channelIndex, 8, 8),
                                      gActiveSeqs[playerIndex].channelData[channelIndex].freqScaleCur);
                }
            }
        }

        // Process setup commands
        if (gActiveSeqs[playerIndex].setupCmdNum != 0) {
            // If there is a SeqCmd to reset the audio heap queued, then drop all setup commands
            if (!Audio_IsSeqCmdNotQueued(SEQ_CMD_RESET_HEAP << 28, SEQ_CMD_MASK)) {
                gActiveSeqs[playerIndex].setupCmdNum = 0;
                return;
            }

            // Only process setup commands once the timer reaches zero
            if (gActiveSeqs[playerIndex].setupCmdTimer != 0) {
                gActiveSeqs[playerIndex].setupCmdTimer--;
                continue;
            }

            // Only process setup commands if the playerIndex if no longer playing
            // i.e. the seqPlayer is no longer enabled
            if (gAudioContext.seqPlayers[playerIndex].enabled) {
                continue;
            }

            for (j = 0; j < gActiveSeqs[playerIndex].setupCmdNum; j++) {
                setupOp = (gActiveSeqs[playerIndex].setupCmd[j] & 0x00F00000) >> 20;
                playerIndexTarget = (gActiveSeqs[playerIndex].setupCmd[j] & 0x000F0000) >> 16;
                setupVal2 = (gActiveSeqs[playerIndex].setupCmd[j] & 0xFF00) >> 8;
                setupVal1 = gActiveSeqs[playerIndex].setupCmd[j] & 0xFF;

                switch (setupOp) {
                    case SEQ_SUB_CMD_SETUP_RESTORE_VOLUME:
                        // Restore playerIndexTarget volume back to normal levels
                        Audio_SetVolumeScale(playerIndexTarget, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal1);
                        break;

                    case SEQ_SUB_CMD_SETUP_RESTORE_VOLUME_IF_QUEUED:
                        // Restore playerIndexTarget volume back to normal levels,
                        // but only if the number of sequence queue requests from sSeqRequests
                        // exactly matches the argument to the command
                        if (setupVal1 == sNumSeqRequests[playerIndex]) {
                            Audio_SetVolumeScale(playerIndexTarget, VOL_SCALE_INDEX_FANFARE, 0x7F, setupVal2);
                        }
                        break;

                    case SEQ_SUB_CMD_SETUP_SEQ_UNQUEUE:
                        // Unqueue playerIndex from sSeqRequests
                        // Note: this command does not work as intended as unqueueing
                        // the seqeuence relies on gActiveSeqs[playerIndex].seqId
                        // However, gActiveSeqs[playerIndex].seqId is reset before the sequence on (playerIndex)
                        // is no longer playing, i.e. before this command can run.
                        // A simple fix would have been to unqueue based on gActiveSeqs[playerIndex].prevSeqId instead
                        AudioSeqCmd_UnqueueSequence(playerIndex, 0, gActiveSeqs[playerIndex].seqId);
                        break;

                    case SEQ_SUB_CMD_SETUP_RESTART_SEQ:
                        // Restart the currently active sequence on playerIndexTarget with full volume.
                        // Sequence on playerIndexTarget must still be active to play (can be muted)
                        AudioSeqCmd_PlaySequence(playerIndexTarget, 1, 0, gActiveSeqs[playerIndexTarget].seqId);
                        gActiveSeqs[playerIndexTarget].fadeVolUpdate = 1;
                        gActiveSeqs[playerIndexTarget].volScales[VOL_SCALE_INDEX_FANFARE] = 0x7F;
                        break;

                    case SEQ_SUB_CMD_SETUP_TEMPO_SCALE:
                        // Scale tempo by a multiplicative factor
                        AudioSeqCmd_ScaleTempo(playerIndexTarget, setupVal2, setupVal1);
                        break;

                    case SEQ_SUB_CMD_SETUP_TEMPO_RESET:
                        // Reset tempo to previous tempo
                        AudioSeqCmd_ResetTempo(playerIndexTarget, setupVal1);
                        break;

                    case SEQ_SUB_CMD_SETUP_PLAY_SEQ:
                        // Play the requested sequence
                        // Uses the fade timer set by SEQ_SUB_CMD_SETUP_SET_FADE_TIMER
                        seqId = gActiveSeqs[playerIndex].setupCmd[j] & 0xFFFF;
                        AudioSeqCmd_PlaySequence(playerIndexTarget, gActiveSeqs[playerIndexTarget].setupFadeTimer, 0,
                                                 seqId);
                        Audio_SetVolumeScale(playerIndexTarget, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
                        gActiveSeqs[playerIndexTarget].setupFadeTimer = 0;
                        break;

                    case SEQ_SUB_CMD_SETUP_SET_FADE_TIMER:
                        // A command specifically to support SEQ_SUB_CMD_SETUP_PLAY_SEQ
                        // Sets the fade timer for the sequence requested in SEQ_SUB_CMD_SETUP_PLAY_SEQ
                        gActiveSeqs[playerIndex].setupFadeTimer = setupVal2;
                        break;

                    case SEQ_SUB_CMD_SETUP_RESTORE_VOLUME_WITH_SCALE:
                        // Restore the volume back to default levels
                        // Allows a scaleIndex to be specified.
                        Audio_SetVolumeScale(playerIndexTarget, setupVal2, 0x7F, setupVal1);
                        break;

                    case SEQ_SUB_CMD_SETUP_POP_CACHE:
                        // Discard audio data by popping one more more audio caches from the audio heap
                        if (setupVal1 & (1 << SEQUENCE_TABLE)) {
                            Audio_QueueCmdS32(0xE3000000, SEQUENCE_TABLE);
                        }
                        if (setupVal1 & (1 << FONT_TABLE)) {
                            Audio_QueueCmdS32(0xE3000000, FONT_TABLE);
                        }
                        if (setupVal1 & (1 << SAMPLE_TABLE)) {
                            Audio_QueueCmdS32(0xE3000000, SAMPLE_TABLE);
                        }
                        break;

                    case SEQ_SUB_CMD_SETUP_DISABLE_CHANNELS:
                        // Disable (or reenable) specific channels of playerIndexTarget
                        channelMask = gActiveSeqs[playerIndex].setupCmd[j] & 0xFFFF;
                        AudioSeqCmd_DisableChannels(playerIndexTarget, channelMask);
                        break;

                    case SEQ_SUB_CMD_SETUP_SET_PLAYER_FREQ:
                        // Scale all channels of playerIndexTarget
                        AudioSeqCmd_SetPlayerFreq(playerIndexTarget, setupVal2, (setupVal1 * 10) & 0xFFFF);
                        break;
                }
            }

            gActiveSeqs[playerIndex].setupCmdNum = 0;
        }
    }
}

u8 func_800FAD34(void) {
    if (D_80133418 != 0) {
        if (D_80133418 == 1) {
            if (func_800E5EDC() == 1) {
                D_80133418 = 0;
                Audio_QueueCmdS8(0x46020000, gSfxChannelLayout);
                func_800F7170();
            }
        } else if (D_80133418 == 2) {
            while (func_800E5EDC() != 1) {}
            D_80133418 = 0;
            Audio_QueueCmdS8(0x46020000, gSfxChannelLayout);
            func_800F7170();
        }
    }

    return D_80133418;
}

void Audio_ResetSequences(void) {
    u8 playerIndex;
    u8 scaleIndex;

    for (playerIndex = 0; playerIndex < 4; playerIndex++) {
        sNumSeqRequests[playerIndex] = 0;

        gActiveSeqs[playerIndex].seqId = NA_BGM_DISABLED;
        gActiveSeqs[playerIndex].prevSeqId = NA_BGM_DISABLED;
        gActiveSeqs[playerIndex].tempoDuration = 0;
        gActiveSeqs[playerIndex].tempoOriginal = 0;
        gActiveSeqs[playerIndex].tempoCmd = 0;
        gActiveSeqs[playerIndex].channelPortMask = 0;
        gActiveSeqs[playerIndex].setupCmdNum = 0;
        gActiveSeqs[playerIndex].setupFadeTimer = 0;
        gActiveSeqs[playerIndex].freqScaleChannelFlags = 0;
        gActiveSeqs[playerIndex].volChannelFlags = 0;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            gActiveSeqs[playerIndex].volScales[scaleIndex] = 0x7F;
        }

        gActiveSeqs[playerIndex].volFadeTimer = 1;
        gActiveSeqs[playerIndex].fadeVolUpdate = 1;
    }
}

void Audio_ResetSequencesAndVolume(void) {
    u8 playerIndex;
    u8 scaleIndex;

    for (playerIndex = 0; playerIndex < 4; playerIndex++) {
        gActiveSeqs[playerIndex].volCur = 1.0f;
        gActiveSeqs[playerIndex].volDuration = 0;
        gActiveSeqs[playerIndex].fadeVolUpdate = 0;
        for (scaleIndex = 0; scaleIndex < VOL_SCALE_INDEX_MAX; scaleIndex++) {
            gActiveSeqs[playerIndex].volScales[scaleIndex] = 0x7F;
        }
    }
    Audio_ResetSequences();
}

/* ====== CUSTOM DEBUG FOR Audio_QueueSeqCmd COMMANDS ====== */

typedef enum {
    /* 0x0 */ PAGE_SEQ,
    /* 0x1 */ PAGE_PLAYER,
    /* 0x2 */ PAGE_CHANNEL,
    /* 0x3 */ PAGE_SETUP1,
    /* 0x4 */ PAGE_SETUP2,
    /* 0x5 */ PAGE_MISC,
    /* 0x6 */ PAGE_MAX
} SeqCmdDebugPage;

typedef enum {
    /* 0x0 */ PAGE_SEQ_TITLE,
    /* 0x1 */ PAGE_SEQ_PLAY,
    /* 0x2 */ PAGE_SEQ_STOP,
    /* 0x3 */ PAGE_SEQ_QUEUE,
    /* 0x4 */ PAGE_SEQ_UNQUEUE,
    /* 0x5 */ PAGE_SEQ_MAX
} SeqCmdDebugSeqPageCmd;

u32 sSeqCmdDebugInputButtonPress = 0;
u32 sSeqCmdDebugInputButtonLast = 0;
s8 sSeqCmdDebugPage = PAGE_SEQ;
u8 gIsSeqCmdDebugEnabled = false;
s8 sIsSeqCmdDebugCmdArgAdj = false;

// Sequence Page Menuing
// Current command selected
s8 sSeqCmdDebugSeqCmdSel = PAGE_SEQ_TITLE; // on PAGE_SEQ
s8 sSeqCmdDebugSeqCmdArgSel = 0;
s8 sIsSeqCmdDebugSeqCmdArgSel = false;
s8 sIsSeqCmdDebugSeqCmdNumArgs[] = { 0, 4, 2, 4, 3 };

// Player Page Menuing
s8 sSeqCmdDebugPlayerCmdSel = 0; // on PAGE_PLAYER
s8 sSeqCmdDebugPlayerCmdArgSel = 0;
s8 sIsSeqCmdDebugPlayerCmdArgSel = false;
s8 sIsSeqCmdDebugPlayerCmdNumArgs[] = { 0, 3, 3, 3, 3, 3, 3, 2, 3 };

// Channel Page Menuing
s8 sSeqCmdDebugChannelCmdSel = 0; // on PAGE_CHANNEL
s8 sSeqCmdDebugChannelCmdArgSel = 0;
s8 sIsSeqCmdDebugChannelCmdArgSel = false;
s8 sIsSeqCmdDebugChannelCmdNumArgs[] = { 0, 4, 4, 4, 2, 2 };

// Setup Page Menuing
s8 sSeqCmdDebugSetup1CmdSel = 0; // on PAGE_SETUP1
s8 sSeqCmdDebugSetup1CmdArgSel = 0;
s8 sIsSeqCmdDebugSetup1CmdArgSel = false;
s8 sIsSeqCmdDebugSetup1CmdNumArgs[] = { 0, 3, 1, 2, 4, 3, 3, 2 };

// Setup Page Menuing
s8 sSeqCmdDebugSetup2CmdSel = 0; // on PAGE_SETUP1
s8 sSeqCmdDebugSetup2CmdArgSel = 0;
s8 sIsSeqCmdDebugSetup2CmdArgSel = false;
s8 sIsSeqCmdDebugSetup2CmdNumArgs[] = { 0, 4, 4, 3, 4, 3, 1 };

// Global Page Menuing
s8 sSeqCmdDebugGlobalCmdSel = 0; // on PAGE_MISC
s8 sSeqCmdDebugGlobalCmdArgSel = 0;
s8 sIsSeqCmdDebugGlobalCmdArgSel = false;
s8 sIsSeqCmdDebugGlobalCmdNumArgs[] = { 0, 1, 1, 2 };

// Previous Command (to Display)
u32 sSeqCmdDebugCmd = 0;
u8 sSeqCmdDebugCmdTimer = 200;

char sSeqCmdDebugPlayerIndexStr[4][9] = { "MAIN BGM", "FANFARE", "SFX", "SUB BGM" };
s8 sSeqCmdDebugPlayerIndex = SEQ_PLAYER_BGM_MAIN;

// sequence args
u8 sSeqCmdDebugFadeTimer = 0;
u8 sSeqCmdDebugSeqArgs = 0;
u8 sSeqCmdDebugPriority = 0;
s8 sSeqCmdDebugSeqId = 0;

// player args
u8 sSeqCmdDebugDuration = 0;
s8 sSeqCmdDebugPort = 0;
u8 sSeqCmdDebugVal = 0;
u8 sSeqCmdDebugVolume = 127;
u16 sSeqCmdDebugFreq = 1000;
u8 sSeqCmdDebugTempoAbs = 120;
u8 sSeqCmdDebugTempoRel = 0;
u8 sSeqCmdDebugTempoScale = 100;

// channel args
s8 sSeqCmdDebugChannelIndex = 0;
u16 sSeqCmdDebugChannelMask = 0;
s8 sSeqCmdDebugChannelMaskToggle = 0;
// Hack to print in binary
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                                                          \
    (byte & 0x8000 ? '1' : '0'), (byte & 0x4000 ? '1' : '0'), (byte & 0x2000 ? '1' : '0'),                            \
        (byte & 0x1000 ? '1' : '0'), (byte & 0x800 ? '1' : '0'), (byte & 0x400 ? '1' : '0'),                          \
        (byte & 0x200 ? '1' : '0'), (byte & 0x100 ? '1' : '0'), (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'),   \
        (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0')

// setup args
s8 sSeqCmdDebugPlayerTargetIndex = 0;
u8 sSeqCmdDebugNumSeq = 0;
u8 sSeqCmdDebugVolumeScale = 0;
u8 sSeqCmdDebugTableTypeFlag = 0;
s8 sSeqCmdDebugSoundMode = 0;
u8 sSeqCmdDebugIsSeqDisabled = false;
s8 sSeqCmdDebugSfxChannelLayout = 0;
s8 sSeqCmdDebugSpecId = 0;

char sSoundModeNamesDebug[4][8] = {
    "STEREO",
    "HEADSET",
    "UNK",
    "MONO",
};
char sSeqNames[110][17] = {
    "GENERAL_SFX",
    "NATURE_AMBIENCE",
    "FIELD_LOGIC",
    "FIELD_INIT",
    "FIELD_DEFAULT_1",
    "FIELD_DEFAULT_2",
    "FIELD_DEFAULT_3",
    "FIELD_DEFAULT_4",
    "FIELD_DEFAULT_5",
    "FIELD_DEFAULT_6",
    "FIELD_DEFAULT_7",
    "FIELD_DEFAULT_8",
    "FIELD_DEFAULT_9",
    "FIELD_DEFAULT_A",
    "FIELD_DEFAULT_B",
    "FIELD_ENEMY_INIT",
    "FIELD_ENEMY_1",
    "FIELD_ENEMY_2",
    "FIELD_ENEMY_3",
    "FIELD_ENEMY_4",
    "FIELD_STILL_1",
    "FIELD_STILL_2",
    "FIELD_STILL_3",
    "FIELD_STILL_4",
    "DUNGEON",
    "KAKARIKO_ADULT",
    "ENEMY",
    "BOSS",
    "INSIDE_DEKU_TREE",
    "MARKET",
    "TITLE",
    "LINK_HOUSE",
    "GAME_OVER",
    "BOSS_CLEAR",
    "ITEM_GET",
    "OPENING_GANON",
    "HEART_GET",
    "OCA_LIGHT",
    "JABU_JABU",
    "KAKARIKO_KID",
    "GREAT_FAIRY",
    "ZELDA_THEME",
    "FIRE_TEMPLE",
    "OPEN_TRE_BOX",
    "FOREST_TEMPLE",
    "COURTYARD",
    "GANON_TOWER",
    "LONLON",
    "GORON_CITY",
    "FIELD_MORNING",
    "SPIRITUAL_STONE",
    "OCA_BOLERO",
    "OCA_MINUET",
    "OCA_SERENADE",
    "OCA_REQUIEM",
    "OCA_NOCTURNE",
    "MINI_BOSS",
    "SMALL_ITEM_GET",
    "TEMPLE_OF_TIME",
    "EVENT_CLEAR",
    "KOKIRI",
    "OCA_FAIRY_GET",
    "SARIA_THEME",
    "SPIRIT_TEMPLE",
    "HORSE",
    "HORSE_GOAL",
    "INGO",
    "MEDALLION_GET",
    "OCA_SARIA",
    "OCA_EPONA",
    "OCA_ZELDA",
    "OCA_SUNS",
    "OCA_TIME",
    "OCA_STORM",
    "NAVI_OPENING",
    "DEKU_TREE_CS",
    "WINDMILL",
    "HYRULE_CS",
    "MINI_GAME",
    "SHEIK",
    "ZORA_DOMAIN",
    "APPEAR",
    "ADULT_LINK",
    "MASTER_SWORD",
    "INTRO_GANON",
    "SHOP",
    "CHAMBER_OF_SAGES",
    "FILE_SELECT",
    "ICE_CAVERN",
    "DOOR_OF_TIME",
    "OWL",
    "SHADOW_TEMPLE",
    "WATER_TEMPLE",
    "BRIDGE_TO_GANONS",
    "OCARINA_OF_TIME",
    "GERUDO_VALLEY",
    "POTION_SHOP",
    "KOTAKE_KOUME",
    "ESCAPE",
    "UNDERGROUND",
    "GANONDORF_BOSS",
    "GANON_BOSS",
    "END_DEMO",
    "STAFF_1",
    "STAFF_2",
    "STAFF_3",
    "STAFF_4",
    "FIRE_BOSS",
    "TIMED_MINI_GAME",
    "VARIOUS_SFX"
};

void AudioSeqCmdDebug_ReadControllerInput(void) {
    Input inputs[4];
    Input* input = &inputs[0];
    u32 btn;

    PadMgr_RequestPadData(&gPadMgr, inputs, 0);

    btn = input->cur.button;
    sSeqCmdDebugInputButtonPress = (btn ^ sSeqCmdDebugInputButtonLast) & btn;
    sSeqCmdDebugInputButtonLast = btn;
}

void AudioSeqCmdDebug_UpdatePageSeq(void) {
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugSeqCmdArgSel = true;
                sSeqCmdDebugSeqCmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugSeqCmdArgSel = false;
            }
        }
    }

    // In left column (commands)
    if (!sIsSeqCmdDebugSeqCmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugSeqCmdSel) {
                case PAGE_SEQ_PLAY:
                    AudioSeqCmd_PlaySequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugFadeTimer, sSeqCmdDebugSeqArgs,
                                             sSeqCmdDebugSeqId);
                    sSeqCmdDebugCmd = ((SEQ_CMD_PLAY << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugFadeTimer) << 16) | ((u8)(sSeqCmdDebugSeqArgs) << 8) |
                                       (u16)(sSeqCmdDebugSeqId));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case PAGE_SEQ_STOP:
                    AudioSeqCmd_StopSequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugFadeTimer);
                    sSeqCmdDebugCmd = ((SEQ_CMD_STOP << 28) | 0xFF | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugFadeTimer) << 16));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case PAGE_SEQ_QUEUE:
                    AudioSeqCmd_QueueSequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugFadeTimer, sSeqCmdDebugPriority,
                                              sSeqCmdDebugSeqId);
                    sSeqCmdDebugCmd = ((SEQ_CMD_QUEUE << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugFadeTimer) << 16) | ((u8)(sSeqCmdDebugPriority) << 8) |
                                       (u8)(sSeqCmdDebugSeqId));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case PAGE_SEQ_UNQUEUE:
                    AudioSeqCmd_UnqueueSequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugFadeTimer, sSeqCmdDebugSeqId);
                    sSeqCmdDebugCmd = ((SEQ_CMD_UNQUEUE << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugFadeTimer) << 16) | (sSeqCmdDebugSeqId));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugSeqCmdSel--;
            if (sSeqCmdDebugSeqCmdSel < 0) {
                sSeqCmdDebugSeqCmdSel = PAGE_SEQ_MAX - 1;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugSeqCmdSel++;
            if (sSeqCmdDebugSeqCmdSel >= PAGE_SEQ_MAX) {
                sSeqCmdDebugSeqCmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugSeqCmdArgSel--;
                if (sSeqCmdDebugSeqCmdArgSel < 0) {
                    sSeqCmdDebugSeqCmdArgSel = sIsSeqCmdDebugSeqCmdNumArgs[sSeqCmdDebugSeqCmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugSeqCmdArgSel++;
                if (sSeqCmdDebugSeqCmdArgSel >= sIsSeqCmdDebugSeqCmdNumArgs[sSeqCmdDebugSeqCmdSel]) {
                    sSeqCmdDebugSeqCmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugSeqCmdSel) {
                case PAGE_SEQ_PLAY:
                    switch (sSeqCmdDebugSeqCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSeqArgs++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSeqArgs--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugSeqArgs += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugSeqArgs -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSeqId++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSeqId--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugSeqId += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugSeqId -= 10;
                            }
                            if (sSeqCmdDebugSeqId > 109) {
                                sSeqCmdDebugSeqId = 0;
                            }
                            if (sSeqCmdDebugSeqId < 0) {
                                sSeqCmdDebugSeqId = 109;
                            }
                            break;
                    }
                    break;

                case PAGE_SEQ_STOP:
                    switch (sSeqCmdDebugSeqCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;
                    }
                    break;

                case PAGE_SEQ_QUEUE:
                    switch (sSeqCmdDebugSeqCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPriority++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPriority--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugPriority += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugPriority -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSeqId++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSeqId--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugSeqId += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugSeqId -= 10;
                            }
                            if (sSeqCmdDebugSeqId > 109) {
                                sSeqCmdDebugSeqId = 0;
                            }
                            if (sSeqCmdDebugSeqId < 0) {
                                sSeqCmdDebugSeqId = 109;
                            }
                            break;
                    }
                    break;

                case PAGE_SEQ_UNQUEUE:
                    switch (sSeqCmdDebugSeqCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSeqId++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSeqId--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugSeqId += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugSeqId -= 10;
                            }
                            if (sSeqCmdDebugSeqId > 109) {
                                sSeqCmdDebugSeqId = 0;
                            }
                            if (sSeqCmdDebugSeqId < 0) {
                                sSeqCmdDebugSeqId = 109;
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

void AudioSeqCmdDebug_UpdatePagePlayer(void) {

    // Move left-right
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugPlayerCmdSel == 0) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugPlayerCmdArgSel = true;
                sSeqCmdDebugPlayerCmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugPlayerCmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugPlayerCmdArgSel = false;
            }
        }
    }

    // Move vertical in left column (commands)
    if (!sIsSeqCmdDebugPlayerCmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugPlayerCmdSel) {
                case 1:
                    AudioSeqCmd_SetPlayerVol(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugVolume);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SET_PLAYER_VOL << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((sSeqCmdDebugDuration) << 16) | (sSeqCmdDebugVolume));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 2:
                    AudioSeqCmd_SetPlayerFreq(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugFreq);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SET_PLAYER_FREQ << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((sSeqCmdDebugDuration) << 16) | (sSeqCmdDebugFreq));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 3:
                    AudioSeqCmd_SetTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugTempoAbs);
                    sSeqCmdDebugCmd = ((SEQ_CMD_TEMPO_CMD << 28) | (SEQ_SUB_CMD_TEMPO_SET << 12) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugDuration) << 16) |
                                       (u16)(sSeqCmdDebugTempoAbs));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 4:
                    AudioSeqCmd_SpeedUpTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugTempoRel);
                    sSeqCmdDebugCmd = ((SEQ_CMD_TEMPO_CMD << 28) | (SEQ_SUB_CMD_TEMPO_SPEED_UP << 12) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugDuration) << 16) |
                                       (u16)(sSeqCmdDebugTempoRel));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 5:
                    AudioSeqCmd_SlowDownTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugTempoRel);
                    sSeqCmdDebugCmd = ((SEQ_CMD_TEMPO_CMD << 28) | (SEQ_SUB_CMD_TEMPO_SLOW_DOWN << 12) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugDuration) << 16) |
                                       (u16)(sSeqCmdDebugTempoRel));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 6:
                    AudioSeqCmd_ScaleTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugTempoScale);
                    sSeqCmdDebugCmd = ((SEQ_CMD_TEMPO_CMD << 28) | (SEQ_SUB_CMD_TEMPO_SCALE << 12) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugDuration) << 16) |
                                       (u16)(sSeqCmdDebugTempoScale));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 7:
                    AudioSeqCmd_ResetTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration);
                    sSeqCmdDebugCmd = ((SEQ_CMD_TEMPO_CMD << 28) | (SEQ_SUB_CMD_TEMPO_RESET << 12) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugDuration) << 16));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 8:
                    AudioSeqCmd_SetPlayerIO(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPort, sSeqCmdDebugVal);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SET_PLAYER_IO << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPort) << 16) | (u8)(sSeqCmdDebugVal));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugPlayerCmdSel--;
            if (sSeqCmdDebugPlayerCmdSel < 0) {
                sSeqCmdDebugPlayerCmdSel = 8;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugPlayerCmdSel++;
            if (sSeqCmdDebugPlayerCmdSel >= 9) {
                sSeqCmdDebugPlayerCmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugPlayerCmdArgSel--;
                if (sSeqCmdDebugPlayerCmdArgSel < 0) {
                    sSeqCmdDebugPlayerCmdArgSel = sIsSeqCmdDebugPlayerCmdNumArgs[sSeqCmdDebugPlayerCmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugPlayerCmdArgSel++;
                if (sSeqCmdDebugPlayerCmdArgSel >= sIsSeqCmdDebugPlayerCmdNumArgs[sSeqCmdDebugPlayerCmdSel]) {
                    sSeqCmdDebugPlayerCmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugPlayerCmdSel) {
                case 1:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugVolume++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugVolume--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugVolume += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugVolume -= 10;
                            }
                            break;
                    }
                    break;

                case 2:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFreq++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFreq--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFreq += 100;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFreq -= 100;
                            }
                            break;
                    }
                    break;

                case 3:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugTempoAbs++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugTempoAbs--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugTempoAbs += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugTempoAbs -= 10;
                            }
                            break;
                    }
                    break;

                case 4:
                case 5:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugTempoRel++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugTempoRel--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugTempoRel += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugTempoRel -= 10;
                            }
                            break;
                    }
                    break;

                case 6:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugTempoScale++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugTempoScale--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugTempoScale += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugTempoScale -= 10;
                            }
                            break;
                    }
                    break;

                case 7:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;
                    }
                    break;

                case 8:
                    switch (sSeqCmdDebugPlayerCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPort++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPort--;
                            }
                            if (sSeqCmdDebugPort >= 8) {
                                sSeqCmdDebugPort = 0;
                            }
                            if (sSeqCmdDebugPort < 0) {
                                sSeqCmdDebugPort = 7;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugVal++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugVal--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugVal += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugVal -= 10;
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

// Move left-right
void AudioSeqCmdDebug_UpdatePageChannel(void) {
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugChannelCmdSel == 0) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugChannelCmdArgSel = true;
                sSeqCmdDebugChannelCmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugChannelCmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugChannelCmdArgSel = false;
            }
        }
    }

    // Move vertical in left column (commands)
    if (!sIsSeqCmdDebugChannelCmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugChannelCmdSel) {
                case 1:
                    AudioSeqCmd_SetChannelVol(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugChannelIndex,
                                              sSeqCmdDebugVolume);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SET_CHANNEL_VOL << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugDuration) << 16) | ((u8)(sSeqCmdDebugChannelIndex) << 8) |
                                       ((u8)sSeqCmdDebugVolume));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 2:
                    AudioSeqCmd_SetChannelFreq(sSeqCmdDebugPlayerIndex, sSeqCmdDebugDuration, sSeqCmdDebugChannelIndex,
                                               sSeqCmdDebugFreq);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SET_CHANNEL_FREQ << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                         ((sSeqCmdDebugDuration) << 16) | ((sSeqCmdDebugChannelIndex) << 12) | (sSeqCmdDebugFreq));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 3:
                    AudioSeqCmd_SetChannelIO(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPort, sSeqCmdDebugChannelIndex,
                                             sSeqCmdDebugVal);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SET_CHANNEL_IO << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPort) << 16) | ((u8)(sSeqCmdDebugChannelIndex) << 8) |
                                       (u8)(sSeqCmdDebugVal));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 4:
                    AudioSeqCmd_DisableChannelIO(sSeqCmdDebugPlayerIndex, sSeqCmdDebugChannelMask);
                    sSeqCmdDebugCmd = (_SHIFTL(SEQ_CMD_DISABLE_CHANNEL_IO, 28, 4) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) | (u16)(sSeqCmdDebugChannelMask));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 5:
                    AudioSeqCmd_DisableChannels(sSeqCmdDebugPlayerIndex, sSeqCmdDebugChannelMask);
                    sSeqCmdDebugCmd = ((SEQ_CMD_DISABLE_CHANNELS << 28) | ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       (u16)(sSeqCmdDebugChannelMask));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugChannelCmdSel--;
            if (sSeqCmdDebugChannelCmdSel < 0) {
                sSeqCmdDebugChannelCmdSel = 5;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugChannelCmdSel++;
            if (sSeqCmdDebugChannelCmdSel > 5) {
                sSeqCmdDebugChannelCmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugChannelCmdArgSel--;
                if (sSeqCmdDebugChannelCmdArgSel < 0) {
                    sSeqCmdDebugChannelCmdArgSel = sIsSeqCmdDebugChannelCmdNumArgs[sSeqCmdDebugChannelCmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugChannelCmdArgSel++;
                if (sSeqCmdDebugChannelCmdArgSel >= sIsSeqCmdDebugChannelCmdNumArgs[sSeqCmdDebugChannelCmdSel]) {
                    sSeqCmdDebugChannelCmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugChannelCmdSel) {
                case 1:
                    switch (sSeqCmdDebugChannelCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugChannelIndex++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugChannelIndex--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugChannelIndex += 4;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugChannelIndex -= 4;
                            }
                            if (sSeqCmdDebugChannelIndex >= 16) {
                                sSeqCmdDebugChannelIndex = 0;
                            }
                            if (sSeqCmdDebugChannelIndex < 0) {
                                sSeqCmdDebugChannelIndex = 15;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugVolume++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugVolume--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugVolume += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugVolume -= 10;
                            }
                            break;
                    }
                    break;

                case 2:
                    switch (sSeqCmdDebugChannelCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugChannelIndex++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugChannelIndex--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugChannelIndex += 4;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugChannelIndex -= 4;
                            }
                            if (sSeqCmdDebugChannelIndex >= 16) {
                                sSeqCmdDebugChannelIndex = 0;
                            }
                            if (sSeqCmdDebugChannelIndex < 0) {
                                sSeqCmdDebugChannelIndex = 15;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFreq++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFreq--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFreq += 100;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFreq -= 100;
                            }
                            break;
                    }
                    break;

                case 3:
                    switch (sSeqCmdDebugChannelCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugChannelIndex++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugChannelIndex--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugChannelIndex += 4;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugChannelIndex -= 4;
                            }
                            if (sSeqCmdDebugChannelIndex >= 16) {
                                sSeqCmdDebugChannelIndex = 0;
                            }
                            if (sSeqCmdDebugChannelIndex < 0) {
                                sSeqCmdDebugChannelIndex = 15;
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPort++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPort--;
                            }
                            if (sSeqCmdDebugPort >= 8) {
                                sSeqCmdDebugPort = 0;
                            }
                            if (sSeqCmdDebugPort < 0) {
                                sSeqCmdDebugPort = 7;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugVal++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugVal--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugVal += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugVal -= 10;
                            }
                            break;
                    }
                    break;

                case 4:
                case 5:
                    switch (sSeqCmdDebugChannelCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugChannelMask |= 1 << sSeqCmdDebugChannelMaskToggle;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugChannelMask &= ~(1 << sSeqCmdDebugChannelMaskToggle);
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugChannelMaskToggle--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugChannelMaskToggle++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_R)) {
                                sSeqCmdDebugChannelMask = 0xFFFF;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_Z)) {
                                sSeqCmdDebugChannelMask = 0;
                            }
                            sSeqCmdDebugChannelMaskToggle = CLAMP(sSeqCmdDebugChannelMaskToggle, 0, 15);
                            break;
                    }
                    break;
            }
        }
    }
}

void AudioSeqCmdDebug_UpdatePageSetup1(void) {
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugSetup1CmdSel == 0) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugSetup1CmdArgSel = true;
                sSeqCmdDebugSetup1CmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugSetup1CmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugSetup1CmdArgSel = false;
            }
        }
    }

    // Move vertical in left column (commands)
    if (!sIsSeqCmdDebugSetup1CmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugSetup1CmdSel) {
                case 1:
                    AudioSeqCmd_SetupRestorePlayerVolume(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                         sSeqCmdDebugFadeTimer);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_RESTORE_VOLUME << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) | (u8)(sSeqCmdDebugFadeTimer));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 2:
                    AudioSeqCmd_SetupUnqueueSequence(sSeqCmdDebugPlayerTargetIndex);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_SEQ_UNQUEUE << 20) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 24));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 3:
                    AudioSeqCmd_SetupRestartSequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_RESTART_SEQ << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 4:
                    AudioSeqCmd_SetupScaleTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                sSeqCmdDebugDuration, sSeqCmdDebugTempoScale);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_TEMPO_SCALE << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) |
                         ((u8)(sSeqCmdDebugDuration) << 8) | (u8)(sSeqCmdDebugTempoScale));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 5:
                    AudioSeqCmd_SetupResetTempo(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                sSeqCmdDebugDuration);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_TEMPO_RESET << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) | (u8)(sSeqCmdDebugDuration));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 6:
                    AudioSeqCmd_SetupPlaySequence(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                  sSeqCmdDebugSeqId);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_PLAY_SEQ << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) | (u16)(sSeqCmdDebugSeqId));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 7:
                    AudioSeqCmd_SetupSetFadeInTimer(sSeqCmdDebugPlayerIndex, sSeqCmdDebugFadeTimer);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_SET_FADE_TIMER << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) |
                         ((u8)(sSeqCmdDebugFadeTimer) << 8));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugSetup1CmdSel--;
            if (sSeqCmdDebugSetup1CmdSel < 0) {
                sSeqCmdDebugSetup1CmdSel = 7;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugSetup1CmdSel++;
            if (sSeqCmdDebugSetup1CmdSel > 7) {
                sSeqCmdDebugSetup1CmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugSetup1CmdArgSel--;
                if (sSeqCmdDebugSetup1CmdArgSel < 0) {
                    sSeqCmdDebugSetup1CmdArgSel = sIsSeqCmdDebugSetup1CmdNumArgs[sSeqCmdDebugSetup1CmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugSetup1CmdArgSel++;
                if (sSeqCmdDebugSetup1CmdArgSel >= sIsSeqCmdDebugSetup1CmdNumArgs[sSeqCmdDebugSetup1CmdSel]) {
                    sSeqCmdDebugSetup1CmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugSetup1CmdSel) {
                case 1:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;
                    }
                    break;

                case 2:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;
                    }
                    break;

                case 3:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;
                    }
                    break;

                case 4:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugTempoScale++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugTempoScale--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugTempoScale += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugTempoScale -= 10;
                            }
                            break;
                    }
                    break;

                case 5:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;
                    }
                    break;

                case 6:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSeqId++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSeqId--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugSeqId += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugSeqId -= 10;
                            }
                            if (sSeqCmdDebugSeqId > 109) {
                                sSeqCmdDebugSeqId = 0;
                            }
                            if (sSeqCmdDebugSeqId < 0) {
                                sSeqCmdDebugSeqId = 109;
                            }
                            break;
                    }
                    break;

                case 7:
                    switch (sSeqCmdDebugSetup1CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

void AudioSeqCmdDebug_UpdatePageSetup2(void) {
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugSetup2CmdSel == 0) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugSetup2CmdArgSel = true;
                sSeqCmdDebugSetup2CmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugSetup2CmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugSetup2CmdArgSel = false;
            }
        }
    }

    // Move vertical in left column (commands)
    if (!sIsSeqCmdDebugSetup2CmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugSetup2CmdSel) {
                case 1:
                    AudioSeqCmd_SetupRestorePlayerVolumeIfQueued(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                                 sSeqCmdDebugFadeTimer, sSeqCmdDebugNumSeq);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_RESTORE_VOLUME_IF_QUEUED << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) |
                         ((u8)(sSeqCmdDebugFadeTimer) << 8) | (u8)(sSeqCmdDebugNumSeq));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 2:
                    AudioSeqCmd_SetupRestorePlayerVolumeWithScale(sSeqCmdDebugPlayerIndex,
                                                                  sSeqCmdDebugPlayerTargetIndex,
                                                                  sSeqCmdDebugVolumeScale, sSeqCmdDebugFadeTimer);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_RESTORE_VOLUME_WITH_SCALE << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) |
                         ((u8)(sSeqCmdDebugVolumeScale) << 8) | (u8)(sSeqCmdDebugFadeTimer));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 3:
                    AudioSeqCmd_SetupDisableChannels(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                     sSeqCmdDebugChannelMask);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_DISABLE_CHANNELS << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) | (u16)(sSeqCmdDebugChannelMask));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 4:
                    AudioSeqCmd_SetupSetPlayerFreq(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                                   sSeqCmdDebugDuration, sSeqCmdDebugFreq);
                    sSeqCmdDebugCmd =
                        ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_SET_PLAYER_FREQ << 20) |
                         ((u8)(sSeqCmdDebugPlayerIndex) << 24) | ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) |
                         ((u8)(sSeqCmdDebugDuration) << 8) | (u8)(sSeqCmdDebugFreq));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 5:
                    AudioSeqCmd_SetupPopCache(sSeqCmdDebugPlayerIndex, sSeqCmdDebugPlayerTargetIndex,
                                              sSeqCmdDebugTableTypeFlag);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_POP_CACHE << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24) |
                                       ((u8)(sSeqCmdDebugPlayerTargetIndex) << 16) | ((u8)sSeqCmdDebugTableTypeFlag));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 6:
                    AudioSeqCmd_ResetSetupCmds(sSeqCmdDebugPlayerIndex);
                    sSeqCmdDebugCmd = ((SEQ_CMD_SETUP_CMD << 28) | (SEQ_SUB_CMD_SETUP_RESET_SETUP_CMDS << 20) |
                                       ((u8)(sSeqCmdDebugPlayerIndex) << 24));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugSetup2CmdSel--;
            if (sSeqCmdDebugSetup2CmdSel < 0) {
                sSeqCmdDebugSetup2CmdSel = 6;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugSetup2CmdSel++;
            if (sSeqCmdDebugSetup2CmdSel > 6) {
                sSeqCmdDebugSetup2CmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugSetup2CmdArgSel--;
                if (sSeqCmdDebugSetup2CmdArgSel < 0) {
                    sSeqCmdDebugSetup2CmdArgSel = sIsSeqCmdDebugSetup2CmdNumArgs[sSeqCmdDebugSetup2CmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugSetup2CmdArgSel++;
                if (sSeqCmdDebugSetup2CmdArgSel >= sIsSeqCmdDebugSetup2CmdNumArgs[sSeqCmdDebugSetup2CmdSel]) {
                    sSeqCmdDebugSetup2CmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugSetup2CmdSel) {
                case 1:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugNumSeq++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugNumSeq--;
                            }
                            break;
                    }
                    break;

                case 2:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugVolumeScale++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugVolumeScale--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugVolumeScale += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugVolumeScale -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFadeTimer++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFadeTimer--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFadeTimer += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFadeTimer -= 10;
                            }
                            break;
                    }
                    break;

                case 3:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugChannelMask |= 1 << sSeqCmdDebugChannelMaskToggle;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugChannelMask &= ~(1 << sSeqCmdDebugChannelMaskToggle);
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugChannelMaskToggle--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugChannelMaskToggle++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_R)) {
                                sSeqCmdDebugChannelMask = 0xFFFF;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_Z)) {
                                sSeqCmdDebugChannelMask = 0;
                            }
                            sSeqCmdDebugChannelMaskToggle = CLAMP(sSeqCmdDebugChannelMaskToggle, 0, 15);
                            break;
                    }
                    break;

                case 4:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugDuration++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugDuration--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugDuration += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugDuration -= 10;
                            }
                            break;

                        case 3:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugFreq++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugFreq--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugFreq += 100;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugFreq -= 100;
                            }
                            break;
                    }
                    break;

                case 5:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerTargetIndex++;
                                if (sSeqCmdDebugPlayerTargetIndex > 3) {
                                    sSeqCmdDebugPlayerTargetIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerTargetIndex--;
                                if (sSeqCmdDebugPlayerTargetIndex < 0) {
                                    sSeqCmdDebugPlayerTargetIndex = 3;
                                }
                            }
                            break;

                        case 2:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugTableTypeFlag++;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugTableTypeFlag--;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
                                sSeqCmdDebugTableTypeFlag += 10;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
                                sSeqCmdDebugTableTypeFlag -= 10;
                            }
                            break;
                    }
                    break;

                case 6:
                    switch (sSeqCmdDebugSetup2CmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugPlayerIndex++;
                                if (sSeqCmdDebugPlayerIndex >= 4) {
                                    sSeqCmdDebugPlayerIndex = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugPlayerIndex--;
                                if (sSeqCmdDebugPlayerIndex < 0) {
                                    sSeqCmdDebugPlayerIndex = 3;
                                }
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

void AudioSeqCmdDebug_UpdatePageGlobal(void) {
    if (!sIsSeqCmdDebugCmdArgAdj) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DRIGHT)) {
            if (sSeqCmdDebugGlobalCmdSel == 0) {
                sSeqCmdDebugPage++;
                if (sSeqCmdDebugPage >= PAGE_MAX) {
                    sSeqCmdDebugPage = PAGE_SEQ;
                }
            } else {
                sIsSeqCmdDebugGlobalCmdArgSel = true;
                sSeqCmdDebugGlobalCmdArgSel = 0;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DLEFT)) {
            if (sSeqCmdDebugGlobalCmdSel == PAGE_SEQ_TITLE) {
                sSeqCmdDebugPage--;
                if (sSeqCmdDebugPage < 0) {
                    sSeqCmdDebugPage = PAGE_MAX - 1;
                }
            } else {
                sIsSeqCmdDebugGlobalCmdArgSel = false;
            }
        }
    }

    // Move vertical in left column (commands)
    if (!sIsSeqCmdDebugGlobalCmdArgSel) {
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            switch (sSeqCmdDebugGlobalCmdSel) {
                case 1:
                    AudioSeqCmd_SetSoundMode(sSeqCmdDebugSoundMode);
                    sSeqCmdDebugCmd = ((SEQ_CMD_GLOBAL_CMD << 28) | (SEQ_SUB_CMD_GLOBAL_SET_SOUND_MODE << 8) |
                                       (u8)(sSeqCmdDebugSoundMode));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 2:
                    AudioSeqCmd_DisableNewSequences(sSeqCmdDebugIsSeqDisabled);
                    sSeqCmdDebugCmd = ((SEQ_CMD_GLOBAL_CMD << 28) | (SEQ_SUB_CMD_GLOBAL_DISABLE_NEW_SEQUENCES << 8) |
                                       (u16)(sSeqCmdDebugIsSeqDisabled));
                    sSeqCmdDebugCmdTimer = 200;
                    break;

                case 3:
                    AudioSeqCmd_ResetAudioHeap(sSeqCmdDebugSfxChannelLayout, sSeqCmdDebugSpecId);
                    sSeqCmdDebugCmd = ((SEQ_CMD_RESET_HEAP << 28) | ((u8)(sSeqCmdDebugSfxChannelLayout) << 8) |
                                       (u8)(sSeqCmdDebugSpecId));
                    sSeqCmdDebugCmdTimer = 200;
                    break;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
            sSeqCmdDebugGlobalCmdSel--;
            if (sSeqCmdDebugGlobalCmdSel < 0) {
                sSeqCmdDebugGlobalCmdSel = 3;
            }
        }
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
            sSeqCmdDebugGlobalCmdSel++;
            if (sSeqCmdDebugGlobalCmdSel > 3) {
                sSeqCmdDebugGlobalCmdSel = 0;
            }
        }
    } else {
        // In right column (arguments)
        if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_L)) {
            sIsSeqCmdDebugCmdArgAdj ^= 1;
        }
        if (!sIsSeqCmdDebugCmdArgAdj) {
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                sSeqCmdDebugGlobalCmdArgSel--;
                if (sSeqCmdDebugGlobalCmdArgSel < 0) {
                    sSeqCmdDebugGlobalCmdArgSel = sIsSeqCmdDebugGlobalCmdNumArgs[sSeqCmdDebugGlobalCmdSel] - 1;
                }
            }
            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                sSeqCmdDebugGlobalCmdArgSel++;
                if (sSeqCmdDebugGlobalCmdArgSel >= sIsSeqCmdDebugGlobalCmdNumArgs[sSeqCmdDebugGlobalCmdSel]) {
                    sSeqCmdDebugGlobalCmdArgSel = 0;
                }
            }
        } else {
            switch (sSeqCmdDebugGlobalCmdSel) {
                case 1:
                    switch (sSeqCmdDebugGlobalCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSoundMode++;
                                if (sSeqCmdDebugSoundMode > 3) {
                                    sSeqCmdDebugSoundMode = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSoundMode--;
                                if (sSeqCmdDebugSoundMode < 0) {
                                    sSeqCmdDebugSoundMode = 3;
                                }
                            }
                            break;
                    }
                    break;

                case 2:
                    switch (sSeqCmdDebugGlobalCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugIsSeqDisabled = true;
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugIsSeqDisabled = false;
                            }
                            break;
                    }
                    break;

                case 3:
                    switch (sSeqCmdDebugGlobalCmdArgSel) {
                        case 0:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSfxChannelLayout++;
                                if (sSeqCmdDebugSfxChannelLayout > 3) {
                                    sSeqCmdDebugSfxChannelLayout = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSfxChannelLayout--;
                                if (sSeqCmdDebugSfxChannelLayout < 0) {
                                    sSeqCmdDebugSfxChannelLayout = 3;
                                }
                            }
                            break;

                        case 1:
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DUP)) {
                                sSeqCmdDebugSpecId++;
                                if (sSeqCmdDebugSpecId > 17) {
                                    sSeqCmdDebugSpecId = 0;
                                }
                            }
                            if (CHECK_BTN_ANY(sSeqCmdDebugInputButtonPress, BTN_DDOWN)) {
                                sSeqCmdDebugSpecId--;
                                if (sSeqCmdDebugSpecId < 0) {
                                    sSeqCmdDebugSpecId = 17;
                                }
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

void AudioSeqCmdDebug_UpdatePage(void) {
    switch (sSeqCmdDebugPage) {
        case PAGE_SEQ:
            AudioSeqCmdDebug_UpdatePageSeq();
            break;

        case PAGE_PLAYER:
            AudioSeqCmdDebug_UpdatePagePlayer();
            break;

        case PAGE_CHANNEL:
            AudioSeqCmdDebug_UpdatePageChannel();
            break;

        case PAGE_SETUP1:
            AudioSeqCmdDebug_UpdatePageSetup1();
            break;

        case PAGE_SETUP2:
            AudioSeqCmdDebug_UpdatePageSetup2();
            break;

        case PAGE_MISC:
            AudioSeqCmdDebug_UpdatePageGlobal();
            break;
    }
}

void AudioSeqCmdDebug_Update(void) {

    AudioSeqCmdDebug_ReadControllerInput();

    if (CHECK_BTN_ALL(sSeqCmdDebugInputButtonPress, BTN_L | BTN_R)) {
        gIsSeqCmdDebugEnabled ^= 1;
    }

    if (gIsSeqCmdDebugEnabled) {
        AudioSeqCmdDebug_UpdatePage();
    }

    if (sSeqCmdDebugCmdTimer > 0) {
        sSeqCmdDebugCmdTimer -= 2;
    }
}

void AudioSeqCmdDebug_DrawPageSeq(GfxPrint* printer) {
    // Draw Page Title
    if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_TITLE) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Sequence SeqCmds");

    // Draw Command Options
    if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_PLAY) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Play Seq");

    if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_STOP) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Stop Seq");

    if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_QUEUE) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Queue Seq");

    if (sSeqCmdDebugSeqCmdSel == PAGE_SEQ_UNQUEUE) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 16);
    GfxPrint_Printf(printer, "Unqueue Seq");

    // Draw Arguments
    if (sSeqCmdDebugSeqCmdSel != 0) {
        if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 0)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 10);
        GfxPrint_Printf(printer, "playerIndex:");
        GfxPrint_SetColor(printer, 200, 200, 200, 0);
        GfxPrint_SetPos(printer, 23, 12);
        GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerIndex]);

        if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 1)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 14);
        GfxPrint_Printf(printer, "fadeTimer: %d", sSeqCmdDebugFadeTimer);
    }
    switch (sSeqCmdDebugSeqCmdSel) {
        case PAGE_SEQ_PLAY:
            if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "seqArgs: %d", sSeqCmdDebugSeqArgs);

            if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "seqId:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 20);
            GfxPrint_Printf(printer, "%s", sSeqNames[sSeqCmdDebugSeqId]);
            break;

        case PAGE_SEQ_STOP:
            break;

        case PAGE_SEQ_QUEUE:
            if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "priority: %d", sSeqCmdDebugPriority);

            if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "seqId:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 20);
            GfxPrint_Printf(printer, "%s", sSeqNames[sSeqCmdDebugSeqId]);
            break;

        case PAGE_SEQ_UNQUEUE:
            if (sIsSeqCmdDebugSeqCmdArgSel && (sSeqCmdDebugSeqCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "seqId:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 18);
            GfxPrint_Printf(printer, "%s", sSeqNames[sSeqCmdDebugSeqId]);
            break;
    }
}

void AudioSeqCmdDebug_DrawPagePlayer(GfxPrint* printer) {
    // Draw Page Title
    if (sSeqCmdDebugPlayerCmdSel == 0) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Player SeqCmds");

    // Draw Command Options
    if (sSeqCmdDebugPlayerCmdSel == 1) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Set Volume");

    if (sSeqCmdDebugPlayerCmdSel == 2) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Set Frequency");

    if (sSeqCmdDebugPlayerCmdSel == 3) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Set Tempo");

    if (sSeqCmdDebugPlayerCmdSel == 4) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 16);
    GfxPrint_Printf(printer, "Speed Up Tempo");

    if (sSeqCmdDebugPlayerCmdSel == 5) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 18);
    GfxPrint_Printf(printer, "Slow Down Tempo");

    if (sSeqCmdDebugPlayerCmdSel == 6) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 20);
    GfxPrint_Printf(printer, "Scale Tempo");

    if (sSeqCmdDebugPlayerCmdSel == 7) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 22);
    GfxPrint_Printf(printer, "Reset Tempo");

    if (sSeqCmdDebugPlayerCmdSel == 8) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 24);
    GfxPrint_Printf(printer, "Set Player IO");

    // Draw Arguments
    if (sSeqCmdDebugPlayerCmdSel != 0) {
        if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 0)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 10);
        GfxPrint_Printf(printer, "playerIndex:");
        GfxPrint_SetColor(printer, 200, 200, 200, 0);
        GfxPrint_SetPos(printer, 23, 12);
        GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerIndex]);

        if (sSeqCmdDebugPlayerCmdSel != 8) {
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "duration: %d", sSeqCmdDebugDuration);
        }
    }

    switch (sSeqCmdDebugPlayerCmdSel) {
        case 1:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "volume: %d", sSeqCmdDebugVolume);
            break;

        case 2:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "frequency: %d", sSeqCmdDebugFreq);
            break;

        case 3:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "tempo (abs): %d", sSeqCmdDebugTempoAbs);
            break;

        case 4:
        case 5:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "tempo (rel): %d", sSeqCmdDebugTempoRel);
            break;

        case 6:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "tempo (scale): %d", sSeqCmdDebugTempoScale);
            break;

        case 8:
            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "port: %d", sSeqCmdDebugPort);

            if (sIsSeqCmdDebugPlayerCmdArgSel && (sSeqCmdDebugPlayerCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "ioData: %d", sSeqCmdDebugVal);
            break;
    }
}

void AudioSeqCmdDebug_DrawPageChannel(GfxPrint* printer) {
    if (sSeqCmdDebugChannelCmdSel == 0) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Channel SeqCmds");

    // Draw Command Options
    if (sSeqCmdDebugChannelCmdSel == 1) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Set Volume");

    if (sSeqCmdDebugChannelCmdSel == 2) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Set Frequency");

    if (sSeqCmdDebugChannelCmdSel == 3) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Set Chan IO");

    if (sSeqCmdDebugChannelCmdSel == 4) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 16);
    GfxPrint_Printf(printer, "Disable Chan IO");

    if (sSeqCmdDebugChannelCmdSel == 5) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 18);
    GfxPrint_Printf(printer, "Disable Chan");

    // Draw Arguments
    if (sSeqCmdDebugChannelCmdSel != 0) {
        if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 0)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 10);
        GfxPrint_Printf(printer, "playerIndex:");
        GfxPrint_SetColor(printer, 200, 200, 200, 0);
        GfxPrint_SetPos(printer, 23, 12);
        GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerIndex]);

        if ((sSeqCmdDebugChannelCmdSel != 4) && (sSeqCmdDebugChannelCmdSel != 5)) {
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "channelIndex: %d", sSeqCmdDebugChannelIndex);
        }

        if ((sSeqCmdDebugChannelCmdSel != 3) && (sSeqCmdDebugChannelCmdSel != 4) && (sSeqCmdDebugChannelCmdSel != 5)) {
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "duration: %d", sSeqCmdDebugDuration);
        }
    }

    switch (sSeqCmdDebugChannelCmdSel) {
        case 1:
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "volume: %d", sSeqCmdDebugVolume);
            break;

        case 2:
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "frequency: %d", sSeqCmdDebugFreq);
            break;

        case 3:
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 16);
            GfxPrint_Printf(printer, "port: %d", sSeqCmdDebugPort);

            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "ioData: %d", sSeqCmdDebugVal);
            break;

        case 4:
        case 5:
            if (sIsSeqCmdDebugChannelCmdArgSel && (sSeqCmdDebugChannelCmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "channel mask:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 16);
            GfxPrint_Printf(printer, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sSeqCmdDebugChannelMask));
            break;
    }
}

void AudioSeqCmdDebug_DrawPageSetup1(GfxPrint* printer) {
    if (sSeqCmdDebugSetup1CmdSel == 0) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Setup Part 1 SeqCmds");

    // Draw Command Options
    if (sSeqCmdDebugSetup1CmdSel == 1) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Restore Vol");

    if (sSeqCmdDebugSetup1CmdSel == 2) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Unqueue Seq");

    if (sSeqCmdDebugSetup1CmdSel == 3) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Restart Seq");

    if (sSeqCmdDebugSetup1CmdSel == 4) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 16);
    GfxPrint_Printf(printer, "Scale Tempo");

    if (sSeqCmdDebugSetup1CmdSel == 5) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 18);
    GfxPrint_Printf(printer, "Reset Tempo");

    if (sSeqCmdDebugSetup1CmdSel == 6) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 20);
    GfxPrint_Printf(printer, "Play Seq");

    if (sSeqCmdDebugSetup1CmdSel == 7) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 22);
    GfxPrint_Printf(printer, "Fade Timer");

    // Draw Arguments
    if (sSeqCmdDebugSetup1CmdSel != 0) {
        if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 0)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 10);
        GfxPrint_Printf(printer, "playerIndex:");
        GfxPrint_SetColor(printer, 200, 200, 200, 0);
        GfxPrint_SetPos(printer, 23, 12);
        GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerIndex]);

        if ((sSeqCmdDebugSetup1CmdSel != 2) && (sSeqCmdDebugSetup1CmdSel != 7)) {
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "playerIndex Target:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 16);
            GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerTargetIndex]);
        }
    }

    switch (sSeqCmdDebugSetup1CmdSel) {
        case 1:
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "fade timer: %d", sSeqCmdDebugFadeTimer);
            break;

        case 4:
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "duration: %d", sSeqCmdDebugDuration);

            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 20);
            GfxPrint_Printf(printer, "tempo scale: %d", sSeqCmdDebugTempoScale);
            break;

        case 5:
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "duration: %d", sSeqCmdDebugDuration);
            break;

        case 6:
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "seqId:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 20);
            GfxPrint_Printf(printer, "%s", sSeqNames[sSeqCmdDebugSeqId]);
            break;

        case 7:
            if (sIsSeqCmdDebugSetup1CmdArgSel && (sSeqCmdDebugSetup1CmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "fade timer: %d", sSeqCmdDebugFadeTimer);
            break;
    }
}

void AudioSeqCmdDebug_DrawPageSetup2(GfxPrint* printer) {
    if (sSeqCmdDebugSetup2CmdSel == 0) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Setup Part 2 SeqCmds");

    if (sSeqCmdDebugSetup2CmdSel == 1) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Reset Vol w Queue");

    if (sSeqCmdDebugSetup2CmdSel == 2) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Reset Vol w Scale");

    if (sSeqCmdDebugSetup2CmdSel == 3) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Disable Chan");

    if (sSeqCmdDebugSetup2CmdSel == 4) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 16);
    GfxPrint_Printf(printer, "Set Player Freq");

    if (sSeqCmdDebugSetup2CmdSel == 5) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 18);
    GfxPrint_Printf(printer, "Pop Cache");

    if (sSeqCmdDebugSetup2CmdSel == 6) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 20);
    GfxPrint_Printf(printer, "Reset Setup Cmds");

    // Draw Arguments
    if (sSeqCmdDebugSetup2CmdSel != 0) {
        if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 0)) {
            GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
        } else {
            GfxPrint_SetColor(printer, 255, 255, 255, 0);
        }
        GfxPrint_SetPos(printer, 20, 10);
        GfxPrint_Printf(printer, "playerIndex:");
        GfxPrint_SetColor(printer, 200, 200, 200, 0);
        GfxPrint_SetPos(printer, 23, 12);
        GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerIndex]);

        if (sSeqCmdDebugSetup2CmdSel != 6) {
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 14);
            GfxPrint_Printf(printer, "playerIndex Target:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 16);
            GfxPrint_Printf(printer, "%s", sSeqCmdDebugPlayerIndexStr[sSeqCmdDebugPlayerTargetIndex]);
        }
    }

    switch (sSeqCmdDebugSetup2CmdSel) {
        case 1:
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "fade timer: %d", sSeqCmdDebugFadeTimer);

            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 20);
            GfxPrint_Printf(printer, "num seq: %d", sSeqCmdDebugNumSeq);
            break;

        case 2:
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "volume scale: %d", sSeqCmdDebugVolumeScale);

            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 20);
            GfxPrint_Printf(printer, "fade timer: %d", sSeqCmdDebugFadeTimer);
            break;

        case 3:
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "channel mask:");
            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 20);
            GfxPrint_Printf(printer, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sSeqCmdDebugChannelMask));
            break;

        case 4:
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "duration: %d", sSeqCmdDebugDuration);

            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 3)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 20);
            GfxPrint_Printf(printer, "frequency: %d", sSeqCmdDebugFreq);
            break;

        case 5:
            if (sIsSeqCmdDebugSetup2CmdArgSel && (sSeqCmdDebugSetup2CmdArgSel == 2)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 18);
            GfxPrint_Printf(printer, "tableTypeFlag: %d", sSeqCmdDebugTableTypeFlag);
    }
}

void AudioSeqCmdDebug_DrawPageGlobal(GfxPrint* printer) {
    if (sSeqCmdDebugGlobalCmdSel == 0) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 7);
    GfxPrint_Printf(printer, "Global SeqCmds");

    // Draw Command Options
    if (sSeqCmdDebugGlobalCmdSel == 1) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 10);
    GfxPrint_Printf(printer, "Set Sound Mode");

    if (sSeqCmdDebugGlobalCmdSel == 2) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 12);
    GfxPrint_Printf(printer, "Disable New Seq");

    if (sSeqCmdDebugGlobalCmdSel == 3) {
        GfxPrint_SetColor(printer, 0, 0, 255, 255);
    } else {
        GfxPrint_SetColor(printer, 255, 255, 255, 0);
    }
    GfxPrint_SetPos(printer, 2, 14);
    GfxPrint_Printf(printer, "Reset Audio Heap");

    switch (sSeqCmdDebugGlobalCmdSel) {
        case 1:
            if (sIsSeqCmdDebugGlobalCmdArgSel && (sSeqCmdDebugGlobalCmdArgSel == 0)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 10);
            GfxPrint_Printf(printer, "sound mode:");

            GfxPrint_SetColor(printer, 200, 200, 200, 0);
            GfxPrint_SetPos(printer, 23, 12);
            GfxPrint_Printf(printer, "%s", sSoundModeNamesDebug[sSeqCmdDebugSoundMode]);
            break;

        case 2:
            if (sIsSeqCmdDebugGlobalCmdArgSel && (sSeqCmdDebugGlobalCmdArgSel == 0)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 10);
            GfxPrint_Printf(printer, "is seq disabled: %d", sSeqCmdDebugIsSeqDisabled);
            break;

        case 3:
            if (sIsSeqCmdDebugGlobalCmdArgSel && (sSeqCmdDebugGlobalCmdArgSel == 0)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 10);
            GfxPrint_Printf(printer, "sfx chan layout: %d", sSeqCmdDebugSfxChannelLayout);

            if (sIsSeqCmdDebugGlobalCmdArgSel && (sSeqCmdDebugGlobalCmdArgSel == 1)) {
                GfxPrint_SetColor(printer, 150 * sIsSeqCmdDebugCmdArgAdj, 150 * sIsSeqCmdDebugCmdArgAdj, 255, 255);
            } else {
                GfxPrint_SetColor(printer, 255, 255, 255, 0);
            }
            GfxPrint_SetPos(printer, 20, 12);
            GfxPrint_Printf(printer, "specId: %d", sSeqCmdDebugSpecId);
            break;
    }
}

void AudioSeqCmdDebug_Draw(GfxPrint* printer) {
    GfxPrint_SetColor(printer, 0, 255, 0, 255);
    GfxPrint_SetPos(printer, 17, 0);
    GfxPrint_Printf(printer, "Audio SeqCmd Debug Mode");

    switch (sSeqCmdDebugPage) {
        case PAGE_SEQ:
            AudioSeqCmdDebug_DrawPageSeq(printer);
            break;

        case PAGE_PLAYER:
            AudioSeqCmdDebug_DrawPagePlayer(printer);
            break;

        case PAGE_CHANNEL:
            AudioSeqCmdDebug_DrawPageChannel(printer);
            break;

        case PAGE_SETUP1:
            AudioSeqCmdDebug_DrawPageSetup1(printer);
            break;

        case PAGE_SETUP2:
            AudioSeqCmdDebug_DrawPageSetup2(printer);
            break;

        case PAGE_MISC:
            AudioSeqCmdDebug_DrawPageGlobal(printer);
            break;
    }

    GfxPrint_SetColor(printer, 255, 200 - sSeqCmdDebugCmdTimer, 200 - sSeqCmdDebugCmdTimer, 255);
    GfxPrint_SetPos(printer, 12, 28);
    GfxPrint_Printf(printer, "Audio_QueueSeqCmd(%08X)", sSeqCmdDebugCmd);
}
