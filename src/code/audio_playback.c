#include "global.h"

void AudioNote_InitSampleState(Note* note, NoteSampleState* sampleState, NoteSubAttributes* attrs) {
    f32 volLeft;
    f32 volRight;
    s32 halfPanIndex;
    u64 pad;
    u8 strongLeft;
    u8 strongRight;
    f32 vel;
    u8 pan;
    u8 targetReverbVol;
    StereoData stereoData;
    s32 stereoHeadsetEffects = note->playbackState.stereoHeadsetEffects;

    vel = attrs->velocity;
    pan = attrs->pan;
    targetReverbVol = attrs->targetReverbVol;
    stereoData = attrs->stereo.s;

    sampleState->bitField0 = note->sampleState.bitField0;
    sampleState->bitField1 = note->sampleState.bitField1;
    sampleState->waveSampleAddr = note->sampleState.waveSampleAddr;
    sampleState->harmonicIndexCurAndPrev = note->sampleState.harmonicIndexCurAndPrev;

    AudioNote_SetResamplingRate(sampleState, attrs->frequency);

    pan &= 0x7F;

    sampleState->bitField0.strongRight = false;
    sampleState->bitField0.strongLeft = false;
    sampleState->bitField0.strongReverbRight = stereoData.strongReverbRight;
    sampleState->bitField0.strongReverbLeft = stereoData.strongReverbLeft;
    if (stereoHeadsetEffects && (gAudioCtx.soundMode == SOUNDMODE_HEADSET)) {
        halfPanIndex = pan >> 1;
        if (halfPanIndex > 0x3F) {
            halfPanIndex = 0x3F;
        }

        sampleState->haasEffectRightDelaySize = gHaasEffectDelaySizes[halfPanIndex];
        sampleState->haasEffectLeftDelaySize = gHaasEffectDelaySizes[0x3F - halfPanIndex];
        sampleState->bitField1.useHaasEffect = true;

        volLeft = gHeadsetPanVolume[pan];
        volRight = gHeadsetPanVolume[0x7F - pan];
    } else if (stereoHeadsetEffects && (gAudioCtx.soundMode == SOUNDMODE_STEREO)) {
        strongLeft = strongRight = 0;
        sampleState->haasEffectLeftDelaySize = 0;
        sampleState->haasEffectRightDelaySize = 0;
        sampleState->bitField1.useHaasEffect = false;

        volLeft = gStereoPanVolume[pan];
        volRight = gStereoPanVolume[0x7F - pan];
        if (pan < 0x20) {
            strongLeft = 1;
        } else if (pan > 0x60) {
            strongRight = 1;
        }

        sampleState->bitField0.strongRight = strongRight;
        sampleState->bitField0.strongLeft = strongLeft;

        switch (stereoData.bit2) {
            case 0:
                break;

            case 1:
                sampleState->bitField0.strongRight = stereoData.strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft;
                break;

            case 2:
                sampleState->bitField0.strongRight = stereoData.strongRight | strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft | strongLeft;
                break;

            case 3:
                sampleState->bitField0.strongRight = stereoData.strongRight ^ strongRight;
                sampleState->bitField0.strongLeft = stereoData.strongLeft ^ strongLeft;
                break;
        }

    } else if (gAudioCtx.soundMode == SOUNDMODE_MONO) {
        sampleState->bitField0.strongReverbRight = false;
        sampleState->bitField0.strongReverbLeft = false;
        volLeft = 0.707f; // approx 1/sqrt(2)
        volRight = 0.707f;
    } else {
        sampleState->bitField0.strongRight = stereoData.strongRight;
        sampleState->bitField0.strongLeft = stereoData.strongLeft;
        volLeft = gDefaultPanVolume[pan];
        volRight = gDefaultPanVolume[0x7F - pan];
    }

    vel = 0.0f > vel ? 0.0f : vel;
    vel = 1.0f < vel ? 1.0f : vel;

    sampleState->targetVolLeft = (s32)((vel * volLeft) * (0x1000 - 0.001f));
    sampleState->targetVolRight = (s32)((vel * volRight) * (0x1000 - 0.001f));

    sampleState->gain = attrs->gain;
    sampleState->filter = attrs->filter;
    sampleState->combFilterSize = attrs->combFilterSize;
    sampleState->combFilterGain = attrs->combFilterGain;
    sampleState->targetReverbVol = targetReverbVol;
}

void AudioNote_SetResamplingRate(NoteSampleState* sampleState, f32 frequency) {
    f32 resamplingRate = 0.0f;

    if (frequency < 2.0f) {
        sampleState->bitField1.hasTwoParts = false;
        resamplingRate = CLAMP_MAX(frequency, 1.99998f);

    } else {
        sampleState->bitField1.hasTwoParts = true;
        if (frequency > 3.99996f) {
            resamplingRate = 1.99998f;
        } else {
            resamplingRate = frequency * 0.5f;
        }
    }
    sampleState->frequencyFixedPoint = (s32)(resamplingRate * 0x8000);
}

void AudioNote_Init(Note* note) {
    if (note->playbackState.parentLayer->adsr.decayIndex == 0) {
        AudioEffects_InitAdsr(&note->playbackState.adsr, note->playbackState.parentLayer->channel->adsr.envelope,
                              &note->playbackState.adsrVolScaleUnused);
    } else {
        AudioEffects_InitAdsr(&note->playbackState.adsr, note->playbackState.parentLayer->adsr.envelope,
                              &note->playbackState.adsrVolScaleUnused);
    }

    note->playbackState.unk_04 = 0;
    note->playbackState.adsr.action.s.state = ADSR_STATE_INITIAL;
    note->sampleState = gDefaultSampleState;
}

void AudioNote_Disable(Note* note) {
    if (note->sampleState.bitField0.needsInit == true) {
        note->sampleState.bitField0.needsInit = false;
    }
    note->playbackState.priority = 0;
    note->sampleState.bitField0.enabled = false;
    note->playbackState.unk_04 = 0;
    note->sampleState.bitField0.finished = false;
    note->playbackState.parentLayer = NO_LAYER;
    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.adsr.action.s.state = ADSR_STATE_DISABLED;
    note->playbackState.adsr.current = 0;
}

void AudioNote_Update(void) {
    s32 pad[2];
    NoteAttributes* attrs;
    NoteSampleState* sampleState;
    NoteSampleState* noteSampleState;
    Note* note;
    NotePlaybackState* playbackState;
    NoteSubAttributes subAttrs;
    u8 bookOffset;
    f32 scale;
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        note = &gAudioCtx.notes[i];
        sampleState = &gAudioCtx.sampleStateList[gAudioCtx.sampleStateBaseIndex + i];
        playbackState = &note->playbackState;
        if (playbackState->parentLayer != NO_LAYER) {
            if ((u32)playbackState->parentLayer < 0x7FFFFFFF) {
                continue;
            }

            if (note != playbackState->parentLayer->note && playbackState->unk_04 == 0) {
                playbackState->adsr.action.s.release = true;
                playbackState->adsr.fadeOutVel = gAudioCtx.audioBufParams.updatesPerFrameInv;
                playbackState->priority = 1;
                playbackState->unk_04 = 2;
                goto out;
            } else if (!playbackState->parentLayer->enabled && playbackState->unk_04 == 0 &&
                       playbackState->priority >= 1) {
                // do nothing
            } else if (playbackState->parentLayer->channel->seqPlayer == NULL) {
                AudioScript_SequenceChannelDisable(playbackState->parentLayer->channel);
                playbackState->priority = 1;
                playbackState->unk_04 = 1;
                continue;
            } else if (playbackState->parentLayer->channel->seqPlayer->muted &&
                       (playbackState->parentLayer->channel->muteFlags & MUTE_FLAGS_STOP_NOTES)) {
                // do nothing
            } else {
                goto out;
            }

            AudioPlayback_SeqLayerNoteRelease(playbackState->parentLayer);
            AudioList_Remove(&note->listItem);
            AudioList_PushFront(&note->listItem.pool->decaying, &note->listItem);
            playbackState->priority = 1;
            playbackState->unk_04 = 2;
        } else if (playbackState->unk_04 == 0 && playbackState->priority >= 1) {
            continue;
        }

    out:
        if (playbackState->priority != 0) {
            if (1) {}
            noteSampleState = &note->sampleState;
            if (playbackState->unk_04 >= 1 || noteSampleState->bitField0.finished) {
                if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED || noteSampleState->bitField0.finished) {
                    if (playbackState->wantedParentLayer != NO_LAYER) {
                        AudioNote_Disable(note);
                        if (playbackState->wantedParentLayer->channel != NULL) {
                            AudioPlayback_NoteInitForLayer(note, playbackState->wantedParentLayer);
                            AudioEffects_InitVibrato(note);
                            AudioEffects_InitPortamento(note);
                            AudioList_Remove(&note->listItem);
                            AudioList_PushBack(&note->listItem.pool->active, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            // don't skip
                        } else {
                            AudioNote_Disable(note);
                            AudioList_Remove(&note->listItem);
                            AudioList_PushBack(&note->listItem.pool->disabled, &note->listItem);
                            playbackState->wantedParentLayer = NO_LAYER;
                            goto skip;
                        }
                    } else {
                        if (playbackState->parentLayer != NO_LAYER) {
                            playbackState->parentLayer->bit1 = true;
                        }
                        AudioNote_Disable(note);
                        AudioList_Remove(&note->listItem);
                        AudioList_PushBack(&note->listItem.pool->disabled, &note->listItem);
                        continue;
                    }
                }
            } else if (playbackState->adsr.action.s.state == ADSR_STATE_DISABLED) {
                if (playbackState->parentLayer != NO_LAYER) {
                    playbackState->parentLayer->bit1 = true;
                }
                AudioNote_Disable(note);
                AudioList_Remove(&note->listItem);
                AudioList_PushBack(&note->listItem.pool->disabled, &note->listItem);
                continue;
            }

            scale = AudioEffects_UpdateAdsr(&playbackState->adsr);
            AudioEffects_UpdateVibrato(note);
            attrs = &playbackState->attributes;
            if (playbackState->unk_04 == 1 || playbackState->unk_04 == 2) {
                subAttrs.frequency = attrs->freqScale;
                subAttrs.velocity = attrs->velocity;
                subAttrs.pan = attrs->pan;
                subAttrs.targetReverbVol = attrs->targetReverbVol;
                subAttrs.stereo = attrs->stereo;
                subAttrs.gain = attrs->gain;
                subAttrs.filter = attrs->filter;
                subAttrs.combFilterSize = attrs->combFilterSize;
                subAttrs.combFilterGain = attrs->combFilterGain;
                bookOffset = noteSampleState->bitField1.bookOffset;
            } else {
                SequenceLayer* layer = playbackState->parentLayer;
                SequenceChannel* channel = layer->channel;

                subAttrs.frequency = layer->noteFreqScale;
                subAttrs.velocity = layer->noteVelocity;
                subAttrs.pan = layer->notePan;
                if (layer->stereo.asByte == 0) {
                    subAttrs.stereo = channel->stereo;
                } else {
                    subAttrs.stereo = layer->stereo;
                }
                subAttrs.targetReverbVol = channel->targetReverbVol;
                subAttrs.gain = channel->gain;
                subAttrs.filter = channel->filter;
                subAttrs.combFilterSize = channel->combFilterSize;
                subAttrs.combFilterGain = channel->combFilterGain;
                bookOffset = channel->bookOffset & 0x7;

                if (channel->seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_3)) {
                    subAttrs.frequency = 0.0f;
                    subAttrs.velocity = 0.0f;
                }
            }

            subAttrs.frequency *= playbackState->vibratoFreqScale * playbackState->portamentoFreqScale;
            subAttrs.frequency *= gAudioCtx.audioBufParams.resampleRate;
            subAttrs.velocity *= scale;
            AudioNote_InitSampleState(note, sampleState, &subAttrs);
            noteSampleState->bitField1.bookOffset = bookOffset;
        skip:;
        }
    }
}

TunedSample* AudioPlayback_GetInstrumentTunedSample(Instrument* instrument, s32 semitone) {
    TunedSample* tunedSample;

    if (semitone < instrument->normalRangeLo) {
        tunedSample = &instrument->lowPitchTunedSample;
    } else if (semitone <= instrument->normalRangeHi) {
        tunedSample = &instrument->normalPitchTunedSample;
    } else {
        tunedSample = &instrument->highPitchTunedSample;
    }
    return tunedSample;
}

Instrument* AudioPlayback_GetInstrumentInner(s32 fontId, s32 instId) {
    Instrument* inst;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (instId >= gAudioCtx.soundFontList[fontId].numInstruments) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + instId) + 0x3000000;
        return NULL;
    }

    inst = gAudioCtx.soundFontList[fontId].instruments[instId];
    if (inst == NULL) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + instId) + 0x1000000;
        return inst;
    }

    return inst;
}

Drum* AudioPlayback_GetDrum(s32 fontId, s32 drumId) {
    Drum* drum;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (drumId >= gAudioCtx.soundFontList[fontId].numDrums) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + drumId) + 0x4000000;
        return NULL;
    }
    if ((u32)gAudioCtx.soundFontList[fontId].drums < AUDIO_RELOCATED_ADDRESS_START) {
        return NULL;
    }
    drum = gAudioCtx.soundFontList[fontId].drums[drumId];

    if (drum == NULL) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + drumId) + 0x5000000;
    }

    return drum;
}

SoundEffect* AudioPlayback_GetSoundEffect(s32 fontId, s32 sfxId) {
    SoundEffect* soundEffect;

    if (fontId == 0xFF) {
        return NULL;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        gAudioCtx.audioErrorFlags = fontId + 0x10000000;
        return NULL;
    }

    if (sfxId >= gAudioCtx.soundFontList[fontId].numSfx) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + sfxId) + 0x4000000;
        return NULL;
    }

    if ((u32)gAudioCtx.soundFontList[fontId].soundEffects < AUDIO_RELOCATED_ADDRESS_START) {
        return NULL;
    }

    soundEffect = &gAudioCtx.soundFontList[fontId].soundEffects[sfxId];

    if (soundEffect == NULL) {
        gAudioCtx.audioErrorFlags = ((fontId << 8) + sfxId) + 0x5000000;
    }

    if (soundEffect->tunedSample.sample == NULL) {
        return NULL;
    }

    return soundEffect;
}

s32 AudioPlayback_SetFontInstrument(s32 instrumentType, s32 fontId, s32 index, void* value) {
    if (fontId == 0xFF) {
        return -1;
    }

    if (!AudioLoad_IsFontLoadComplete(fontId)) {
        return -2;
    }

    switch (instrumentType) {
        case 0:
            if (index >= gAudioCtx.soundFontList[fontId].numDrums) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].drums[index] = value;
            break;

        case 1:
            if (index >= gAudioCtx.soundFontList[fontId].numSfx) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].soundEffects[index] = *(SoundEffect*)value;
            break;

        default:
            if (index >= gAudioCtx.soundFontList[fontId].numInstruments) {
                return -3;
            }
            gAudioCtx.soundFontList[fontId].instruments[index] = value;
            break;
    }

    return 0;
}

void AudioPlayback_SeqLayerDecayRelease(SequenceLayer* layer, s32 target) {
    Note* note;
    NoteAttributes* attrs;
    SequenceChannel* chan;
    s32 i;

    if (layer == NO_LAYER) {
        return;
    }

    layer->bit3 = false;

    if (layer->note == NULL) {
        return;
    }

    note = layer->note;
    attrs = &note->playbackState.attributes;

    if (note->playbackState.wantedParentLayer == layer) {
        note->playbackState.wantedParentLayer = NO_LAYER;
    }

    if (note->playbackState.parentLayer != layer) {
        if (note->playbackState.parentLayer == NO_LAYER && note->playbackState.wantedParentLayer == NO_LAYER &&
            note->playbackState.prevParentLayer == layer && target != ADSR_STATE_DECAY) {
            note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufParams.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
        }
        return;
    }

    if (note->playbackState.adsr.action.s.state != ADSR_STATE_DECAY) {
        attrs->freqScale = layer->noteFreqScale;
        attrs->velocity = layer->noteVelocity;
        attrs->pan = layer->notePan;

        if (layer->channel != NULL) {
            chan = layer->channel;
            attrs->targetReverbVol = chan->targetReverbVol;
            attrs->gain = chan->gain;
            attrs->filter = chan->filter;

            if (attrs->filter != NULL) {
                for (i = 0; i < 8; i++) {
                    attrs->filterBuf[i] = attrs->filter[i];
                }
                attrs->filter = attrs->filterBuf;
            }

            attrs->combFilterGain = chan->combFilterGain;
            attrs->combFilterSize = chan->combFilterSize;

            if (chan->seqPlayer->muted && (chan->muteFlags & MUTE_FLAGS_3)) {
                note->sampleState.bitField0.finished = true;
            }

            if (layer->stereo.asByte == 0) {
                attrs->stereo = chan->stereo;
            } else {
                attrs->stereo = layer->stereo;
            }
            note->playbackState.priority = chan->someOtherPriority;
        } else {
            attrs->stereo = layer->stereo;
            note->playbackState.priority = 1;
        }

        note->playbackState.prevParentLayer = note->playbackState.parentLayer;
        note->playbackState.parentLayer = NO_LAYER;
        if (target == ADSR_STATE_RELEASE) {
            note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufParams.updatesPerFrameInv;
            note->playbackState.adsr.action.s.release = true;
            note->playbackState.unk_04 = 2;
        } else {
            note->playbackState.unk_04 = 1;
            note->playbackState.adsr.action.s.decay = true;
            if (layer->adsr.decayIndex == 0) {
                note->playbackState.adsr.fadeOutVel = gAudioCtx.adsrDecayTable[layer->channel->adsr.decayIndex];
            } else {
                note->playbackState.adsr.fadeOutVel = gAudioCtx.adsrDecayTable[layer->adsr.decayIndex];
            }
            note->playbackState.adsr.sustain =
                ((f32)(s32)(layer->channel->adsr.sustain) * note->playbackState.adsr.current) / 256.0f;
        }
    }

    if (target == ADSR_STATE_DECAY) {
        AudioList_Remove(&note->listItem);
        AudioList_PushFront(&note->listItem.pool->decaying, &note->listItem);
    }
}

void AudioPlayback_SeqLayerNoteDecay(SequenceLayer* layer) {
    AudioPlayback_SeqLayerDecayRelease(layer, ADSR_STATE_DECAY);
}

void AudioPlayback_SeqLayerNoteRelease(SequenceLayer* layer) {
    AudioPlayback_SeqLayerDecayRelease(layer, ADSR_STATE_RELEASE);
}

/**
 * Extract the synthetic wave to use from gWaveSamples and update corresponding frequencies
 *
 * @param note
 * @param layer
 * @param waveId the index of the type of synthetic wave to use, offset by 128
 * @return harmonicIndex, the index of the harmonic for the synthetic wave contained in gWaveSamples
 */
s32 AudioPlayback_BuildSyntheticWave(Note* note, SequenceLayer* layer, s32 waveId) {
    f32 freqScale;
    f32 freqRatio;
    u8 harmonicIndex;

    if (waveId < 128) {
        waveId = 128;
    }

    freqScale = layer->freqScale;
    if (layer->portamento.mode != 0 && 0.0f < layer->portamento.extent) {
        freqScale *= (layer->portamento.extent + 1.0f);
    }

    // Map frequency to the harmonic to use from gWaveSamples
    if (freqScale < 0.99999f) {
        harmonicIndex = 0;
        freqRatio = 1.0465f;
    } else if (freqScale < 1.99999f) {
        harmonicIndex = 1;
        freqRatio = 1.0465f / 2;
    } else if (freqScale < 3.99999f) {
        harmonicIndex = 2;
        freqRatio = 1.0465f / 4 + 1.005E-3;
    } else {
        harmonicIndex = 3;
        freqRatio = 1.0465f / 8 - 2.5E-6;
    }

    // Update results
    layer->freqScale *= freqRatio;
    note->playbackState.waveId = waveId;
    note->playbackState.harmonicIndex = harmonicIndex;

    // Save the pointer to the synthethic wave
    // waveId index starts at 128, there are WAVE_SAMPLE_COUNT samples to read from
    note->sampleState.waveSampleAddr = &gWaveSamples[waveId - 128][harmonicIndex * WAVE_SAMPLE_COUNT];

    return harmonicIndex;
}

void AudioPlayback_InitSyntheticWave(Note* note, SequenceLayer* layer) {
    s32 prevHarmonicIndex;
    s32 curHarmonicIndex;
    s32 waveId = layer->instOrWave;

    if (waveId == 0xFF) {
        waveId = layer->channel->instOrWave;
    }

    prevHarmonicIndex = note->playbackState.harmonicIndex;
    curHarmonicIndex = AudioPlayback_BuildSyntheticWave(note, layer, waveId);

    if (curHarmonicIndex != prevHarmonicIndex) {
        note->sampleState.harmonicIndexCurAndPrev = (curHarmonicIndex << 2) + prevHarmonicIndex;
    }
}

void AudioList_InitNoteList(AudioListItem* list) {
    list->prev = list;
    list->next = list;
    list->u.count = 0;
}

void AudioList_InitNoteLists(NotePool* pool) {
    AudioList_InitNoteList(&pool->disabled);
    AudioList_InitNoteList(&pool->decaying);
    AudioList_InitNoteList(&pool->releasing);
    AudioList_InitNoteList(&pool->active);
    pool->disabled.pool = pool;
    pool->decaying.pool = pool;
    pool->releasing.pool = pool;
    pool->active.pool = pool;
}

void AudioList_InitNoteFreeList(void) {
    s32 i;

    AudioList_InitNoteLists(&gAudioCtx.noteFreeLists);
    for (i = 0; i < gAudioCtx.numNotes; i++) {
        gAudioCtx.notes[i].listItem.u.value = &gAudioCtx.notes[i];
        gAudioCtx.notes[i].listItem.prev = NULL;
        AudioList_PushBack(&gAudioCtx.noteFreeLists.disabled, &gAudioCtx.notes[i].listItem);
    }
}

void AudioList_NotePoolClear(NotePool* pool) {
    s32 i;
    AudioListItem* source;
    AudioListItem* cur;
    AudioListItem* dest;

    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                source = &pool->disabled;
                dest = &gAudioCtx.noteFreeLists.disabled;
                break;

            case 1:
                source = &pool->decaying;
                dest = &gAudioCtx.noteFreeLists.decaying;
                break;

            case 2:
                source = &pool->releasing;
                dest = &gAudioCtx.noteFreeLists.releasing;
                break;

            case 3:
                source = &pool->active;
                dest = &gAudioCtx.noteFreeLists.active;
                break;
        }

        while (true) {
            cur = source->next;
            if (cur == source || cur == NULL) {
                break;
            }
            AudioList_Remove(cur);
            AudioList_PushBack(dest, cur);
        }
    }
}

void AudioList_NotePoolFill(NotePool* pool, s32 count) {
    s32 i;
    s32 j;
    Note* note;
    AudioListItem* source;
    AudioListItem* dest;

    AudioList_NotePoolClear(pool);

    for (i = 0, j = 0; j < count; i++) {
        if (i == 4) {
            return;
        }

        switch (i) {
            case 0:
                source = &gAudioCtx.noteFreeLists.disabled;
                dest = &pool->disabled;
                break;

            case 1:
                source = &gAudioCtx.noteFreeLists.decaying;
                dest = &pool->decaying;
                break;

            case 2:
                source = &gAudioCtx.noteFreeLists.releasing;
                dest = &pool->releasing;
                break;

            case 3:
                source = &gAudioCtx.noteFreeLists.active;
                dest = &pool->active;
                break;
        }

        while (j < count) {
            note = AudioList_PopBack(source);
            if (note == NULL) {
                break;
            }
            AudioList_PushBack(dest, &note->listItem);
            j++;
        }
    }
}

/**
 * Add 'item' to the front of the list given by 'list', if it's not in any list
 */
void AudioList_PushFront(AudioListItem* list, AudioListItem* item) {
    if (item->prev == NULL) {
        item->prev = list;
        item->next = list->next;
        list->next->prev = item;
        list->next = item;
        list->u.count++;
        item->pool = list->pool;
    }
}

/**
 * Remove 'item' from the list it's in, if any
 */
void AudioList_Remove(AudioListItem* item) {
    if (item->prev != NULL) {
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = NULL;
    }
}

Note* AudioList_FindNodeWithPrioLessThan(AudioListItem* list, s32 limit) {
    AudioListItem* cur = list->next;
    AudioListItem* best;

    if (cur == list) {
        return NULL;
    }

    for (best = cur; cur != list; cur = cur->next) {
        if (((Note*)best->u.value)->playbackState.priority >= ((Note*)cur->u.value)->playbackState.priority) {
            best = cur;
        }
    }

    if (best == NULL) {
        return NULL;
    }

    if (limit <= ((Note*)best->u.value)->playbackState.priority) {
        return NULL;
    }

    return best->u.value;
}

void AudioPlayback_NoteInitForLayer(Note* note, SequenceLayer* layer) {
    s32 pad[3];
    s16 instId;
    NotePlaybackState* playbackState = &note->playbackState;
    NoteSampleState* noteSampleState = &note->sampleState;

    note->playbackState.prevParentLayer = NO_LAYER;
    note->playbackState.parentLayer = layer;
    playbackState->priority = layer->channel->notePriority;
    layer->notePropertiesNeedInit = true;
    layer->bit3 = true;
    layer->note = note;
    layer->channel->noteUnused = note;
    layer->channel->layerUnused = layer;
    layer->noteVelocity = 0.0f;
    AudioNote_Init(note);
    instId = layer->instOrWave;

    if (instId == 0xFF) {
        instId = layer->channel->instOrWave;
    }
    noteSampleState->tunedSample = layer->tunedSample;

    if (instId >= 0x80 && instId < 0xC0) {
        noteSampleState->bitField1.isSyntheticWave = true;
    } else {
        noteSampleState->bitField1.isSyntheticWave = false;
    }

    if (noteSampleState->bitField1.isSyntheticWave) {
        AudioPlayback_BuildSyntheticWave(note, layer, instId);
    }

    playbackState->fontId = layer->channel->fontId;
    playbackState->stereoHeadsetEffects = layer->channel->stereoHeadsetEffects;
    noteSampleState->bitField1.reverbIndex = layer->channel->reverbIndex & 3;
}

// similar to AudioNote_ReleaseAndTakeOwnership, hard to say what the difference is
void func_800E82C0(Note* note, SequenceLayer* layer) {
    AudioPlayback_SeqLayerNoteRelease(note->playbackState.parentLayer);
    note->playbackState.wantedParentLayer = layer;
}

void AudioNote_ReleaseAndTakeOwnership(Note* note, SequenceLayer* layer) {
    note->playbackState.wantedParentLayer = layer;
    note->playbackState.priority = layer->channel->notePriority;

    note->playbackState.adsr.fadeOutVel = gAudioCtx.audioBufParams.updatesPerFrameInv;
    note->playbackState.adsr.action.s.release = true;
}

Note* AudioNote_AllocFromDisabled(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioList_PopBack(&pool->disabled);
    if (note != NULL) {
        AudioPlayback_NoteInitForLayer(note, layer);
        AudioList_PushFront(&pool->active, &note->listItem);
    }
    return note;
}

Note* AudioNote_AllocFromDecaying(NotePool* pool, SequenceLayer* layer) {
    Note* note = AudioList_PopBack(&pool->decaying);
    if (note != NULL) {
        AudioNote_ReleaseAndTakeOwnership(note, layer);
        AudioList_PushBack(&pool->releasing, &note->listItem);
    }
    return note;
}

Note* AudioNote_AllocFromActive(NotePool* pool, SequenceLayer* layer) {
    Note* rNote;
    Note* aNote;
    s32 rPriority;
    s32 aPriority;

    rPriority = aPriority = 0x10;
    rNote = AudioList_FindNodeWithPrioLessThan(&pool->releasing, layer->channel->notePriority);

    if (rNote != NULL) {
        rPriority = rNote->playbackState.priority;
    }

    aNote = AudioList_FindNodeWithPrioLessThan(&pool->active, layer->channel->notePriority);

    if (aNote != NULL) {
        aPriority = aNote->playbackState.priority;
    }

    if (rNote == NULL && aNote == NULL) {
        return NULL;
    }

    if (aPriority < rPriority) {
        AudioList_Remove(&aNote->listItem);
        func_800E82C0(aNote, layer);
        AudioList_PushBack(&pool->releasing, &aNote->listItem);
        aNote->playbackState.priority = layer->channel->notePriority;
        return aNote;
    }
    rNote->playbackState.wantedParentLayer = layer;
    rNote->playbackState.priority = layer->channel->notePriority;
    return rNote;
}

Note* AudioNote_Alloc(SequenceLayer* layer) {
    Note* note;
    u32 policy = layer->channel->noteAllocPolicy;

    if (policy & 1) {
        note = layer->note;
        if (note != NULL && note->playbackState.prevParentLayer == layer &&
            note->playbackState.wantedParentLayer == NO_LAYER) {
            AudioNote_ReleaseAndTakeOwnership(note, layer);
            AudioList_Remove(&note->listItem);
            AudioList_PushBack(&note->listItem.pool->releasing, &note->listItem);
            return note;
        }
    }

    if (policy & 2) {
        if (!(note = AudioNote_AllocFromDisabled(&layer->channel->notePool, layer)) &&
            !(note = AudioNote_AllocFromDecaying(&layer->channel->notePool, layer)) &&
            !(note = AudioNote_AllocFromActive(&layer->channel->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 4) {
        if (!(note = AudioNote_AllocFromDisabled(&layer->channel->notePool, layer)) &&
            !(note = AudioNote_AllocFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = AudioNote_AllocFromDecaying(&layer->channel->notePool, layer)) &&
            !(note = AudioNote_AllocFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
            !(note = AudioNote_AllocFromActive(&layer->channel->notePool, layer)) &&
            !(note = AudioNote_AllocFromActive(&layer->channel->seqPlayer->notePool, layer))) {
            goto null_return;
        }
        return note;
    }

    if (policy & 8) {
        if (!(note = AudioNote_AllocFromDisabled(&gAudioCtx.noteFreeLists, layer)) &&
            !(note = AudioNote_AllocFromDecaying(&gAudioCtx.noteFreeLists, layer)) &&
            !(note = AudioNote_AllocFromActive(&gAudioCtx.noteFreeLists, layer))) {
            goto null_return;
        }
        return note;
    }

    if (!(note = AudioNote_AllocFromDisabled(&layer->channel->notePool, layer)) &&
        !(note = AudioNote_AllocFromDisabled(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioNote_AllocFromDisabled(&gAudioCtx.noteFreeLists, layer)) &&
        !(note = AudioNote_AllocFromDecaying(&layer->channel->notePool, layer)) &&
        !(note = AudioNote_AllocFromDecaying(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioNote_AllocFromDecaying(&gAudioCtx.noteFreeLists, layer)) &&
        !(note = AudioNote_AllocFromActive(&layer->channel->notePool, layer)) &&
        !(note = AudioNote_AllocFromActive(&layer->channel->seqPlayer->notePool, layer)) &&
        !(note = AudioNote_AllocFromActive(&gAudioCtx.noteFreeLists, layer))) {
        goto null_return;
    }
    return note;

null_return:
    layer->bit3 = true;
    return NULL;
}

void AudioNote_InitAll(void) {
    Note* note;
    s32 i;

    for (i = 0; i < gAudioCtx.numNotes; i++) {
        note = &gAudioCtx.notes[i];
        note->sampleState = gZeroedSampleState;
        note->playbackState.priority = 0;
        note->playbackState.unk_04 = 0;
        note->playbackState.parentLayer = NO_LAYER;
        note->playbackState.wantedParentLayer = NO_LAYER;
        note->playbackState.prevParentLayer = NO_LAYER;
        note->playbackState.waveId = 0;
        note->playbackState.attributes.velocity = 0.0f;
        note->playbackState.adsrVolScaleUnused = 0;
        note->playbackState.adsr.action.asByte = 0;
        note->playbackState.vibratoState.active = 0;
        note->playbackState.portamento.cur = 0;
        note->playbackState.portamento.speed = 0;
        note->playbackState.stereoHeadsetEffects = false;
        note->playbackState.startSamplePos = 0;
        note->synthesisState.synthesisBuffers =
            AudioHeap_AllocDmaMemory(&gAudioCtx.miscPool, sizeof(NoteSynthesisBuffers));
    }
}
