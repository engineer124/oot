/**
 * @file audio_effects.c
 *
 * The first half of this file processes sound on the seqPlayer, channel, and layer level
 * once the .seq file is finished for this update.
 *
 * The second half of this file implements three types of audio effects over long periods of times:
 * - Vibrato: regular, pulsating change of pitch
 * - Portamento: pitch sliding from one note to another
 * - Multi-Point ADSR Envelope: volume changing over time
 */
#include "ultra64.h"
#include "global.h"

void AudioScript_SequenceChannelProcessSound(SequenceChannel* channel, s32 recalculateVolume, s32 applyBend) {
    f32 channelVolume;
    f32 chanFreqScale;
    s32 i;

    if (channel->changes.s.volume || recalculateVolume) {
        channelVolume = channel->volume * channel->volumeScale * channel->seqPlayer->appliedFadeVolume;
        if (channel->seqPlayer->muted && (channel->muteFlags & MUTE_FLAGS_SOFTEN)) {
            channelVolume = channel->seqPlayer->muteVolumeScale * channelVolume;
        }
        channel->appliedVolume = SQ(channelVolume);
    }

    if (channel->changes.s.pan) {
        channel->pan = channel->newPan * channel->panChannelWeight;
    }

    chanFreqScale = channel->freqScale;
    if (applyBend) {
        chanFreqScale *= channel->seqPlayer->bend;
        channel->changes.s.freqScale = true;
    }

    for (i = 0; i < ARRAY_COUNT(channel->layers); i++) {
        SequenceLayer* layer = channel->layers[i];

        if ((layer != NULL) && layer->enabled && (layer->note != NULL)) {
            if (layer->notePropertiesNeedInit) {
                layer->noteFreqScale = layer->freqScale * chanFreqScale;
                layer->noteVelocity = layer->velocitySquare2 * channel->appliedVolume;
                layer->notePan = (channel->pan + layer->pan * (0x80 - channel->panChannelWeight)) >> 7;
                layer->notePropertiesNeedInit = false;
            } else {
                if (channel->changes.s.freqScale) {
                    layer->noteFreqScale = layer->freqScale * chanFreqScale;
                }
                if (channel->changes.s.volume || recalculateVolume) {
                    layer->noteVelocity = layer->velocitySquare2 * channel->appliedVolume;
                }
                if (channel->changes.s.pan) {
                    layer->notePan = (channel->pan + layer->pan * (0x80 - channel->panChannelWeight)) >> 7;
                }
            }
        }
    }
    channel->changes.asByte = 0;
}

void AudioScript_SequencePlayerProcessSound(SequencePlayer* seqPlayer) {
    s32 i;

    if (seqPlayer->fadeTimer != 0) {
        seqPlayer->fadeVolume += seqPlayer->fadeVelocity;
        seqPlayer->recalculateVolume = true;

        if (seqPlayer->fadeVolume > 1.0f) {
            seqPlayer->fadeVolume = 1.0f;
        }
        if (seqPlayer->fadeVolume < 0.0f) {
            seqPlayer->fadeVolume = 0.0f;
        }

        seqPlayer->fadeTimer--;
        if ((seqPlayer->fadeTimer == 0) && (seqPlayer->state == SEQPLAYER_STATE_FADE_OUT)) {
            AudioScript_SequencePlayerDisable(seqPlayer);
            return;
        }
    }

    if (seqPlayer->recalculateVolume) {
        seqPlayer->appliedFadeVolume = seqPlayer->fadeVolume * seqPlayer->fadeVolumeScale;
    }

    for (i = 0; i < SEQ_NUM_CHANNELS; i++) {
        if (seqPlayer->channels[i]->enabled == true) {
            AudioScript_SequenceChannelProcessSound(seqPlayer->channels[i], seqPlayer->recalculateVolume,
                                                    seqPlayer->applyBend);
        }
    }

    seqPlayer->recalculateVolume = false;
}

/**
 * @return freqScale
 */
f32 AudioEffects_UpdatePortamento(Portamento* portamento) {
    u32 bendIndex;
    f32 portamentoFreq;

    portamento->cur += portamento->speed;
    bendIndex = (portamento->cur >> 8) & 0xFF;

    if (bendIndex >= 127) {
        bendIndex = 127;
        portamento->mode = 0;
    }

    portamentoFreq = 1.0f + portamento->extent * (gBendPitchOneOctaveFrequencies[bendIndex + 128] - 1.0f);

    return portamentoFreq;
}

/**
 * time: 0x400 is 1 unit of time, 0x10000 is 1 period
 */
s16 AudioEffects_GetVibratoPitchChange(VibratoState* vib) {
    s32 index;

    vib->time += (s32)vib->rate;
    index = (vib->time >> 10) % WAVE_SAMPLE_COUNT;
    return vib->sineWave[index];
}

/**
 * @return freqScale
 */
f32 AudioEffects_UpdateVibrato(VibratoState* vib) {
    static f32 sActiveVibratoFreqScaleSum = 0.0f;
    static s32 sActiveVibratoCount = 0;
    f32 pitchChange;
    f32 extent;
    f32 invExtent;
    f32 result;
    f32 scaledExtent;
    SequenceChannel* channel = vib->channel;

    if (vib->delay != 0) {
        vib->delay--;
        return 1.0f;
    }

    //! @bug this probably meant to compare with gAudioCtx.sequenceChannelNone.
    //! -1 isn't used as a channel pointer anywhere else.
    if (channel != ((SequenceChannel*)(-1))) {
        if (vib->extentChangeTimer) {
            if (vib->extentChangeTimer == 1) {
                vib->extent = (s32)channel->vibratoDepthTarget;
            } else {
                vib->extent += ((s32)channel->vibratoDepthTarget - vib->extent) / (s32)vib->extentChangeTimer;
            }

            vib->extentChangeTimer--;
        } else if (channel->vibratoDepthTarget != (s32)vib->extent) {
            if ((vib->extentChangeTimer = channel->vibratoDepthChangeDelay) == 0) {
                vib->extent = (s32)channel->vibratoDepthTarget;
            }
        }

        if (vib->rateChangeTimer) {
            if (vib->rateChangeTimer == 1) {
                vib->rate = (s32)channel->vibratoRateTarget;
            } else {
                vib->rate += ((s32)channel->vibratoRateTarget - vib->rate) / (s32)vib->rateChangeTimer;
            }

            vib->rateChangeTimer--;
        } else if (channel->vibratoRateTarget != (s32)vib->rate) {
            if ((vib->rateChangeTimer = channel->vibratoRateChangeDelay) == 0) {
                vib->rate = (s32)channel->vibratoRateTarget;
            }
        }
    }

    if (vib->extent == 0.0f) {
        return 1.0f;
    }

    pitchChange = (f32)AudioEffects_GetVibratoPitchChange(vib) + 0x8000;
    scaledExtent = vib->extent / 4096.0f;
    extent = scaledExtent + 1.0f;
    invExtent = 1.0f / extent;

    // Inverse linear interpolation
    result = 1.0f / ((extent - invExtent) * pitchChange / 0x10000 + invExtent);

    sActiveVibratoFreqScaleSum += result;
    sActiveVibratoCount++;

    return result;
}

void AudioEffects_UpdatePortamentoAndVibrato(Note* note) {
    // Update Portamento
    if (note->playbackState.portamento.mode != 0) {
        note->playbackState.portamentoFreqScale = AudioEffects_UpdatePortamento(&note->playbackState.portamento);
    }
    // Update Vibrato
    if (note->playbackState.vibratoState.active) {
        note->playbackState.vibratoFreqScale = AudioEffects_UpdateVibrato(&note->playbackState.vibratoState);
    }
}

void AudioEffects_InitVibrato(Note* note) {
    VibratoState* vib = &note->playbackState.vibratoState;
    SequenceChannel* channel;

    note->playbackState.vibratoFreqScale = 1.0f;

    vib->active = true;
    vib->time = 0;
    vib->sineWave = gWaveSamples[2]; // gSineWaveSample
    vib->channel = note->playbackState.parentLayer->channel;

    channel = vib->channel;

    if ((vib->extentChangeTimer = channel->vibratoDepthChangeDelay) == 0) {
        vib->extent = (s32)channel->vibratoDepthTarget;
    } else {
        vib->extent = (s32)channel->vibratoDepthStart;
    }

    if ((vib->rateChangeTimer = channel->vibratoRateChangeDelay) == 0) {
        vib->rate = (s32)channel->vibratoRateTarget;
    } else {
        vib->rate = (s32)channel->vibratoRateStart;
    }
    vib->delay = channel->vibratoDelay;
}

void AudioEffects_InitPortamento(Note* note) {
    note->playbackState.portamentoFreqScale = 1.0f;
    note->playbackState.portamento = note->playbackState.parentLayer->portamento;
}

void AudioEffects_InitAdsr(AdsrState* adsr, EnvelopePoint* envelope, s16* volOut) {
    adsr->action.asByte = 0;
    adsr->delay = 0;
    adsr->envelope = envelope;
    adsr->sustain = 0.0f;
    adsr->current = 0.0f;
    // (An older versions of the audio engine used in Super Mario 64 did
    // adsr->volOut = volOut. That line and associated struct member were
    // removed, but the function parameter was forgotten and remains.)
}

/**
 * @return volumeScale
 */
f32 AudioEffects_UpdateAdsr(AdsrState* adsr) {
    u8 state = adsr->action.s.state;

    switch (state) {
        case ADSR_STATE_DISABLED:
            return 0.0f;

        case ADSR_STATE_INITIAL:
            if (adsr->action.s.hang) {
                adsr->action.s.state = ADSR_STATE_HANG;
                break;
            }
            FALLTHROUGH;
        case ADSR_STATE_START_LOOP:
            adsr->envelopeIndex = 0;
            adsr->action.s.state = ADSR_STATE_LOOP;
        retry:;
            FALLTHROUGH;
        case ADSR_STATE_LOOP:
            adsr->delay = adsr->envelope[adsr->envelopeIndex].delay;
            switch (adsr->delay) {
                case ADSR_DISABLE:
                    adsr->action.s.state = ADSR_STATE_DISABLED;
                    break;

                case ADSR_HANG:
                    adsr->action.s.state = ADSR_STATE_HANG;
                    break;

                case ADSR_GOTO:
                    adsr->envelopeIndex = adsr->envelope[adsr->envelopeIndex].arg;
                    goto retry;

                case ADSR_RESTART:
                    adsr->action.s.state = ADSR_STATE_INITIAL;
                    break;

                default:
                    adsr->delay *= gAudioCtx.audioBufParams.updatesPerFrameScaled;
                    if (adsr->delay == 0) {
                        adsr->delay = 1;
                    }
                    adsr->target = adsr->envelope[adsr->envelopeIndex].arg / 32767.0f;
                    adsr->target = adsr->target * adsr->target;
                    adsr->velocity = (adsr->target - adsr->current) / adsr->delay;
                    adsr->action.s.state = ADSR_STATE_FADE;
                    adsr->envelopeIndex++;
                    break;
            }
            if (adsr->action.s.state != ADSR_STATE_FADE) {
                break;
            }
            FALLTHROUGH;
        case ADSR_STATE_FADE:
            adsr->current += adsr->velocity;
            adsr->delay--;
            if (adsr->delay <= 0) {
                adsr->action.s.state = ADSR_STATE_LOOP;
            }
            FALLTHROUGH;
        case ADSR_STATE_HANG:
            break;

        case ADSR_STATE_DECAY:
        case ADSR_STATE_RELEASE:
            adsr->current -= adsr->fadeOutVel;
            if (adsr->sustain != 0.0f && state == ADSR_STATE_DECAY) {
                if (adsr->current < adsr->sustain) {
                    adsr->current = adsr->sustain;
                    adsr->delay = 128;
                    adsr->action.s.state = ADSR_STATE_SUSTAIN;
                }
                break;
            }

            if (adsr->current < 0.00001f) {
                adsr->current = 0.0f;
                adsr->action.s.state = ADSR_STATE_DISABLED;
            }
            break;

        case ADSR_STATE_SUSTAIN:
            adsr->delay--;
            if (adsr->delay == 0) {
                adsr->action.s.state = ADSR_STATE_RELEASE;
            }
            break;
    }

    if (adsr->action.s.decay) {
        adsr->action.s.state = ADSR_STATE_DECAY;
        adsr->action.s.decay = false;
    }

    if (adsr->action.s.release) {
        adsr->action.s.state = ADSR_STATE_RELEASE;
        adsr->action.s.release = false;
    }

    if (adsr->current < 0.0f) {
        return 0.0f;
    }

    if (adsr->current > 1.0f) {
        return 1.0f;
    }

    return adsr->current;
}
