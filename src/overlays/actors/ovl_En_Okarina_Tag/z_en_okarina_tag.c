/*
 * File: z_en_okarina_tag.c
 * Overlay: ovl_En_Okarina_Tag
 * Description: Music Staff (Ocarina) spot, teach suns song spot
 */

#include "z_en_okarina_tag.h"
#include "assets/scenes/misc/hakaana_ouke/hakaana_ouke_scene.h"
#include "assets/scenes/overworld/spot02/spot02_scene.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_OCARINA_NO_FREEZE)

void EnOkarinaTag_Init(Actor* thisx, PlayState* play);
void EnOkarinaTag_Destroy(Actor* thisx, PlayState* play);
void EnOkarinaTag_Update(Actor* thisx, PlayState* play);

void EnOkarinaTag_SwitchFlag_WaitForOcarina(EnOkarinaTag* this, PlayState* play);
void EnOkarinaTag_SwitchFlag_ListenToOcarina(EnOkarinaTag* this, PlayState* play);

void EnOkarinaTag_Custom_WaitForOcarina(EnOkarinaTag* this, PlayState* play);
void EnOkarinaTag_Custom_ListenToOcarina(EnOkarinaTag* this, PlayState* play);

void EnOkarinaTag_SunsSongEngraving_Wait(EnOkarinaTag* this, PlayState* play);
void EnOkarinaTag_SunsSongEngraving_Interact(EnOkarinaTag* this, PlayState* play);

ActorInit En_Okarina_Tag_InitVars = {
    ACTOR_EN_OKARINA_TAG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnOkarinaTag),
    (ActorFunc)EnOkarinaTag_Init,
    (ActorFunc)EnOkarinaTag_Destroy,
    (ActorFunc)EnOkarinaTag_Update,
    NULL,
};

extern CutsceneData gWindmillOpenWellCs[];
extern CutsceneData gOpenDoorOfTimeCs[];

void EnOkarinaTag_Destroy(Actor* thisx, PlayState* play) {
}

void EnOkarinaTag_Init(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    osSyncPrintf("\n\n");
    // "Ocarina tag outbreak"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナタグ発生 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_0;

    this->type = OCARINASPOT_GET_TYPE(&this->actor);
    this->ocarinaSong = OCARINASPOT_GET_SONG(&this->actor);
    this->switchFlag = OCARINASPOT_GET_SWITCHFLAGS(&this->actor);

    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }

    if (this->ocarinaSong == OCARINASPOT_SONG_ANY) {
        this->ocarinaSong = OCARINASPOT_SONG_SARIA;
        this->anySongAllowed = true;
    }

    this->actor.targetMode = 1;
    if (this->actor.world.rot.z > 0) {
        this->xzRange = this->actor.world.rot.z * 40.0f;
    }

    // "Save information"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ セーブ情報\t ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
    // "Type index"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 種類インデックス ☆☆☆☆☆ %d\n" VT_RST, this->type);
    // "Correct answer information"
    osSyncPrintf(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 正解情報\t ☆☆☆☆☆ %d\n" VT_RST, this->ocarinaSong);
    // "Range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 範囲情報\t ☆☆☆☆☆ %d\n" VT_RST, this->actor.world.rot.z);
    // "Processing range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 処理範囲情報\t ☆☆☆☆☆ %f\n" VT_RST, this->xzRange);
    // "Hit?"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 当り？\t\t ☆☆☆☆☆ %d\n" VT_RST, this->anySongAllowed);
    osSyncPrintf("\n\n");

    if ((this->switchFlag >= 0) && (Flags_GetSwitch(play, this->switchFlag))) {
        Actor_Kill(&this->actor);
        return;
    }

    switch (this->type) {
        case OCARINASPOT_TYPE_SWITCHFLAG:
            this->actionFunc = EnOkarinaTag_SwitchFlag_WaitForOcarina;
            break;

        case OCARINASPOT_TYPE_CHECK_WINDMILL:
            if (LINK_IS_ADULT) {
                Actor_Kill(&this->actor);
                break;
            }
            FALLTHROUGH;
        case OCARINASPOT_TYPE_CHECK_ZORA_WATERFALL:
        case OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME:
        case OCARINASPOT_TYPE_CHECK_ROYAL_TOMB:
            this->actionFunc = EnOkarinaTag_Custom_WaitForOcarina;
            break;

        case OCARINASPOT_TYPE_LEARN_SUNS:
            this->actor.textId = 0x5021;
            this->actionFunc = EnOkarinaTag_SunsSongEngraving_Wait;
            break;

        default:
            Actor_Kill(&this->actor);
            break;
    }
}

void EnOkarinaTag_SwitchFlag_WaitForOcarina(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 ocarinaSong;

    this->debugDisplayTimer++;

    if ((this->switchFlag >= 0) && Flags_GetSwitch(play, this->switchFlag)) {
        // Switch is already set
        this->actor.flags &= ~ACTOR_FLAG_0;
        return;
    }

    if ((this->ocarinaSong == OCARINASPOT_SONG_SCARECROW) && !gSaveContext.scarecrowSpawnSongSet) {
        // Scarecrow is not set
        return;
    }

    if (player->stateFlags2 & PLAYER_STATE2_OCARINA_START_READY) {
        // "North! ! ! ! !"
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ 北！！！！！ ☆☆☆☆☆ %f\n" VT_RST, this->actor.xzDistToPlayer);
    }

    if ((this->actor.xzDistToPlayer < (90.0f + this->xzRange)) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
        if (player->stateFlags2 & PLAYER_STATE2_OCARINA_ON_WITH_ACTOR) {
            ocarinaSong = this->ocarinaSong;
            if (ocarinaSong == OCARINASPOT_SONG_SCARECROW) {
                ocarinaSong = OCARINA_ACTION_SCARECROW_SPAWN_RECORDING - OCARINA_ACTION_CHECK_SARIA;
            }
            player->stateFlags2 |= PLAYER_STATE2_OCARINA_START_OVERRIDE;
            Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_SARIA + ocarinaSong);
            this->actionFunc = EnOkarinaTag_SwitchFlag_ListenToOcarina;
        } else if ((this->actor.xzDistToPlayer < (50.0f + this->xzRange) &&
                    ((fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)))) {
            this->debugDisplayTimer = 0;
            player->ocarinaActor = &this->actor;
        }
    }
}

void EnOkarinaTag_SwitchFlag_ListenToOcarina(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
        this->actionFunc = EnOkarinaTag_SwitchFlag_WaitForOcarina;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        if (this->switchFlag >= 0) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        if (play->sceneId == SCENE_MIZUSIN) {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
        }
        if ((play->sceneId != SCENE_DAIYOUSEI_IZUMI) && (play->sceneId != SCENE_YOUSEI_IZUMI_YOKO)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        }
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        this->actionFunc = EnOkarinaTag_SwitchFlag_WaitForOcarina;
        return;
    }

    if (this->anySongAllowed) {
        if ((play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SARIA) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_EPONA) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_ZL) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SUNS) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SOT) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_STORMS) ||
            (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_UNSET)) {
            if (this->switchFlag >= 0) {
                Flags_SetSwitch(play, this->switchFlag);
            }
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = EnOkarinaTag_SwitchFlag_WaitForOcarina;
            return;
        }
    }

    if ((play->msgCtx.ocarinaMode >= OCARINA_MODE_PLAYED_SARIA) &&
        (play->msgCtx.ocarinaMode <= OCARINA_MODE_PLAYED_UNSET)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        this->actionFunc = EnOkarinaTag_SwitchFlag_WaitForOcarina;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) {
        player->stateFlags2 |= PLAYER_STATE2_OCARINA_START_OVERRIDE;
    }
}

void EnOkarinaTag_Custom_WaitForOcarina(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->debugDisplayTimer++;

    if ((this->ocarinaSong == OCARINASPOT_SONG_SCARECROW) && !gSaveContext.scarecrowSpawnSongSet) {
        return;
    }

    if ((this->switchFlag >= 0) && Flags_GetSwitch(play, this->switchFlag)) {
        this->actor.flags &= ~ACTOR_FLAG_0;
        return;
    }

    if ((this->type == OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME) && GET_EVENTCHKINF(EVENTCHKINF_DOOR_OF_TIME_OPENED)) {
        return;
    }

    if ((this->type == OCARINASPOT_TYPE_CHECK_ROYAL_TOMB) && GET_EVENTCHKINF(EVENTCHKINF_ROYAL_TOMB_OPENED)) {
        return;
    }

    if ((this->actor.xzDistToPlayer < (90.0f + this->xzRange)) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
        if (player->stateFlags2 & PLAYER_STATE2_OCARINA_START_READY) {
            switch (this->type) {
                case OCARINASPOT_TYPE_CHECK_ZORA_WATERFALL:
                    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_LULLABY);
                    break;

                case OCARINASPOT_TYPE_CHECK_WINDMILL:
                    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_STORMS);
                    break;

                case OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME:
                    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_TIME);
                    break;

                case OCARINASPOT_TYPE_CHECK_ROYAL_TOMB:
                    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_LULLABY);
                    break;

                default:
                    // "Ocarina Invisible-kun demo start check error source"
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナ透明君デモ開始チェックエラー原 ☆☆☆☆☆ %d\n" VT_RST,
                                 this->type);
                    Actor_Kill(&this->actor);
                    break;
            }
            player->stateFlags2 |= PLAYER_STATE2_OCARINA_START_OVERRIDE;
            this->actionFunc = EnOkarinaTag_Custom_ListenToOcarina;
            return;
        }

        if ((this->actor.xzDistToPlayer < (50.0f + this->xzRange)) &&
            (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)) {
            this->debugDisplayTimer = 0;
            player->stateFlags2 |= PLAYER_STATE2_OCARINA_START_OVERRIDE;
        }
    }
}

void EnOkarinaTag_Custom_ListenToOcarina(EnOkarinaTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
        this->actionFunc = EnOkarinaTag_Custom_WaitForOcarina;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        if (this->switchFlag >= 0) {
            Flags_SetSwitch(play, this->switchFlag);
        }

        switch (this->type) {
            case OCARINASPOT_TYPE_CHECK_ZORA_WATERFALL:
                Flags_SetSwitch(play, this->switchFlag);
                SET_EVENTCHKINF(EVENTCHKINF_ZORA_WATERFALL_OPENED);
                break;

            case OCARINASPOT_TYPE_CHECK_WINDMILL:
                play->csCtx.segment = gWindmillOpenWellCs;
                gSaveContext.cutsceneTrigger = 1;
                // Increase pitch by 3 semitones i.e. 2^(3/12), scale tempo by same ratio
                // Applies to the windmill bgm once the song of storms fanfare is complete
                Audio_SetMainBgmTempoFreqAfterFanfare(1.18921f, 90);
                break;

            case OCARINASPOT_TYPE_CHECK_DOOR_OF_TIME:
                play->csCtx.segment = gOpenDoorOfTimeCs;
                gSaveContext.cutsceneTrigger = 1;
                break;

            case OCARINASPOT_TYPE_CHECK_ROYAL_TOMB:
                play->csCtx.segment = LINK_IS_ADULT ? SEGMENTED_TO_VIRTUAL(gGraveyardOpenRoyalTombAdultCs)
                                                    : SEGMENTED_TO_VIRTUAL(gGraveyardOpenRoyalTombChildCs);
                gSaveContext.cutsceneTrigger = 1;
                SET_EVENTCHKINF(EVENTCHKINF_ROYAL_TOMB_OPENED);
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                break;

            default:
                break;
        }
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        this->actionFunc = EnOkarinaTag_Custom_WaitForOcarina;
        return;
    }

    if ((play->msgCtx.ocarinaMode >= OCARINA_MODE_PLAYED_SARIA) &&
        (play->msgCtx.ocarinaMode <= OCARINA_MODE_PLAYED_UNSET)) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        this->actionFunc = EnOkarinaTag_Custom_WaitForOcarina;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) {
        player->stateFlags2 |= PLAYER_STATE2_OCARINA_START_OVERRIDE;
    }
}

void EnOkarinaTag_SunsSongEngraving_Wait(EnOkarinaTag* this, PlayState* play) {
    s16 yawDiff;
    s16 absYawDiff;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnOkarinaTag_SunsSongEngraving_Interact;
        return;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    this->debugDisplayTimer++;

    if (this->actor.xzDistToPlayer > 120.0f) {
        return;
    }

    if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
        this->actor.textId = 0x5021;
    }

    absYawDiff = ABS(yawDiff);
    if (absYawDiff < 0x4300) {
        this->debugDisplayTimer = 0;
        func_8002F2CC(&this->actor, play, 70.0f);
    }
}

void EnOkarinaTag_SunsSongEngraving_Interact(EnOkarinaTag* this, PlayState* play) {
    // "Open sesame sesame!"
    osSyncPrintf(VT_FGCOL(MAGENTA) "☆☆☆☆☆ 開けゴマゴマゴマ！ ☆☆☆☆☆ %d\n" VT_RST, Message_GetState(&play->msgCtx));

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (!CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gSunSongGraveSunSongTeachCs);
            gSaveContext.cutsceneTrigger = 1;
        }
        this->actionFunc = EnOkarinaTag_SunsSongEngraving_Wait;
    }
}

void EnOkarinaTag_Update(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    this->actionFunc(this, play);

    if (R_DBG_DRAW_ON) {
        if (this->debugDisplayTimer != 0) {
            if (!(this->debugDisplayTimer & 1)) {
                DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 120, 255, 4, play->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                   this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
        }
    }
}
