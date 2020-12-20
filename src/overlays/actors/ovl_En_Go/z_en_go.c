#include "z_en_go.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#define FLAGS 0x00000039

#define THIS ((EnGo*)thisx)

void EnGo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnGo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnGo_Update(Actor* thisx, GlobalContext* globalCtx);
void EnGo_Draw(Actor* thisx, GlobalContext* globalCtx);

u16 EnGo_GetTextID(GlobalContext* globalCtx, EnGo* this);
s16 EnGo_SetFlagsGetStates(GlobalContext* globalCtx, EnGo* this);

s32 func_80A3ED24(GlobalContext* globalCtx, EnGo* this, struct_80034A14_arg1* arg2, f32 arg3,
                  u16 (*GetTextID)(GlobalContext*, Actor*), s16 (*unkFunc2)(GlobalContext*, Actor*));

void EnGo_SetupAction(EnGo* this, void* actionFunc);
void func_80A3EDE0(EnGo* this, s32 unk_val);

s32 EnGo_IsActorSpawned(EnGo* this, GlobalContext* globalCtx);
f32 EnGo_GetGoronSize(EnGo* this);

void func_80A3F060(EnGo* this, GlobalContext* globalCtx);
void func_80A3F0E4(EnGo* this);

s32 EnGo_FollowPath(EnGo* this, GlobalContext* globalCtx);

s32 EnGo_IsCameraModified(EnGo* this, GlobalContext* globalCtx);

void EnGo_SwapInitialFrameAnimFrameCount(EnGo* this);
void func_80A3F274(EnGo* this);

s32 EnGo_SetMovedPos(EnGo* this, GlobalContext* globalCtx);
s32 EnGo_SpawnDust(EnGo* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius, f32 unk_arg6);
s32 EnGo_IsRollingOnGround(EnGo* this, s16 unk_arg1, f32 unk_arg2);

void func_80A3F908(EnGo* this, GlobalContext* globalCtx);
void func_80A3FEB4(EnGo* this, GlobalContext* globalCtx);
void EnGo_StopRolling(EnGo* this, GlobalContext* globalCtx);
void func_80A4008C(EnGo* this, GlobalContext* globalCtx);
void EnGo_GoronLinkRolling(EnGo* this, GlobalContext* globalCtx);
void EnGo_FireGenericActionFunc(EnGo* this, GlobalContext* globalCtx);
void EnGo_WakeUp(EnGo* this, GlobalContext* globalCtx);
void EnGo_WakeUpAnimation(EnGo* this, GlobalContext* globalCtx);

void func_80A40494(EnGo* this, GlobalContext* globalCtx);
void func_80A405CC(EnGo* this, GlobalContext* globalCtx);
void EnGo_BiggoronActionFunc(EnGo* this, GlobalContext* globalCtx);
void func_80A408D8(EnGo* this, GlobalContext* globalCtx);

void func_80A40A54(EnGo* this, GlobalContext* globalCtx);
void func_80A40B1C(EnGo* this, GlobalContext* globalCtx);
void EnGo_GetItem(EnGo* this, GlobalContext* globalCtx);
void func_80A40C78(EnGo* this, GlobalContext* globalCtx);
void EnGo_Eyedrops(EnGo* this, GlobalContext* globalCtx);
void func_80A40DCC(EnGo* this, GlobalContext* globalCtx);

void func_80A40F58(EnGo* this, GlobalContext* globalCtx);
void func_80A41068(EnGo* this, GlobalContext* globalCtx);

// draw
s32 EnGo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnGo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

void EnGo_AddDust(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep);
void EnGo_UpdateDust(EnGo* this);
void EnGo_DrawDust(EnGo* this, GlobalContext* globalCtx);

extern AnimationHeader D_060029A8;
extern AnimationHeader D_06004930;
extern Gfx* D_0600BD80;
extern Gfx* D_0600C140;
extern Gfx* D_0600CE80;
extern Gfx* D_0600DE80;
extern Gfx* D_0600FD40;
extern Gfx* D_0600FD50;
extern FlexSkeletonHeader D_0600FEF0;
extern AnimationHeader D_06010590;

typedef struct {
    AnimationHeader* animationseg;
    f32 playbackSpeed;
    u8 mode;
    f32 transitionRate;
} EnGoSkelAnime;

const ActorInit En_Go_InitVars = {
    ACTOR_EN_GO,
    ACTORTYPE_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGo),
    (ActorFunc)EnGo_Init,
    (ActorFunc)EnGo_Destroy,
    (ActorFunc)EnGo_Update,
    (ActorFunc)EnGo_Draw,
};

// static ColliderCylinderInit sCylinderInit
ColliderCylinderInit D_80A41B00 = {
    { COLTYPE_UNK10, 0x00, 0x00, 0x39, 0x20, COLSHAPE_CYLINDER },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 20, 46, 0, { 0, 0, 0 } },
};

// static CollisionCheckInfoInit2 sColChkInfoInit
CollisionCheckInfoInit2 D_80A41B2C = {
    0x00, 0x0000, 0x0000, 0x0000, 0xFF,
};

EnGoSkelAnime D_80A41B38[4] = { { &D_06004930, 0.0f, 0x01, 0.0f },
                                { &D_06004930, 0.0f, 0x01, -10.0f },
                                { &D_060029A8, 1.0f, 0x01, -10.0f },
                                { &D_06010590, 1.0f, 0x01, -10.0f } };



void EnGo_SetupAction(EnGo* this, void* actionFunc) {
    this->actionFunc = actionFunc;
}

u16 EnGo_GetTextID(GlobalContext* globalCtx, EnGo* this) {
    Player* player = PLAYER;

    switch (this->actor.params & 0xF0) {
        case 0x90:
            if (gSaveContext.bgsFlag) {
                return 0x305E;
            } else if (INV_CONTENT(ITEM_POCKET_EGG) >= ITEM_CLAIM_CHECK) {
                if (func_800775CC(globalCtx) >= 3) {
                    return 0x305E;
                } else {
                    return 0x305D;
                }
            } else if (INV_CONTENT(ITEM_POCKET_EGG) >= ITEM_EYEDROPS) {
                player->exchangeItemId = EXCH_ITEM_EYEDROPS;
                return 0x3059;
            } else if (INV_CONTENT(ITEM_POCKET_EGG) >= ITEM_PRESCRIPTION) {
                return 0x3058;
            } else {
                player->exchangeItemId = EXCH_ITEM_SWORD_BROKEN;
                return 0x3053;
            }
        case 0x00:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
                if (gSaveContext.infTable[16] & 0x8000) {
                    return 0x3042;
                } else {
                    return 0x3041;
                }
            } else if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1) || (gSaveContext.infTable[16] & 0x2000)) {
                if (gSaveContext.infTable[16] & 0x4000) {
                    return 0x3038;
                } else {
                    return 0x3037;
                }
            } else {
                if (gSaveContext.infTable[16] & 0x200) {
                    if (gSaveContext.infTable[16] & 0x400) {
                        return 0x3033;
                    } else {
                        return 0x3032;
                    }
                } else {
                    return 0x3030;
                }
            }
        // GORON1_FIRE_GENERIC
        case 0x10:
            if (Flags_GetSwitch(globalCtx, this->actor.params >> 8)) {
                return 0x3052;
            } else {
                return 0x3051;
            }
        case 0x20:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.eventChkInf[2] & 0x8) {
                return 0x3021;
            } else if (gSaveContext.infTable[14] & 0x1) {
                return 0x302A;
            } else {
                return 0x3008;
            }
        case 0x30:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.eventChkInf[2] & 0x8) {
                return 0x3026;
            } else {
                return 0x3009;
            }
        case 0x40:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.eventChkInf[2] & 0x8) {
                return 0x3026;
            } else {
                return 0x300A;
            }
        case 0x50:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.infTable[15] & 1) {
                return 0x3015;
            } else {
                return 0x3014;
            }
        case 0x60:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.infTable[15] & 0x10) {
                return 0x3017;
            } else {
                return 0x3016;
            }
        case 0x70:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (gSaveContext.infTable[15] & 0x100) {
                return 0x3019;
            } else {
                return 0x3018;
            }
        default:
            return 0x0000;
    }
}

s16 EnGo_SetFlagsGetStates(GlobalContext* globalCtx, EnGo* this) {
    s16 unkState;
    f32 xzRange;
    f32 yRange;

    yRange = fabsf(this->actor.yDistFromLink) + 1.0f;
    xzRange = this->actor.xzDistFromLink + 1.0f;
    unkState = 1;

    switch (func_8010BDBC(&globalCtx->msgCtx)) {
        if (globalCtx) {}
        case 2:
            switch (this->actor.textId) {
                case 0x3008:
                    gSaveContext.infTable[14] |= 1;
                    unkState = 0;
                    break;
                case 0x300B:
                    gSaveContext.infTable[14] |= 0x800;
                    unkState = 0;
                    break;
                case 0x3014:
                    gSaveContext.infTable[15] |= 1;
                    unkState = 0;
                    break;
                case 0x3016:
                    gSaveContext.infTable[15] |= 0x10;
                    unkState = 0;
                    break;
                case 0x3018:
                    gSaveContext.infTable[15] |= 0x100;
                    unkState = 0;
                    break;
                case 0x3036:
                    func_8002F434(&this->actor, globalCtx, GI_TUNIC_GORON, xzRange, yRange);
                    gSaveContext.infTable[16] |= 0x2000; // EnGo exclusive flag
                    unkState = 2;
                    break;
                case 0x3037:
                    gSaveContext.infTable[16] |= 0x4000;
                    unkState = 0;
                    break;
                case 0x3041:
                    gSaveContext.infTable[16] |= 0x8000;
                    unkState = 0;
                    break;
                case 0x3059:
                    unkState = 2;
                    break;
                case 0x3052:
                case 0x3054:
                case 0x3055:
                case 0x305A:
                    unkState = 2;
                    break;
                case 0x305E:
                    unkState = 2;
                    break;
                default:
                    unkState = 0;
                    break;
            }
            break;
        case 4:
            if (func_80106BC8(globalCtx)) {
                switch (this->actor.textId) {
                    case 0x300A:
                        if (globalCtx->msgCtx.choiceIndex == 0) {
                            if (CUR_UPG_VALUE(UPG_STRENGTH) || (gSaveContext.infTable[14] & 0x800)) {
                                this->actor.textId = 0x300B;
                            } else {
                                this->actor.textId = 0x300C;
                            }
                        } else {
                            this->actor.textId = 0x300D;
                        }
                        func_8010B720(globalCtx, this->actor.textId);
                        unkState = 1;
                        break;
                    case 0x3034:
                        if (globalCtx->msgCtx.choiceIndex == 0) {
                            if (gSaveContext.infTable[16] & 0x800) {
                                this->actor.textId = 0x3033;
                            } else {
                                this->actor.textId = 0x3035;
                            }
                        } else if (gSaveContext.infTable[16] & 0x800) {
                            this->actor.textId = 0x3036;
                        } else {
                            this->actor.textId = 0x3033;
                        }
                        func_8010B720(globalCtx, this->actor.textId);
                        unkState = 1;
                        break;
                    case 0x3054:
                    case 0x3055:
                        if (globalCtx->msgCtx.choiceIndex == 0) {
                            unkState = 2;
                        } else {
                            this->actor.textId = 0x3056;
                            func_8010B720(globalCtx, this->actor.textId);
                            unkState = 1;
                        }
                        gSaveContext.infTable[11] |= 0x10;
                        break;
                }
            }
            break;
        case 5:
            if (func_80106BC8(globalCtx)) {
                switch (this->actor.textId) {
                    case 0x3035:
                        gSaveContext.infTable[16] |= 0x800;
                    case 0x3032:
                    case 0x3033:
                        this->actor.textId = 0x3034;
                        func_8010B720(globalCtx, this->actor.textId);
                        unkState = 1;
                        break;
                    default:
                        unkState = 2;
                        break;
                }
            }
            break;
        case 6:
            if (func_80106BC8(globalCtx)) {
                unkState = 3;
            }
            break;
        case 0:
        case 1:
        case 3:
        case 7:
        case 9:
            break;
    }
    return unkState;
}

s32 func_80A3ED24(GlobalContext* globalCtx, EnGo* this, struct_80034A14_arg1* arg2, f32 arg3,
                  u16 (*GetTextID)(GlobalContext*, Actor*), s16 (*unkFunc2)(GlobalContext*, Actor*)) {
    if (arg2->unk_00) {
        arg2->unk_00 = unkFunc2(globalCtx, &this->actor);
        return false;
    } else if (func_8002F194(&this->actor, globalCtx)) {
        arg2->unk_00 = 1;
        return true;
    } else if (func_8002F2CC(&this->actor, globalCtx, arg3) == 0) {
        return false;
    } else {
        this->actor.textId = GetTextID(globalCtx, &this->actor);
        return false;
    }
}

void func_80A3EDE0(EnGo* this, s32 unk_val) {
    SkelAnime_ChangeAnim(&this->skelanime, (&D_80A41B38[unk_val])->animationseg,
                         (&D_80A41B38[unk_val])->playbackSpeed * ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f),
                         0.0f, SkelAnime_GetFrameCount((&D_80A41B38[unk_val])->animationseg),
                         (&D_80A41B38[unk_val])->mode, (&D_80A41B38[unk_val])->transitionRate);
}

s32 EnGo_IsActorSpawned(EnGo* this, GlobalContext* globalCtx) {
    // GORON1_DMT_BIGGORON
    if (((this->actor.params) & 0xF0) == 0x90) {
        return true;
    // GORON1_FIRE_GENERIC
    } else if (globalCtx->sceneNum == SCENE_HIDAN && Flags_GetSwitch(globalCtx, (this->actor.params) >> 8) == 0 &&
               LINK_IS_ADULT && (this->actor.params & 0xF0) == 0x10) {
        return true;
    // GORON1_CITY_LINK
    } else if (globalCtx->sceneNum == SCENE_SPOT18 && LINK_IS_ADULT && (this->actor.params & 0xF0) == 0x00) {
        return true;
    // GORON1_DMT_DC_ENTRANCE, GORON1_DMT_ROLLING_SMALL, GORON1_DMT_BOMB_FLOWER
    } else if (globalCtx->sceneNum == SCENE_SPOT16 && gSaveContext.linkAge == 1 &&
               ((this->actor.params & 0xF0) == 0x20 || (this->actor.params & 0xF0) == 0x30 ||
                (this->actor.params & 0xF0) == 0x40)) {
        return true;
    // GORON1_CITY_ENTRANCE, GORON1_CITY_ISLAND, GORON1_CITY_LOST_WOODS
    } else if (globalCtx->sceneNum == SCENE_SPOT18 && gSaveContext.linkAge == 1 &&
               ((this->actor.params & 0xF0) == 0x50 || (this->actor.params & 0xF0) == 0x60 ||
                (this->actor.params & 0xF0) == 0x70)) {
        return true;
    } else {
        return false;
    }
}

f32 EnGo_GetGoronSize(EnGo* this) {
    switch (this->actor.params & 0xF0) {
        case 0x00:
            return 10.0f;
        case 0x20:
        case 0x30:
        case 0x50:
        case 0x60:
        case 0x70:
            return 20.0f;
        case 0x40:
            return 60.0f;
        default:
            return 20.0f;
    }
}

void func_80A3F060(EnGo* this, GlobalContext* globalCtx) {
    Player* player = PLAYER;
    s16 unkVal;

    if (this->actionFunc != EnGo_BiggoronActionFunc && this->actionFunc != EnGo_FireGenericActionFunc && this->actionFunc != func_80A40B1C) {
        unkVal = 1;
    }

    this->unk_1E0.unk_18 = player->actor.posRot.pos;
    this->unk_1E0.unk_14 = EnGo_GetGoronSize(this);
    func_80034A14(&this->actor, &this->unk_1E0, 4, unkVal);
}

void func_80A3F0E4(EnGo* this) {
    if (DECR(this->unk_214) == 0) {
        this->unk_216++;
        if (this->unk_216 >= 3) {
            this->unk_214 = Math_Rand_S16Offset(30, 30);
            this->unk_216 = 0;
        }
    }
}

s32 EnGo_IsCameraModified(EnGo* this, GlobalContext* globalCtx) {
    f32 xyzDist;
    s16 yawDiff;
    Camera* camera;

    camera = globalCtx->cameraPtrs[0];

    yawDiff = (s16)(this->actor.yawTowardsLink - this->actor.shape.rot.y);

    if (fabsf(yawDiff) > 10920.0f) {
        return 0;
    }

    xyzDist = (this->actor.scale.x / 0.01f) * 10000.0f;

    if ((this->actor.params & 0xF0) == 0x90) {
        Camera_ChangeSetting(camera, CAM_SET_TEPPEN);
        xyzDist *= 4.8f;
    }

    if (fabsf(this->actor.xyzDistFromLinkSq) > xyzDist) {
        if (camera->setting == 0x3F) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }
        return 0;
    } else {
        return 1;
    }
}

void EnGo_SwapInitialFrameAnimFrameCount(EnGo* this) {
    f32 initialFrame;

    initialFrame = this->skelanime.initialFrame;
    this->skelanime.initialFrame = this->skelanime.animFrameCount;
    this->skelanime.animFrameCount = initialFrame;
}

void func_80A3F274(EnGo* this) {
    s16 unk_14;
    s16 unk_14Target;
    f32 currentFrame;

    currentFrame = this->skelanime.animCurrentFrame;

    unk_14Target =
        (this->skelanime.animation == &D_06004930 && currentFrame > 32.0f) || this->skelanime.animation != &D_06004930
            ? 0xFF
            : 0;

    unk_14 = this->actor.shape.unk_14;
    Math_SmoothScaleMaxMinS(&unk_14, unk_14Target, 10, 60, 1);
    this->actor.shape.unk_14 = unk_14;
}

s32 EnGo_FollowPath(EnGo* this, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* pointPos;
    f32 xDist;
    f32 zDist;
    s32 pathIndex;
    s32 phi_return;

    pathIndex = this->actor.params & 0xF;
    if (pathIndex == 0xF) {
        return false;
    }

    path = &globalCtx->setupPathList[pathIndex];
    pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->unk_218;
    xDist = pointPos->x - this->actor.posRot.pos.x;
    zDist = pointPos->z - this->actor.posRot.pos.z;
    Math_SmoothScaleMaxMinS(&this->actor.posRot.rot.y, (s16)(Math_atan2f(xDist, zDist) * (32768.0f / M_PI)), 10, 1000,
                            1);

    phi_return = false;

    if ((SQ(xDist) + SQ(zDist)) < 600.0f) {
        this->unk_218++;
        if (this->unk_218 >= path->count) {
            this->unk_218 = 0;
        }
        if ((this->actor.params & 0xF0) != 0x00) {
            return true;
        } else if (Flags_GetSwitch(globalCtx, this->actor.params >> 8)) {
            return true;
        } else if (this->unk_218 >= this->actor.shape.rot.z) {
            this->unk_218 = 0;
        }
        return true;
    }
    return phi_return;
}

s32 EnGo_SetMovedPos(EnGo* this, GlobalContext* globalCtx) {
    Path* path;
    Vec3s* pointPos;

    if ((this->actor.params & 0xF) == 0xF) {
        return false;
    } else {
        path = &globalCtx->setupPathList[(this->actor.params & 0xF)];
        pointPos = SEGMENTED_TO_VIRTUAL(path->points);
        pointPos += (path->count - 1);

        this->actor.posRot.pos.x = pointPos->x;
        this->actor.posRot.pos.y = pointPos->y;
        this->actor.posRot.pos.z = pointPos->z;

        this->actor.initPosRot.pos = this->actor.posRot.pos;
        return true;
    }
}

s32 EnGo_SpawnDust(EnGo* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius, f32 xzAccel) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s16 angle;
    s32 i;

    pos = this->actor.posRot.pos;
    pos.y = this->actor.groundY;
    angle = (Math_Rand_ZeroOne() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.x = (Math_Rand_ZeroOne() - 0.5f) * xzAccel;
        accel.z = (Math_Rand_ZeroOne() - 0.5f) * xzAccel;
        pos.x = (Math_Sins(angle) * radius) + this->actor.posRot.pos.x;
        pos.z = (Math_Coss(angle) * radius) + this->actor.posRot.pos.z;
        EnGo_AddDust(this, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}

// EnGo2_IsRollingOnGround
s32 EnGo_IsRollingOnGround(EnGo* this, s16 unk_arg1, f32 unk_arg2) {
    if ((this->actor.bgCheckFlags & 1) == 0 || this->actor.velocity.y > 0.0f) {
        return false;
    } else if (this->unk_1E0.unk_00) {
        return true;
    } else if (DECR(this->unk_21C)) {
        if ((this->unk_21C & 1)) {
            this->actor.posRot.pos.y += 1.5f;
        } else {
            this->actor.posRot.pos.y -= 1.5f;
        }
        return true;
    } else {
        this->unk_21A--;
        if (this->unk_21A <= 0) {
            if (this->unk_21A == 0) {
                this->unk_21C = Math_Rand_S16Offset(60, 30);
                this->unk_21A = 0;
                this->actor.velocity.y = 0.0f;
                return true;
            }
            this->unk_21A = unk_arg1;
        }
        this->actor.velocity.y = ((f32)this->unk_21A / (f32)unk_arg1) * unk_arg2;
        return true;
    }
}

void func_80A3F908(EnGo* this, GlobalContext* globalCtx) {
    Player* player;
    f32 float1;
    s32 isUnkCondition;

    player = PLAYER;

    if (this->actionFunc == EnGo_BiggoronActionFunc || this->actionFunc == EnGo_GoronLinkRolling || this->actionFunc == EnGo_FireGenericActionFunc ||
        this->actionFunc == EnGo_Eyedrops || this->actionFunc == func_80A40DCC || this->actionFunc == EnGo_GetItem ||
        this->actionFunc == func_80A40C78 || this->actionFunc == func_80A40B1C) {

        float1 = (this->collider.dim.radius + 30.0f);
        float1 *= (this->actor.scale.x / 0.01f);

        if ((this->actor.params & 0xF0) == 0x90) {
            float1 *= 4.8f;
        }

        if ((this->actor.params & 0xF0) == 0x90) {
            isUnkCondition =
                func_80A3ED24(globalCtx, &this->actor, &this->unk_1E0, float1, EnGo_GetTextID, EnGo_SetFlagsGetStates);
        } else {
            isUnkCondition =
                func_800343CC(globalCtx, &this->actor, &this->unk_1E0, float1, EnGo_GetTextID, EnGo_SetFlagsGetStates);
        }

        if (((this->actor.params & 0xF0) == 0x90) && (isUnkCondition == true)) {

            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_SWORD_BROKEN) {
                if (func_8002F368(globalCtx) == EXCH_ITEM_SWORD_BROKEN) {
                    if (gSaveContext.infTable[11] & 0x10) {
                        this->actor.textId = 0x3055;
                    } else {
                        this->actor.textId = 0x3054;
                    }
                } else {
                    this->actor.textId = 0x3053;
                }
                player->actor.textId = this->actor.textId;
            }

            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_EYEDROPS) {
                if (func_8002F368(globalCtx) == EXCH_ITEM_EYEDROPS) {
                    this->actor.textId = 0x3059;
                } else {
                    this->actor.textId = 0x3058;
                }
                player->actor.textId = this->actor.textId;
            }
        }
    }
}

void EnGo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnGo* this = THIS;
    s32 pad;
    Vec3f D_80A41B9C = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f D_80A41BA8 = { 0.0f, 0.0f, 0.0f }; // unused

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFunc_Circle, 30.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelanime, &D_0600FEF0, NULL, 0, 0, 0);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &D_80A41B00);

    func_80061EFC(&this->actor.colChkInfo, DamageTable_Get(0x16), &D_80A41B2C);
    if (EnGo_IsActorSpawned(this, globalCtx) == 0) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((this->actor.params & 0xF0) && ((this->actor.params & 0xF0) != 0x90)) {
        this->actor.flags &= ~0x10;
        this->actor.flags &= ~0x20;
    }

    func_80A3EDE0(this, 0);
    this->actor.unk_1F = 6;
    this->unk_1E0.unk_00 = 0;
    this->actor.gravity = -1.0f;

    switch (this->actor.params & 0xF0) {
        case 0x00:
            Actor_SetScale(&this->actor, 0.008f);
            if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                EnGo_SetMovedPos(this, globalCtx);
                EnGo_SetupAction(this, EnGo_WakeUp);
            } else {
                this->actor.shape.unk_08 = 1400.0f;
                this->actor.speedXZ = 3.0f;
                EnGo_SetupAction(this, EnGo_GoronLinkRolling);
            }
            break;
        case 0x10:
            this->skelanime.animCurrentFrame = SkelAnime_GetFrameCount(&D_06004930);
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, EnGo_FireGenericActionFunc);
            break;
        case 0x40:
            if (gSaveContext.infTable[14] & 0x800) {
                EnGo_SetMovedPos(this, globalCtx);
            }
            Actor_SetScale(&this->actor, 0.015f);
            EnGo_SetupAction(this, EnGo_WakeUp);
            break;
        case 0x30:
            this->actor.shape.unk_08 = 1400.0f;
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, func_80A3FEB4);
            break;
        case 0x90:
            this->actor.unk_1F = 5;
            Actor_SetScale(&this->actor, 0.16f);
            EnGo_SetupAction(this, EnGo_WakeUp);
            break;
        case 0x20:
        case 0x50:
        case 0x60:
        case 0x70:
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, EnGo_WakeUp);
            break;
        default:
            Actor_Kill(&this->actor);
    }
}

void EnGo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnGo* this = THIS;
    SkelAnime_Free(&this->skelanime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void func_80A3FEB4(EnGo* this, GlobalContext* globalCtx) {
    if (!(this->actor.xyzDistFromLinkSq > SQ(1200.0f))) {
        EnGo_SetupAction(this, EnGo_StopRolling);
    }
}

void EnGo_StopRolling(EnGo* this, GlobalContext* globalCtx) {
    EnBom* bomb;

    if (DECR(this->unk_20E) == 0) {
        if (this->collider.base.maskB & 1) {
            this->collider.base.maskB &= ~1;
            globalCtx->damagePlayer(globalCtx, -4);
            func_8002F71C(globalCtx, &this->actor, 4.0f, this->actor.yawTowardsLink, 6.0f);
            this->unk_20E = 0x10;
        }
    }

    this->actor.speedXZ = 3.0f;
    if ((EnGo_FollowPath(this, globalCtx) == true) && (this->unk_218 == 0)) {
        bomb = (EnBom*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOM, this->actor.posRot.pos.x,
                                   this->actor.posRot.pos.y, this->actor.posRot.pos.z, 0, 0, 0, 0);
        if (bomb != NULL) {
            bomb->timer = 0;
        }
        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, func_80A4008C);
    }
    this->actor.shape.rot = this->actor.posRot.rot;
    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        EnGo_SpawnDust(this, 0xC, 0.16f, 0.1f, 1, 10.0f, 20.0f);
    }
}

void func_80A4008C(EnGo* this, GlobalContext* globalCtx) {
    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        if (this->unk_21A == 0) {
            this->actor.shape.unk_08 = 0.0f;
            EnGo_SetupAction(this, EnGo_WakeUp);
        } else {
            EnGo_SpawnDust(this, 0xC, 0.16f, 0.1f, 1, 10.0f, 20.0f);
        }
    }
}

void EnGo_GoronLinkRolling(EnGo* this, GlobalContext* globalCtx) {
    if ((EnGo_FollowPath(this, globalCtx) == true) && Flags_GetSwitch(globalCtx, this->actor.params >> 8) &&
        (this->unk_218 == 0)) {
        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, func_80A4008C);
        gSaveContext.infTable[16] |= 0x200;
    }

    this->actor.shape.rot = this->actor.posRot.rot;
    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        EnGo_SpawnDust(this, 0xC, 0.18f, 0.2f, 2, 13.0f, 20.0f);
    }
}

void EnGo_FireGenericActionFunc(EnGo* this, GlobalContext* globalCtx) {
}

void EnGo_WakeUp(EnGo* this, GlobalContext* globalCtx) {
    if ((DECR(this->unk_210) == 0) && EnGo_IsCameraModified(this, globalCtx)) {
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_WAKE_UP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);

        this->skelanime.animPlaybackSpeed = 0.1f;
        this->skelanime.animPlaybackSpeed *= (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f;

        EnGo_SetupAction(this, EnGo_WakeUpAnimation);
        if ((this->actor.params & 0xF0) == 0x90) {
            func_800800F8(globalCtx, 0x1068, -0x63, &this->actor, 0);
        }
    }
}

void EnGo_WakeUpAnimation(EnGo* this, GlobalContext* globalCtx) {
    f32 float1;

    if (this->skelanime.animPlaybackSpeed != 0.0f) {

        Math_SmoothScaleMaxMinF(&this->skelanime.animPlaybackSpeed,
                                ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * 0.5f, 0.1f, 1000.0f, 0.1f);

        float1 = this->skelanime.animCurrentFrame;
        float1 += this->skelanime.animPlaybackSpeed;

        if (float1 <= 12.0f) {
            return;
        } else {
            this->skelanime.animCurrentFrame = 12.0f;
            this->skelanime.animPlaybackSpeed = 0.0f;
            if ((this->actor.params & 0xF0) != 0x90) {
                this->unk_212 = 0x1E;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnGo_SetupAction(this, &func_80A405CC);
        return;
    } else if (EnGo_IsCameraModified(this, globalCtx) == 0) {
        EnGo_SwapInitialFrameAnimFrameCount(this);
        this->skelanime.animPlaybackSpeed = 0.0f;
        EnGo_SetupAction(this, func_80A40494);
    }
}

void func_80A40494(EnGo* this, GlobalContext* globalCtx) {
    f32 float1;

    Math_SmoothScaleMaxMinF(&this->skelanime.animPlaybackSpeed,
                            ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * -0.5f, 0.1f, 1000.0f, 0.1f);
    float1 = this->skelanime.animCurrentFrame;
    float1 += this->skelanime.animPlaybackSpeed;
    if (!(float1 >= 0.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_DODO_M_GND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnGo_SpawnDust(this, 0xA, 0.4f, 0.1f, 0x10, 26.0f, 2.0f);
        EnGo_SwapInitialFrameAnimFrameCount(this);
        this->skelanime.animPlaybackSpeed = 0.0f;
        this->skelanime.animCurrentFrame = 0.0f;
        this->unk_210 = Math_Rand_S16Offset(30, 30);
        EnGo_SetupAction(this, EnGo_WakeUp);
    }
}

void func_80A405CC(EnGo* this, GlobalContext* globalCtx) {
    f32 frameCount;
    f32 float1;

    frameCount = SkelAnime_GetFrameCount(&D_06004930);
    Math_SmoothScaleMaxMinF(&this->skelanime.animPlaybackSpeed, (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f, 0.1f,
                            1000.0f, 0.1f);

    float1 = this->skelanime.animCurrentFrame;
    float1 += this->skelanime.animPlaybackSpeed;
    if (!(float1 < frameCount)) {
        this->skelanime.animCurrentFrame = frameCount;
        this->skelanime.animPlaybackSpeed = 0.0f;
        this->unk_212 = Math_Rand_S16Offset(30, 30);
        if (((this->actor.params & 0xF0) == 0x40) && ((gSaveContext.infTable[14] & 0x800) == 0)) {
            EnGo_SetupAction(this, func_80A40B1C);
        } else {
            EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
        }
    }
}

void EnGo_BiggoronActionFunc(EnGo* this, GlobalContext* globalCtx) {
    if (((this->actor.params & 0xF0) == 0x90) && (this->unk_1E0.unk_00 == 2)) {
        if (gSaveContext.bgsFlag != 0) {
            this->unk_1E0.unk_00 = 0;
        } else {
            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_EYEDROPS) {
                func_80A3EDE0(this, 2);
                this->unk_21E = 0x64;
                this->unk_1E0.unk_00 = 0;
                EnGo_SetupAction(this, EnGo_Eyedrops);
                globalCtx->msgCtx.msgMode = 0x37;
                gSaveContext.timer2State = 0;
                func_800800F8(globalCtx, 0x105E, -0x63, &this->actor, 0);
            } else {
                this->unk_1E0.unk_00 = 0;
                EnGo_SetupAction(this, EnGo_GetItem);
                func_80106CCC(globalCtx);
                EnGo_GetItem(this, globalCtx);
            }
        }
    }

    else if (((this->actor.params & 0xF0) == 0) && (this->unk_1E0.unk_00 == 2)) {
        EnGo_SetupAction(this, EnGo_GetItem);
        globalCtx->msgCtx.unk_E3E7 = 4;
        globalCtx->msgCtx.msgMode = 0x36;
    } else {
        if ((DECR(this->unk_212) == 0) && (EnGo_IsCameraModified(this, globalCtx) == 0)) {
            EnGo_SwapInitialFrameAnimFrameCount(this);
            this->skelanime.animPlaybackSpeed = -0.1f;
            this->skelanime.animPlaybackSpeed *= (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f;
            EnGo_SetupAction(this, func_80A408D8);
        }
    }
}

void func_80A408D8(EnGo* this, GlobalContext* globalCtx) {
    f32 float1;

    if (this->skelanime.animPlaybackSpeed != 0.0f) {

        Math_SmoothScaleMaxMinF(&this->skelanime.animPlaybackSpeed,
                                ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * -1.0f, 0.1f, 1000.0f, 0.1f);

        float1 = this->skelanime.animCurrentFrame;
        float1 += this->skelanime.animPlaybackSpeed;

        if ((float1 >= 12.0f)) {
            return;
        } else {
            this->skelanime.animCurrentFrame = 12.0f;
            this->skelanime.animPlaybackSpeed = 0.0f;
            if ((this->actor.params & 0xF0) != 0x90) {
                this->unk_212 = 0x1E;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        EnGo_SetupAction(this, func_80A40494);
    } else if (EnGo_IsCameraModified(this, globalCtx)) {
        EnGo_SwapInitialFrameAnimFrameCount(this);
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        this->skelanime.animPlaybackSpeed = 0.0f;
        EnGo_SetupAction(this, func_80A405CC);
    }
}

void func_80A40A54(EnGo* this, GlobalContext* globalCtx) {
    f32 float1;
    f32 float2;

    float1 = (32768.0f / SkelAnime_GetFrameCount(&D_06010590));
    float2 = this->skelanime.animCurrentFrame * float1;
    this->actor.speedXZ = Math_Sins((s16)float2);

    if (EnGo_FollowPath(this, globalCtx) && this->unk_218 == 0) {
        func_80A3EDE0(this, 1);
        this->skelanime.animCurrentFrame = SkelAnime_GetFrameCount(&D_06004930);
        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
    }
}

void func_80A40B1C(EnGo* this, GlobalContext* globalCtx) {
    if (gSaveContext.infTable[14] & 0x800) {
        func_80A3EDE0(this, 3);
        EnGo_SetupAction(this, func_80A40A54);
        return;
    } else {
        EnGo_BiggoronActionFunc(this, globalCtx);
    }
}

// EnGo2_InitGetItem & EnGo2_SetGetItem & EnGo2_GetItem
void EnGo_GetItem(EnGo* this, GlobalContext* globalCtx) {
    f32 xzDist;
    f32 yDist;
    s32 getItem;

    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->unk_1E0.unk_00 = 2;
        this->actor.parent = NULL;
        EnGo_SetupAction(this, func_80A40C78);
    } else {
        this->unk_20C = 0;

        if ((this->actor.params & 0xF0) == 0x90) {
            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_CLAIM_CHECK) {
                getItem = GI_SWORD_BGS;
                this->unk_20C = 1;
            }
            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_EYEDROPS) {
                getItem = GI_CLAIM_CHECK;
            }
            if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_SWORD_BROKEN) {
                getItem = GI_PRESCRIPTION;
            }
        }

        if ((this->actor.params & 0xF0) == 0) {
            getItem = GI_TUNIC_GORON;
        }

        yDist = fabsf(this->actor.yDistFromLink) + 1.0f;
        xzDist = this->actor.xzDistFromLink + 1.0f;
        func_8002F434(&this->actor, globalCtx, getItem, xzDist, yDist);
    }
}

void func_80A40C78(EnGo* this, GlobalContext* globalCtx) {
    if (this->unk_1E0.unk_00 == 3) {
        EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
        if ((this->actor.params & 0xF0) != 0x90) {
            this->unk_1E0.unk_00 = 0;
        } else if (this->unk_20C) {
            this->unk_1E0.unk_00 = 0;
            gSaveContext.bgsFlag = 1;
        } else if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_PRESCRIPTION) {
            // "I've been waiting forrrrr you, with tearrrrrrs in my eyes... Please say hello to Kinnng Zorrra!"
            this->actor.textId = 0x3058;
            func_8010B720(globalCtx, this->actor.textId);
            this->unk_1E0.unk_00 = 1;
        } else if (INV_CONTENT(ITEM_POCKET_EGG) == ITEM_CLAIM_CHECK) {
            // "Afterrrr a few days... Please returrrrrrn... Wait, just wait patiently..."
            this->actor.textId = 0x305C; // 0x305C is used in EnGo but not EnGo2
            func_8010B720(globalCtx, this->actor.textId);
            this->unk_1E0.unk_00 = 1;
            func_800775D8();
        }
    }
}

void EnGo_Eyedrops(EnGo* this, GlobalContext* globalCtx) {
    if (DECR(this->unk_21E) == 0) {
        // "Wowwwwwwwwwwwwww!!  This is stimulating! It's worrrrrking grrrrreat!
        // Now I can get back to my blade business!
        // My worrrrrk is not  verrrry consistent, so I'll give this to you so you won't forrrrrget."
        // [goto 305C] "Afterrrr a few days... Please returrrrrrn... Wait, just wait patiently..." }
        this->actor.textId = 0x305A;
        func_8010B720(globalCtx, this->actor.textId);
        this->unk_1E0.unk_00 = 1;
        EnGo_SetupAction(this, func_80A40DCC);
    }
}

void func_80A40DCC(EnGo* this, GlobalContext* globalCtx) {
    if (this->unk_1E0.unk_00 == 2) {
        func_80A3EDE0(this, 1);
        this->skelanime.animCurrentFrame = SkelAnime_GetFrameCount(&D_06004930);
        func_80106CCC(globalCtx);
        EnGo_SetupAction(this, EnGo_GetItem);
        EnGo_GetItem(this, globalCtx);
    }
}

void EnGo_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnGo* this = THIS;
    ColliderCylinder* collider;

    collider = &this->collider;

    Collider_CylinderUpdate(&this->actor, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, collider);
    SkelAnime_FrameUpdateMatrix(&this->skelanime);
    if (this->actionFunc == EnGo_BiggoronActionFunc || this->actionFunc == EnGo_FireGenericActionFunc || this->actionFunc == func_80A40B1C) {
        func_80034F54(globalCtx, this->unk_220, this->unk_244, 0x12);
    }
    func_80A3F274(this);
    if (this->unk_1E0.unk_00 == 0) {
        Actor_MoveForward(&this->actor);
    }
    func_8002E4B4(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    func_80A3F0E4(this);
    func_80A3F908(this, globalCtx);
    this->actionFunc(this, globalCtx);
    func_80A3F060(this, globalCtx);
}

void func_80A40F58(EnGo* this, GlobalContext* globalCtx) {
    Vec3f D_80A41BB4 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2320);
    Matrix_Push();
    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go.c", 2326),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, &D_0600BD80);

    Matrix_MultVec3f(&D_80A41BB4, &this->actor.posRot2);
    Matrix_Pull();
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2341);
}

void func_80A41068(EnGo* this, GlobalContext* globalCtx) {

    Vec3f D_80A41BC0 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2355);
    Matrix_Push();
    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_RotateRPY((s16)(globalCtx->state.frames * ((s16)this->actor.speedXZ * 0x578)), 0, this->actor.shape.rot.z,
                     MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go.c", 2368),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, &D_0600C140);
    Matrix_MultVec3f(&D_80A41BC0, &this->actor.posRot2);
    Matrix_Pull();

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2383);
}

s32 EnGo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo* this = THIS;
    Vec3s vec1;
    f32 float1;

    if (limb == 0x11) {
        Matrix_Translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec1 = this->unk_1E0.unk_08;
        float1 = (vec1.y / 32768.0f) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
        float1 = (vec1.x / 32768.0f) * M_PI;
        Matrix_RotateZ(float1, MTXMODE_APPLY);
        Matrix_Translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limb == 0xA) {
        vec1 = this->unk_1E0.unk_0E;
        float1 = (vec1.y / 32768.0f) * M_PI;
        Matrix_RotateY(float1, MTXMODE_APPLY);
        float1 = (vec1.x / 32768.0f) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
    }

    if ((limb == 0xA) || (limb == 0xB) || (limb == 0xE)) {
        float1 = Math_Sins(this->unk_220[limb]);
        rot->y += float1 * 200.0f;
        float1 = Math_Coss(this->unk_244[limb]);
        rot->z += float1 * 200.0f;
    }

    return 0;
}

void EnGo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo* this = THIS;
    Vec3f D_80A41BCC = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 0x11) {
        Matrix_MultVec3f(&D_80A41BCC, &this->actor.posRot2.pos);
    }
}

void EnGo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnGo* this = THIS;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2479);
    EnGo_UpdateDust(this);
    Matrix_Push();
    EnGo_DrawDust(this, globalCtx);
    Matrix_Pull();

    if (this->actionFunc == EnGo_WakeUp) {
        func_80A40F58(this, globalCtx);
        return;
    } else if (this->actionFunc == EnGo_GoronLinkRolling || this->actionFunc == func_80A3FEB4 ||
               this->actionFunc == EnGo_StopRolling || this->actionFunc == func_80A3FEB4) {
        func_80A41068(this, globalCtx);
        return;
    } else {
        func_800943C8(globalCtx->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(&D_0600CE80));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(&D_0600DE80));

        SkelAnime_DrawFlexOpa(globalCtx, this->skelanime.skeleton, this->skelanime.limbDrawTbl,
                              this->skelanime.dListCount, EnGo_OverrideLimbDraw, EnGo_PostLimbDraw, &this->actor);
        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2525);
        EnGo_DrawDust(this, globalCtx);
    }
}

void EnGo_AddDust(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep) {
    EnGoEffect* dustEffect = this->dustEffects;
    s16 i;
    s16 timer;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            if (1) {}
            timer = initialTimer;
            dustEffect->timer = timer;
            dustEffect->type = 1;
            dustEffect->initialTimer = initialTimer;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            return;
        }
    }
}

void EnGo_UpdateDust(EnGo* this) {
    EnGoEffect* dustEffect;
    f32 randomNumber;
    s16 i;

    dustEffect = this->dustEffects;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;

            if ((dustEffect->timer) == 0) {
                dustEffect->type = 0;
            }

            dustEffect->accel.x = (Math_Rand_ZeroOne() * 0.4f) - 0.2f;
            randomNumber = Math_Rand_ZeroOne() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

void EnGo_DrawDust(EnGo* this, GlobalContext* globalCtx) {
    static Gfx* sDLists[] = { 0x040539B0, 0x040535B0, 0x040531B0, 0x04052DB0, 0x040529B0,
                              0x040525B0, 0x040521B0, 0x04051DB0, 0x00000000, 0x00000000 };
    EnGoEffect* dustEffect = this->dustEffects;
    s16 alpha;
    s16 firstDone;
    s16 index;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2626);

    firstDone = false;
    func_80093D84(globalCtx->state.gfxCtx);
    if (1) {}

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, &D_0600FD40);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
                firstDone = true;
            }

            alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
            func_800D1FD4(&globalCtx->mf_11DA0);
            Matrix_Scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_go.c", 2664),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sDLists[index]));
            gSPDisplayList(POLY_XLU_DISP++, &D_0600FD50);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_go.c", 2678);
}
