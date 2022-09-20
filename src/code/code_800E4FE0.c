#include "global.h"

#define SAMPLES_TO_OVERPRODUCE 0x10
#define EXTRA_BUFFERED_AI_SAMPLES_TARGET 0x80

typedef enum {
    CHAN_UPD_UNK_0,           // 0
    CHAN_UPD_VOL_SCALE,       // 1
    CHAN_UPD_VOL,             // 2
    CHAN_UPD_PAN_SIGNED,      // 3
    CHAN_UPD_FREQ_SCALE,      // 4
    CHAN_UPD_REVERB,          // 5
    CHAN_UPD_SCRIPT_IO,       // 6
    CHAN_UPD_PAN_UNSIGNED,    // 7
    CHAN_UPD_STOP_SOMETHING2, // 8
    CHAN_UPD_MUTE_BEHAVE,     // 9
    CHAN_UPD_VIBE_X8,         // 10
    CHAN_UPD_VIBE_X32,        // 11
    CHAN_UPD_UNK_0F,          // 12
    CHAN_UPD_UNK_20,          // 13
    CHAN_UPD_STEREO           // 14
} ChannelUpdateType;

void AudioThread_ProcessChannelCmd(SequenceChannel* channel, AudioCmd* cmd);
void AudioThread_SetFadeInTimer(s32 seqPlayerIndex, s32 fadeTimer);
void AudioThread_InitMesgQueues(void);
AudioTask* AudioThread_UpdateImpl(void);
void AudioThread_ProcessCmds(u32);
void AudioThread_ProcessSeqPlayerCmd(SequencePlayer* seqPlayer, AudioCmd* cmd);
void AudioThread_SetFadeOutTimer(s32 seqPlayerIndex, s32 fadeTimer);
s32 AudioThread_CountAndReleaseNotes(s32 arg0);

// AudioMgr_Retrace
AudioTask* AudioThread_Update(void) {
    return AudioThread_UpdateImpl();
}

/**
 * This is Audio_Update for the audio thread
 */
AudioTask* AudioThread_UpdateImpl(void) {
    static s32 sMaxAbiCmdCnt = 0x80;
    static AudioTask* sWaitingAudioTask = NULL;
    u32 samplesRemainingInAi;
    s32 abiCmdCnt;
    s32 pad;
    s32 j;
    s32 sp5C;
    s16* currAiBuffer;
    OSTask_t* task;
    s32 index;
    u32 sp4C;
    s32 sp48;
    s32 i;

    gAudioCtx.totalTaskCount++;
    if (gAudioCtx.totalTaskCount % (gAudioCtx.audioBufferParameters.specUnk4) != 0) {
        if (gCustomAudioUpdateFunction != NULL) {
            gCustomAudioUpdateFunction();
        }

        if ((gAudioCtx.totalTaskCount % gAudioCtx.audioBufferParameters.specUnk4) + 1 ==
            gAudioCtx.audioBufferParameters.specUnk4) {
            return sWaitingAudioTask;
        } else {
            return NULL;
        }
    }

    osSendMesg(gAudioCtx.taskStartQueueP, (OSMesg)gAudioCtx.totalTaskCount, OS_MESG_NOBLOCK);
    gAudioCtx.rspTaskIndex ^= 1;
    gAudioCtx.curAiBufIndex++;
    gAudioCtx.curAiBufIndex %= 3;
    index = (gAudioCtx.curAiBufIndex + 1) % 3;

    // Division converts size to numSamples: 2 channels (left/right) * 2 bytes per sample
    samplesRemainingInAi = osAiGetLength() / (2 * SAMPLE_SIZE);

    if (gAudioCtx.resetTimer < 16) {
        if (gAudioCtx.aiBufLengths[index] != 0) {
            osAiSetNextBuffer(gAudioCtx.aiBuffers[index], gAudioCtx.aiBufLengths[index] * 4);
            if (gAudioCtx.aiBuffers[index]) {}
            if (gAudioCtx.aiBufLengths[index]) {}
        }
    }

    if (gCustomAudioUpdateFunction != NULL) {
        gCustomAudioUpdateFunction();
    }

    sp5C = gAudioCtx.curAudioFrameDmaCount;
    for (i = 0; i < gAudioCtx.curAudioFrameDmaCount; i++) {
        if (osRecvMesg(&gAudioCtx.currAudioFrameDmaQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            sp5C--;
        }
    }

    if (sp5C != 0) {
        for (i = 0; i < sp5C; i++) {
            osRecvMesg(&gAudioCtx.currAudioFrameDmaQueue, NULL, OS_MESG_BLOCK);
        }
    }

    sp48 = MQ_GET_COUNT(&gAudioCtx.currAudioFrameDmaQueue);
    if (sp48 != 0) {
        for (i = 0; i < sp48; i++) {
            osRecvMesg(&gAudioCtx.currAudioFrameDmaQueue, NULL, OS_MESG_NOBLOCK);
        }
    }

    gAudioCtx.curAudioFrameDmaCount = 0;
    AudioLoad_DecreaseSampleDmaTtls();
    AudioLoad_ProcessLoads(gAudioCtx.resetStatus);
    AudioLoad_ProcessScriptLoads();

    if (gAudioCtx.resetStatus != 0) {
        if (AudioHeap_ResetStep() == 0) {
            if (gAudioCtx.resetStatus == 0) {
                osSendMesg(gAudioCtx.audioResetQueueP, (OSMesg)(u32)gAudioCtx.specId, OS_MESG_NOBLOCK);
            }

            sWaitingAudioTask = NULL;
            return NULL;
        }
    }

    if (gAudioCtx.resetTimer > 16) {
        return NULL;
    }
    if (gAudioCtx.resetTimer != 0) {
        gAudioCtx.resetTimer++;
    }

    gAudioCtx.curTask = &gAudioCtx.rspTask[gAudioCtx.rspTaskIndex];
    gAudioCtx.curAbiCmdBuf = gAudioCtx.abiCmdBufs[gAudioCtx.rspTaskIndex];

    index = gAudioCtx.curAiBufIndex;
    currAiBuffer = gAudioCtx.aiBuffers[index];

    gAudioCtx.aiBufLengths[index] =
        (s16)((((gAudioCtx.audioBufferParameters.samplesPerFrameTarget - samplesRemainingInAi) +
                EXTRA_BUFFERED_AI_SAMPLES_TARGET) &
               ~0xF) +
              SAMPLES_TO_OVERPRODUCE);
    if (gAudioCtx.aiBufLengths[index] < gAudioCtx.audioBufferParameters.minAiBufferLength) {
        gAudioCtx.aiBufLengths[index] = gAudioCtx.audioBufferParameters.minAiBufferLength;
    }

    if (gAudioCtx.aiBufLengths[index] > gAudioCtx.audioBufferParameters.maxAiBufferLength) {
        gAudioCtx.aiBufLengths[index] = gAudioCtx.audioBufferParameters.maxAiBufferLength;
    }

    j = 0;
    if (gAudioCtx.resetStatus == 0) {
        // msg = 0000RREE R = read pos, E = End Pos
        while (osRecvMesg(gAudioCtx.cmdProcQueueP, (OSMesg*)&sp4C, OS_MESG_NOBLOCK) != -1) {
            if (1) {}
            AudioThread_ProcessCmds(sp4C);
            j++;
        }
        if ((j == 0) && (gAudioCtx.cmdQueueFinished)) {
            AudioThread_ScheduleProcessCmds();
        }
    }

    gAudioCtx.curAbiCmdBuf =
        AudioSynth_Update(gAudioCtx.curAbiCmdBuf, &abiCmdCnt, currAiBuffer, gAudioCtx.aiBufLengths[index]);

    // Update audioRandom to the next random number
    gAudioCtx.audioRandom = (gAudioCtx.audioRandom + gAudioCtx.totalTaskCount) * osGetCount();
    gAudioCtx.audioRandom = gAudioCtx.audioRandom + gAudioCtx.aiBuffers[index][gAudioCtx.totalTaskCount & 0xFF];

    // gWaveSamples[8] interprets compiled assembly code as s16 samples as a way to generate sound with noise.
    // Start with the address of AudioThread_Update, and offset it by a random number between 0 - 0xFFF0
    // Use the resulting address as the starting address to interpret an array of samples i.e. `s16 samples[]`
    gWaveSamples[8] = (s16*)(((u8*)AudioThread_Update) + (gAudioCtx.audioRandom & 0xFFF0));

    index = gAudioCtx.rspTaskIndex;
    gAudioCtx.curTask->msgQueue = NULL;
    gAudioCtx.curTask->unk_44 = NULL;

    task = &gAudioCtx.curTask->task.t;
    task->type = M_AUDTASK;
    task->flags = 0;
    task->ucode_boot = aspMainTextStart;
    task->ucode_boot_size = SP_UCODE_SIZE;
    task->ucode_data_size = (size_t)(aspMainDataEnd - aspMainDataStart) * sizeof(u64) - 1;
    task->ucode = aspMainTextStart;
    task->ucode_data = aspMainDataStart;
    task->ucode_size = SP_UCODE_SIZE;
    task->dram_stack = NULL;
    task->dram_stack_size = 0;
    task->output_buff = NULL;
    task->output_buff_size = NULL;
    if (1) {}
    task->data_ptr = (u64*)gAudioCtx.abiCmdBufs[index];
    task->data_size = abiCmdCnt * sizeof(Acmd);
    task->yield_data_ptr = NULL;
    task->yield_data_size = 0;

    if (sMaxAbiCmdCnt < abiCmdCnt) {
        sMaxAbiCmdCnt = abiCmdCnt;
    }

    if (gAudioCtx.audioBufferParameters.specUnk4 == 1) {
        return gAudioCtx.curTask;
    } else {
        sWaitingAudioTask = gAudioCtx.curTask;
        return NULL;
    }
}

void AudioThread_ProcessGlobalCmd(AudioCmd* cmd) {
    s32 i;
    s32 pad;
    s32 pad2;
    u32 temp_a1_5;
    u32 temp_t7;

    switch (cmd->op) {
        case 0x81:
            AudioLoad_SyncLoadSeqParts(cmd->arg1, cmd->arg2);
            break;

        case 0x82:
            AudioLoad_SyncInitSeqPlayer(cmd->arg0, cmd->arg1, cmd->arg2);
            AudioThread_SetFadeInTimer(cmd->arg0, cmd->asInt);
            break;

        case 0x85:
            AudioLoad_SyncInitSeqPlayerSkipTicks(cmd->arg0, cmd->arg1, cmd->asInt);
            break;

        case 0x83:
            if (gAudioCtx.seqPlayers[cmd->arg0].enabled) {
                if (cmd->asInt == 0) {
                    AudioScript_SequencePlayerDisableAsFinished(&gAudioCtx.seqPlayers[cmd->arg0]);
                } else {
                    AudioThread_SetFadeOutTimer(cmd->arg0, cmd->asInt);
                }
            }
            break;

        case 0xF0:
            gAudioCtx.soundMode = cmd->asUInt;
            break;

        case 0xF1:
            for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[i];

                seqPlayer->muted = 1;
                seqPlayer->recalculateVolume = 1;
            }
            break;

        case 0xF2:
            if (cmd->asUInt == 1) {
                for (i = 0; i < gAudioCtx.numNotes; i++) {
                    Note* note = &gAudioCtx.notes[i];
                    NoteSubEu* subEu = &note->noteSubEu;

                    if (subEu->bitField0.enabled && note->playbackState.unk_04 == 0) {
                        if (note->playbackState.parentLayer->channel->muteFlags & MUTE_FLAGS_3) {
                            subEu->bitField0.finished = true;
                        }
                    }
                }
            }

            for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[i];

                seqPlayer->muted = 0;
                seqPlayer->recalculateVolume = 1;
            }
            break;

        case 0xF3:
            AudioLoad_SyncLoadInstrument(cmd->arg0, cmd->arg1, cmd->arg2);
            break;

        case 0xF4:
            AudioLoad_AsyncLoadSampleBank(cmd->arg0, cmd->arg1, cmd->arg2, &gAudioCtx.externalLoadQueue);
            break;

        case 0xF5:
            AudioLoad_AsyncLoadFont(cmd->arg0, cmd->arg1, cmd->arg2, &gAudioCtx.externalLoadQueue);
            break;

        case 0xFC:
            AudioLoad_AsyncLoadSeq(cmd->arg0, cmd->arg1, cmd->arg2, &gAudioCtx.externalLoadQueue);
            break;

        case 0xF6:
            AudioLoad_DiscardSeqFonts(cmd->arg1);
            break;

        case 0x90:
            gAudioCtx.unk_5BDC[cmd->arg0] = cmd->asUShort;
            break;

        case 0xF9:
            gAudioCtx.resetStatus = 5;
            gAudioCtx.specId = cmd->asUInt;
            break;

        case 0xFB:
            gCustomAudioUpdateFunction = (void (*)(void))cmd->asUInt;
            break;

        case 0xE0:
        case 0xE1:
        case 0xE2:
            AudioPlayback_SetFontInstrument(cmd->op - 0xE0, cmd->arg0, cmd->arg1, cmd->data);
            break;

        case 0xFE:
            temp_t7 = cmd->asUInt;
            if (temp_t7 == 1) {
                for (i = 0; i < gAudioCtx.audioBufferParameters.numSequencePlayers; i++) {
                    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[i];

                    if (seqPlayer->enabled) {
                        AudioScript_SequencePlayerDisableAsFinished(seqPlayer);
                    }
                }
            }
            AudioThread_CountAndReleaseNotes(temp_t7);
            break;

        case 0xE3:
            AudioHeap_PopPersistentCache(cmd->asInt);
            break;

        default:
            break;
    }
}

void AudioThread_SetFadeOutTimer(s32 seqPlayerIndex, s32 fadeTimer) {
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[seqPlayerIndex];

    if (fadeTimer == 0) {
        fadeTimer = 1;
    }

    seqPlayer->fadeVelocity = -(seqPlayer->fadeVolume / fadeTimer);
    seqPlayer->state = 2;
    seqPlayer->fadeTimer = fadeTimer;
}

void AudioThread_SetFadeInTimer(s32 seqPlayerIndex, s32 fadeTimer) {
    SequencePlayer* seqPlayer;

    if (fadeTimer != 0) {
        seqPlayer = &gAudioCtx.seqPlayers[seqPlayerIndex];
        seqPlayer->state = 1;
        seqPlayer->fadeTimerUnkEu = fadeTimer;
        seqPlayer->fadeTimer = fadeTimer;
        seqPlayer->fadeVolume = 0.0f;
        seqPlayer->fadeVelocity = 0.0f;
    }
}

void AudioThread_InitMesgQueuesInternal(void) {
    gAudioCtx.cmdWritePos = 0;
    gAudioCtx.cmdReadPos = 0;
    gAudioCtx.cmdQueueFinished = 0;
    gAudioCtx.taskStartQueueP = &gAudioCtx.taskStartQueue;
    gAudioCtx.cmdProcQueueP = &gAudioCtx.cmdProcQueue;
    gAudioCtx.audioResetQueueP = &gAudioCtx.audioResetQueue;
    osCreateMesgQueue(gAudioCtx.taskStartQueueP, gAudioCtx.taskStartMsgBuf, ARRAY_COUNT(gAudioCtx.taskStartMsgBuf));
    osCreateMesgQueue(gAudioCtx.cmdProcQueueP, gAudioCtx.cmdProcMsgBuf, ARRAY_COUNT(gAudioCtx.cmdProcMsgBuf));
    osCreateMesgQueue(gAudioCtx.audioResetQueueP, gAudioCtx.audioResetMsgBuf, ARRAY_COUNT(gAudioCtx.audioResetMsgBuf));
}

void AudioThread_QueueCmd(u32 opArgs, void** data) {
    AudioCmd* cmd = &gAudioCtx.cmdBuf[gAudioCtx.cmdWritePos & 0xFF];

    cmd->opArgs = opArgs;
    cmd->data = *data;

    gAudioCtx.cmdWritePos++;

    if (gAudioCtx.cmdWritePos == gAudioCtx.cmdReadPos) {
        gAudioCtx.cmdWritePos--;
    }
}

void AudioThread_QueueCmdF32(u32 opArgs, f32 data) {
    AudioThread_QueueCmd(opArgs, (void**)&data);
}

void AudioThread_QueueCmdS32(u32 opArgs, s32 data) {
    AudioThread_QueueCmd(opArgs, (void**)&data);
}

void AudioThread_QueueCmdS8(u32 opArgs, s8 data) {
    u32 uData = data << 0x18;

    AudioThread_QueueCmd(opArgs, (void**)&uData);
}

void AudioThread_QueueCmdU16(u32 opArgs, u16 data) {
    u32 uData = data << 0x10;

    AudioThread_QueueCmd(opArgs, (void**)&uData);
}

s32 AudioThread_ScheduleProcessCmds(void) {
    static s32 D_801304E8 = 0;
    s32 ret;

    if (D_801304E8 < (u8)((gAudioCtx.cmdWritePos - gAudioCtx.cmdReadPos) + 0x100)) {
        D_801304E8 = (u8)((gAudioCtx.cmdWritePos - gAudioCtx.cmdReadPos) + 0x100);
    }

    ret = osSendMesg(gAudioCtx.cmdProcQueueP,
                     (OSMesg)(((gAudioCtx.cmdReadPos & 0xFF) << 8) | (gAudioCtx.cmdWritePos & 0xFF)), OS_MESG_NOBLOCK);
    if (ret != -1) {
        gAudioCtx.cmdReadPos = gAudioCtx.cmdWritePos;
        ret = 0;
    } else {
        return -1;
    }

    return ret;
}

void AudioThread_ResetCmdQueue(void) {
    gAudioCtx.cmdQueueFinished = 0;
    gAudioCtx.cmdReadPos = gAudioCtx.cmdWritePos;
}

void AudioThread_ProcessCmd(AudioCmd* cmd) {
    SequencePlayer* seqPlayer;
    u16 phi_v0;
    s32 i;

    if ((cmd->op & 0xF0) == 0xF0) {
        AudioThread_ProcessGlobalCmd(cmd);
        return;
    }

    if (cmd->arg0 < gAudioCtx.audioBufferParameters.numSequencePlayers) {
        seqPlayer = &gAudioCtx.seqPlayers[cmd->arg0];
        if (cmd->op & 0x80) {
            AudioThread_ProcessGlobalCmd(cmd);
            return;
        }
        if (cmd->op & 0x40) {
            AudioThread_ProcessSeqPlayerCmd(seqPlayer, cmd);
            return;
        }

        if (cmd->arg1 < SEQ_NUM_CHANNELS) {
            AudioThread_ProcessChannelCmd(seqPlayer->channels[cmd->arg1], cmd);
            return;
        }
        if (cmd->arg1 == 0xFF) {
            phi_v0 = gAudioCtx.unk_5BDC[cmd->arg0];
            for (i = 0; i < 16; i++) {
                if (phi_v0 & 1) {
                    AudioThread_ProcessChannelCmd(seqPlayer->channels[i], cmd);
                }
                phi_v0 = phi_v0 >> 1;
            }
        }
    }
}

void AudioThread_ProcessCmds(u32 msg) {
    static u8 sCurCmdRdPos = 0;
    AudioCmd* cmd;
    u8 endPos;

    if (!gAudioCtx.cmdQueueFinished) {
        sCurCmdRdPos = msg >> 8;
    }

    while (true) {
        endPos = msg & 0xFF;
        if (sCurCmdRdPos == endPos) {
            gAudioCtx.cmdQueueFinished = 0;
            return;
        }

        cmd = &gAudioCtx.cmdBuf[sCurCmdRdPos++ & 0xFF];
        if (cmd->op == 0xF8) {
            gAudioCtx.cmdQueueFinished = 1;
            return;
        }

        AudioThread_ProcessCmd(cmd);
        cmd->op = 0;
    }
}

u32 func_800E5E20(u32* out) {
    u32 sp1C;

    if (osRecvMesg(&gAudioCtx.externalLoadQueue, (OSMesg*)&sp1C, OS_MESG_NOBLOCK) == -1) {
        *out = 0;
        return 0;
    }
    *out = sp1C & 0xFFFFFF;
    return sp1C >> 0x18;
}

u8* AudioThread_GetFontsForSequence(s32 seqId, u32* outNumFonts) {
    return AudioLoad_GetFontsForSequence(seqId, outNumFonts);
}

void AudioThread_GetSampleBankIdsOfFont(s32 fontId, u32* sampleBankId1, u32* sampleBankId2) {
    *sampleBankId1 = gAudioCtx.soundFontList[fontId].sampleBankId1;
    *sampleBankId2 = gAudioCtx.soundFontList[fontId].sampleBankId2;
}

s32 func_800E5EDC(void) {
    s32 pad;
    s32 sp18;

    if (osRecvMesg(gAudioCtx.audioResetQueueP, (OSMesg*)&sp18, OS_MESG_NOBLOCK) == -1) {
        return 0;
    } else if (gAudioCtx.specId != sp18) {
        return -1;
    } else {
        return 1;
    }
}

void AudioThread_WaitForAudioResetQueueP(void) {
    // macro?
    // clang-format off
    s32 chk = -1; OSMesg msg; do {} while (osRecvMesg(gAudioCtx.audioResetQueueP, &msg, OS_MESG_NOBLOCK) != chk);
    // clang-format on
}

s32 AudioThread_ResetAudioHeap(s32 specId) {
    s32 resetStatus;
    OSMesg msg;
    s32 pad;

    AudioThread_WaitForAudioResetQueueP();
    resetStatus = gAudioCtx.resetStatus;
    if (resetStatus != 0) {
        AudioThread_ResetCmdQueue();
        if (gAudioCtx.specId == specId) {
            return -2;
        } else if (resetStatus > 2) {
            gAudioCtx.specId = specId;
            return -3;
        } else {
            osRecvMesg(gAudioCtx.audioResetQueueP, &msg, OS_MESG_BLOCK);
        }
    }

    AudioThread_WaitForAudioResetQueueP();
    AudioThread_QueueCmdS32(0xF9000000, specId);

    return AudioThread_ScheduleProcessCmds();
}

void AudioThread_PreNMIInternal(void) {
    gAudioCtx.resetTimer = 1;
    if (gAudioContextInitialized) {
        AudioThread_ResetAudioHeap(0);
        gAudioCtx.resetStatus = 0;
    }
}

s8 AudioThread_GetChannelIO(s32 seqPlayerIndex, s32 channelIndex, s32 scriptIdx) {
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[seqPlayerIndex];
    SequenceChannel* channel;

    if (seqPlayer->enabled) {
        channel = seqPlayer->channels[channelIndex];
        return channel->seqScriptIO[scriptIdx];
    } else {
        return -1;
    }
}

s8 AudioThread_GetSeqPlayerIO(s32 seqPlayerIndex, s32 port) {
    return gAudioCtx.seqPlayers[seqPlayerIndex].seqScriptIO[port];
}

void AudioThread_InitExternalPool(void* ramAddr, u32 size) {
    AudioHeap_InitPool(&gAudioCtx.externalPool, ramAddr, size);
}

void AudioThread_ResetExternalPool(void) {
    gAudioCtx.externalPool.startRamAddr = NULL;
}

void AudioThread_ProcessSeqPlayerCmd(SequencePlayer* seqPlayer, AudioCmd* cmd) {
    f32 fadeVolume;

    switch (cmd->op) {
        case 0x41:
            if (seqPlayer->fadeVolumeScale != cmd->asFloat) {
                seqPlayer->fadeVolumeScale = cmd->asFloat;
                seqPlayer->recalculateVolume = 1;
            }
            break;

        case 0x47:
            seqPlayer->tempo = cmd->asInt * 0x30;
            break;

        case 0x49:
            seqPlayer->unk_0C = cmd->asInt * 0x30;
            break;

        case 0x4E:
            seqPlayer->unk_0C = cmd->asInt;
            break;

        case 0x48:
            seqPlayer->transposition = cmd->asSbyte;
            break;

        case 0x46:
            seqPlayer->seqScriptIO[cmd->arg2] = cmd->asSbyte;
            break;

        case 0x4A:
            fadeVolume = (s32)cmd->arg1 / 127.0f;
            goto block_11;

        case 0x4B:
            fadeVolume = ((s32)cmd->arg1 / 100.0f) * seqPlayer->fadeVolume;
        block_11:
            if (seqPlayer->state != 2) {
                seqPlayer->volume = seqPlayer->fadeVolume;
                if (cmd->asInt == 0) {
                    seqPlayer->fadeVolume = fadeVolume;
                } else {
                    s32 fadeTimer = cmd->asInt;

                    seqPlayer->state = 0;
                    seqPlayer->fadeTimer = fadeTimer;
                    seqPlayer->fadeVelocity = (fadeVolume - seqPlayer->fadeVolume) / fadeTimer;
                }
            }
            break;

        case 0x4C:
            if (seqPlayer->state != 2) {
                if (cmd->asInt == 0) {
                    seqPlayer->fadeVolume = seqPlayer->volume;
                } else {
                    s32 fadeTimer = cmd->asInt;

                    seqPlayer->state = 0;
                    seqPlayer->fadeTimer = fadeTimer;
                    seqPlayer->fadeVelocity = (seqPlayer->volume - seqPlayer->fadeVolume) / fadeTimer;
                }
            }
            break;

        case 0x4D:
            seqPlayer->bend = cmd->asFloat;
            if (seqPlayer->bend == 1.0f) {
                seqPlayer->applyBend = false;
            } else {
                seqPlayer->applyBend = true;
            }
            break;

        default:
            break;
    }
}

void AudioThread_ProcessChannelCmd(SequenceChannel* channel, AudioCmd* cmd) {
    switch (cmd->op) {
        case CHAN_UPD_VOL_SCALE:
            if (channel->volumeScale != cmd->asFloat) {
                channel->volumeScale = cmd->asFloat;
                channel->changes.s.volume = 1;
            }
            break;

        case CHAN_UPD_VOL:
            if (channel->volume != cmd->asFloat) {
                channel->volume = cmd->asFloat;
                channel->changes.s.volume = 1;
            }
            break;

        case CHAN_UPD_PAN_SIGNED:
            if (channel->newPan != cmd->asSbyte) {
                channel->newPan = cmd->asSbyte;
                channel->changes.s.pan = 1;
            }
            break;

        case CHAN_UPD_PAN_UNSIGNED:
            if (channel->newPan != cmd->asSbyte) {
                channel->panChannelWeight = cmd->asSbyte;
                channel->changes.s.pan = 1;
            }
            break;

        case CHAN_UPD_FREQ_SCALE:
            if (channel->freqScale != cmd->asFloat) {
                channel->freqScale = cmd->asFloat;
                channel->changes.s.freqScale = 1;
            }
            break;

        case CHAN_UPD_REVERB:
            if (channel->reverb != cmd->asSbyte) {
                channel->reverb = cmd->asSbyte;
            }
            break;

        case CHAN_UPD_SCRIPT_IO:
            if (cmd->arg2 < 8) {
                channel->seqScriptIO[cmd->arg2] = cmd->asSbyte;
            }
            break;

        case CHAN_UPD_STOP_SOMETHING2:
            channel->stopSomething2 = cmd->asSbyte;
            break;

        case CHAN_UPD_MUTE_BEHAVE:
            channel->muteFlags = cmd->asSbyte;
            break;

        case CHAN_UPD_VIBE_X8:
            channel->vibratoExtentTarget = cmd->asUbyte * 8;
            channel->vibratoExtentChangeDelay = 1;
            break;

        case CHAN_UPD_VIBE_X32:
            channel->vibratoRateTarget = cmd->asUbyte * 32;
            channel->vibratoRateChangeDelay = 1;
            break;

        case CHAN_UPD_UNK_0F:
            channel->unk_0F = cmd->asUbyte;
            break;

        case CHAN_UPD_UNK_20:
            channel->unk_20 = cmd->asUShort;
            break;

        case CHAN_UPD_STEREO:
            channel->stereo.asByte = cmd->asUbyte;
            break;

        default:
            break;
    }
}

void func_800E64B0(s32 arg0, s32 arg1, s32 arg2) {
    AudioThread_QueueCmdS32(((arg0 & 0xFF) << 0x10) | 0xFA000000 | ((arg1 & 0xFF) << 8) | (arg2 & 0xFF), 1);
}

void func_800E64F8(void) {
    AudioThread_QueueCmdS32(0xFA000000, 0);
}

void func_800E651C(u32 arg0, s32 arg1) {
    AudioThread_QueueCmdS32((arg1 & 0xFF) | 0xFD000000, arg0);
}

void AudioThread_WaitForAudioTask(void) {
    osRecvMesg(gAudioCtx.taskStartQueueP, NULL, OS_MESG_NOBLOCK);
    osRecvMesg(gAudioCtx.taskStartQueueP, NULL, OS_MESG_BLOCK);
}

s32 AudioThread_GetNumSamplesUntilEnd(s32 seqPlayerIndex, s32 channelIndex, s32 layerIndex) {
    SequencePlayer* seqPlayer;
    SequenceLayer* layer;
    Note* note;
    TunedSample* tunedSample;
    s32 loopEnd;
    s32 samplePos;

    seqPlayer = &gAudioCtx.seqPlayers[seqPlayerIndex];
    if (seqPlayer->enabled && seqPlayer->channels[channelIndex]->enabled) {
        layer = seqPlayer->channels[channelIndex]->layers[layerIndex];
        if (layer == NULL) {
            return 0;
        }

        if (layer->enabled) {
            if (layer->note == NULL) {
                return 0;
            }

            if (!layer->bit3) {
                return 0;
            }

            note = layer->note;
            if (layer == note->playbackState.parentLayer) {
                tunedSample = note->noteSubEu.tunedSample;
                if (tunedSample == NULL) {
                    return 0;
                }
                loopEnd = tunedSample->sample->loop->end;
                samplePos = note->synthesisState.samplePosInt;
                return loopEnd - samplePos;
            }
            return 0;
        }
    }
    return 0;
}

s32 AudioThread_GetEnabledNotesCount(void) {
    return AudioThread_CountAndReleaseNotes(0);
}

s32 AudioThread_GetEnabledSampledNotesCount(void) {
    return AudioThread_CountAndReleaseNotes(2);
}

/**
 * @param flags 0: count notes. 1: release all notes. 2: count sample notes 3: release sample notes
 */
s32 AudioThread_CountAndReleaseNotes(s32 flags) {
    s32 noteCount;
    NotePlaybackState* playbackState;
    NoteSubEu* noteSubEu;
    s32 i;
    Note* note;
    TunedSample* tunedSample;

    noteCount = 0;
    for (i = 0; i < gAudioCtx.numNotes; i++) {
        note = &gAudioCtx.notes[i];
        playbackState = &note->playbackState;
        if (note->noteSubEu.bitField0.enabled) {
            noteSubEu = &note->noteSubEu;
            if (playbackState->adsr.action.s.state != 0) {
                if (flags >= 2) {
                    tunedSample = noteSubEu->tunedSample;
                    if ((tunedSample == NULL) || noteSubEu->bitField1.isSyntheticWave) {
                        continue;
                    }
                    if (tunedSample->sample->medium == MEDIUM_RAM) {
                        continue;
                    }
                }

                noteCount++;
                if ((flags & 1) == 1) {
                    playbackState->adsr.fadeOutVel = gAudioCtx.audioBufferParameters.updatesPerFrameInv;
                    playbackState->adsr.action.s.release = 1;
                }
            }
        }
    }
    return noteCount;
}

u32 AudioThread_NextRandom(void) {
    static u32 sAudioRandom = 0x12345678;

    sAudioRandom = ((osGetCount() + 0x1234567) * (sAudioRandom + gAudioCtx.totalTaskCount));
    sAudioRandom += gAudioCtx.audioRandom;

    return sAudioRandom;
}

void AudioThread_InitMesgQueues(void) {
    AudioThread_InitMesgQueuesInternal();
}
