#include "ultra64.h"
#include "global.h"

// DMEM Addresses for the RSP
#define DMEM_TEMP 0x3C0
#define DMEM_UNCOMPRESSED_NOTE 0x580
#define DMEM_HAAS_TEMP 0x5C0
#define DMEM_COMB_TEMP 0x760             // = DMEM_TEMP + DMEM_2CH_SIZE + a bit more
#define DMEM_COMPRESSED_ADPCM_DATA 0x940 // = DMEM_LEFT_CH
#define DMEM_LEFT_CH 0x940
#define DMEM_RIGHT_CH 0xAE0
#define DMEM_WET_TEMP 0x3E0
#define DMEM_WET_SCRATCH 0x720 // = DMEM_WET_TEMP + DMEM_2CH_SIZE
#define DMEM_WET_LEFT_CH 0xC80
#define DMEM_WET_RIGHT_CH 0xE20 // = DMEM_WET_LEFT_CH + DMEM_1CH_SIZE

typedef enum {
    /* 0 */ HAAS_EFFECT_DELAY_NONE,
    /* 1 */ HAAS_EFFECT_DELAY_LEFT, // Delay left channel so that right channel is heard first
    /* 2 */ HAAS_EFFECT_DELAY_RIGHT // Delay right channel so that left channel is heard first
} HaasEffectDelaySide;

Acmd* AudioSynth_LoadReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb);
Acmd* AudioSynth_SaveBufferOffset(Acmd* cmd, u16 dmem, u16 offset, s32 size, s16* buf);
Acmd* AudioSynth_SaveReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb);
Acmd* AudioSynth_ProcessSamples(s16* aiBuf, s32 aiBufLen, Acmd* cmd, s32 updateIndex);
Acmd* AudioSynth_ProcessSample(s32 noteIndex, NoteSampleState* sampleState, NoteSynthesisState* synthState, s16* aiBuf,
                               s32 aiBufLen, Acmd* cmd, s32 updateIndex);
Acmd* AudioSynth_LoadWaveSamples(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesToLoad);
Acmd* AudioSynth_ApplyHaasEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 size,
                                 s32 flags, s32 haasEffectDelaySide);
Acmd* AudioSynth_ProcessEnvelope(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 aiBufLen,
                                 u16 dmemSrc, s32 haasEffectDelaySide, s32 flags);
Acmd* AudioSynth_FinalResample(Acmd* cmd, NoteSynthesisState* synthState, s32 size, u16 pitch, u16 inpDmem,
                               s32 resampleFlags);

u32 sEnvMixerOp = _SHIFTL(A_ENVMIXER, 24, 8);

// Store the left dry channel in a temp space to be delayed to produce the haas effect
u32 sEnvMixerLeftHaasDmemDests =
    MK_CMD(DMEM_HAAS_TEMP >> 4, DMEM_RIGHT_CH >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

// Store the right dry channel in a temp space to be delayed to produce the haas effect
u32 sEnvMixerRightHaasDmemDests =
    MK_CMD(DMEM_LEFT_CH >> 4, DMEM_HAAS_TEMP >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

u32 sEnvMixerDefaultDmemDests =
    MK_CMD(DMEM_LEFT_CH >> 4, DMEM_RIGHT_CH >> 4, DMEM_WET_LEFT_CH >> 4, DMEM_WET_RIGHT_CH >> 4);

u16 D_801304B0[] = {
    0x7FFF, 0xD001, 0x3FFF, 0xF001, 0x5FFF, 0x9001, 0x7FFF, 0x8001,
};

u8 sNumSamplesPerWavePeriod[] = {
    WAVE_SAMPLE_COUNT,     // 1st harmonic
    WAVE_SAMPLE_COUNT / 2, // 2nd harmonic
    WAVE_SAMPLE_COUNT / 4, // 4th harmonic
    WAVE_SAMPLE_COUNT / 8, // 8th harmonic
};

void AudioSynth_AddReverbSampleBufferEntry(s32 numSamples, s32 updateIndex, s32 reverbIndex) {
    ReverbSampleBufferEntry* entry;
    s32 pad[3];
    SynthesisReverb* reverb = &gAudioCtx.synthesisReverbs[reverbIndex];
    s32 temp_a0_2;
    s32 nextReverbSubBufPos;
    s32 numSamplesAfterDownsampling;
    s32 extraSamples;
    s32 i;
    s32 j;

    if (reverb->downsampleRate >= 2) {
        if (reverb->framesToIgnore == 0) {
            entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
            Audio_InvalDCache(entry->toDownsampleLeft, DMEM_2CH_SIZE);

            for (j = 0, i = 0; i < entry->size / (s32)SAMPLE_SIZE; j += reverb->downsampleRate, i++) {
                reverb->leftRingBuf[entry->startPos + i] = entry->toDownsampleLeft[j];
                reverb->rightRingBuf[entry->startPos + i] = entry->toDownsampleRight[j];
            }

            for (i = 0; i < entry->wrappedSize / (s32)SAMPLE_SIZE; j += reverb->downsampleRate, i++) {
                reverb->leftRingBuf[i] = entry->toDownsampleLeft[j];
                reverb->rightRingBuf[i] = entry->toDownsampleRight[j];
            }
        }
    }

    entry = &reverb->bufEntry[reverb->curFrame][updateIndex];
    numSamplesAfterDownsampling = numSamples / reverb->downsampleRate;
    extraSamples = (numSamplesAfterDownsampling + reverb->nextRingBufPos) - reverb->delayNumSamples;
    temp_a0_2 = reverb->nextRingBufPos;

    // Add a reverb entry
    if (extraSamples < 0) {
        entry->size = numSamplesAfterDownsampling * SAMPLE_SIZE;
        entry->wrappedSize = 0;
        entry->startPos = reverb->nextRingBufPos;
        reverb->nextRingBufPos += numSamplesAfterDownsampling;
    } else {
        // End of the buffer is reached. Loop back around
        entry->size = (numSamplesAfterDownsampling - extraSamples) * SAMPLE_SIZE;
        entry->wrappedSize = extraSamples * SAMPLE_SIZE;
        entry->startPos = reverb->nextRingBufPos;
        reverb->nextRingBufPos = extraSamples;
    }

    entry->numSamplesAfterDownsampling = numSamplesAfterDownsampling;
    entry->numSamples = numSamples;

    // Add a sub-reverb entry
    if (reverb->subDelay != 0) {
        nextReverbSubBufPos = reverb->subDelay + temp_a0_2;
        if (nextReverbSubBufPos >= reverb->delayNumSamples) {
            nextReverbSubBufPos -= reverb->delayNumSamples;
        }

        entry = &reverb->subBufEntry[reverb->curFrame][updateIndex];
        numSamplesAfterDownsampling = numSamples / reverb->downsampleRate;
        extraSamples = (nextReverbSubBufPos + numSamplesAfterDownsampling) - reverb->delayNumSamples;

        if (extraSamples < 0) {
            entry->size = numSamplesAfterDownsampling * SAMPLE_SIZE;
            entry->wrappedSize = 0;
            entry->startPos = nextReverbSubBufPos;
        } else {
            // End of the buffer is reached. Loop back around
            entry->size = (numSamplesAfterDownsampling - extraSamples) * SAMPLE_SIZE;
            entry->wrappedSize = extraSamples * SAMPLE_SIZE;
            entry->startPos = nextReverbSubBufPos;
        }

        entry->numSamplesAfterDownsampling = numSamplesAfterDownsampling;
        entry->numSamples = numSamples;
    }
}

void AudioSynth_SyncSampleStates(s32 updateIndex) {
    NoteSampleState* noteSampleState;
    NoteSampleState* sampleState;
    s32 sampleStateBaseIndex;
    s32 i;

    sampleStateBaseIndex = gAudioCtx.numNotes * updateIndex;
    for (i = 0; i < gAudioCtx.numNotes; i++) {
        noteSampleState = &gAudioCtx.notes[i].noteSubEu;
        sampleState = &gAudioCtx.sampleStateList[sampleStateBaseIndex + i];
        if (noteSampleState->bitField0.enabled) {
            noteSampleState->bitField0.needsInit = false;
        } else {
            sampleState->bitField0.enabled = false;
        }

        noteSampleState->harmonicIndexCurAndPrev = 0;
    }
}

Acmd* AudioSynth_Update(Acmd* cmdStart, s32* numAbiCmds, s16* aiStart, s32 numSamplesPerFrame) {
    s32 numSamplesPerUpdate;
    s16* aiBufP;
    Acmd* cmdP;
    s32 i;
    s32 j;
    SynthesisReverb* reverb;

    cmdP = cmdStart;
    for (i = gAudioCtx.audioBufferParameters.updatesPerFrame; i > 0; i--) {
        AudioScript_ProcessSequences(i - 1);
        AudioSynth_SyncSampleStates(gAudioCtx.audioBufferParameters.updatesPerFrame - i);
    }

    aiBufP = aiStart;
    gAudioCtx.adpcmCodeBook = NULL;

    for (i = gAudioCtx.audioBufferParameters.updatesPerFrame; i > 0; i--) {
        if (i == 1) {
            // Final Update
            numSamplesPerUpdate = numSamplesPerFrame;
        } else if ((numSamplesPerFrame / i) >= gAudioCtx.audioBufferParameters.samplesPerUpdateMax) {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.samplesPerUpdateMax;
        } else if (gAudioCtx.audioBufferParameters.samplesPerUpdateMin >= (numSamplesPerFrame / i)) {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.samplesPerUpdateMin;
        } else {
            numSamplesPerUpdate = gAudioCtx.audioBufferParameters.samplesPerUpdate;
        }

        for (j = 0; j < gAudioCtx.numSynthesisReverbs; j++) {
            if (gAudioCtx.synthesisReverbs[j].useReverb) {
                AudioSynth_AddReverbSampleBufferEntry(numSamplesPerUpdate,
                                                      gAudioCtx.audioBufferParameters.updatesPerFrame - i, j);
            }
        }

        cmdP = AudioSynth_ProcessSamples(aiBufP, numSamplesPerUpdate, cmdP,
                                         gAudioCtx.audioBufferParameters.updatesPerFrame - i);
        numSamplesPerFrame -= numSamplesPerUpdate;
        aiBufP += numSamplesPerUpdate * SAMPLE_SIZE;
    }

    // Update reverb frame info
    for (j = 0; j < gAudioCtx.numSynthesisReverbs; j++) {
        if (gAudioCtx.synthesisReverbs[j].framesToIgnore != 0) {
            gAudioCtx.synthesisReverbs[j].framesToIgnore--;
        }
        gAudioCtx.synthesisReverbs[j].curFrame ^= 1;
    }

    *numAbiCmds = cmdP - cmdStart;
    return cmdP;
}

void AudioSynth_DisableSampleStates(s32 updateIndex, s32 noteIndex) {
    NoteSampleState* sampleState;
    s32 i;

    for (i = updateIndex + 1; i < gAudioCtx.audioBufferParameters.updatesPerFrame; i++) {
        sampleState = &gAudioCtx.sampleStateList[(gAudioCtx.numNotes * i) + noteIndex];
        if (!sampleState->bitField0.needsInit) {
            sampleState->bitField0.enabled = false;
        } else {
            break;
        }
    }
}

Acmd* AudioSynth_LoadReverbSamplesToTempDmem(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos, entry->size, reverb);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP + entry->size, 0, entry->wrappedSize, reverb);
    }
    return cmd;
}

Acmd* AudioSynth_SaveReverbSamplesFromTempDmem(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_TEMP, entry->startPos, entry->size, reverb);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_TEMP + entry->size, 0, entry->wrappedSize, reverb);
    }
    return cmd;
}

/**
 * Leak some audio from the left reverb channel into the right reverb channel and vice versa (pan)
 */
Acmd* AudioSynth_LeakReverb(Acmd* cmd, SynthesisReverb* reverb) {
    aDMEMMove(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_SCRATCH, DMEM_1CH_SIZE);
    aMix(cmd++, DMEM_1CH_SIZE >> 4, reverb->leakRtl, DMEM_WET_RIGHT_CH, DMEM_WET_LEFT_CH);
    aMix(cmd++, DMEM_1CH_SIZE >> 4, reverb->leakLtr, DMEM_WET_SCRATCH, DMEM_WET_RIGHT_CH);
    return cmd;
}

Acmd* func_800DB4E4(Acmd* cmd, s32 aiBufLen, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* item = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s16 offsetA;
    s16 offsetB;

    offsetA = (item->startPos & 7) * SAMPLE_SIZE;
    offsetB = ALIGN16(offsetA + item->size);
    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, item->startPos - (offsetA / (s32)SAMPLE_SIZE),
                                           DMEM_1CH_SIZE, reverb);
    if (item->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP + offsetB, 0, DMEM_1CH_SIZE - offsetB, reverb);
    }
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + offsetA, DMEM_WET_LEFT_CH, aiBufLen * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, reverb->unk_0E, reverb->unk_30);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + DMEM_1CH_SIZE + offsetA, DMEM_WET_RIGHT_CH, aiBufLen * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, reverb->unk_0E, reverb->unk_34);
    return cmd;
}

Acmd* func_800DB680(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    aSetBuffer(cmd++, 0, DMEM_WET_LEFT_CH, DMEM_WET_SCRATCH, entry->unk_18 * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->unk_16, reverb->unk_38);

    cmd = AudioSynth_SaveBufferOffset(cmd, DMEM_WET_SCRATCH, entry->startPos, entry->size, reverb->leftRingBuf);
    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveBufferOffset(cmd, DMEM_WET_SCRATCH + entry->size, 0, entry->wrappedSize,
                                          reverb->leftRingBuf);
    }
    aSetBuffer(cmd++, 0, DMEM_WET_RIGHT_CH, DMEM_WET_SCRATCH, entry->unk_18 * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, entry->unk_16, reverb->unk_3C);
    cmd = AudioSynth_SaveBufferOffset(cmd, DMEM_WET_SCRATCH, entry->startPos, entry->size, reverb->rightRingBuf);

    if (entry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_SaveBufferOffset(cmd, DMEM_WET_SCRATCH + entry->size, 0, entry->wrappedSize,
                                          reverb->rightRingBuf);
    }

    return cmd;
}

Acmd* func_800DB828(Acmd* cmd, s32 aiBufLen, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* item = &reverb->bufEntry[reverb->curFrame][updateIndex];
    s16 offsetA;
    s16 offsetB;

    item->unk_14 = (item->unk_18 << 0xF) / aiBufLen;
    offsetA = (item->startPos & 7) * SAMPLE_SIZE;
    item->unk_16 = (aiBufLen << 0xF) / item->unk_18;
    offsetB = ALIGN16(offsetA + item->size);
    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP, item->startPos - (offsetA / (s32)SAMPLE_SIZE),
                                           DMEM_1CH_SIZE, reverb);
    if (item->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_TEMP + offsetB, 0, DMEM_1CH_SIZE - offsetB, reverb);
    }
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + offsetA, DMEM_WET_LEFT_CH, aiBufLen * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, item->unk_14, reverb->unk_30);
    aSetBuffer(cmd++, 0, DMEM_WET_TEMP + DMEM_1CH_SIZE + offsetA, DMEM_WET_RIGHT_CH, aiBufLen * SAMPLE_SIZE);
    aResample(cmd++, reverb->resampleFlags, item->unk_14, reverb->unk_34);
    return cmd;
}

/**
 * Apply a filter (convolution) to each reverb channel.
 */
Acmd* AudioSynth_FilterReverb(Acmd* cmd, s32 size, SynthesisReverb* reverb) {
    if (reverb->filterLeft != NULL) {
        aFilter(cmd++, 2, size, reverb->filterLeft);
        aFilter(cmd++, reverb->resampleFlags, DMEM_WET_LEFT_CH, reverb->filterLeftState);
    }

    if (reverb->filterRight != NULL) {
        aFilter(cmd++, 2, size, reverb->filterRight);
        aFilter(cmd++, reverb->resampleFlags, DMEM_WET_RIGHT_CH, reverb->filterRightState);
    }
    return cmd;
}

/**
 * Mix in reverb from a different reverb index
 */
Acmd* AudioSynth_MixOtherReverbIndex(Acmd* cmd, SynthesisReverb* reverb, s32 updateIndex) {
    SynthesisReverb* mixReverb;

    mixReverb = &gAudioCtx.synthesisReverbs[reverb->mixReverbIndex];
    if (mixReverb->downsampleRate == 1) {
        cmd = AudioSynth_LoadReverbSamplesToTempDmem(cmd, mixReverb, updateIndex);
        aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->mixReverbStrength, DMEM_WET_LEFT_CH, DMEM_WET_TEMP);
        cmd = AudioSynth_SaveReverbSamplesFromTempDmem(cmd, mixReverb, updateIndex);
    }

    return cmd;
}

void AudioSynth_Noop1(void) {
}

void AudioSynth_ClearBuffer(Acmd* cmd, s32 dmem, s32 size) {
    aClearBuffer(cmd, dmem, size);
}

void AudioSynth_Noop2(void) {
}

void AudioSynth_Noop3(void) {
}

void AudioSynth_Noop4(void) {
}

void AudioSynth_Mix(Acmd* cmd, size_t size, s32 gain, s32 dmemIn, s32 dmemOut) {
    aMix(cmd, size, gain, dmemIn, dmemOut);
}

void AudioSynth_Noop5(void) {
}

void AudioSynth_Noop6(void) {
}

void AudioSynth_Noop7(void) {
}

void AudioSynth_SetBuffer(Acmd* cmd, s32 flags, s32 dmemIn, s32 dmemOut, u32 size) {
    aSetBuffer(cmd, flags, dmemIn, dmemOut, size);
}

void AudioSynth_Noop8(void) {
}

void AudioSynth_Noop9(void) {
}

// possible fake match?
void AudioSynth_DMemMove(Acmd* cmd, s32 dmemIn, s32 dmemOut, u32 size) {
    cmd->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(dmemIn, 0, 24);
    cmd->words.w1 = _SHIFTL(dmemOut, 16, 16) | _SHIFTL(size, 0, 16);
}

void AudioSynth_Noop10(void) {
}

void AudioSynth_Noop11(void) {
}

void AudioSynth_Noop12(void) {
}

void AudioSynth_Noop13(void) {
}

void AudioSynth_InterL(Acmd* cmd, s32 dmemIn, s32 dmemOut, s32 numSamples) {
    cmd->words.w0 = _SHIFTL(A_INTERL, 24, 8) | _SHIFTL(numSamples, 0, 16);
    cmd->words.w1 = _SHIFTL(dmemIn, 16, 16) | _SHIFTL(dmemOut, 0, 16);
}

void AudioSynth_EnvSetup1(Acmd* cmd, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    aEnvSetup1(cmd, arg1, arg2, arg3, arg4);
}

void AudioSynth_Noop14(void) {
}

void AudioSynth_LoadBuffer(Acmd* cmd, s32 dmemDest, s32 size, void* addrSrc) {
    aLoadBuffer(cmd, addrSrc, dmemDest, size);
}

void AudioSynth_SaveBuffer(Acmd* cmd, s32 dmemSrc, s32 size, void* addrDest) {
    aSaveBuffer(cmd, dmemSrc, addrDest, size);
}

void AudioSynth_EnvSetup2(Acmd* cmd, s32 volLeft, s32 volRight) {
    cmd->words.w0 = _SHIFTL(A_ENVSETUP2, 24, 8);
    cmd->words.w1 = _SHIFTL(volLeft, 16, 16) | _SHIFTL(volRight, 0, 16);
}

void AudioSynth_Noop15(void) {
}

void AudioSynth_Noop16(void) {
}

void AudioSynth_Noop17(void) {
}

void AudioSynth_S8Dec(Acmd* cmd, s32 flags, s16* state) {
    aS8Dec(cmd, flags, state);
}

void AudioSynth_HiLoGain(Acmd* cmd, s32 gain, s32 dmemIn, s32 dmemOut, s32 size) {
    cmd->words.w0 = _SHIFTL(A_HILOGAIN, 24, 8) | _SHIFTL(gain, 16, 8) | _SHIFTL(size, 0, 16);
    cmd->words.w1 = _SHIFTL(dmemIn, 16, 16) | _SHIFTL(dmemOut, 0, 16);
}

void AudioSynth_UnkCmd19(Acmd* cmd, s32 arg1, s32 arg2, s32 size, s32 arg4) {
    cmd->words.w0 = _SHIFTL(A_UNK19, 24, 8) | _SHIFTL(arg4, 16, 8) | _SHIFTL(size, 0, 16);
    cmd->words.w1 = _SHIFTL(arg1, 16, 16) | _SHIFTL(arg2, 0, 16);
}

void AudioSynth_Noop18(void) {
}

void AudioSynth_Noop19(void) {
}

void AudioSynth_Noop20(void) {
}

void AudioSynth_Noop21(void) {
}

void AudioSynth_UnkCmd3(Acmd* cmd, s32 arg1, s32 arg2, s32 size) {
    cmd->words.w0 = _SHIFTL(A_UNK3, 24, 8) | _SHIFTL(size, 0, 16);
    cmd->words.w1 = _SHIFTL(arg1, 16, 16) | _SHIFTL(arg2, 0, 16);
}

void AudioSynth_Noop22(void) {
}

void AudioSynth_Noop23(void) {
}

void AudioSynth_Noop24(void) {
}

void AudioSynth_LoadFilterBuffer(Acmd* cmd, s32 flags, s32 buf, void* addr) {
    aFilter(cmd, flags, buf, addr);
}

void AudioSynth_LoadFilterSize(Acmd* cmd, s32 size, void* addr) {
    aFilter(cmd, 2, size, addr);
}

Acmd* AudioSynth_LoadRingBuffer1(Acmd* cmd, s32 aiBufLen, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* ringBufferItem = &reverb->bufEntry[reverb->curFrame][updateIndex];

    cmd =
        AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, ringBufferItem->startPos, ringBufferItem->size, reverb);
    if (ringBufferItem->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + ringBufferItem->size, 0,
                                               ringBufferItem->wrappedSize, reverb);
    }

    return cmd;
}

Acmd* AudioSynth_LoadSubReverbSamples(Acmd* cmd, s32 aiBufLen, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* subEntry = &reverb->subBufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, subEntry->startPos, subEntry->size, reverb);
    if (subEntry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd =
            AudioSynth_LoadReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + subEntry->size, 0, subEntry->wrappedSize, reverb);
    }
    return cmd;
}

Acmd* AudioSynth_LoadReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb) {
    aLoadBuffer(cmd++, &reverb->leftRingBuf[startPos], dmem, size);
    aLoadBuffer(cmd++, &reverb->rightRingBuf[startPos], dmem + DMEM_1CH_SIZE, size);

    return cmd;
}

Acmd* AudioSynth_SaveReverbSamplesImpl(Acmd* cmd, u16 dmem, u16 startPos, s32 size, SynthesisReverb* reverb) {
    aSaveBuffer(cmd++, dmem, &reverb->leftRingBuf[startPos], size);
    aSaveBuffer(cmd++, dmem + DMEM_1CH_SIZE, &reverb->rightRingBuf[startPos], size);

    return cmd;
}

Acmd* AudioSynth_SaveBufferOffset(Acmd* cmd, u16 dmem, u16 offset, s32 size, s16* buf) {
    aSaveBuffer(cmd++, dmem, &buf[offset], size);
    return cmd;
}

Acmd* AudioSynth_LoadSubReverbSamplesWithoutDownsample(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb,
                                                       s16 updateIndex) {
    if (reverb->downsampleRate == 1) {
        cmd = AudioSynth_LoadSubReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);
    }

    return cmd;
}

Acmd* AudioSynth_LoadReverbSamples(Acmd* cmd, s32 numSamplesPerUpdate, SynthesisReverb* reverb, s16 updateIndex) {
    // Sets DMEM_WET_{LEFT,RIGHT}_CH, clobbers DMEM_TEMP
    if (reverb->downsampleRate == 1) {
        if (reverb->resampleEffectOn) {
            cmd = func_800DB828(cmd, numSamplesPerUpdate, reverb, updateIndex);
        } else {
            cmd = AudioSynth_LoadRingBuffer1(cmd, numSamplesPerUpdate, reverb, updateIndex);
        }
    } else {
        cmd = func_800DB4E4(cmd, numSamplesPerUpdate, reverb, updateIndex);
    }
    return cmd;
}

Acmd* AudioSynth_SaveReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* entry = &reverb->bufEntry[reverb->curFrame][updateIndex];

    if (reverb->downsampleRate == 1) {
        if (reverb->resampleEffectOn) {
            cmd = func_800DB680(cmd, reverb, updateIndex);
        } else {
            // Put the oldest samples in the ring buffer into the wet channels
            cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, entry->startPos, entry->size, reverb);
            if (entry->wrappedSize != 0) {
                // Ring buffer wrapped
                cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + entry->size, 0, entry->wrappedSize,
                                                       reverb);
            }
        }
    } else {
        // Downsampling is done later by CPU when RSP is done, therefore we need to have
        // double buffering. Left and right buffers are adjacent in memory.
        AudioSynth_SaveBuffer(cmd++, DMEM_WET_LEFT_CH, DMEM_2CH_SIZE,
                              reverb->bufEntry[reverb->curFrame][updateIndex].toDownsampleLeft);
    }

    reverb->resampleFlags = 0;
    return cmd;
}

Acmd* AudioSynth_SaveSubReverbSamples(Acmd* cmd, SynthesisReverb* reverb, s16 updateIndex) {
    ReverbSampleBufferEntry* subEntry = &reverb->subBufEntry[reverb->curFrame][updateIndex];

    cmd = AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH, subEntry->startPos, subEntry->size, reverb);
    if (subEntry->wrappedSize != 0) {
        // Ring buffer wrapped
        cmd =
            AudioSynth_SaveReverbSamplesImpl(cmd, DMEM_WET_LEFT_CH + subEntry->size, 0, subEntry->wrappedSize, reverb);
    }
    return cmd;
}

Acmd* AudioSynth_ProcessSamples(s16* aiBuf, s32 numSamplesPerUpdate, Acmd* cmd, s32 updateIndex) {
    u8 noteIndices[0x5C];
    s16 count;
    s16 reverbIndex;
    SynthesisReverb* reverb;
    s32 useReverb;
    s32 t;
    s32 i;
    NoteSampleState* noteSubEu;
    NoteSampleState* noteSubEu2;
    s32 subDelay;

    t = gAudioCtx.numNotes * updateIndex;
    count = 0;
    if (gAudioCtx.numSynthesisReverbs == 0) {
        for (i = 0; i < gAudioCtx.numNotes; i++) {
            if (gAudioCtx.sampleStateList[t + i].bitField0.enabled) {
                noteIndices[count++] = i;
            }
        }
    } else {
        for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
            for (i = 0; i < gAudioCtx.numNotes; i++) {
                noteSubEu = &gAudioCtx.sampleStateList[t + i];
                if (noteSubEu->bitField0.enabled && (noteSubEu->bitField1.reverbIndex == reverbIndex)) {
                    noteIndices[count++] = i;
                }
            }
        }

        for (i = 0; i < gAudioCtx.numNotes; i++) {
            noteSubEu = &gAudioCtx.sampleStateList[t + i];
            if (noteSubEu->bitField0.enabled && (noteSubEu->bitField1.reverbIndex >= gAudioCtx.numSynthesisReverbs)) {
                noteIndices[count++] = i;
            }
        }
    }

    aClearBuffer(cmd++, DMEM_LEFT_CH, DMEM_2CH_SIZE);

    i = 0;
    for (reverbIndex = 0; reverbIndex < gAudioCtx.numSynthesisReverbs; reverbIndex++) {
        reverb = &gAudioCtx.synthesisReverbs[reverbIndex];
        useReverb = reverb->useReverb;
        if (useReverb) {

            // Loads reverb samples from RDRAM (ringBuffer) into DMEM (DMEM_WET_LEFT_CH)
            cmd = AudioSynth_LoadReverbSamples(cmd, numSamplesPerUpdate, reverb, updateIndex);

            // Mixes reverb sample into the main dry channel
            // reverb->volume is always set to 0x7FFF (audio spec), and DMEM_LEFT_CH is cleared before the loop.
            // So for the first iteration, this is essentially a DMEMmove from DMEM_WET_LEFT_CH to DMEM_LEFT_CH
            aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->volume, DMEM_WET_LEFT_CH, DMEM_LEFT_CH);

            subDelay = reverb->subDelay;
            if (subDelay != 0) {
                aDMEMMove(cmd++, DMEM_WET_LEFT_CH, DMEM_WET_TEMP, DMEM_2CH_SIZE);
            }

            // Decays reverb over time. The (+ 0x8000) here is -100%
            aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->decayRatio + 0x8000, DMEM_WET_LEFT_CH, DMEM_WET_LEFT_CH);

            // Leak reverb between the left and right channels
            if (reverb->leakRtl != 0 || reverb->leakLtr != 0) {
                cmd = AudioSynth_LeakReverb(cmd, reverb);
            }

            if (subDelay != 0) {
                // Saves the wet channel sample from DMEM (DMEM_WET_LEFT_CH) into RDRAM (ringBuffer) for future use
                cmd = AudioSynth_SaveReverbSamples(cmd, reverb, updateIndex);
                if (reverb->mixReverbIndex != -1) {
                    cmd = AudioSynth_MixOtherReverbIndex(cmd, reverb, updateIndex);
                }
                cmd = AudioSynth_LoadSubReverbSamplesWithoutDownsample(cmd, numSamplesPerUpdate, reverb, updateIndex);
                aMix(cmd++, DMEM_2CH_SIZE >> 4, reverb->unk_16, DMEM_WET_TEMP, DMEM_WET_LEFT_CH);
            }
        }

        while (i < count) {
            noteSubEu2 = &gAudioCtx.sampleStateList[noteIndices[i] + t];
            if (noteSubEu2->bitField1.reverbIndex == reverbIndex) {
                cmd = AudioSynth_ProcessSample(noteIndices[i], noteSubEu2,
                                               &gAudioCtx.notes[noteIndices[i]].synthesisState, aiBuf,
                                               numSamplesPerUpdate, cmd, updateIndex);
            } else {
                break;
            }
            i++;
        }

        if (useReverb) {
            if ((reverb->filterLeft != NULL) || (reverb->filterRight != NULL)) {
                cmd = AudioSynth_FilterReverb(cmd, numSamplesPerUpdate * SAMPLE_SIZE, reverb);
            }

            // Saves the wet channel sample from DMEM (DMEM_WET_LEFT_CH) into RDRAM (ringBuffer) for future use
            if (subDelay != 0) {
                cmd = AudioSynth_SaveSubReverbSamples(cmd, reverb, updateIndex);
            } else {
                cmd = AudioSynth_SaveReverbSamples(cmd, reverb, updateIndex);
                if (reverb->mixReverbIndex != -1) {
                    cmd = AudioSynth_MixOtherReverbIndex(cmd, reverb, updateIndex);
                }
            }
        }
    }

    while (i < count) {
        cmd = AudioSynth_ProcessSample(noteIndices[i], &gAudioCtx.sampleStateList[t + noteIndices[i]],
                                       &gAudioCtx.notes[noteIndices[i]].synthesisState, aiBuf, numSamplesPerUpdate, cmd,
                                       updateIndex);
        i++;
    }

    aInterleave(cmd++, DMEM_TEMP, DMEM_LEFT_CH, DMEM_RIGHT_CH, 2 * numSamplesPerUpdate);
    aSaveBuffer(cmd++, DMEM_TEMP, aiBuf, 2 * (numSamplesPerUpdate * (s32)SAMPLE_SIZE));

    return cmd;
}

Acmd* AudioSynth_ProcessSample(s32 noteIndex, NoteSampleState* sampleState, NoteSynthesisState* synthState, s16* aiBuf,
                               s32 aiBufLen, Acmd* cmd, s32 updateIndex) {
    s32 pad1[3];
    Sample* sample;
    AdpcmLoop* loopInfo;
    s32 numSamplesUntilEnd;
    s32 nSamplesInThisIteration;
    s32 sampleFinished;
    s32 restart;
    s32 flags;
    u16 resamplingRateFixedPoint;
    s32 numSamplesInFirstFrame;
    s32 numTrailingSamplesToIgnore;
    s32 gain;
    s32 frameIndex;
    s32 skipBytes;
    s32 temp_v1_6;
    void* combFilterState;
    s32 numSamplesToDecode;
    u8* sampleAddr;
    u32 samplesLenFixedPoint;
    s32 numSamplesToLoadAdj; // seems to be used as both bytes and numSamples?
    s32 numSamplesProcessed;
    s32 loopEndPos;
    s32 numSamplesToProcess;
    s32 phi_s4;
    s32 numFirstFrameSamplesToIgnore;
    s32 pad2[7];
    s32 frameSize;
    s32 numFramesToDecode;
    s32 skipInitialSamples;
    s32 sampleDataStart;
    u8* sampleData;
    s32 nParts;
    s32 curPart;
    s32 sampleDataStartPad;
    s32 haasEffectDelaySide;
    s32 numSamplesToLoadFirstPart;
    u16 sampleDmemBeforeResampling;
    s32 sampleDataOffset;
    s32 combFilterDmemAddr;
    s32 s5;
    Note* note;
    u32 numSamplesToLoad;
    u16 combFilterSize;
    u16 combFilterGain;
    s16* filter;
    s32 bookOffset;
    s32 finished;
    s32 aligned;
    s16 addr;

    bookOffset = sampleState->bitField1.bookOffset;
    finished = sampleState->bitField0.finished;
    note = &gAudioCtx.notes[noteIndex];
    flags = A_CONTINUE;

    // Initialize the synthesis state
    if (sampleState->bitField0.needsInit == true) {
        flags = A_INIT;
        synthState->restart = 0;
        synthState->samplePosInt = note->startSamplePos;
        synthState->samplePosFrac = 0;
        synthState->curVolLeft = 0;
        synthState->curVolRight = 0;
        synthState->prevHaasEffectLeftDelaySize = 0;
        synthState->prevHaasEffectRightDelaySize = 0;
        synthState->reverbVol = sampleState->reverbVol;
        synthState->numParts = 0;
        synthState->unk_1A = 1;
        note->noteSubEu.bitField0.finished = false;
        finished = false;
    }

    // Process the sample in either one or two parts
    nParts = sampleState->bitField1.hasTwoParts + 1;

    // Determine number of samples to load based on numSamplesPerUpdate and relative frequency
    resamplingRateFixedPoint = sampleState->resamplingRateFixedPoint;
    samplesLenFixedPoint = (resamplingRateFixedPoint * aiBufLen * 2) + synthState->samplePosFrac;
    numSamplesToLoad = samplesLenFixedPoint >> 16;
    synthState->samplePosFrac = samplesLenFixedPoint & 0xFFFF;

    // Partially-optimized out no-op ifs required for matching. SM64 decomp
    // makes it clear that this is how it should look.
    if (synthState->numParts == 1 && nParts == 2) {
    } else if (synthState->numParts == 2 && nParts == 1) {
    } else {
    }

    synthState->numParts = nParts;

    if (sampleState->bitField1.isSyntheticWave) {
        cmd = AudioSynth_LoadWaveSamples(cmd, sampleState, synthState, numSamplesToLoad);
        sampleDmemBeforeResampling = DMEM_UNCOMPRESSED_NOTE + (synthState->samplePosInt * (s32)SAMPLE_SIZE);
        synthState->samplePosInt += numSamplesToLoad;
    } else {
        sample = sampleState->tunedSample->sample;
        loopInfo = sample->loop;
        loopEndPos = loopInfo->end;
        sampleAddr = sample->sampleAddr;
        numSamplesToLoadFirstPart = 0;

        // If the frequency requested is more than double that of the raw sample,
        // then the sample processing is split into two parts.
        for (curPart = 0; curPart < nParts; curPart++) {
            numSamplesProcessed = 0;
            s5 = 0;

            // Adjust the number of samples to load only if there are two parts and an odd number of samples
            if (nParts == 1) {
                numSamplesToLoadAdj = numSamplesToLoad;
            } else if (numSamplesToLoad & 1) {
                // round down for the first part
                // round up for the second part
                numSamplesToLoadAdj = (numSamplesToLoad & ~1) + (curPart * 2);
            } else {
                numSamplesToLoadAdj = numSamplesToLoad;
            }

            // Load the ADPCM codeBook
            if ((sample->codec == CODEC_ADPCM) || (sample->codec == CODEC_SMALL_ADPCM)) {
                if (gAudioCtx.adpcmCodeBook != sample->book->book) {
                    u32 nEntries;

                    switch (bookOffset) {
                        case 1:
                            gAudioCtx.adpcmCodeBook = &gInvalidAdpcmCodeBook[1];
                            break;

                        case 2:
                        case 3:
                        default:
                            gAudioCtx.adpcmCodeBook = sample->book->book;
                            break;
                    }
                    if (1) {}
                    if (1) {}
                    if (1) {}
                    nEntries = SAMPLES_PER_FRAME * sample->book->order * sample->book->numPredictors;
                    aLoadADPCM(cmd++, nEntries, gAudioCtx.adpcmCodeBook);
                }
            }

            while (numSamplesProcessed != numSamplesToLoadAdj) {
                sampleFinished = false;
                restart = false;
                phi_s4 = 0;

                numFirstFrameSamplesToIgnore = synthState->samplePosInt & 0xF;
                numSamplesUntilEnd = loopEndPos - synthState->samplePosInt;
                numSamplesToProcess = numSamplesToLoadAdj - numSamplesProcessed;

                if ((numFirstFrameSamplesToIgnore == 0) && !synthState->restart) {
                    numFirstFrameSamplesToIgnore = SAMPLES_PER_FRAME;
                }
                numSamplesInFirstFrame = SAMPLES_PER_FRAME - numFirstFrameSamplesToIgnore;

                // Determine the number of samples to decode based on whether the end will be reached or not.
                if (numSamplesToProcess < numSamplesUntilEnd) {
                    // The end will not be reached.
                    numFramesToDecode =
                        (s32)(numSamplesToProcess - numSamplesInFirstFrame + SAMPLES_PER_FRAME - 1) / SAMPLES_PER_FRAME;
                    numSamplesToDecode = numFramesToDecode * SAMPLES_PER_FRAME;
                    numTrailingSamplesToIgnore = numSamplesInFirstFrame + numSamplesToDecode - numSamplesToProcess;
                } else {
                    // The end will be reached.
                    numSamplesToDecode = numSamplesUntilEnd - numSamplesInFirstFrame;
                    numTrailingSamplesToIgnore = 0;
                    if (numSamplesToDecode <= 0) {
                        numSamplesToDecode = 0;
                        numSamplesInFirstFrame = numSamplesUntilEnd;
                    }
                    numFramesToDecode = (numSamplesToDecode + SAMPLES_PER_FRAME - 1) / SAMPLES_PER_FRAME;
                    if (loopInfo->count != 0) {
                        // Loop around and restart
                        restart = true;
                    } else {
                        sampleFinished = true;
                    }
                }

                // Set parameters based on compression type
                switch (sample->codec) {
                    case CODEC_ADPCM:
                        // 16 2-byte samples (32 bytes) compressed into 4-bit samples (8 bytes) + 1 header byte
                        frameSize = 9;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        sampleDataStart = 0;
                        break;

                    case CODEC_SMALL_ADPCM:
                        // 16 2-byte samples (32 bytes) compressed into 2-bit samples (4 bytes) + 1 header byte
                        frameSize = 5;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        sampleDataStart = 0;
                        break;

                    case CODEC_S8:
                        // 16 2-byte samples (32 bytes) compressed into 8-bit samples (16 bytes)
                        frameSize = 16;
                        skipInitialSamples = SAMPLES_PER_FRAME;
                        sampleDataStart = 0;
                        break;

                    case CODEC_S16_INMEMORY:
                        AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE,
                                               (numSamplesToLoadAdj + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
                        flags = A_CONTINUE;
                        skipBytes = 0;
                        numSamplesProcessed = numSamplesToLoadAdj;
                        s5 = numSamplesToLoadAdj;
                        goto skip;

                    case CODEC_S16:
                        AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE,
                                               (numSamplesToLoadAdj + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
                        flags = A_CONTINUE;
                        skipBytes = 0;
                        numSamplesProcessed = numSamplesToLoadAdj;
                        s5 = numSamplesToLoadAdj;
                        goto skip;

                    case CODEC_REVERB:
                        break;
                }

                // Move the compressed raw sample data from ram into the rsp (DMEM)
                if (numFramesToDecode != 0) {
                    // Get the offset from the start of the sample to where the sample is currently playing from
                    frameIndex = (synthState->samplePosInt + skipInitialSamples - numFirstFrameSamplesToIgnore) /
                                 SAMPLES_PER_FRAME;
                    sampleDataOffset = frameIndex * frameSize;

                    // Get the ram address of the requested sample chunk
                    if (sample->medium == MEDIUM_RAM) {
                        // Sample is already loaded into ram
                        sampleData = sampleAddr + (sampleDataStart + sampleDataOffset);
                    } else if (sample->medium == MEDIUM_UNK) {
                        // This medium is unsupported so terminate processing this note
                        return cmd;
                    } else {
                        // This medium is not in ram, so dma the requested sample into ram
                        sampleData =
                            AudioLoad_DmaSampleData((uintptr_t)(sampleAddr + (sampleDataStart + sampleDataOffset)),
                                                    ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME), flags,
                                                    &synthState->sampleDmaIndex, sample->medium);
                    }

                    if (sampleData == NULL) {
                        // The ram address was unsuccessfully allocated
                        return cmd;
                    }

                    // Move the raw sample chunk from ram to the rsp
                    // DMEM at the addresses before DMEM_COMPRESSED_ADPCM_DATA
                    sampleDataStartPad = (u32)sampleData & 0xF;
                    aligned = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                    addr = DMEM_COMPRESSED_ADPCM_DATA - aligned;
                    aLoadBuffer(cmd++, sampleData - sampleDataStartPad, addr, aligned);
                } else {
                    numSamplesToDecode = 0;
                    sampleDataStartPad = 0;
                }

                if (synthState->restart) {
                    aSetLoop(cmd++, sample->loop->predictorState);
                    flags = A_LOOP;
                    synthState->restart = false;
                }

                nSamplesInThisIteration = numSamplesToDecode + numSamplesInFirstFrame - numTrailingSamplesToIgnore;

                if (numSamplesProcessed == 0) {
                    if (1) {}
                    skipBytes = numFirstFrameSamplesToIgnore * SAMPLE_SIZE;
                } else {
                    phi_s4 = ALIGN16(s5 + 8 * SAMPLE_SIZE);
                }

                // Decompress the raw sample chunks in the rsp
                // Goes from adpcm (compressed) sample data to pcm (uncompressed) sample data
                switch (sample->codec) {
                    case CODEC_ADPCM:
                        aligned = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        addr = DMEM_COMPRESSED_ADPCM_DATA - aligned;
                        aSetBuffer(cmd++, 0, addr + sampleDataStartPad, DMEM_UNCOMPRESSED_NOTE + phi_s4,
                                   numSamplesToDecode * SAMPLE_SIZE);
                        aADPCMdec(cmd++, flags, synthState->synthesisBuffers->adpcmdecState);
                        break;

                    case CODEC_SMALL_ADPCM:
                        aligned = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        addr = DMEM_COMPRESSED_ADPCM_DATA - aligned;
                        aSetBuffer(cmd++, 0, addr + sampleDataStartPad, DMEM_UNCOMPRESSED_NOTE + phi_s4,
                                   numSamplesToDecode * SAMPLE_SIZE);
                        aADPCMdec(cmd++, flags | 4, synthState->synthesisBuffers->adpcmdecState);
                        break;

                    case CODEC_S8:
                        aligned = ALIGN16((numFramesToDecode * frameSize) + SAMPLES_PER_FRAME);
                        addr = DMEM_COMPRESSED_ADPCM_DATA - aligned;
                        AudioSynth_SetBuffer(cmd++, 0, addr + sampleDataStartPad, DMEM_UNCOMPRESSED_NOTE + phi_s4,
                                             numSamplesToDecode * SAMPLE_SIZE);
                        AudioSynth_S8Dec(cmd++, flags, synthState->synthesisBuffers->adpcmdecState);
                        break;
                }

                if (numSamplesProcessed != 0) {
                    aDMEMMove(cmd++, DMEM_UNCOMPRESSED_NOTE + phi_s4 + (numFirstFrameSamplesToIgnore * SAMPLE_SIZE),
                              DMEM_UNCOMPRESSED_NOTE + s5, nSamplesInThisIteration * SAMPLE_SIZE);
                }

                numSamplesProcessed += nSamplesInThisIteration;

                switch (flags) {
                    case A_INIT:
                        skipBytes = SAMPLES_PER_FRAME * SAMPLE_SIZE;
                        s5 = (numSamplesToDecode + SAMPLES_PER_FRAME) * SAMPLE_SIZE;
                        break;

                    case A_LOOP:
                        s5 = nSamplesInThisIteration * SAMPLE_SIZE + s5;
                        break;

                    default:
                        if (s5 != 0) {
                            s5 = nSamplesInThisIteration * SAMPLE_SIZE + s5;
                        } else {
                            s5 = (numFirstFrameSamplesToIgnore + nSamplesInThisIteration) * SAMPLE_SIZE;
                        }
                        break;
                }

                flags = A_CONTINUE;

            skip:

                // Update what to do with the samples next
                if (sampleFinished) {
                    AudioSynth_ClearBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE + s5,
                                           (numSamplesToLoadAdj - numSamplesProcessed) * SAMPLE_SIZE);
                    finished = true;
                    note->noteSubEu.bitField0.finished = true;
                    AudioSynth_DisableSampleStates(updateIndex, noteIndex);
                    break; // break out of the for-loop
                } else if (restart) {
                    synthState->restart = true;
                    synthState->samplePosInt = loopInfo->start;
                } else {
                    synthState->samplePosInt += numSamplesToProcess;
                }
            }

            switch (nParts) {
                case 1:
                    sampleDmemBeforeResampling = DMEM_UNCOMPRESSED_NOTE + skipBytes;
                    break;

                case 2:
                    switch (curPart) {
                        case 0:
                            AudioSynth_InterL(cmd++, DMEM_UNCOMPRESSED_NOTE + skipBytes,
                                              DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE),
                                              ALIGN8(numSamplesToLoadAdj / 2));
                            numSamplesToLoadFirstPart = numSamplesToLoadAdj;
                            sampleDmemBeforeResampling = DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE);
                            if (finished) {
                                AudioSynth_ClearBuffer(cmd++, sampleDmemBeforeResampling + numSamplesToLoadFirstPart,
                                                       numSamplesToLoadAdj + SAMPLES_PER_FRAME);
                            }
                            break;

                        case 1:
                            AudioSynth_InterL(cmd++, DMEM_UNCOMPRESSED_NOTE + skipBytes,
                                              DMEM_TEMP + (SAMPLES_PER_FRAME * SAMPLE_SIZE) + numSamplesToLoadFirstPart,
                                              ALIGN8(numSamplesToLoadAdj / 2));
                            break;
                    }
            }
            if (finished) {
                break;
            }
        }
    }

    // Update the flags for the signal processing below
    flags = A_CONTINUE;
    if (sampleState->bitField0.needsInit == true) {
        sampleState->bitField0.needsInit = false;
        flags = A_INIT;
    }

    // Resample the decompressed mono-signal to the correct pitch
    cmd = AudioSynth_FinalResample(cmd, synthState, aiBufLen * (s32)SAMPLE_SIZE, resamplingRateFixedPoint,
                                   sampleDmemBeforeResampling, flags);

    // Unused blocks, bookOffset is always 0
    if (bookOffset == 3) {
        AudioSynth_UnkCmd19(cmd++, DMEM_TEMP, DMEM_TEMP, aiBufLen * SAMPLE_SIZE, 0);
    }
    if (bookOffset == 2) {
        AudioSynth_UnkCmd3(cmd++, DMEM_TEMP, DMEM_TEMP, aiBufLen * SAMPLE_SIZE);
    }

    gain = sampleState->gain;
    if (gain != 0) {
        // A gain of 0x10 (a UQ4.4 number) is equivalent to 1.0 and represents no volume change
        if (gain < 0x10) {
            gain = 0x10;
        }
        AudioSynth_HiLoGain(cmd++, gain, DMEM_TEMP, 0, (aiBufLen + SAMPLES_PER_FRAME) * SAMPLE_SIZE);
    }

    // Apply the filter to the mono-signal
    filter = sampleState->filter;
    if (filter != 0) {
        AudioSynth_LoadFilterSize(cmd++, aiBufLen * SAMPLE_SIZE, filter);
        AudioSynth_LoadFilterBuffer(cmd++, flags, DMEM_TEMP, synthState->synthesisBuffers->mixEnvelopeState);
    }

    // Apply the comb filter to the mono-signal by taking the signal with a small temporal offset,
    // and adding it back to itself
    combFilterSize = sampleState->unk_07;
    combFilterGain = sampleState->unk_0E;
    combFilterState = synthState->synthesisBuffers->combFilterState;
    if ((combFilterSize != 0) && (sampleState->unk_0E != 0)) {
        AudioSynth_DMemMove(cmd++, DMEM_TEMP, DMEM_COMB_TEMP, aiBufLen * SAMPLE_SIZE);
        combFilterDmemAddr = DMEM_COMB_TEMP - combFilterSize;
        if (synthState->unk_1A != 0) {
            AudioSynth_ClearBuffer(cmd++, combFilterDmemAddr, combFilterSize);
            synthState->unk_1A = 0;
        } else {
            AudioSynth_LoadBuffer(cmd++, combFilterDmemAddr, combFilterSize, combFilterState);
        }
        AudioSynth_SaveBuffer(cmd++, DMEM_TEMP + (aiBufLen * SAMPLE_SIZE) - combFilterSize, combFilterSize,
                              combFilterState);
        AudioSynth_Mix(cmd++, (aiBufLen * (s32)SAMPLE_SIZE) >> 4, combFilterGain, DMEM_COMB_TEMP, combFilterDmemAddr);
        AudioSynth_DMemMove(cmd++, combFilterDmemAddr, DMEM_TEMP, aiBufLen * SAMPLE_SIZE);
    } else {
        synthState->unk_1A = 1;
    }

    // Determine the behavior of the audio processing that leads to the haas effect
    if ((sampleState->haasEffectLeftDelaySize != 0) || (synthState->prevHaasEffectLeftDelaySize != 0)) {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_LEFT;
    } else if ((sampleState->haasEffectRightDelaySize != 0) || (synthState->prevHaasEffectRightDelaySize != 0)) {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_RIGHT;
    } else {
        haasEffectDelaySide = HAAS_EFFECT_DELAY_NONE;
    }

    // Split the mono-signal into left and right channels:
    // Both for dry signal (to go to the speakers now)
    // and for wet signal (to go to a reverb buffer to be stored, and brought back later to produce an echo)
    cmd = AudioSynth_ProcessEnvelope(cmd, sampleState, synthState, aiBufLen, DMEM_TEMP, haasEffectDelaySide, flags);

    // Apply the haas effect by delaying either the left or the right channel by a small amount
    if (sampleState->bitField1.useHaasEffect) {
        if (!(flags & A_INIT)) {
            flags = A_CONTINUE;
        }
        cmd = AudioSynth_ApplyHaasEffect(cmd, sampleState, synthState, aiBufLen * (s32)SAMPLE_SIZE, flags,
                                         haasEffectDelaySide);
    }

    return cmd;
}

Acmd* AudioSynth_FinalResample(Acmd* cmd, NoteSynthesisState* synthState, s32 size, u16 pitch, u16 inpDmem,
                               s32 resampleFlags) {
    if (pitch == 0) {
        AudioSynth_ClearBuffer(cmd++, DMEM_TEMP, size);
    } else {
        aSetBuffer(cmd++, 0, inpDmem, DMEM_TEMP, size);
        aResample(cmd++, resampleFlags, pitch, synthState->synthesisBuffers->finalResampleState);
    }
    return cmd;
}

Acmd* AudioSynth_ProcessEnvelope(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 aiBufLen,
                                 u16 dmemSrc, s32 haasEffectDelaySide, s32 flags) {
    u32 dmemDests;
    u16 curVolLeft;
    u16 targetVolLeft;
    s32 phi_t1;
    s16 reverbVol;
    u16 curVolRight;
    s16 rampLeft;
    s16 rampRight;
    s16 rampReverb;
    s16 sourceReverbVol;
    u16 targetVolRight;
    s32 pad;

    curVolLeft = synthState->curVolLeft;
    targetVolLeft = sampleState->targetVolLeft;
    targetVolLeft <<= 4;
    reverbVol = sampleState->reverbVol;
    curVolRight = synthState->curVolRight;
    targetVolRight = sampleState->targetVolRight;
    targetVolRight <<= 4;

    if (targetVolLeft != curVolLeft) {
        rampLeft = (targetVolLeft - curVolLeft) / (aiBufLen >> 3);
    } else {
        rampLeft = 0;
    }
    if (targetVolRight != curVolRight) {
        rampRight = (targetVolRight - curVolRight) / (aiBufLen >> 3);
    } else {
        rampRight = 0;
    }

    sourceReverbVol = synthState->reverbVol;
    phi_t1 = sourceReverbVol & 0x7F;

    if (sourceReverbVol != reverbVol) {
        rampReverb = (((reverbVol & 0x7F) - phi_t1) << 9) / (aiBufLen >> 3);
        synthState->reverbVol = reverbVol;
    } else {
        rampReverb = 0;
    }

    synthState->curVolLeft = curVolLeft + (rampLeft * (aiBufLen >> 3));
    synthState->curVolRight = curVolRight + (rampRight * (aiBufLen >> 3));

    if (sampleState->bitField1.useHaasEffect) {
        AudioSynth_ClearBuffer(cmd++, DMEM_HAAS_TEMP, DMEM_1CH_SIZE);
        AudioSynth_EnvSetup1(cmd++, phi_t1 * 2, rampReverb, rampLeft, rampRight);
        AudioSynth_EnvSetup2(cmd++, curVolLeft, curVolRight);

        switch (haasEffectDelaySide) {
            case HAAS_EFFECT_DELAY_LEFT:
                // Store the left dry channel in a temp space to be delayed to produce the haas effect
                dmemDests = sEnvMixerLeftHaasDmemDests;
                break;

            case HAAS_EFFECT_DELAY_RIGHT:
                // Store the right dry channel in a temp space to be delayed to produce the haas effect
                dmemDests = sEnvMixerRightHaasDmemDests;
                break;

            default: // HAAS_EFFECT_DELAY_NONE
                dmemDests = sEnvMixerDefaultDmemDests;
                break;
        }
    } else {
        aEnvSetup1(cmd++, phi_t1 * 2, rampReverb, rampLeft, rampRight);
        aEnvSetup2(cmd++, curVolLeft, curVolRight);
        dmemDests = sEnvMixerDefaultDmemDests;
    }

    aEnvMixer(cmd++, dmemSrc, aiBufLen, (sourceReverbVol & 0x80) >> 7, sampleState->bitField0.stereoHeadsetEffects,
              sampleState->bitField0.usesHeadsetPanEffects, sampleState->bitField0.stereoStrongRight,
              sampleState->bitField0.stereoStrongLeft, dmemDests, sEnvMixerOp);

    return cmd;
}

Acmd* AudioSynth_LoadWaveSamples(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState,
                                 s32 numSamplesToLoad) {
    s32 numSamplesAvail;
    s32 harmonicIndexCurAndPrev = sampleState->harmonicIndexCurAndPrev;
    s32 samplePosInt = synthState->samplePosInt;
    s32 numDuplicates;

    if (sampleState->bitField1.bookOffset != 0) {
        // Move the noise wave (that reads compiled assembly as samples) from ram to dmem
        AudioSynth_LoadBuffer(cmd++, DMEM_UNCOMPRESSED_NOTE, ALIGN16(numSamplesToLoad * SAMPLE_SIZE), gWaveSamples[8]);
        // Offset the address for the samples read by gWaveSamples[8] to the next set of samples
        gWaveSamples[8] += numSamplesToLoad * SAMPLE_SIZE;
        return cmd;
    } else {
        // Move the synthetic wave from ram to dmem
        aLoadBuffer(cmd++, sampleState->waveSampleAddr, DMEM_UNCOMPRESSED_NOTE, WAVE_SAMPLE_COUNT * SAMPLE_SIZE);

        // If the harmonic changes, map the offset in the wave from one harmonic to another for continuity
        if (harmonicIndexCurAndPrev != 0) {
            samplePosInt = samplePosInt * sNumSamplesPerWavePeriod[harmonicIndexCurAndPrev >> 2] /
                           sNumSamplesPerWavePeriod[harmonicIndexCurAndPrev & 3];
        }

        // Offset in the WAVE_SAMPLE_COUNT samples of gWaveSamples to start processing the wave for continuity
        samplePosInt = (u32)samplePosInt % WAVE_SAMPLE_COUNT;
        // Number of samples in the initial WAVE_SAMPLE_COUNT samples available to be used to process
        numSamplesAvail = WAVE_SAMPLE_COUNT - samplePosInt;

        // Require duplicates if there are more samples to load than available
        if (numSamplesToLoad > numSamplesAvail) {
            // Duplicate (copy) the WAVE_SAMPLE_COUNT samples as many times as needed to reach numSamplesToLoad.
            // (numSamplesToLoad - numSamplesAvail) is the number of samples missing.
            // Divide by WAVE_SAMPLE_COUNT, rounding up, to get the amount of duplicates
            numDuplicates = ((numSamplesToLoad - numSamplesAvail + WAVE_SAMPLE_COUNT - 1) / WAVE_SAMPLE_COUNT);
            if (numDuplicates != 0) {
                aDuplicate(cmd++, numDuplicates, DMEM_UNCOMPRESSED_NOTE,
                           DMEM_UNCOMPRESSED_NOTE + (WAVE_SAMPLE_COUNT * SAMPLE_SIZE));
            }
        }
        synthState->samplePosInt = samplePosInt;
    }
    return cmd;
}

/**
 * The Haas Effect gives directionality to sound by applying a small (< 35ms) delay to either the left or right channel.
 * The delay is small enough that the sound is still perceived as one sound, but the channel that is not delayed will
 * reach our ear first and give a sense of directionality. The sound is directed towards the opposite side of the delay.
 */
Acmd* AudioSynth_ApplyHaasEffect(Acmd* cmd, NoteSampleState* sampleState, NoteSynthesisState* synthState, s32 size,
                                 s32 flags, s32 haasEffectDelaySide) {
    u16 dmemDest;
    u16 pitch;
    u8 prevHaasEffectDelaySize;
    u8 haasEffectDelaySize;

    switch (haasEffectDelaySide) {
        case HAAS_EFFECT_DELAY_LEFT:
            // Delay the sample on the left channel
            // This allows the right channel to be heard first
            dmemDest = DMEM_LEFT_CH;
            haasEffectDelaySize = sampleState->haasEffectLeftDelaySize;
            prevHaasEffectDelaySize = synthState->prevHaasEffectLeftDelaySize;
            synthState->prevHaasEffectRightDelaySize = 0;
            synthState->prevHaasEffectLeftDelaySize = haasEffectDelaySize;
            break;

        case HAAS_EFFECT_DELAY_RIGHT:
            // Delay the sample on the right channel
            // This allows the left channel to be heard first
            dmemDest = DMEM_RIGHT_CH;
            haasEffectDelaySize = sampleState->haasEffectRightDelaySize;
            prevHaasEffectDelaySize = synthState->prevHaasEffectRightDelaySize;
            synthState->prevHaasEffectRightDelaySize = haasEffectDelaySize;
            synthState->prevHaasEffectLeftDelaySize = 0;
            break;

        default: // HAAS_EFFECT_DELAY_NONE
            return cmd;
    }

    if (flags != A_INIT) {
        // Slightly adjust the sample rate in order to fit a change in sample delay
        if (haasEffectDelaySize != prevHaasEffectDelaySize) {
            pitch = (((size << 0xF) / 2) - 1) / ((size + haasEffectDelaySize - prevHaasEffectDelaySize - 2) / 2);
            aSetBuffer(cmd++, 0, DMEM_HAAS_TEMP, DMEM_TEMP, size + haasEffectDelaySize - prevHaasEffectDelaySize);
            aResampleZoh(cmd++, pitch, 0);
        } else {
            aDMEMMove(cmd++, DMEM_HAAS_TEMP, DMEM_TEMP, size);
        }

        if (prevHaasEffectDelaySize != 0) {
            aLoadBuffer(cmd++, synthState->synthesisBuffers->haasEffectDelayState, DMEM_HAAS_TEMP,
                        ALIGN16(prevHaasEffectDelaySize));
            aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP + prevHaasEffectDelaySize,
                      size + haasEffectDelaySize - prevHaasEffectDelaySize);
        } else {
            aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP, size + haasEffectDelaySize);
        }
    } else {
        // Just apply a delay directly
        aDMEMMove(cmd++, DMEM_HAAS_TEMP, DMEM_TEMP, size);
        aClearBuffer(cmd++, DMEM_HAAS_TEMP, haasEffectDelaySize);
        aDMEMMove(cmd++, DMEM_TEMP, DMEM_HAAS_TEMP + haasEffectDelaySize, size);
    }

    if (haasEffectDelaySize) { // != 0
        // Save excessive samples for next iteration
        aSaveBuffer(cmd++, DMEM_HAAS_TEMP + size, synthState->synthesisBuffers->haasEffectDelayState,
                    ALIGN16(haasEffectDelaySize));
    }

    aAddMixer(cmd++, ALIGN64(size), DMEM_HAAS_TEMP, dmemDest, 0x7FFF);

    return cmd;
}
