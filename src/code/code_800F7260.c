#include "ultra64.h"
#include "global.h"
#include "vt.h"

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x04 */ Vec3f* pos;
    /* 0x08 */ u8 token;
    /* 0x0C */ f32* freqScale;
    /* 0x10 */ f32* vol;
    /* 0x14 */ s8* reverbAdd;
} SfxRequest; // size = 0x18

typedef struct {
    /* 0x00 */ f32 value;
    /* 0x04 */ f32 target;
    /* 0x08 */ f32 step;
    /* 0x0C */ u16 remainingFrames;
} UnusedBankLerp; // size = 0x10

SfxBankEntry D_8016BAD0[9];
SfxBankEntry D_8016BC80[12];
SfxBankEntry D_8016BEC0[22];
SfxBankEntry D_8016C2E0[20];
SfxBankEntry D_8016C6A0[8];
SfxBankEntry D_8016C820[3];
SfxBankEntry D_8016C8B0[5];
SfxRequest sSfxRequests[0x100];
u8 sSfxBankListEnd[7];
u8 sSfxBankFreeListStart[7];
u8 sSfxBankUnused[7];
ActiveSfx gActiveSfx[7][3];
u8 sCurSfxPlayerChannelIdx;
u8 gSfxBankMuted[7];
UnusedBankLerp sUnusedBankLerp[7];
u16 gAudioSfxSwapSource[10];
u16 gAudioSfxSwapTarget[10];
u8 gAudioSfxSwapMode[10];

void Audio_SetSfxBanksMute(u16 muteMask) {
    u8 bankId;

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        if (muteMask & 1) {
            gSfxBankMuted[bankId] = true;
        } else {
            gSfxBankMuted[bankId] = false;
        }
        muteMask = muteMask >> 1;
    }
}

/**
 * Lowers volume of seqPlayers 0 & 3 so that sfx can be more pronounced,
 * Each sfx channel stores its own bit to lower the volume.
 * Only a single channel needs to request this to lower the volume
 */
void Audio_LowerBgmVolumeWithFlag(u8 channelIdx) {
    gSfxChannelLowerVolumeFlag |= (1 << channelIdx);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 2, 0x40, 0xF);
    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 2, 0x40, 0xF);
}

/**
 * Clear the flag for the specific channel to lower the volulme.
 * If all flags are cleared, then players 0 & 3 are restored to full volume
 */
void Audio_RestoreBgmVolumeWithFlag(u8 channelIdx) {
    gSfxChannelLowerVolumeFlag &= ((1 << channelIdx) ^ 0xFFFF);
    if (gSfxChannelLowerVolumeFlag == 0) {
        Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 2, 0x7F, 0xF);
        Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 2, 0x7F, 0xF);
    }
}

void Audio_PlaySfxGeneral(u16 sfxId, Vec3f* pos, u8 token, f32* freqScale, f32* vol, s8* reverbAdd) {
    u8 i;
    SfxRequest* req;

    if (!gSfxBankMuted[SFX_BANK_SHIFT(sfxId)]) {
        req = &sSfxRequests[gSfxRequestWriteIndex];
        if (!gAudioSfxSwapOff) {
            for (i = 0; i < 10; i++) {
                if (sfxId == gAudioSfxSwapSource[i]) {
                    if (gAudioSfxSwapMode[i] == 0) { // "SWAP"
                        sfxId = gAudioSfxSwapTarget[i];
                    } else { // "ADD"
                        req->sfxId = gAudioSfxSwapTarget[i];
                        req->pos = pos;
                        req->token = token;
                        req->freqScale = freqScale;
                        req->vol = vol;
                        req->reverbAdd = reverbAdd;
                        gSfxRequestWriteIndex++;
                        req = &sSfxRequests[gSfxRequestWriteIndex];
                    }
                    i = 10; // "break;"
                }
            }
        }
        req->sfxId = sfxId;
        req->pos = pos;
        req->token = token;
        req->freqScale = freqScale;
        req->vol = vol;
        req->reverbAdd = reverbAdd;
        gSfxRequestWriteIndex++;
    }
}

void Audio_RemoveMatchingSfxRequests(u8 aspect, SfxBankEntry* cmp) {
    SfxRequest* req;
    s32 remove;
    u8 i = gSfxRequestReadIndex;

    for (; i != gSfxRequestWriteIndex; i++) {
        remove = false;
        req = &sSfxRequests[i];
        switch (aspect) {
            case 0:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId)) {
                    remove = true;
                }
                break;
            case 1:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(cmp->sfxId) && (&req->pos->x == cmp->posX)) {
                    remove = true;
                }
                break;
            case 2:
                if (&req->pos->x == cmp->posX) {
                    remove = true;
                }
                break;
            case 3:
                if (&req->pos->x == cmp->posX && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 4:
                if (req->token == cmp->token && req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
            case 5:
                if (req->sfxId == cmp->sfxId) {
                    remove = true;
                }
                break;
        }
        if (remove) {
            req->sfxId = 0;
        }
    }
}

void Audio_ProcessSfxRequest(void) {
    u16 sfxId;
    u8 channelCount;
    u8 index;
    SfxRequest* req;
    SfxBankEntry* entry;
    SfxParams* sfxParams;
    s32 bankId;
    u8 evictImportance;
    u8 evictIndex;

    req = &sSfxRequests[gSfxRequestReadIndex];
    evictIndex = 0x80;
    if (req->sfxId == 0) {
        return;
    }

    // Get bankId
    bankId = SFX_BANK(req->sfxId);

    // Debug print
    if ((1 << bankId) & sAudioDebugPrintSfxId) {
        AudioDebug_ScrPrt("SE", req->sfxId);
        bankId = SFX_BANK(req->sfxId);
    }

    channelCount = 0;
    index = gSfxBanks[bankId][0].next;

    // Compare the sfx request to all sfx bank entries
    while (index != 0xFF && index != 0) {

        // If an existing sfx from the same source exists in the bank, then we should either:
        // renew if it is the same sfx, or interrupt that sfx and replace it with the new sfx request,
        // or drop the new sfx request.
        if (&req->pos->x == gSfxBanks[bankId][index].posX) {

            // If the new sfx has equal importance to the existing sfx,
            // drop the request if the existing sfx has the "SFX_FLAG_BLOCK_EQUAL_IMPORTANCE" flag.
            // Otherwise, keep processing the new sfx request
            if ((gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].params &
                 SFX_FLAG_BLOCK_EQUAL_IMPORTANCE) &&
                gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance ==
                    gSfxBanks[bankId][index].sfxImportance) {
                // Drop the new request
                return;
            }

            if (gSfxBanks[bankId][index].sfxId == req->sfxId) {
                // The new sfx is the same as the existing sfx
                // Set channelCount to max value
                channelCount = gUsedChannelsPerBank[gSfxChannelLayout][bankId];
            } else {

                if (channelCount == 0) {
                    // If this is the first sfx at the same source, than store the existing sfx info into evict temps
                    evictIndex = index;
                    sfxId = gSfxBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSfxParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                } else if (gSfxBanks[bankId][index].sfxImportance < evictImportance) {
                    // If a different existing sfx is at the same source, than update the evict temps if the next
                    // existing src has lower importance than the previous existing source
                    evictIndex = index;
                    sfxId = gSfxBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSfxParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                }

                channelCount++;

                // If channelCount is at max value
                if (channelCount == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                    if (gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance >= evictImportance) {
                        index = evictIndex;
                    } else {
                        index = 0;
                    }
                }
            }

            // If channelCount is at max value
            if (channelCount == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                sfxParams = &gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];

                // If the existing sfx is "all-frames" or "SFX_FLAG_2" or "(index == evictIndex)"
                // interrupt existing sfx and play the new sfx instead.
                if ((req->sfxId & 0xC00) || (sfxParams->params & SFX_FLAG_2) || (index == evictIndex)) {

                    // Restore bgm if the sfx about to be replaced has the right flag
                    if ((gSfxBanks[bankId][index].sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) &&
                        gSfxBanks[bankId][index].state != SFX_STATE_QUEUED) {
                        Audio_RestoreBgmVolumeWithFlag(gSfxBanks[bankId][index].channelIdx);
                    }

                    gSfxBanks[bankId][index].token = req->token;
                    gSfxBanks[bankId][index].sfxId = req->sfxId;
                    gSfxBanks[bankId][index].state = SFX_STATE_QUEUED;
                    gSfxBanks[bankId][index].freshness = 2;
                    gSfxBanks[bankId][index].freqScale = req->freqScale;
                    gSfxBanks[bankId][index].vol = req->vol;
                    gSfxBanks[bankId][index].reverbAdd = req->reverbAdd;
                    gSfxBanks[bankId][index].sfxParams = sfxParams->params;
                    gSfxBanks[bankId][index].sfxImportance = sfxParams->importance;
                } else if (gSfxBanks[bankId][index].state == SFX_STATE_PLAYING_ONE_FRAME) {
                    // The new sfx is "one-frame" and equals the existing sfx,
                    // continue playing the "one-frame" sfx by setting "SFX_STATE_PLAYING".
                    // This will avoid triggering deletion and get "SFX_STATE_REFRESH"
                    // The state will be set back to "SFX_STATE_PLAYING_ONE_FRAME" at the end of the cycle
                    gSfxBanks[bankId][index].state = SFX_STATE_PLAYING;
                }
                // Terminate loop, function is finished (nothing more is processed)
                index = 0;
            }
        }

        if (index != 0) {
            index = gSfxBanks[bankId][index].next;
        }
    }

    if (gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].next != 0xFF && index != 0) {
        // Allocate from free list
        index = sSfxBankFreeListStart[bankId];

        entry = &gSfxBanks[bankId][index];
        entry->posX = &req->pos->x;
        entry->posY = &req->pos->y;
        entry->posZ = &req->pos->z;
        entry->token = req->token;
        entry->freqScale = req->freqScale;
        entry->vol = req->vol;
        entry->reverbAdd = req->reverbAdd;

        sfxParams = &gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];

        entry->sfxParams = sfxParams->params;
        entry->sfxImportance = sfxParams->importance;
        entry->sfxId = req->sfxId;
        entry->state = SFX_STATE_QUEUED;
        entry->freshness = 2;
        entry->prev = sSfxBankListEnd[bankId];

        // Append to end of used list and pop from front of free list
        gSfxBanks[bankId][sSfxBankListEnd[bankId]].next = sSfxBankFreeListStart[bankId];
        sSfxBankListEnd[bankId] = sSfxBankFreeListStart[bankId];
        sSfxBankFreeListStart[bankId] = gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].next;
        gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].prev = 0xFF;
        entry->next = 0xFF;
    }
}

void Audio_RemoveSfxBankEntry(u8 bankId, u8 entryIndex) {
    SfxBankEntry* entry = &gSfxBanks[bankId][entryIndex];
    u8 i;

    if (entry->sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) {
        Audio_RestoreBgmVolumeWithFlag(entry->channelIdx);
    }
    if (entryIndex == sSfxBankListEnd[bankId]) {
        sSfxBankListEnd[bankId] = entry->prev;
    } else {
        gSfxBanks[bankId][entry->next].prev = entry->prev;
    }
    gSfxBanks[bankId][entry->prev].next = entry->next;
    entry->next = sSfxBankFreeListStart[bankId];
    entry->prev = 0xFF;
    gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].prev = entryIndex;
    sSfxBankFreeListStart[bankId] = entryIndex;
    entry->state = SFX_STATE_EMPTY;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        if (gActiveSfx[bankId][i].entryIndex == entryIndex) {
            gActiveSfx[bankId][i].entryIndex = 0xFF;
            i = gChannelsPerBank[gSfxChannelLayout][bankId];
        }
    }
}

void Audio_ChooseActiveSfx(u8 bankId) {
    u8 numChosenSfx;
    u8 numChannels;
    u8 entryIndex;
    u8 i;
    u8 j;
    u8 k;
    u8 sfxImportance;
    u8 needNewSfx;
    u8 chosenEntryIndex;
    u16 temp3;
    f32 tempf1;
    SfxBankEntry* entry;
    ActiveSfx chosenSfx[MAX_CHANNELS_PER_BANK];
    ActiveSfx* activeSfx;
    s32 pad;

    numChosenSfx = 0;
    for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
        chosenSfx[i].priority = 0x7FFFFFFF;
        chosenSfx[i].entryIndex = 0xFF;
    }
    entryIndex = gSfxBanks[bankId][0].next;
    k = 0;

    // Delete stale sfx and prioritize remaining sfx into the gActiveSfx arrays
    while (entryIndex != 0xFF) {

        // Update the freshness for an "all-frame" sfx if it is still queued
        if ((gSfxBanks[bankId][entryIndex].state == SFX_STATE_QUEUED) &&
            (gSfxBanks[bankId][entryIndex].sfxId & 0xC00)) {
            gSfxBanks[bankId][entryIndex].freshness--;
        }

        // If a "one-frame" sfx is still in "SFX_STATE_PLAYING_ONE_FRAME", then remove the sfx
        else if (!(gSfxBanks[bankId][entryIndex].sfxId & 0xC00) &&
                 (gSfxBanks[bankId][entryIndex].state == SFX_STATE_PLAYING_ONE_FRAME)) {
            // CHAN_UPD_SCRIPT_IO (ioPort 0, force stop sfx in seq 0)
            Audio_QueueCmdS8((gSfxBanks[bankId][entryIndex].channelIdx << 8) | 0x6020000, 0);
            Audio_RemoveSfxBankEntry(bankId, entryIndex);
        }

        // If a "all-frame" sfx goes 2 frames in the "queued" state
        // (because it is too low priority), then remove the sfx
        if (gSfxBanks[bankId][entryIndex].freshness == 0) {
            Audio_RemoveSfxBankEntry(bankId, entryIndex);
        } else if (gSfxBanks[bankId][entryIndex].state != SFX_STATE_EMPTY) {
            // process the entry
            entry = &gSfxBanks[bankId][entryIndex];

            // Recompute distSq each frame since the sound's position may have changed
            // (later converted into dist)
            if (&gSfxDefaultPos.x == entry[0].posX) {
                entry->dist = 0.0f;
            } else {
                tempf1 = *entry->posY * 1;
                entry->dist = (SQ(*entry->posX) + SQ(tempf1) + SQ(*entry->posZ)) * 1;
            }

            // Recompute priority, possibly based on sfx position relative to the camera.
            // (Note that the priority is the opposite of importance; lower is more preserved)
            sfxImportance = entry->sfxImportance;
            if (entry->sfxParams & SFX_FLAG_PRIORITY_NO_DIST) {
                // Priority is independent of distance
                entry->priority = SQ(0xFF - sfxImportance) * SQ(76);
            } else {
                // Priority value increases with distance (more likely to eject)
                if (entry->dist > 0x7FFFFFD0) {
                    entry->dist = 0x70000008;
                    osSyncPrintf(VT_COL(RED, WHITE) "<INAGAKI CHECK> dist over! "
                                                    "flag:%04X ptr:%08X pos:%f-%f-%f" VT_RST "\n",
                                 entry->sfxId, entry->posX, entry->posZ, *entry->posX, *entry->posY, *entry->posZ);
                }
                temp3 = entry->sfxId; // fake
                entry->priority = (u32)entry->dist + (SQ(0xFF - sfxImportance) * SQ(76)) + temp3 - temp3;
                if (*entry->posZ < 0.0f) {
                    entry->priority += (s32)(-*entry->posZ * 6.0f);
                }
            }

            // Sfx is too far away, do not consider for chosen sfx
            if (entry->dist > SQ(1e5f)) {

                // If too far away and also playing, stop playing
                if (entry->state == SFX_STATE_PLAYING) {
                    // CHAN_UPD_SCRIPT_IO (ioPort 0, force stop sfx in seq 0)
                    Audio_QueueCmdS8((entry->channelIdx << 8) | 0x6020000, 0);
                    if (entry->sfxId & 0xC00) {
                        Audio_RemoveSfxBankEntry(bankId, entryIndex);
                        entryIndex = k;
                    }
                }
            } else {
                // Loop through all channels allocated to a specific bank
                // Choose which sfx from gSfxBanks to play
                // This includes all sfx requests and sfxs already playing
                // Sort all current sfx entries in gSfxBanks by priority
                // This is where the "active" sfxs are chosen
                numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];
                for (i = 0; i < numChannels; i++) {
                    // Sort all sfx entries by priority
                    if (entry->priority <= chosenSfx[i].priority) {
                        // Update the number of sfx to attampt to play
                        if (numChosenSfx < gChannelsPerBank[gSfxChannelLayout][bankId]) {
                            numChosenSfx++;
                        }
                        for (j = numChannels - 1; j > i; j--) {
                            chosenSfx[j].priority = chosenSfx[j - 1].priority;
                            chosenSfx[j].entryIndex = chosenSfx[j - 1].entryIndex;
                        }
                        chosenSfx[i].priority = entry->priority;
                        chosenSfx[i].entryIndex = entryIndex;
                        i = numChannels; // "break;"
                    }
                }
            }
            k = entryIndex;
        }
        entryIndex = gSfxBanks[bankId][k].next;
    }

    // If a sfx is chosen, update its state
    for (i = 0; i < numChosenSfx; i++) {
        entry = &gSfxBanks[bankId][chosenSfx[i].entryIndex];
        if (entry->state == SFX_STATE_QUEUED) {
            // sfx is a new entry
            entry->state = SFX_STATE_READY;
        } else if (entry->state == SFX_STATE_PLAYING) {
            // sfx is already playing
            entry->state = SFX_STATE_PLAYING_REFRESH;
        }
    }

    // Apply the chosenSfxs to the activeSfxs in each channel
    numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];
    for (i = 0; i < numChannels; i++) {
        needNewSfx = false;
        activeSfx = &gActiveSfx[bankId][i];

        // Check if a sfx is already in the channel
        if (activeSfx->entryIndex == 0xFF) {
            // No sfx playing in the channel
            // Available for use
            needNewSfx = true;
        } else {
            // The channel is already playing a sfx
            // Get the entry of the sfx playing
            entry = &gSfxBanks[bankId][activeSfx[0].entryIndex];

            // Check the state of the sfx playing
            if (entry->state == SFX_STATE_PLAYING) {
                //
                if (entry->sfxId & 0xC00) {
                    // For "all-frames" sfx, remove the entry
                    Audio_RemoveSfxBankEntry(bankId, activeSfx->entryIndex);
                } else {
                    // For "one-frame" sfx, reset the state to queued
                    entry->state = SFX_STATE_QUEUED;
                }
                needNewSfx = true;
            } else if (entry->state == SFX_STATE_EMPTY) {
                // The sfx already in the channel is empty
                // Can replace the sfx
                activeSfx->entryIndex = 0xFF;
                needNewSfx = true;
            } else {
                // Sfx is already playing as it should, nothing to do.
                for (j = 0; j < numChannels; j++) {
                    if (activeSfx->entryIndex == chosenSfx[j].entryIndex) {
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels;
                    }
                }
                numChosenSfx--;
            }
        }

        // enter the new sfx into the activeSfx
        if (needNewSfx == true) {
            for (j = 0; j < numChannels; j++) {
                chosenEntryIndex = chosenSfx[j].entryIndex;
                if ((chosenEntryIndex != 0xFF) &&
                    (gSfxBanks[bankId][chosenEntryIndex].state != SFX_STATE_PLAYING_REFRESH)) {
                    for (k = 0; k < numChannels; k++) {
                        if (chosenEntryIndex == gActiveSfx[bankId][k].entryIndex) {
                            needNewSfx = false;
                            k = numChannels; // "break;"
                        }
                    }
                    if (needNewSfx == true) {
                        activeSfx->entryIndex = chosenEntryIndex;
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels + 1;
                        numChosenSfx--;
                    }
                }
            }
            if (j == numChannels) {
                // nothing found
                activeSfx->entryIndex = 0xFF;
            }
        }
    }
}

void Audio_PlayActiveSfx(u8 bankId) {
    u8 entryIndex;
    SequenceChannel* channel;
    SfxBankEntry* entry;
    u8 i;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        entryIndex = gActiveSfx[bankId][i].entryIndex;
        // If entry is not empty
        if (entryIndex != 0xFF) {
            entry = &gSfxBanks[bankId][entryIndex];
            channel = gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[sCurSfxPlayerChannelIdx];

            if (entry->state == SFX_STATE_READY) {
                // Initialize a sfx (new sfx request)
                entry->channelIdx = sCurSfxPlayerChannelIdx;

                if (entry->sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) {
                    Audio_LowerBgmVolumeWithFlag(sCurSfxPlayerChannelIdx);
                }

                // Add noise that will offset the frequency of the sfx
                if ((entry->sfxParams & SFX_PARAM_67_MASK) != (0 << SFX_PARAM_67_SHIFT)) {
                    switch (entry->sfxParams & SFX_PARAM_67_MASK) {
                        case (1 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Audio_NextRandom() & 0xF;
                            break;
                        case (2 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Audio_NextRandom() & 0x1F;
                            break;
                        case (3 << SFX_PARAM_67_SHIFT):
                            entry->unk_2F = Audio_NextRandom() & 0x3F;
                            break;
                        default:
                            entry->unk_2F = 0;
                            break;
                    }
                }

                // Calculate all the properties of sfx
                Audio_SetSfxProperties(bankId, entryIndex, sCurSfxPlayerChannelIdx);

                // CHAN_UPD_SCRIPT_IO (ioPort 0, enable the sfx to play in seq 0)
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8), 1);

                // CHAN_UPD_SCRIPT_IO (ioPort 4, write the lower bits sfx index to seq 0 so it can find the right code
                // to execute)
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8) | 4,
                                 entry->sfxId & 0xFF);

                if (gSfxBankHasMoreThan255Entries[bankId]) {
                    // CHAN_UPD_SCRIPT_IO (ioPort 5, write the upper bits sfx index to seq 0, for banks with > 0xFF
                    // entries)
                    Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((sCurSfxPlayerChannelIdx & 0xFF) << 8) | 5,
                                     (entry->sfxId & 0x100) >> 8);
                }

                // Update playing state
                if (entry->sfxId & 0xC00) {
                    // "all-frames" sfx
                    entry->state = SFX_STATE_PLAYING;
                } else {
                    // "one-frame" sfx
                    entry->state = SFX_STATE_PLAYING_ONE_FRAME;
                }
            } else if ((u8)channel->soundScriptIO[1] == 0xFF) {
                // Signal from seq 0 that the sfx is finished playing. Remove entry
                Audio_RemoveSfxBankEntry(bankId, entryIndex);
            } else if (entry->state == SFX_STATE_PLAYING_REFRESH) {
                // Sfx is playing but a refresh is requested
                Audio_SetSfxProperties(bankId, entryIndex, sCurSfxPlayerChannelIdx);

                // Update playing state
                if (entry->sfxId & 0xC00) {
                    // "all-frames" sfx
                    entry->state = SFX_STATE_PLAYING;
                } else {
                    // "one-frame" sfx
                    entry->state = SFX_STATE_PLAYING_ONE_FRAME;
                }
            }
        }
        sCurSfxPlayerChannelIdx++;
    }
}

void Audio_StopSfxByBank(u8 bankId) {
    SfxBankEntry* entry;
    s32 pad;
    SfxBankEntry cmp;
    u8 entryIndex = gSfxBanks[bankId][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[bankId][entryIndex];
        if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
        }
        if (entry->state != SFX_STATE_EMPTY) {
            Audio_RemoveSfxBankEntry(bankId, entryIndex);
        }
        entryIndex = gSfxBanks[bankId][0].next;
    }
    cmp.sfxId = bankId << 12;
    Audio_RemoveMatchingSfxRequests(0, &cmp);
}

void func_800F8884(u8 bankId, Vec3f* pos) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[bankId][0].next;
    u8 prevEntryIndex = 0;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[bankId][entryIndex];
        if (entry->posX == &pos->x) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSfxBankEntry(bankId, entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = gSfxBanks[bankId][prevEntryIndex].next;
    }
}

void Audio_StopSfxByPosAndBank(u8 bankId, Vec3f* pos) {
    SfxBankEntry cmp;

    func_800F8884(bankId, pos);
    cmp.sfxId = bankId << 12;
    cmp.posX = &pos->x;
    Audio_RemoveMatchingSfxRequests(1, &cmp);
}

void Audio_StopSfxByPos(Vec3f* pos) {
    u8 i;
    SfxBankEntry cmp;

    for (i = 0; i < ARRAY_COUNT(gSfxBanks); i++) {
        func_800F8884(i, pos);
    }
    cmp.posX = &pos->x;
    Audio_RemoveMatchingSfxRequests(2, &cmp);
}

void Audio_StopSfxByPosAndId(Vec3f* pos, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->posX == &pos->x && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSfxBankEntry(SFX_BANK(sfxId), entryIndex);
            }
            entryIndex = 0xFF;
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.posX = &pos->x;
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSfxRequests(3, &cmp);
}

void Audio_StopSfxByTokenAndId(u8 token, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->token == token && entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSfxBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        if (entryIndex != 0xFF) {
            entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }
    cmp.token = token;
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSfxRequests(4, &cmp);
}

void Audio_StopSfxById(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry cmp;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | ((entry->channelIdx & 0xFF) << 8), 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                Audio_RemoveSfxBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }
        entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
    }
    cmp.sfxId = sfxId;
    Audio_RemoveMatchingSfxRequests(5, &cmp);
}

void Audio_ProcessSfxRequests(void) {
    while (gSfxRequestWriteIndex != gSfxRequestReadIndex) {
        Audio_ProcessSfxRequest();
        gSfxRequestReadIndex++;
    }
}

void Audio_SetUnusedBankLerp(u8 bankId, u8 target, u16 delay) {
    if (delay == 0) {
        delay++;
    }
    sUnusedBankLerp[bankId].target = target / 127.0f;
    sUnusedBankLerp[bankId].remainingFrames = delay;
    sUnusedBankLerp[bankId].step = ((sUnusedBankLerp[bankId].value - sUnusedBankLerp[bankId].target) / delay);
}

void Audio_StepUnusedBankLerp(u8 bankId) {
    if (sUnusedBankLerp[bankId].remainingFrames != 0) {
        sUnusedBankLerp[bankId].remainingFrames--;
        if (sUnusedBankLerp[bankId].remainingFrames != 0) {
            sUnusedBankLerp[bankId].value -= sUnusedBankLerp[bankId].step;
        } else {
            sUnusedBankLerp[bankId].value = sUnusedBankLerp[bankId].target;
        }
    }
}

void Audio_ProcessActiveSfx(void) {
    u8 bankId;

    if (IS_SEQUENCE_CHANNEL_VALID(gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[0])) {
        sCurSfxPlayerChannelIdx = 0;
        for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
            Audio_ChooseActiveSfx(bankId);
            Audio_PlayActiveSfx(bankId);
            Audio_StepUnusedBankLerp(bankId);
        }
    }
}

u8 Audio_IsSfxPlaying(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            return true;
        }
        entryIndex = entry->next;
    }
    return false;
}

void Audio_ResetSfx(void) {
    u8 bankId;
    u8 i;
    u8 entryIndex;

    gSfxRequestWriteIndex = 0;
    gSfxRequestReadIndex = 0;
    gSfxChannelLowerVolumeFlag = 0;
    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        sSfxBankListEnd[bankId] = 0;
        sSfxBankFreeListStart[bankId] = 1;
        sSfxBankUnused[bankId] = 0;
        gSfxBankMuted[bankId] = false;
        sUnusedBankLerp[bankId].value = 1.0f;
        sUnusedBankLerp[bankId].remainingFrames = 0;
    }
    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
            gActiveSfx[bankId][i].entryIndex = 0xFF;
        }
    }
    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        gSfxBanks[bankId][0].prev = 0xFF;
        gSfxBanks[bankId][0].next = 0xFF;
        for (i = 1; i < gSfxBankSizes[bankId] - 1; i++) {
            gSfxBanks[bankId][i].prev = i - 1;
            gSfxBanks[bankId][i].next = i + 1;
        }
        gSfxBanks[bankId][i].prev = i - 1;
        gSfxBanks[bankId][i].next = 0xFF;
    }
    if (D_801333F8 == 0) {
        for (bankId = 0; bankId < 10; bankId++) {
            gAudioSfxSwapSource[bankId] = 0;
            gAudioSfxSwapTarget[bankId] = 0;
            gAudioSfxSwapMode[bankId] = 0;
        }
        D_801333F8++;
    }
}
