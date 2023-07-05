/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Player
 */

#include "ultra64.h"
#include "global.h"
#include "quake.h"

#include "overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.h"
#include "overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_link_child/object_link_child.h"

void Player_SetupTalk(PlayState* play, Actor* actor);
void Player_UseItem(PlayState* play, Player* this, s32 itemId);
s32 Player_StartFishing(PlayState* play);
void Player_SetupIdleWithMorph(Player* this, PlayState* play);
void Player_Setup1_IdleAll(Player* this, PlayState* play);
s32 Player_SetupCutsceneWithCsMode(PlayState* play, Actor* actor, s32 csMode);
void Player_ChangeItem(PlayState* play, Player* this, s8 itemAction);
s32 func_8083C61C(PlayState* play, Player* this);
void func_8084FF7C(Player* this);
s32 Player_TryGrabbingPlayer(PlayState* play, Player* this);
void Player_UpdateCommon(Player* this, PlayState* play, Input* input);
void Player_UpdateBunnyEars(Player* this);
s32 Player_IsDroppingFish(PlayState* play);
s32 Player_InflictDamage(PlayState* play, s32 damage);
s32 Player_TryDebugNoClip(Player* this, PlayState* play);

void Player_InitMode_0(PlayState* play, Player* this);
void Player_InitMode_AgeSwap(PlayState* play, Player* this);
void Player_InitMode_BlueWarp(PlayState* play, Player* this);
void Player_InitMode_Door(PlayState* play, Player* this);
void Player_InitMode_Grotto(PlayState* play, Player* this);
void Player_InitMode_WarpSong(PlayState* play, Player* this);
void Player_InitMode_FaroresWind(PlayState* play, Player* this);
void Player_InitMode_Knockback(PlayState* play, Player* this);
void Player_InitMode_D(PlayState* play, Player* this);
void Player_InitMode_Other(PlayState* play, Player* this);
void Player_InitMode_F(PlayState* play, Player* this);

void Player_Action_IdleLockOnEnemy(Player* this, PlayState* play);
void Player_Action_IdleZParallelOrLockOnFriend(Player* this, PlayState* play);
void Player_Action_Idle(Player* this, PlayState* play);
void Player_Action_SidewalkSlow(Player* this, PlayState* play);
void Player_Action_BackwalkFriend(Player* this, PlayState* play);
void Player_Action_BackwalkHaltFriend(Player* this, PlayState* play);
void Player_Action_BackwalkEndHaltFriend(Player* this, PlayState* play);
void Player_Action_SidewalkFast(Player* this, PlayState* play);
void Player_Action_Turn(Player* this, PlayState* play);
void Player_Action_Run(Player* this, PlayState* play);
void Player_Action_RunZTarget(Player* this, PlayState* play);
void Player_Action_BackwalkEnemy(Player* this, PlayState* play);
void Player_Action_BackwalkEndEnemy(Player* this, PlayState* play);
void Player_Action_PlantMagicBeans(Player* this, PlayState* play);
void Player_Action_ShieldCrouched(Player* this, PlayState* play);
void Player_Action_ShieldDeflectAttack(Player* this, PlayState* play);
void Player_Action_Damage(Player* this, PlayState* play);
void Player_Action_KnockbackFly(Player* this, PlayState* play);
void Player_Action_KnockbackDown(Player* this, PlayState* play);
void Player_Action_KnockbackGetUp(Player* this, PlayState* play);
void Player_Action_Die(Player* this, PlayState* play);
void Player_Action_Midair(Player* this, PlayState* play);
void Player_Action_Roll(Player* this, PlayState* play);
void Player_Action_FallDive(Player* this, PlayState* play);
void Player_Action_JumpSlash(Player* this, PlayState* play);
void Player_Action_ChargeSpinAttack(Player* this, PlayState* play);
void Player_Action_ChargeSpinAttackWalk(Player* this, PlayState* play);
void Player_Action_ChargeSpinAttackSidewalk(Player* this, PlayState* play);
void Player_Action_JumpToLedge(Player* this, PlayState* play);
void Player_Action_CsIntoAction(Player* this, PlayState* play);
void Player_Action_MiniCutscene(Player* this, PlayState* play);
void Player_Action_OpenDoor(Player* this, PlayState* play);
void Player_Action_Lift(Player* this, PlayState* play);
void Player_Action_LiftThrowHeavyBlock(Player* this, PlayState* play);
void Player_Action_LiftSilverRock(Player* this, PlayState* play);
void Player_Action_ThrowSilverRock(Player* this, PlayState* play);
void Player_Action_LiftFail(Player* this, PlayState* play);
void Player_Action_PutDown(Player* this, PlayState* play);
void Player_Action_Throw(Player* this, PlayState* play);
void Player_Action_AimFirstPerson(Player* this, PlayState* play);
void Player_Action_Talk(Player* this, PlayState* play);
void Player_Action_GrabWall(Player* this, PlayState* play);
void Player_Action_Push(Player* this, PlayState* play);
void Player_Action_Pull(Player* this, PlayState* play);
void Player_Action_GrabLedge(Player* this, PlayState* play);
void Player_Action_ClimbLedge(Player* this, PlayState* play);
void Player_Action_ClimbWall(Player* this, PlayState* play);
void Player_Action_ClimbEnd(Player* this, PlayState* play);
void Player_Action_Crawl(Player* this, PlayState* play);
void Player_Action_LeaveCrawlspace(Player* this, PlayState* play);
void Player_Action_RideHorse(Player* this, PlayState* play);
void Player_Action_DismountHorse(Player* this, PlayState* play);
void Player_Action_SwimIdle(Player* this, PlayState* play);
void Player_Action_SwimSpawn(Player* this, PlayState* play);
void Player_Action_SwimMove(Player* this, PlayState* play);
void Player_Action_SwimZTarget(Player* this, PlayState* play);
void Player_Action_SwimDive(Player* this, PlayState* play);
void Player_Action_SwimGetItem(Player* this, PlayState* play);
void Player_Action_SwimDamage(Player* this, PlayState* play);
void Player_Action_SwimDrown(Player* this, PlayState* play);
void Player_Action_PlayOcarina(Player* this, PlayState* play);
void Player_Action_ThrowDekuNut(Player* this, PlayState* play);
void Player_Action_GetItem(Player* this, PlayState* play);
void Player_Action_SpawnFromAgeSwap(Player* this, PlayState* play);
void Player_Action_DrinkFromBottle(Player* this, PlayState* play);
void Player_Action_SwingBottle(Player* this, PlayState* play);
void Player_Action_ReleaseFairyFromBottle(Player* this, PlayState* play);
void Player_Action_DropItemFromBottle(Player* this, PlayState* play);
void Player_Action_ExchangeItem(Player* this, PlayState* play);
void Player_Action_Grabbed(Player* this, PlayState* play);
void Player_Action_SlipOnSlope(Player* this, PlayState* play);
void Player_Action_StartCutsceneDelayed(Player* this, PlayState* play);
void Player_Action_SpawnFromWarpSong(Player* this, PlayState* play);
void Player_Action_SpawnFromBlueWarp(Player* this, PlayState* play);
void Player_Action_EnterGrotto(Player* this, PlayState* play);
void Player_Action_SpawnFromDoor(Player* this, PlayState* play);
void Player_Action_SpawnFromGrotto(Player* this, PlayState* play);
void Player_Action_PlayShootingGallery(Player* this, PlayState* play);
void Player_Action_FrozenInIce(Player* this, PlayState* play);
void Player_Action_ElectricShock(Player* this, PlayState* play);
void Player_Action_Attack(Player* this, PlayState* play);
void Player_Action_Recoil(Player* this, PlayState* play);
void Player_Action_FaroresWindChoice(Player* this, PlayState* play);
void Player_Action_SpawnFromFaroresWind(Player* this, PlayState* play);
void Player_Action_CastMagicSpell(Player* this, PlayState* play);
void Player_Action_HookshotFly(Player* this, PlayState* play);
void Player_Action_CastFishingRod(Player* this, PlayState* play);
void Player_Action_ReleaseFishFromFishingRod(Player* this, PlayState* play);
void Player_Action_StartCutscene(Player* this, PlayState* play);

s32 Player_UpperAction_IdleDefault(Player* this, PlayState* play);
s32 Player_UpperAction_IdleWithSword(Player* this, PlayState* play);
s32 Player_UpperAction_2(Player* this, PlayState* play);
s32 Player_UpperAction_ShieldStanding(Player* this, PlayState* play);
s32 Player_UpperAction_X(Player* this, PlayState* play);
s32 Player_UpperAction_ShieldStandingEnd(Player* this, PlayState* play);
s32 Player_UpperAction_IA_Aim(Player* this, PlayState* play);
s32 Player_UpperAction_ReadyFpsItemToShoot(Player* this, PlayState* play);
s32 Player_UpperAction_7(Player* this, PlayState* play);
s32 Player_UpperAction_8(Player* this, PlayState* play);
s32 Player_UpperAction_CarryAboveHead(Player* this, PlayState* play);
s32 Player_UpperAction_HoldBoomerang(Player* this, PlayState* play);
s32 Player_UpperAction_PullOutBoomerang(Player* this, PlayState* play);
s32 Player_UpperAction_AimBoomerang(Player* this, PlayState* play);
s32 Player_UpperAction_ThrowBoomerang(Player* this, PlayState* play);
s32 Player_UpperAction_WaitForThrownBoomerang(Player* this, PlayState* play);
s32 Player_UpperAction_CatchBoomerang(Player* this, PlayState* play);

void Player_InitItemAction_DoNothing(PlayState* play, Player* this);
void Player_InitItemAction_Hammer(PlayState* play, Player* this);
void Player_InitItemAction_Aim(PlayState* play, Player* this);
void Player_InitItemAction_DekuStick(PlayState* play, Player* this);
void Player_InitItemAction_Explosive(PlayState* play, Player* this);
void Player_InitItemAction_Hookshot(PlayState* play, Player* this);
void Player_InitItemAction_Boomerang(PlayState* play, Player* this);

// Try swapping a new action
s32 Player_SwapAction_TryCUp(Player* this, PlayState* play);
s32 Player_SwapAction_TryOpeningDoor(Player* this, PlayState* play);
s32 Player_SwapAction_TryGetItem(Player* this, PlayState* play);
s32 Player_SwapAction_TryMountingHorse(Player* this, PlayState* play);
s32 Player_SwapAction_TryTalking(Player* this, PlayState* play);
s32 Player_SwapAction_TrySpecialWallInteraction(Player* this, PlayState* play);
s32 Player_SwapAction_TryRolling(Player* this, PlayState* play);
s32 Player_SwapAction_TryAttackBottleFishingRod(Player* this, PlayState* play);
s32 Player_SwapAction_TryChargingSpinAttack(Player* this, PlayState* play);
s32 Player_SwapAction_TryThrowPutDown(Player* this, PlayState* play);
s32 Player_SwapAction_TryAButtonActions(Player* this, PlayState* play);
s32 Player_SwapAction_TryShieldingCrouched(Player* this, PlayState* play);
s32 Player_SwapAction_TryJumpToLedge(Player* this, PlayState* play);
s32 Player_SwapAction_TryItemCsFirstPerson(Player* this, PlayState* play);

// Setup a new action with a mini cutscene
void Player_CsIntoAction_SetupLift(PlayState* play, Player* this);
void Player_CsIntoAction_SetupTalk(PlayState* play, Player* this);
void Player_CsIntoAction_SetupRideHorse(PlayState* play, Player* this);
void Player_CsIntoAction_SetupGrabWall(PlayState* play, Player* this);
void Player_CsIntoAction_SetupClimb(PlayState* play, Player* this);
void Player_CsIntoAction_SetupCrawl(PlayState* play, Player* this);
void Player_CsIntoAction_SetupGetItem(PlayState* play, Player* this);

void Player_CsAnim_SetHorizontalSpeedToZero(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceWithMorphReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceAdjustedWithLongMorphReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjustedWithLongMorphReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayOnceAdjustedUnkFlags(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayOnce(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayLoopAdjustedUnkFlags(PlayState* play, Player* this, void* anim);
void Player_CsAnim_ReplacePlayLoop(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnce(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoop(PlayState* play, Player* this, void* anim);
void Player_CsAnim_Update(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjustedWithLongMorphOnceFinished(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjustedUnkFlagsOnceFinished(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceForOneFrameReset(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayOnceAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjusted(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayLoopAdjustedOnceFinished(PlayState* play, Player* this, void* anim);
void Player_CsAnim_PlayAnimSfx(PlayState* play, Player* this, void* arg2);

void Player_CsAction_SwimIdle(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_SwimSurfaceFromDive(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_Idle(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_3(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_4(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_Wait(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_6(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_7(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_8(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_9(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_10(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_11(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_12(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_13(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_14(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_15(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_16(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_17(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_18(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_19(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_20(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_21(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_22(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_23(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_24(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_25(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_26(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_27(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_28(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_29(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_30(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_31(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_32(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_33(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_DrawPlayer(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_35(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_36(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_37(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_38(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_39(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_40(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_41(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_42(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_43(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_44(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_45(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_46(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_47(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_48(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_49(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_50(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_51(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_52(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_53(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_SetStartPosAndYaw(PlayState* play, Player* this, CsCmdActorCue* cue);
void Player_CsAction_55(PlayState* play, Player* this, CsCmdActorCue* cue);

typedef enum AnimSfxType {
    /*  1 */ ANIMSFX_TYPE_GENERAL = 1,
    /*  2 */ ANIMSFX_TYPE_FLOOR,
    /*  3 */ ANIMSFX_TYPE_FLOOR_BY_AGE,
    /*  4 */ ANIMSFX_TYPE_VOICE,
    /*  5 */ ANIMSFX_TYPE_FLOOR_LAND,
    /*  6 */ ANIMSFX_TYPE_FLOOR_WALK_HIGH, // High frequency, high volume
    /*  7 */ ANIMSFX_TYPE_FLOOR_JUMP,
    /*  8 */ ANIMSFX_TYPE_FLOOR_WALK_LOW, // Low frequency, low volume
    /*  9 */ ANIMSFX_TYPE_WALK_WOOD
} AnimSfxType;

#define ANIMSFX_SHIFT_TYPE(type) ((type) << 11)

#define ANIMSFX_CONTINUE (1)
#define ANIMSFX_STOP (0)

#define ANIMSFX_FLAGS(type, frame, cond) \
    (((ANIMSFX_##cond) == ANIMSFX_STOP ? -1 : 1) * (ANIMSFX_SHIFT_TYPE(type) | ((frame)&0x7FF)))

#define ANIMSFX(type, frame, sfxId, cond) \
    { (sfxId), ANIMSFX_FLAGS(type, frame, cond) }

#define ANIMSFX_GET_TYPE(data) ((data)&0x7800)
#define ANIMSFX_GET_FRAME(data) ((data)&0x7FF)

typedef struct {
    /* 0x0 */ u16 sfxId;
    /* 0x2 */ s16 flags; // negative marks the end
} AnimSfxEntry;          // size = 0x4

#define PLAYER_ROT_OVERRIDE_FOCUS_ROT_X (1 << 0)
#define PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y (1 << 1)
#define PLAYER_ROT_OVERRIDE_FOCUS_ROT_Z (1 << 2)

#define PLAYER_ROT_OVERRIDE_HEAD_ROT_X (1 << 3)
#define PLAYER_ROT_OVERRIDE_HEAD_ROT_Y (1 << 4)
#define PLAYER_ROT_OVERRIDE_HEAD_ROT_Z (1 << 5)

#define PLAYER_ROT_OVERRIDE_UPPER_ROT_X (1 << 6)
#define PLAYER_ROT_OVERRIDE_UPPER_ROT_Y (1 << 7)
#define PLAYER_ROT_OVERRIDE_UPPER_ROT_Z (1 << 8)

typedef enum {
    /* 0 */ PLAYER_DMGREACTION_DEFAULT,
    /* 1 */ PLAYER_DMGREACTION_KNOCKBACK,
    /* 2 */ PLAYER_DMGREACTION_FLINCH,
    /* 3 */ PLAYER_DMGREACTION_FROZEN,
    /* 4 */ PLAYER_DMGREACTION_ELECTRIC_SHOCK
} PlayerDamageReaction;

// .bss part 1
static s32 D_80858AA0;
static s32 D_80858AA4;
static Vec3f sInteractWallCheckResult;
static Input* sControlInput;

// .data

static u8 D_80853410[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

#define SFX_VOICE_BANK_SIZE 0x20
#define SFX_PLAYER_BANK_SIZE 0x80

static PlayerAgeProperties sAgeProperties[] = {
    // LINK_AGE_ADULT
    {
        56.0f,            // ceilingCheckHeight
        90.0f,            // unk_04
        1.0f,             // unk_08
        111.0f,           // unk_0C
        70.0f,            // unk_10
        79.4f,            // unk_14
        59.0f,            // unk_18
        41.0f,            // unk_1C
        19.0f,            // unk_20
        36.0f,            // unk_24
        44.8f,            // buoyancyDepthInWater
        56.0f,            // unk_2C
        68.0f,            // unk_30
        70.0f,            // unk_34
        18.0f,            // wallCheckRadius
        15.0f,            // unk_3C
        70.0f,            // unk_40
        { 9, 4671, 359 }, // unk_44
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { 9, 6122, 359 },
            { 9, 7693, 380 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // climbLeftSpeed
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                     // unk_86
        0 * SFX_VOICE_BANK_SIZE,               // voiceSfxIdOffset
        1 * SFX_PLAYER_BANK_SIZE,              // surfaceSfxIdOffset
        &gPlayerAnim_link_demo_Tbox_open,      // unk_98
        &gPlayerAnim_link_demo_back_to_past,   // unk_9C
        &gPlayerAnim_link_demo_return_to_past, // unk_A0
        &gPlayerAnim_link_normal_climb_startA, // unk_A4
        &gPlayerAnim_link_normal_climb_startB, // unk_A8
        { &gPlayerAnim_link_normal_climb_upL, &gPlayerAnim_link_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // climbHorizontalAnims
        { &gPlayerAnim_link_normal_climb_endAL, &gPlayerAnim_link_normal_climb_endAR },   // unk_C4
        { &gPlayerAnim_link_normal_climb_endBR, &gPlayerAnim_link_normal_climb_endBL },   // unk_CC
    },
    // LINK_AGE_CHILD
    {
        40.0f,                   // ceilingCheckHeight
        60.0f,                   // unk_04
        11.0f / 17.0f,           // unk_08
        71.0f,                   // unk_0C
        50.0f,                   // unk_10
        47.0f,                   // unk_14
        39.0f,                   // unk_18
        27.0f,                   // unk_1C
        19.0f,                   // unk_20
        22.0f,                   // unk_24
        29.6f,                   // buoyancyDepthInWater
        32.0f,                   // unk_2C
        48.0f,                   // unk_30
        70.0f * (11.0f / 17.0f), // unk_34
        14.0f,                   // wallCheckRadius
        12.0f,                   // unk_3C
        55.0f,                   // unk_40
        { -24, 3565, 876 },      // unk_44
        {
            { -24, 3474, 862 },
            { -24, 4977, 937 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { -24, 4977, 937 },
            { -24, 6495, 937 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // climbLeftSpeed
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                        // unk_86
        1 * SFX_VOICE_BANK_SIZE,                  // voiceSfxIdOffset
        0 * SFX_PLAYER_BANK_SIZE,                 // surfaceSfxIdOffset
        &gPlayerAnim_clink_demo_Tbox_open,        // unk_98
        &gPlayerAnim_clink_demo_goto_future,      // unk_9C
        &gPlayerAnim_clink_demo_return_to_future, // unk_A0
        &gPlayerAnim_clink_normal_climb_startA,   // unk_A4
        &gPlayerAnim_clink_normal_climb_startB,   // unk_A8
        { &gPlayerAnim_clink_normal_climb_upL, &gPlayerAnim_clink_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // climbHorizontalAnims
        { &gPlayerAnim_clink_normal_climb_endAL, &gPlayerAnim_clink_normal_climb_endAR }, // unk_C4
        { &gPlayerAnim_clink_normal_climb_endBR, &gPlayerAnim_clink_normal_climb_endBL }, // unk_CC
    },
};

static u32 D_808535D0 = false;
static f32 sAnalogStickDistance = 0.0f;
static s16 sAnalogStickAngle = 0;
static s16 sCameraOffsetAnalogStickAngle = 0;
static s32 D_808535E0 = false;
static s32 sFloorType = FLOOR_TYPE_0;
static f32 sPlayerUnderwaterSpeedAdjustment = 1.0f;
static f32 D_808535EC = 1.0f;
static u32 sTouchedWallFlags = 0;
static u32 sConveyorSpeed = CONVEYOR_SPEED_DISABLED;
static s16 sIsFloorConveyor = false;
static s16 sConveyorYaw = 0;
static f32 sYDistToFloor = 0.0f;
static s32 sPrevFloorProperty = FLOOR_PROPERTY_0; // floor property from the previous frame
static s32 sShapeYawToTouchedWall = 0;
static s32 sWorldYawToTouchedWall = 0;
static s16 sFloorPitchShape = 0;
static s32 D_80853614 = 0;
static s32 D_80853618 = 0;

static u16 D_8085361C[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, OBJECT_INVALID }

typedef struct {
    /* 0x00 */ u8 itemId;
    /* 0x01 */ u8 field; // various bit-packed data
    /* 0x02 */ s8 gi;    // defines the draw id and chest opening animation
    /* 0x03 */ u8 textId;
    /* 0x04 */ u16 objectId;
} GetItemEntry; // size = 0x06

static GetItemEntry sGetItemTable[] = {
    // GI_BOMBS_5
    GET_ITEM(ITEM_BOMBS_5, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    // GI_DEKU_NUTS_5
    GET_ITEM(ITEM_DEKU_NUTS_5, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_10
    GET_ITEM(ITEM_BOMBCHU, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    // GI_BOW
    GET_ITEM(ITEM_BOW, OBJECT_GI_BOW, GID_BOW, 0x31, 0x80, CHEST_ANIM_LONG),
    // GI_SLINGSHOT
    GET_ITEM(ITEM_SLINGSHOT, OBJECT_GI_PACHINKO, GID_SLINGSHOT, 0x30, 0x80, CHEST_ANIM_LONG),
    // GI_BOOMERANG
    GET_ITEM(ITEM_BOOMERANG, OBJECT_GI_BOOMERANG, GID_BOOMERANG, 0x35, 0x80, CHEST_ANIM_LONG),
    // GI_DEKU_STICKS_1
    GET_ITEM(ITEM_DEKU_STICK, OBJECT_GI_STICK, GID_DEKU_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    // GI_HOOKSHOT
    GET_ITEM(ITEM_HOOKSHOT, OBJECT_GI_HOOKSHOT, GID_HOOKSHOT, 0x36, 0x80, CHEST_ANIM_LONG),
    // GI_LONGSHOT
    GET_ITEM(ITEM_LONGSHOT, OBJECT_GI_HOOKSHOT, GID_LONGSHOT, 0x4F, 0x80, CHEST_ANIM_LONG),
    // GI_LENS_OF_TRUTH
    GET_ITEM(ITEM_LENS_OF_TRUTH, OBJECT_GI_GLASSES, GID_LENS_OF_TRUTH, 0x39, 0x80, CHEST_ANIM_LONG),
    // GI_ZELDAS_LETTER
    GET_ITEM(ITEM_ZELDAS_LETTER, OBJECT_GI_LETTER, GID_ZELDAS_LETTER, 0x69, 0x80, CHEST_ANIM_LONG),
    // GI_OCARINA_OF_TIME
    GET_ITEM(ITEM_OCARINA_OF_TIME, OBJECT_GI_OCARINA, GID_OCARINA_OF_TIME, 0x3A, 0x80, CHEST_ANIM_LONG),
    // GI_HAMMER
    GET_ITEM(ITEM_HAMMER, OBJECT_GI_HAMMER, GID_HAMMER, 0x38, 0x80, CHEST_ANIM_LONG),
    // GI_COJIRO
    GET_ITEM(ITEM_COJIRO, OBJECT_GI_NIWATORI, GID_COJIRO, 0x02, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_EMPTY
    GET_ITEM(ITEM_BOTTLE_EMPTY, OBJECT_GI_BOTTLE, GID_BOTTLE_EMPTY, 0x42, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_POTION_RED
    GET_ITEM(ITEM_BOTTLE_POTION_RED, OBJECT_GI_LIQUID, GID_BOTTLE_POTION_RED, 0x43, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_POTION_GREEN
    GET_ITEM(ITEM_BOTTLE_POTION_GREEN, OBJECT_GI_LIQUID, GID_BOTTLE_POTION_GREEN, 0x44, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_POTION_BLUE
    GET_ITEM(ITEM_BOTTLE_POTION_BLUE, OBJECT_GI_LIQUID, GID_BOTTLE_POTION_BLUE, 0x45, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_FAIRY
    GET_ITEM(ITEM_BOTTLE_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE_EMPTY, 0x46, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_MILK_FULL
    GET_ITEM(ITEM_BOTTLE_MILK_FULL, OBJECT_GI_MILK, GID_BOTTLE_MILK_FULL, 0x98, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_RUTOS_LETTER
    GET_ITEM(ITEM_BOTTLE_RUTOS_LETTER, OBJECT_GI_BOTTLE_LETTER, GID_BOTTLE_RUTOS_LETTER, 0x99, 0x80, CHEST_ANIM_LONG),
    // GI_MAGIC_BEAN
    GET_ITEM(ITEM_MAGIC_BEAN, OBJECT_GI_BEAN, GID_MAGIC_BEAN, 0x48, 0x80, CHEST_ANIM_SHORT),
    // GI_MASK_SKULL
    GET_ITEM(ITEM_MASK_SKULL, OBJECT_GI_SKJ_MASK, GID_MASK_SKULL, 0x10, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_SPOOKY
    GET_ITEM(ITEM_MASK_SPOOKY, OBJECT_GI_REDEAD_MASK, GID_MASK_SPOOKY, 0x11, 0x80, CHEST_ANIM_LONG),
    // GI_CHICKEN
    GET_ITEM(ITEM_CHICKEN, OBJECT_GI_NIWATORI, GID_CUCCO, 0x48, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_KEATON
    GET_ITEM(ITEM_MASK_KEATON, OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, 0x12, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_BUNNY_HOOD
    GET_ITEM(ITEM_MASK_BUNNY_HOOD, OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY_HOOD, 0x13, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_TRUTH
    GET_ITEM(ITEM_MASK_TRUTH, OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, 0x17, 0x80, CHEST_ANIM_LONG),
    // GI_POCKET_EGG
    GET_ITEM(ITEM_POCKET_EGG, OBJECT_GI_EGG, GID_EGG, 0x01, 0x80, CHEST_ANIM_LONG),
    // GI_POCKET_CUCCO
    GET_ITEM(ITEM_POCKET_CUCCO, OBJECT_GI_NIWATORI, GID_CUCCO, 0x48, 0x80, CHEST_ANIM_LONG),
    // GI_ODD_MUSHROOM
    GET_ITEM(ITEM_ODD_MUSHROOM, OBJECT_GI_MUSHROOM, GID_ODD_MUSHROOM, 0x03, 0x80, CHEST_ANIM_LONG),
    // GI_ODD_POTION
    GET_ITEM(ITEM_ODD_POTION, OBJECT_GI_POWDER, GID_ODD_POTION, 0x04, 0x80, CHEST_ANIM_LONG),
    // GI_POACHERS_SAW
    GET_ITEM(ITEM_POACHERS_SAW, OBJECT_GI_SAW, GID_POACHERS_SAW, 0x05, 0x80, CHEST_ANIM_LONG),
    // GI_BROKEN_GORONS_SWORD
    GET_ITEM(ITEM_BROKEN_GORONS_SWORD, OBJECT_GI_BROKENSWORD, GID_BROKEN_GORONS_SWORD, 0x08, 0x80, CHEST_ANIM_LONG),
    // GI_PRESCRIPTION
    GET_ITEM(ITEM_PRESCRIPTION, OBJECT_GI_PRESCRIPTION, GID_PRESCRIPTION, 0x09, 0x80, CHEST_ANIM_LONG),
    // GI_EYEBALL_FROG
    GET_ITEM(ITEM_EYEBALL_FROG, OBJECT_GI_FROG, GID_EYEBALL_FROG, 0x0D, 0x80, CHEST_ANIM_LONG),
    // GI_EYE_DROPS
    GET_ITEM(ITEM_EYE_DROPS, OBJECT_GI_EYE_LOTION, GID_EYE_DROPS, 0x0E, 0x80, CHEST_ANIM_LONG),
    // GI_CLAIM_CHECK
    GET_ITEM(ITEM_CLAIM_CHECK, OBJECT_GI_TICKETSTONE, GID_CLAIM_CHECK, 0x0A, 0x80, CHEST_ANIM_LONG),
    // GI_SWORD_KOKIRI
    GET_ITEM(ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, 0xA4, 0x80, CHEST_ANIM_LONG),
    // GI_SWORD_KNIFE
    GET_ITEM(ITEM_SWORD_BIGGORON, OBJECT_GI_LONGSWORD, GID_SWORD_BIGGORON, 0x4B, 0x80, CHEST_ANIM_LONG),
    // GI_SHIELD_DEKU
    GET_ITEM(ITEM_SHIELD_DEKU, OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU, 0x4C, 0xA0, CHEST_ANIM_SHORT),
    // GI_SHIELD_HYLIAN
    GET_ITEM(ITEM_SHIELD_HYLIAN, OBJECT_GI_SHIELD_2, GID_SHIELD_HYLIAN, 0x4D, 0xA0, CHEST_ANIM_SHORT),
    // GI_SHIELD_MIRROR
    GET_ITEM(ITEM_SHIELD_MIRROR, OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x4E, 0x80, CHEST_ANIM_LONG),
    // GI_TUNIC_GORON
    GET_ITEM(ITEM_TUNIC_GORON, OBJECT_GI_CLOTHES, GID_TUNIC_GORON, 0x50, 0xA0, CHEST_ANIM_LONG),
    // GI_TUNIC_ZORA
    GET_ITEM(ITEM_TUNIC_ZORA, OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, 0x51, 0xA0, CHEST_ANIM_LONG),
    // GI_BOOTS_IRON
    GET_ITEM(ITEM_BOOTS_IRON, OBJECT_GI_BOOTS_2, GID_BOOTS_IRON, 0x53, 0x80, CHEST_ANIM_LONG),
    // GI_BOOTS_HOVER
    GET_ITEM(ITEM_BOOTS_HOVER, OBJECT_GI_HOVERBOOTS, GID_BOOTS_HOVER, 0x54, 0x80, CHEST_ANIM_LONG),
    // GI_QUIVER_40
    GET_ITEM(ITEM_QUIVER_40, OBJECT_GI_ARROWCASE, GID_QUIVER_40, 0x56, 0x80, CHEST_ANIM_LONG),
    // GI_QUIVER_50
    GET_ITEM(ITEM_QUIVER_50, OBJECT_GI_ARROWCASE, GID_QUIVER_50, 0x57, 0x80, CHEST_ANIM_LONG),
    // GI_BOMB_BAG_20
    GET_ITEM(ITEM_BOMB_BAG_20, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, 0x58, 0x80, CHEST_ANIM_LONG),
    // GI_BOMB_BAG_30
    GET_ITEM(ITEM_BOMB_BAG_30, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, 0x59, 0x80, CHEST_ANIM_LONG),
    // GI_BOMB_BAG_40
    GET_ITEM(ITEM_BOMB_BAG_40, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_40, 0x5A, 0x80, CHEST_ANIM_LONG),
    // GI_SILVER_GAUNTLETS
    GET_ITEM(ITEM_STRENGTH_SILVER_GAUNTLETS, OBJECT_GI_GLOVES, GID_SILVER_GAUNTLETS, 0x5B, 0x80, CHEST_ANIM_LONG),
    // GI_GOLD_GAUNTLETS
    GET_ITEM(ITEM_STRENGTH_GOLD_GAUNTLETS, OBJECT_GI_GLOVES, GID_GOLD_GAUNTLETS, 0x5C, 0x80, CHEST_ANIM_LONG),
    // GI_SCALE_SILVER
    GET_ITEM(ITEM_SCALE_SILVER, OBJECT_GI_SCALE, GID_SCALE_SILVER, 0xCD, 0x80, CHEST_ANIM_LONG),
    // GI_SCALE_GOLDEN
    GET_ITEM(ITEM_SCALE_GOLDEN, OBJECT_GI_SCALE, GID_SCALE_GOLDEN, 0xCE, 0x80, CHEST_ANIM_LONG),
    // GI_STONE_OF_AGONY
    GET_ITEM(ITEM_STONE_OF_AGONY, OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x68, 0x80, CHEST_ANIM_LONG),
    // GI_GERUDOS_CARD
    GET_ITEM(ITEM_GERUDOS_CARD, OBJECT_GI_GERUDO, GID_GERUDOS_CARD, 0x7B, 0x80, CHEST_ANIM_LONG),
    // GI_OCARINA_FAIRY
    GET_ITEM(ITEM_OCARINA_FAIRY, OBJECT_GI_OCARINA_0, GID_OCARINA_FAIRY, 0x3A, 0x80, CHEST_ANIM_LONG),
    // GI_DEKU_SEEDS_5
    GET_ITEM(ITEM_DEKU_SEEDS, OBJECT_GI_SEED, GID_DEKU_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT),
    // GI_HEART_CONTAINER
    GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG),
    // GI_HEART_PIECE
    GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xC2, 0x80, CHEST_ANIM_LONG),
    // GI_BOSS_KEY
    GET_ITEM(ITEM_DUNGEON_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_BOSS_KEY, 0xC7, 0x80, CHEST_ANIM_LONG),
    // GI_COMPASS
    GET_ITEM(ITEM_DUNGEON_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, 0x67, 0x80, CHEST_ANIM_LONG),
    // GI_DUNGEON_MAP
    GET_ITEM(ITEM_DUNGEON_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, 0x66, 0x80, CHEST_ANIM_LONG),
    // GI_SMALL_KEY
    GET_ITEM(ITEM_SMALL_KEY, OBJECT_GI_KEY, GID_SMALL_KEY, 0x60, 0x80, CHEST_ANIM_SHORT),
    // GI_MAGIC_JAR_SMALL
    GET_ITEM(ITEM_MAGIC_JAR_SMALL, OBJECT_GI_MAGICPOT, GID_MAGIC_JAR_SMALL, 0x52, 0x6F, CHEST_ANIM_SHORT),
    // GI_MAGIC_JAR_LARGE
    GET_ITEM(ITEM_MAGIC_JAR_BIG, OBJECT_GI_MAGICPOT, GID_MAGIC_JAR_LARGE, 0x52, 0x6E, CHEST_ANIM_SHORT),
    // GI_WALLET_ADULT
    GET_ITEM(ITEM_ADULTS_WALLET, OBJECT_GI_PURSE, GID_WALLET_ADULT, 0x5E, 0x80, CHEST_ANIM_LONG),
    // GI_WALLET_GIANT
    GET_ITEM(ITEM_GIANTS_WALLET, OBJECT_GI_PURSE, GID_WALLET_GIANT, 0x5F, 0x80, CHEST_ANIM_LONG),
    // GI_WEIRD_EGG
    GET_ITEM(ITEM_WEIRD_EGG, OBJECT_GI_EGG, GID_EGG, 0x9A, 0x80, CHEST_ANIM_LONG),
    // GI_RECOVERY_HEART
    GET_ITEM(ITEM_RECOVERY_HEART, OBJECT_GI_HEART, GID_RECOVERY_HEART, 0x55, 0x80, CHEST_ANIM_LONG),
    // GI_ARROWS_5
    GET_ITEM(ITEM_ARROWS_5, OBJECT_GI_ARROW, GID_ARROWS_5, 0xE6, 0x48, CHEST_ANIM_SHORT),
    // GI_ARROWS_10
    GET_ITEM(ITEM_ARROWS_10, OBJECT_GI_ARROW, GID_ARROWS_10, 0xE6, 0x49, CHEST_ANIM_SHORT),
    // GI_ARROWS_30
    GET_ITEM(ITEM_ARROWS_30, OBJECT_GI_ARROW, GID_ARROWS_30, 0xE6, 0x4A, CHEST_ANIM_SHORT),
    // GI_RUPEE_GREEN
    GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0x6F, 0x00, CHEST_ANIM_SHORT),
    // GI_RUPEE_BLUE
    GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xCC, 0x01, CHEST_ANIM_SHORT),
    // GI_RUPEE_RED
    GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF0, 0x02, CHEST_ANIM_SHORT),
    // GI_HEART_CONTAINER_2
    GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG),
    // GI_MILK
    GET_ITEM(ITEM_MILK, OBJECT_GI_MILK, GID_BOTTLE_MILK_FULL, 0x98, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_GORON
    GET_ITEM(ITEM_MASK_GORON, OBJECT_GI_GOLONMASK, GID_MASK_GORON, 0x14, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_ZORA
    GET_ITEM(ITEM_MASK_ZORA, OBJECT_GI_ZORAMASK, GID_MASK_ZORA, 0x15, 0x80, CHEST_ANIM_LONG),
    // GI_MASK_GERUDO
    GET_ITEM(ITEM_MASK_GERUDO, OBJECT_GI_GERUDOMASK, GID_MASK_GERUDO, 0x16, 0x80, CHEST_ANIM_LONG),
    // GI_GORONS_BRACELET
    GET_ITEM(ITEM_STRENGTH_GORONS_BRACELET, OBJECT_GI_BRACELET, GID_GORONS_BRACELET, 0x79, 0x80, CHEST_ANIM_LONG),
    // GI_RUPEE_PURPLE
    GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF1, 0x14, CHEST_ANIM_SHORT),
    // GI_RUPEE_GOLD
    GET_ITEM(ITEM_RUPEE_GOLD, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0xF2, 0x13, CHEST_ANIM_SHORT),
    // GI_SWORD_BIGGORON
    GET_ITEM(ITEM_SWORD_BIGGORON, OBJECT_GI_LONGSWORD, GID_SWORD_BIGGORON, 0x0C, 0x80, CHEST_ANIM_LONG),
    // GI_ARROW_FIRE
    GET_ITEM(ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x70, 0x80, CHEST_ANIM_LONG),
    // GI_ARROW_ICE
    GET_ITEM(ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x71, 0x80, CHEST_ANIM_LONG),
    // GI_ARROW_LIGHT
    GET_ITEM(ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW, GID_ARROW_LIGHT, 0x72, 0x80, CHEST_ANIM_LONG),
    // GI_SKULL_TOKEN
    GET_ITEM(ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0xB4, 0x80, CHEST_ANIM_SHORT),
    // GI_DINS_FIRE
    GET_ITEM(ITEM_DINS_FIRE, OBJECT_GI_GODDESS, GID_DINS_FIRE, 0xAD, 0x80, CHEST_ANIM_LONG),
    // GI_FARORES_WIND
    GET_ITEM(ITEM_FARORES_WIND, OBJECT_GI_GODDESS, GID_FARORES_WIND, 0xAE, 0x80, CHEST_ANIM_LONG),
    // GI_NAYRUS_LOVE
    GET_ITEM(ITEM_NAYRUS_LOVE, OBJECT_GI_GODDESS, GID_NAYRUS_LOVE, 0xAF, 0x80, CHEST_ANIM_LONG),
    // GI_BULLET_BAG_30
    GET_ITEM(ITEM_BULLET_BAG_30, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG),
    // GI_BULLET_BAG_40
    GET_ITEM(ITEM_BULLET_BAG_40, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG),
    // GI_DEKU_STICKS_5
    GET_ITEM(ITEM_DEKU_STICKS_5, OBJECT_GI_STICK, GID_DEKU_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    // GI_DEKU_STICKS_10
    GET_ITEM(ITEM_DEKU_STICKS_10, OBJECT_GI_STICK, GID_DEKU_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT),
    // GI_DEKU_NUTS_5_2
    GET_ITEM(ITEM_DEKU_NUTS_5, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    // GI_DEKU_NUTS_10
    GET_ITEM(ITEM_DEKU_NUTS_10, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT),
    // GI_BOMBS_1
    GET_ITEM(ITEM_BOMB, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    // GI_BOMBS_10
    GET_ITEM(ITEM_BOMBS_10, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    // GI_BOMBS_20
    GET_ITEM(ITEM_BOMBS_20, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    // GI_BOMBS_30
    GET_ITEM(ITEM_BOMBS_30, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT),
    // GI_DEKU_SEEDS_30
    GET_ITEM(ITEM_DEKU_SEEDS_30, OBJECT_GI_SEED, GID_DEKU_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_5
    GET_ITEM(ITEM_BOMBCHUS_5, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    // GI_BOMBCHUS_20
    GET_ITEM(ITEM_BOMBCHUS_20, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT),
    // GI_BOTTLE_FISH
    GET_ITEM(ITEM_BOTTLE_FISH, OBJECT_GI_FISH, GID_FISH, 0x47, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_BUGS
    GET_ITEM(ITEM_BOTTLE_BUG, OBJECT_GI_INSECT, GID_BUG, 0x7A, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_BLUE_FIRE
    GET_ITEM(ITEM_BOTTLE_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, 0x5D, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_POE
    GET_ITEM(ITEM_BOTTLE_POE, OBJECT_GI_GHOST, GID_POE, 0x97, 0x80, CHEST_ANIM_LONG),
    // GI_BOTTLE_BIG_POE
    GET_ITEM(ITEM_BOTTLE_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, 0xF9, 0x80, CHEST_ANIM_LONG),
    // GI_DOOR_KEY
    GET_ITEM(ITEM_SMALL_KEY, OBJECT_GI_KEY, GID_SMALL_KEY, 0xF3, 0x80, CHEST_ANIM_SHORT),
    // GI_RUPEE_GREEN_LOSE
    GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0xF4, 0x00, CHEST_ANIM_SHORT),
    // GI_RUPEE_BLUE_LOSE
    GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xF5, 0x01, CHEST_ANIM_SHORT),
    // GI_RUPEE_RED_LOSE
    GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF6, 0x02, CHEST_ANIM_SHORT),
    // GI_RUPEE_PURPLE_LOSE
    GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF7, 0x14, CHEST_ANIM_SHORT),
    // GI_HEART_PIECE_WIN
    GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xFA, 0x80, CHEST_ANIM_LONG),
    // GI_DEKU_STICK_UPGRADE_20
    GET_ITEM(ITEM_DEKU_STICK_UPGRADE_20, OBJECT_GI_STICK, GID_DEKU_STICK, 0x90, 0x80, CHEST_ANIM_SHORT),
    // GI_DEKU_STICK_UPGRADE_30
    GET_ITEM(ITEM_DEKU_STICK_UPGRADE_30, OBJECT_GI_STICK, GID_DEKU_STICK, 0x91, 0x80, CHEST_ANIM_SHORT),
    // GI_DEKU_NUT_UPGRADE_30
    GET_ITEM(ITEM_DEKU_NUT_UPGRADE_30, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0xA7, 0x80, CHEST_ANIM_SHORT),
    // GI_DEKU_NUT_UPGRADE_40
    GET_ITEM(ITEM_DEKU_NUT_UPGRADE_40, OBJECT_GI_NUTS, GID_DEKU_NUTS, 0xA8, 0x80, CHEST_ANIM_SHORT),
    // GI_BULLET_BAG_50
    GET_ITEM(ITEM_BULLET_BAG_50, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG_50, 0x6C, 0x80, CHEST_ANIM_LONG),
    // GI_ICE_TRAP
    GET_ITEM_NONE,
    // GI_TEXT_0
    GET_ITEM_NONE,
};

#define GET_PLAYER_ANIM(group, type) D_80853914[group * PLAYER_ANIMTYPE_MAX + type]

static PlayerAnimationHeader* D_80853914[PLAYER_ANIMGROUP_MAX * PLAYER_ANIMTYPE_MAX] = {
    /* PLAYER_ANIMGROUP_wait */
    &gPlayerAnim_link_normal_wait_free,
    &gPlayerAnim_link_normal_wait,
    &gPlayerAnim_link_normal_wait,
    &gPlayerAnim_link_fighter_wait_long,
    &gPlayerAnim_link_normal_wait_free,
    &gPlayerAnim_link_normal_wait_free,
    /* PLAYER_ANIMGROUP_walk */
    &gPlayerAnim_link_normal_walk_free,
    &gPlayerAnim_link_normal_walk,
    &gPlayerAnim_link_normal_walk,
    &gPlayerAnim_link_fighter_walk_long,
    &gPlayerAnim_link_normal_walk_free,
    &gPlayerAnim_link_normal_walk_free,
    /* PLAYER_ANIMGROUP_run */
    &gPlayerAnim_link_normal_run_free,
    &gPlayerAnim_link_fighter_run,
    &gPlayerAnim_link_normal_run,
    &gPlayerAnim_link_fighter_run_long,
    &gPlayerAnim_link_normal_run_free,
    &gPlayerAnim_link_normal_run_free,
    /* PLAYER_ANIMGROUP_FLINCH */
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_fighter_damage_run,
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_fighter_damage_run_long,
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_normal_damage_run_free,
    /* PLAYER_ANIMGROUP_heavy_run */
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_normal_heavy_run,
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_fighter_heavy_run_long,
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_normal_heavy_run_free,
    /* PLAYER_ANIMGROUP_waitL */
    &gPlayerAnim_link_normal_waitL_free,
    &gPlayerAnim_link_anchor_waitL,
    &gPlayerAnim_link_anchor_waitL,
    &gPlayerAnim_link_fighter_waitL_long,
    &gPlayerAnim_link_normal_waitL_free,
    &gPlayerAnim_link_normal_waitL_free,
    /* PLAYER_ANIMGROUP_waitR */
    &gPlayerAnim_link_normal_waitR_free,
    &gPlayerAnim_link_anchor_waitR,
    &gPlayerAnim_link_anchor_waitR,
    &gPlayerAnim_link_fighter_waitR_long,
    &gPlayerAnim_link_normal_waitR_free,
    &gPlayerAnim_link_normal_waitR_free,
    /* PLAYER_ANIMGROUP_wait2waitR */
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_normal_wait2waitR,
    &gPlayerAnim_link_normal_wait2waitR,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    /* PLAYER_ANIMGROUP_normal2fighter */
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_fighter_normal2fighter,
    &gPlayerAnim_link_fighter_normal2fighter,
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_normal_normal2fighter_free,
    /* PLAYER_ANIMGROUP_doorA_free */
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link,
    &gPlayerAnim_link_demo_doorA_link,
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link_free,
    /* PLAYER_ANIMGROUP_doorA */
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    /* PLAYER_ANIMGROUP_doorB_free */
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link,
    &gPlayerAnim_link_demo_doorB_link,
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link_free,
    /* PLAYER_ANIMGROUP_doorB */
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    /* PLAYER_ANIMGROUP_carryB */
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB,
    &gPlayerAnim_link_normal_carryB,
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB_free,
    /* PLAYER_ANIMGROUP_landing */
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing,
    &gPlayerAnim_link_normal_landing,
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing_free,
    /* PLAYER_ANIMGROUP_short_landing */
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing,
    &gPlayerAnim_link_normal_short_landing,
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing_free,
    /* PLAYER_ANIMGROUP_landing_roll */
    &gPlayerAnim_link_normal_landing_roll_free,
    &gPlayerAnim_link_normal_landing_roll,
    &gPlayerAnim_link_normal_landing_roll,
    &gPlayerAnim_link_fighter_landing_roll_long,
    &gPlayerAnim_link_normal_landing_roll_free,
    &gPlayerAnim_link_normal_landing_roll_free,
    /* PLAYER_ANIMGROUP_hip_down */
    &gPlayerAnim_link_normal_hip_down_free,
    &gPlayerAnim_link_normal_hip_down,
    &gPlayerAnim_link_normal_hip_down,
    &gPlayerAnim_link_normal_hip_down_long,
    &gPlayerAnim_link_normal_hip_down_free,
    &gPlayerAnim_link_normal_hip_down_free,
    /* PLAYER_ANIMGROUP_walk_endL */
    &gPlayerAnim_link_normal_walk_endL_free,
    &gPlayerAnim_link_normal_walk_endL,
    &gPlayerAnim_link_normal_walk_endL,
    &gPlayerAnim_link_fighter_walk_endL_long,
    &gPlayerAnim_link_normal_walk_endL_free,
    &gPlayerAnim_link_normal_walk_endL_free,
    /* PLAYER_ANIMGROUP_walk_endR */
    &gPlayerAnim_link_normal_walk_endR_free,
    &gPlayerAnim_link_normal_walk_endR,
    &gPlayerAnim_link_normal_walk_endR,
    &gPlayerAnim_link_fighter_walk_endR_long,
    &gPlayerAnim_link_normal_walk_endR_free,
    &gPlayerAnim_link_normal_walk_endR_free,
    /* PLAYER_ANIMGROUP_defense */
    &gPlayerAnim_link_normal_defense_free,
    &gPlayerAnim_link_normal_defense,
    &gPlayerAnim_link_normal_defense,
    &gPlayerAnim_link_normal_defense_free,
    &gPlayerAnim_link_bow_defense,
    &gPlayerAnim_link_normal_defense_free,
    /* PLAYER_ANIMGROUP_defense_wait */
    &gPlayerAnim_link_normal_defense_wait_free,
    &gPlayerAnim_link_normal_defense_wait,
    &gPlayerAnim_link_normal_defense_wait,
    &gPlayerAnim_link_normal_defense_wait_free,
    &gPlayerAnim_link_bow_defense_wait,
    &gPlayerAnim_link_normal_defense_wait_free,
    /* PLAYER_ANIMGROUP_defense_end */
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end,
    &gPlayerAnim_link_normal_defense_end,
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end_free,
    /* PLAYER_ANIMGROUP_side_walk */
    &gPlayerAnim_link_normal_side_walk_free,
    &gPlayerAnim_link_normal_side_walk,
    &gPlayerAnim_link_normal_side_walk,
    &gPlayerAnim_link_fighter_side_walk_long,
    &gPlayerAnim_link_normal_side_walk_free,
    &gPlayerAnim_link_normal_side_walk_free,
    /* PLAYER_ANIMGROUP_side_walkL */
    &gPlayerAnim_link_normal_side_walkL_free,
    &gPlayerAnim_link_anchor_side_walkL,
    &gPlayerAnim_link_anchor_side_walkL,
    &gPlayerAnim_link_fighter_side_walkL_long,
    &gPlayerAnim_link_normal_side_walkL_free,
    &gPlayerAnim_link_normal_side_walkL_free,
    /* PLAYER_ANIMGROUP_side_walkR */
    &gPlayerAnim_link_normal_side_walkR_free,
    &gPlayerAnim_link_anchor_side_walkR,
    &gPlayerAnim_link_anchor_side_walkR,
    &gPlayerAnim_link_fighter_side_walkR_long,
    &gPlayerAnim_link_normal_side_walkR_free,
    &gPlayerAnim_link_normal_side_walkR_free,
    /* PLAYER_ANIMGROUP_45_turn */
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn,
    &gPlayerAnim_link_normal_45_turn,
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn_free,
    /* PLAYER_ANIMGROUP_waitL2wait */
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_normal_waitL2wait,
    &gPlayerAnim_link_normal_waitL2wait,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    /* PLAYER_ANIMGROUP_waitR2wait */
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_normal_waitR2wait,
    &gPlayerAnim_link_normal_waitR2wait,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    /* PLAYER_ANIMGROUP_throw */
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw,
    &gPlayerAnim_link_normal_throw,
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw_free,
    /* PLAYER_ANIMGROUP_put */
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put,
    &gPlayerAnim_link_normal_put,
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put_free,
    /* PLAYER_ANIMGROUP_back_walk */
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    /* PLAYER_ANIMGROUP_check */
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check,
    &gPlayerAnim_link_normal_check,
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check_free,
    /* PLAYER_ANIMGROUP_check_wait */
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait,
    &gPlayerAnim_link_normal_check_wait,
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait_free,
    /* PLAYER_ANIMGROUP_check_end */
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end,
    &gPlayerAnim_link_normal_check_end,
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end_free,
    /* PLAYER_ANIMGROUP_pull_start */
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start,
    &gPlayerAnim_link_normal_pull_start,
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start_free,
    /* PLAYER_ANIMGROUP_pulling */
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling,
    &gPlayerAnim_link_normal_pulling,
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling_free,
    /* PLAYER_ANIMGROUP_pull_end */
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end,
    &gPlayerAnim_link_normal_pull_end,
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end_free,
    /* PLAYER_ANIMGROUP_fall_up */
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up,
    &gPlayerAnim_link_normal_fall_up,
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up_free,
    /* PLAYER_ANIMGROUP_jump_climb_hold */
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold,
    &gPlayerAnim_link_normal_jump_climb_hold,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    /* PLAYER_ANIMGROUP_jump_climb_wait */
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait,
    &gPlayerAnim_link_normal_jump_climb_wait,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    /* PLAYER_ANIMGROUP_jump_climb_up */
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up,
    &gPlayerAnim_link_normal_jump_climb_up,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    /* PLAYER_ANIMGROUP_down_slope_slip_end */
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    &gPlayerAnim_link_normal_down_slope_slip_end,
    &gPlayerAnim_link_normal_down_slope_slip_end,
    &gPlayerAnim_link_normal_down_slope_slip_end_long,
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    /* PLAYER_ANIMGROUP_up_slope_slip_end */
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    &gPlayerAnim_link_normal_up_slope_slip_end,
    &gPlayerAnim_link_normal_up_slope_slip_end,
    &gPlayerAnim_link_normal_up_slope_slip_end_long,
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    /* PLAYER_ANIMGROUP_nwait */
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_lkt_nwait,
    &gPlayerAnim_lkt_nwait,
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_sude_nwait,
};

static PlayerAnimationHeader* D_80853D4C[][3] = {
    { &gPlayerAnim_link_fighter_front_jump, &gPlayerAnim_link_fighter_front_jump_end,
      &gPlayerAnim_link_fighter_front_jump_endR },
    { &gPlayerAnim_link_fighter_Lside_jump, &gPlayerAnim_link_fighter_Lside_jump_end,
      &gPlayerAnim_link_fighter_Lside_jump_endL },
    { &gPlayerAnim_link_fighter_backturn_jump, &gPlayerAnim_link_fighter_backturn_jump_end,
      &gPlayerAnim_link_fighter_backturn_jump_endR },
    { &gPlayerAnim_link_fighter_Rside_jump, &gPlayerAnim_link_fighter_Rside_jump_end,
      &gPlayerAnim_link_fighter_Rside_jump_endR },
};

static PlayerAnimationHeader* D_80853D7C[][2] = {
    { &gPlayerAnim_link_normal_wait_typeA_20f, &gPlayerAnim_link_normal_waitF_typeA_20f },
    { &gPlayerAnim_link_normal_wait_typeC_20f, &gPlayerAnim_link_normal_waitF_typeC_20f },
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },
    { &gPlayerAnim_link_wait_heat1_20f, &gPlayerAnim_link_waitF_heat1_20f },
    { &gPlayerAnim_link_wait_heat2_20f, &gPlayerAnim_link_waitF_heat2_20f },
    { &gPlayerAnim_link_wait_itemD1_20f, &gPlayerAnim_link_wait_itemD1_20f },
    { &gPlayerAnim_link_wait_itemA_20f, &gPlayerAnim_link_waitF_itemA_20f },
    { &gPlayerAnim_link_wait_itemB_20f, &gPlayerAnim_link_waitF_itemB_20f },
    { &gPlayerAnim_link_wait_itemC_20f, &gPlayerAnim_link_wait_itemC_20f },
    { &gPlayerAnim_link_wait_itemD2_20f, &gPlayerAnim_link_wait_itemD2_20f }
};

static AnimSfxEntry D_80853DEC[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 8, NA_SE_VO_LI_SNEEZE, STOP),
};

static AnimSfxEntry D_80853DF0[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 18, NA_SE_VO_LI_SWEAT, STOP),
};

static AnimSfxEntry D_80853DF4[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 13, NA_SE_VO_LI_BREATH_REST, STOP),
};

static AnimSfxEntry D_80853DF8[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 10, NA_SE_VO_LI_BREATH_REST, STOP),
};

static AnimSfxEntry D_80853DFC[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 44, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 48, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 52, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 56, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 60, NA_SE_PL_CALM_HIT, STOP),
};

static AnimSfxEntry D_80853E10[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 25, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 30, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 44, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 48, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 52, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 56, NA_SE_NONE, STOP),
};

static AnimSfxEntry D_80853E28[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 16, NA_SE_IT_SHIELD_POSTURE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_IT_SHIELD_POSTURE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 70, NA_SE_IT_SHIELD_POSTURE, STOP),
};

static AnimSfxEntry D_80853E34[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 10, NA_SE_IT_HAMMER_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 10, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 22, NA_SE_IT_SWORD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 22, NA_SE_VO_LI_SWORD_N, STOP),
};

static AnimSfxEntry D_80853E44[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 39, NA_SE_IT_SWORD_SWING, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 39, NA_SE_VO_LI_SWORD_N, STOP),
};

static AnimSfxEntry D_80853E4C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 20, NA_SE_VO_LI_RELAX, STOP),
};

static AnimSfxEntry* D_80853E50[] = {
    D_80853DEC, D_80853DF0, D_80853DF4, D_80853DF8, D_80853DFC, D_80853E10,
    D_80853E28, D_80853E34, D_80853E44, D_80853E4C, NULL,
};

static u8 D_80853E7C[] = {
    0, 0, 1, 1, 2, 2, 2, 2, 10, 10, 10, 10, 10, 10, 3, 3, 4, 4, 8, 8, 5, 5, 6, 6, 7, 7, 9, 9, 0,
};

// Used to map item IDs to item actions
static s8 sItemActions[] = {
    PLAYER_IA_DEKU_STICK,          // ITEM_DEKU_STICK
    PLAYER_IA_DEKU_NUT,            // ITEM_DEKU_NUT
    PLAYER_IA_BOMB,                // ITEM_BOMB
    PLAYER_IA_BOW,                 // ITEM_BOW
    PLAYER_IA_BOW_FIRE,            // ITEM_ARROW_FIRE
    PLAYER_IA_DINS_FIRE,           // ITEM_DINS_FIRE
    PLAYER_IA_SLINGSHOT,           // ITEM_SLINGSHOT
    PLAYER_IA_OCARINA_FAIRY,       // ITEM_OCARINA_FAIRY
    PLAYER_IA_OCARINA_OF_TIME,     // ITEM_OCARINA_OF_TIME
    PLAYER_IA_BOMBCHU,             // ITEM_BOMBCHU
    PLAYER_IA_HOOKSHOT,            // ITEM_HOOKSHOT
    PLAYER_IA_LONGSHOT,            // ITEM_LONGSHOT
    PLAYER_IA_BOW_ICE,             // ITEM_ARROW_ICE
    PLAYER_IA_FARORES_WIND,        // ITEM_FARORES_WIND
    PLAYER_IA_BOOMERANG,           // ITEM_BOOMERANG
    PLAYER_IA_LENS_OF_TRUTH,       // ITEM_LENS_OF_TRUTH
    PLAYER_IA_MAGIC_BEAN,          // ITEM_MAGIC_BEAN
    PLAYER_IA_HAMMER,              // ITEM_HAMMER
    PLAYER_IA_BOW_LIGHT,           // ITEM_ARROW_LIGHT
    PLAYER_IA_NAYRUS_LOVE,         // ITEM_NAYRUS_LOVE
    PLAYER_IA_BOTTLE,              // ITEM_BOTTLE_EMPTY
    PLAYER_IA_BOTTLE_POTION_RED,   // ITEM_BOTTLE_POTION_RED
    PLAYER_IA_BOTTLE_POTION_GREEN, // ITEM_BOTTLE_POTION_GREEN
    PLAYER_IA_BOTTLE_POTION_BLUE,  // ITEM_BOTTLE_POTION_BLUE
    PLAYER_IA_BOTTLE_FAIRY,        // ITEM_BOTTLE_FAIRY
    PLAYER_IA_BOTTLE_FISH,         // ITEM_BOTTLE_FISH
    PLAYER_IA_BOTTLE_MILK_FULL,    // ITEM_BOTTLE_MILK_FULL
    PLAYER_IA_BOTTLE_RUTOS_LETTER, // ITEM_BOTTLE_RUTOS_LETTER
    PLAYER_IA_BOTTLE_FIRE,         // ITEM_BOTTLE_BLUE_FIRE
    PLAYER_IA_BOTTLE_BUG,          // ITEM_BOTTLE_BUG
    PLAYER_IA_BOTTLE_BIG_POE,      // ITEM_BOTTLE_BIG_POE
    PLAYER_IA_BOTTLE_MILK_HALF,    // ITEM_BOTTLE_MILK_HALF
    PLAYER_IA_BOTTLE_POE,          // ITEM_BOTTLE_POE
    PLAYER_IA_WEIRD_EGG,           // ITEM_WEIRD_EGG
    PLAYER_IA_CHICKEN,             // ITEM_CHICKEN
    PLAYER_IA_ZELDAS_LETTER,       // ITEM_ZELDAS_LETTER
    PLAYER_IA_MASK_KEATON,         // ITEM_MASK_KEATON
    PLAYER_IA_MASK_SKULL,          // ITEM_MASK_SKULL
    PLAYER_IA_MASK_SPOOKY,         // ITEM_MASK_SPOOKY
    PLAYER_IA_MASK_BUNNY_HOOD,     // ITEM_MASK_BUNNY_HOOD
    PLAYER_IA_MASK_GORON,          // ITEM_MASK_GORON
    PLAYER_IA_MASK_ZORA,           // ITEM_MASK_ZORA
    PLAYER_IA_MASK_GERUDO,         // ITEM_MASK_GERUDO
    PLAYER_IA_MASK_TRUTH,          // ITEM_MASK_TRUTH
    PLAYER_IA_SWORD_MASTER,        // ITEM_SOLD_OUT
    PLAYER_IA_POCKET_EGG,          // ITEM_POCKET_EGG
    PLAYER_IA_POCKET_CUCCO,        // ITEM_POCKET_CUCCO
    PLAYER_IA_COJIRO,              // ITEM_COJIRO
    PLAYER_IA_ODD_MUSHROOM,        // ITEM_ODD_MUSHROOM
    PLAYER_IA_ODD_POTION,          // ITEM_ODD_POTION
    PLAYER_IA_POACHERS_SAW,        // ITEM_POACHERS_SAW
    PLAYER_IA_BROKEN_GORONS_SWORD, // ITEM_BROKEN_GORONS_SWORD
    PLAYER_IA_PRESCRIPTION,        // ITEM_PRESCRIPTION
    PLAYER_IA_FROG,                // ITEM_EYEBALL_FROG
    PLAYER_IA_EYEDROPS,            // ITEM_EYE_DROPS
    PLAYER_IA_CLAIM_CHECK,         // ITEM_CLAIM_CHECK
    PLAYER_IA_BOW_FIRE,            // ITEM_BOW_FIRE
    PLAYER_IA_BOW_ICE,             // ITEM_BOW_ICE
    PLAYER_IA_BOW_LIGHT,           // ITEM_BOW_LIGHT
    PLAYER_IA_SWORD_KOKIRI,        // ITEM_SWORD_KOKIRI
    PLAYER_IA_SWORD_MASTER,        // ITEM_SWORD_MASTER
    PLAYER_IA_SWORD_BIGGORON,      // ITEM_SWORD_BIGGORON
};

static s32 (*sUpdateItemActionFuncs[])(Player* this, PlayState* play) = {
    Player_UpperAction_IdleDefault,    // PLAYER_IA_NONE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_LAST_USED
    Player_UpperAction_IdleDefault,    // PLAYER_IA_FISHING_POLE
    Player_UpperAction_IdleWithSword,  // PLAYER_IA_SWORD_MASTER
    Player_UpperAction_IdleWithSword,  // PLAYER_IA_SWORD_KOKIRI
    Player_UpperAction_IdleWithSword,  // PLAYER_IA_SWORD_BIGGORON
    Player_UpperAction_IdleDefault,    // PLAYER_IA_DEKU_STICK
    Player_UpperAction_IdleDefault,    // PLAYER_IA_HAMMER
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_FIRE
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_ICE
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_LIGHT
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_0C
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_0D
    Player_UpperAction_IA_Aim,         // PLAYER_IA_BOW_0E
    Player_UpperAction_IA_Aim,         // PLAYER_IA_SLINGSHOT
    Player_UpperAction_IA_Aim,         // PLAYER_IA_HOOKSHOT
    Player_UpperAction_IA_Aim,         // PLAYER_IA_LONGSHOT
    Player_UpperAction_CarryAboveHead, // PLAYER_IA_BOMB
    Player_UpperAction_CarryAboveHead, // PLAYER_IA_BOMBCHU
    Player_UpperAction_HoldBoomerang,  // PLAYER_IA_BOOMERANG
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MAGIC_SPELL_15
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MAGIC_SPELL_16
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MAGIC_SPELL_17
    Player_UpperAction_IdleDefault,    // PLAYER_IA_FARORES_WIND
    Player_UpperAction_IdleDefault,    // PLAYER_IA_NAYRUS_LOVE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_DINS_FIRE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_DEKU_NUT
    Player_UpperAction_IdleDefault,    // PLAYER_IA_OCARINA_FAIRY
    Player_UpperAction_IdleDefault,    // PLAYER_IA_OCARINA_OF_TIME
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_FISH
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_FIRE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_BUG
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_POE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_BIG_POE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_RUTOS_LETTER
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_POTION_RED
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_POTION_BLUE
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_POTION_GREEN
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_MILK_FULL
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_MILK_HALF
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BOTTLE_FAIRY
    Player_UpperAction_IdleDefault,    // PLAYER_IA_ZELDAS_LETTER
    Player_UpperAction_IdleDefault,    // PLAYER_IA_WEIRD_EGG
    Player_UpperAction_IdleDefault,    // PLAYER_IA_CHICKEN
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MAGIC_BEAN
    Player_UpperAction_IdleDefault,    // PLAYER_IA_POCKET_EGG
    Player_UpperAction_IdleDefault,    // PLAYER_IA_POCKET_CUCCO
    Player_UpperAction_IdleDefault,    // PLAYER_IA_COJIRO
    Player_UpperAction_IdleDefault,    // PLAYER_IA_ODD_MUSHROOM
    Player_UpperAction_IdleDefault,    // PLAYER_IA_ODD_POTION
    Player_UpperAction_IdleDefault,    // PLAYER_IA_POACHERS_SAW
    Player_UpperAction_IdleDefault,    // PLAYER_IA_BROKEN_GORONS_SWORD
    Player_UpperAction_IdleDefault,    // PLAYER_IA_PRESCRIPTION
    Player_UpperAction_IdleDefault,    // PLAYER_IA_FROG
    Player_UpperAction_IdleDefault,    // PLAYER_IA_EYEDROPS
    Player_UpperAction_IdleDefault,    // PLAYER_IA_CLAIM_CHECK
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_KEATON
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_SKULL
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_SPOOKY
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_BUNNY_HOOD
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_GORON
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_ZORA
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_GERUDO
    Player_UpperAction_IdleDefault,    // PLAYER_IA_MASK_TRUTH
    Player_UpperAction_IdleDefault,    // PLAYER_IA_LENS_OF_TRUTH
};

static void (*sInitItemActionFuncs[])(PlayState* play, Player* this) = {
    Player_InitItemAction_DoNothing, // PLAYER_IA_NONE
    Player_InitItemAction_DoNothing, // PLAYER_IA_LAST_USED
    Player_InitItemAction_DoNothing, // PLAYER_IA_FISHING_POLE
    Player_InitItemAction_DoNothing, // PLAYER_IA_SWORD_MASTER
    Player_InitItemAction_DoNothing, // PLAYER_IA_SWORD_KOKIRI
    Player_InitItemAction_DoNothing, // PLAYER_IA_SWORD_BIGGORON
    Player_InitItemAction_DekuStick, // PLAYER_IA_DEKU_STICK
    Player_InitItemAction_Hammer,    // PLAYER_IA_HAMMER
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_FIRE
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_ICE
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_LIGHT
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_0C
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_0D
    Player_InitItemAction_Aim,       // PLAYER_IA_BOW_0E
    Player_InitItemAction_Aim,       // PLAYER_IA_SLINGSHOT
    Player_InitItemAction_Hookshot,  // PLAYER_IA_HOOKSHOT
    Player_InitItemAction_Hookshot,  // PLAYER_IA_LONGSHOT
    Player_InitItemAction_Explosive, // PLAYER_IA_BOMB
    Player_InitItemAction_Explosive, // PLAYER_IA_BOMBCHU
    Player_InitItemAction_Boomerang, // PLAYER_IA_BOOMERANG
    Player_InitItemAction_DoNothing, // PLAYER_IA_MAGIC_SPELL_15
    Player_InitItemAction_DoNothing, // PLAYER_IA_MAGIC_SPELL_16
    Player_InitItemAction_DoNothing, // PLAYER_IA_MAGIC_SPELL_17
    Player_InitItemAction_DoNothing, // PLAYER_IA_FARORES_WIND
    Player_InitItemAction_DoNothing, // PLAYER_IA_NAYRUS_LOVE
    Player_InitItemAction_DoNothing, // PLAYER_IA_DINS_FIRE
    Player_InitItemAction_DoNothing, // PLAYER_IA_DEKU_NUT
    Player_InitItemAction_DoNothing, // PLAYER_IA_OCARINA_FAIRY
    Player_InitItemAction_DoNothing, // PLAYER_IA_OCARINA_OF_TIME
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_FISH
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_FIRE
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_BUG
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_POE
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_BIG_POE
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_RUTOS_LETTER
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_POTION_RED
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_POTION_BLUE
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_POTION_GREEN
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_MILK_FULL
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_MILK_HALF
    Player_InitItemAction_DoNothing, // PLAYER_IA_BOTTLE_FAIRY
    Player_InitItemAction_DoNothing, // PLAYER_IA_ZELDAS_LETTER
    Player_InitItemAction_DoNothing, // PLAYER_IA_WEIRD_EGG
    Player_InitItemAction_DoNothing, // PLAYER_IA_CHICKEN
    Player_InitItemAction_DoNothing, // PLAYER_IA_MAGIC_BEAN
    Player_InitItemAction_DoNothing, // PLAYER_IA_POCKET_EGG
    Player_InitItemAction_DoNothing, // PLAYER_IA_POCKET_CUCCO
    Player_InitItemAction_DoNothing, // PLAYER_IA_COJIRO
    Player_InitItemAction_DoNothing, // PLAYER_IA_ODD_MUSHROOM
    Player_InitItemAction_DoNothing, // PLAYER_IA_ODD_POTION
    Player_InitItemAction_DoNothing, // PLAYER_IA_POACHERS_SAW
    Player_InitItemAction_DoNothing, // PLAYER_IA_BROKEN_GORONS_SWORD
    Player_InitItemAction_DoNothing, // PLAYER_IA_PRESCRIPTION
    Player_InitItemAction_DoNothing, // PLAYER_IA_FROG
    Player_InitItemAction_DoNothing, // PLAYER_IA_EYEDROPS
    Player_InitItemAction_DoNothing, // PLAYER_IA_CLAIM_CHECK
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_KEATON
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_SKULL
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_SPOOKY
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_BUNNY_HOOD
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_GORON
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_ZORA
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_GERUDO
    Player_InitItemAction_DoNothing, // PLAYER_IA_MASK_TRUTH
    Player_InitItemAction_DoNothing, // PLAYER_IA_LENS_OF_TRUTH
};

typedef enum {
    /*  0 */ PLAYER_D_808540F4_0,
    /*  1 */ PLAYER_D_808540F4_1,
    /*  2 */ PLAYER_D_808540F4_2,
    /*  3 */ PLAYER_D_808540F4_3,
    /*  4 */ PLAYER_D_808540F4_4,
    /*  5 */ PLAYER_D_808540F4_5,
    /*  6 */ PLAYER_D_808540F4_6,
    /*  7 */ PLAYER_D_808540F4_7,
    /*  8 */ PLAYER_D_808540F4_8,
    /*  9 */ PLAYER_D_808540F4_9,
    /* 10 */ PLAYER_D_808540F4_10,
    /* 11 */ PLAYER_D_808540F4_11,
    /* 12 */ PLAYER_D_808540F4_12,
    /* 13 */ PLAYER_D_808540F4_13,
    /* 14 */ PLAYER_D_808540F4_MAX
} PlayerD_808540F4Index;

typedef struct {
    /* 0x00 */ PlayerAnimationHeader* anim;
    /* 0x04 */ u8 unk_04;
} struct_808540F4; // size = 0x08

static struct_808540F4 D_808540F4[PLAYER_D_808540F4_MAX] = {
    /* PLAYER_D_808540F4_0 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_D_808540F4_1 */ { &gPlayerAnim_link_normal_normal2fighter, 6 },
    /* PLAYER_D_808540F4_2 */ { &gPlayerAnim_link_hammer_normal2long, 8 },
    /* PLAYER_D_808540F4_3 */ { &gPlayerAnim_link_normal_normal2free, 8 },
    /* PLAYER_D_808540F4_4 */ { &gPlayerAnim_link_fighter_fighter2long, 8 },
    /* PLAYER_D_808540F4_5 */ { &gPlayerAnim_link_normal_fighter2free, 10 },
    /* PLAYER_D_808540F4_6 */ { &gPlayerAnim_link_hammer_long2free, 7 },
    /* PLAYER_D_808540F4_7 */ { &gPlayerAnim_link_hammer_long2long, 11 },
    /* PLAYER_D_808540F4_8 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_D_808540F4_9 */ { &gPlayerAnim_link_normal_normal2bom, 4 },
    /* PLAYER_D_808540F4_10 */ { &gPlayerAnim_link_normal_long2bom, 4 },
    /* PLAYER_D_808540F4_11 */ { &gPlayerAnim_link_normal_free2bom, 4 },
    /* PLAYER_D_808540F4_12 */ { &gPlayerAnim_link_anchor_anchor2fighter, 5 },
    /* PLAYER_D_808540F4_13 */ { &gPlayerAnim_link_normal_free2freeB, 13 },
};

static s8 D_80854164[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
    { PLAYER_D_808540F4_5, PLAYER_D_808540F4_0, -PLAYER_D_808540F4_1, PLAYER_D_808540F4_4, PLAYER_D_808540F4_5,
      PLAYER_D_808540F4_9 },
    { PLAYER_D_808540F4_3, PLAYER_D_808540F4_1, PLAYER_D_808540F4_0, PLAYER_D_808540F4_2, PLAYER_D_808540F4_3,
      PLAYER_D_808540F4_9 },
    { PLAYER_D_808540F4_6, -PLAYER_D_808540F4_4, -PLAYER_D_808540F4_2, PLAYER_D_808540F4_7, PLAYER_D_808540F4_6,
      PLAYER_D_808540F4_10 },
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
    { PLAYER_D_808540F4_8, -PLAYER_D_808540F4_5, -PLAYER_D_808540F4_3, -PLAYER_D_808540F4_6, PLAYER_D_808540F4_8,
      PLAYER_D_808540F4_11 },
};

typedef struct {
    /* 0x00 */ u8 itemId;
    /* 0x02 */ s16 actorId;
} ExplosiveInfo; // size = 0x04

static ExplosiveInfo sExplosiveInfos[] = {
    { ITEM_BOMB, ACTOR_EN_BOM },
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU },
};

typedef struct {
    /* 0x00 */ PlayerAnimationHeader* unk_00;
    /* 0x04 */ PlayerAnimationHeader* unk_04;
    /* 0x08 */ PlayerAnimationHeader* unk_08;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
} struct_80854190; // size = 0x10

static struct_80854190 D_80854190[PLAYER_MWA_MAX] = {
    /* PLAYER_MWA_FORWARD_SLASH_1H */
    { &gPlayerAnim_link_fighter_normal_kiru, &gPlayerAnim_link_fighter_normal_kiru_end,
      &gPlayerAnim_link_fighter_normal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_SLASH_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru, &gPlayerAnim_link_fighter_Lnormal_kiru_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_COMBO_1H */
    { &gPlayerAnim_link_fighter_normal_kiru_finsh, &gPlayerAnim_link_fighter_normal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_normal_kiru_finsh_endR, 0, 5 },
    /* PLAYER_MWA_FORWARD_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru_finsh, &gPlayerAnim_link_fighter_Lnormal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_finsh_endR, 1, 7 },
    /* PLAYER_MWA_RIGHT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru, &gPlayerAnim_link_fighter_Lside_kiru_end,
      &gPlayerAnim_link_anchor_Lside_kiru_endR, 1, 4 },
    /* PLAYER_MWA_RIGHT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru, &gPlayerAnim_link_fighter_LLside_kiru_end,
      &gPlayerAnim_link_anchor_LLside_kiru_endL, 0, 5 },
    /* PLAYER_MWA_RIGHT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru_finsh, &gPlayerAnim_link_fighter_Lside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lside_kiru_finsh_endR, 2, 8 },
    /* PLAYER_MWA_RIGHT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru_finsh, &gPlayerAnim_link_fighter_LLside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LLside_kiru_finsh_endR, 3, 8 },
    /* PLAYER_MWA_LEFT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru, &gPlayerAnim_link_fighter_Rside_kiru_end,
      &gPlayerAnim_link_anchor_Rside_kiru_endR, 0, 4 },
    /* PLAYER_MWA_LEFT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru, &gPlayerAnim_link_fighter_LRside_kiru_end,
      &gPlayerAnim_link_anchor_LRside_kiru_endR, 0, 5 },
    /* PLAYER_MWA_LEFT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru_finsh, &gPlayerAnim_link_fighter_Rside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Rside_kiru_finsh_endR, 0, 6 },
    /* PLAYER_MWA_LEFT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru_finsh, &gPlayerAnim_link_fighter_LRside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LRside_kiru_finsh_endL, 1, 5 },
    /* PLAYER_MWA_STAB_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru, &gPlayerAnim_link_fighter_pierce_kiru_end,
      &gPlayerAnim_link_anchor_pierce_kiru_endR, 0, 3 },
    /* PLAYER_MWA_STAB_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru, &gPlayerAnim_link_fighter_Lpierce_kiru_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_endL, 0, 3 },
    /* PLAYER_MWA_STAB_COMBO_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru_finsh, &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR, 1, 9 },
    /* PLAYER_MWA_STAB_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru_finsh, &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR, 1, 8 },
    /* PLAYER_MWA_FLIPSLASH_START */
    { &gPlayerAnim_link_fighter_jump_rollkiru, &gPlayerAnim_link_fighter_jump_kiru_finsh,
      &gPlayerAnim_link_fighter_jump_kiru_finsh, 1, 10 },
    /* PLAYER_MWA_JUMPSLASH_START */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru, &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, 1, 11 },
    /* PLAYER_MWA_FLIPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_jump_kiru_finsh, &gPlayerAnim_link_fighter_jump_kiru_finsh_end,
      &gPlayerAnim_link_fighter_jump_kiru_finsh_end, 1, 2 },
    /* PLAYER_MWA_JUMPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, &gPlayerAnim_link_fighter_Lpower_jump_kiru_end,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_end, 1, 2 },
    /* PLAYER_MWA_BACKSLASH_RIGHT */
    { &gPlayerAnim_link_fighter_turn_kiruR, &gPlayerAnim_link_fighter_turn_kiruR_end,
      &gPlayerAnim_link_fighter_turn_kiruR_end, 1, 5 },
    /* PLAYER_MWA_BACKSLASH_LEFT */
    { &gPlayerAnim_link_fighter_turn_kiruL, &gPlayerAnim_link_fighter_turn_kiruL_end,
      &gPlayerAnim_link_fighter_turn_kiruL_end, 1, 4 },
    /* PLAYER_MWA_HAMMER_FORWARD */
    { &gPlayerAnim_link_hammer_hit, &gPlayerAnim_link_hammer_hit_end, &gPlayerAnim_link_hammer_hit_endR, 3, 10 },
    /* PLAYER_MWA_HAMMER_SIDE */
    { &gPlayerAnim_link_hammer_side_hit, &gPlayerAnim_link_hammer_side_hit_end, &gPlayerAnim_link_hammer_side_hit_endR,
      2, 11 },
    /* PLAYER_MWA_SPIN_ATTACK_1H */
    { &gPlayerAnim_link_fighter_rolling_kiru, &gPlayerAnim_link_fighter_rolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 12 },
    /* PLAYER_MWA_SPIN_ATTACK_2H */
    { &gPlayerAnim_link_fighter_Lrolling_kiru, &gPlayerAnim_link_fighter_Lrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 15 },
    /* PLAYER_MWA_BIG_SPIN_1H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 16 },
    /* PLAYER_MWA_BIG_SPIN_2H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 16 },
};

static PlayerAnimationHeader* D_80854350[] = {
    &gPlayerAnim_link_fighter_power_kiru_start,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static PlayerAnimationHeader* D_80854358[] = {
    &gPlayerAnim_link_fighter_power_kiru_startL,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static PlayerAnimationHeader* sSpinAttackChargeAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait,
};

static PlayerAnimationHeader* D_80854368[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait_end,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait_end,
};

static PlayerAnimationHeader* sSpinAttackChargeWalkAnims[] = {
    &gPlayerAnim_link_fighter_power_kiru_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_walk,
};

static PlayerAnimationHeader* D_80854378[] = {
    &gPlayerAnim_link_fighter_power_kiru_side_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_side_walk,
};

static u8 D_80854380[2] = { PLAYER_MWA_SPIN_ATTACK_1H, PLAYER_MWA_SPIN_ATTACK_2H };
static u8 D_80854384[2] = { PLAYER_MWA_BIG_SPIN_1H, PLAYER_MWA_BIG_SPIN_2H };

static u16 sUseItemButtons[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT };

static u8 sMagicSpellCosts[] = { 12, 24, 24, 12, 24, 12 };

// return type can't be void due to regalloc in Player_TryDebugNoClip
BAD_RETURN(s32) Player_SetHorizontalSpeedToZero(Player* this) {
    this->actor.speed = 0.0f;
    this->speedXZ = 0.0f;
}

// return type can't be void due to regalloc in func_8083F72C
BAD_RETURN(s32) Player_ClearAttentionModeAndStopMoving(Player* this) {
    Player_SetHorizontalSpeedToZero(this);
    this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
}

s32 Player_IsTalking(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK_REQUESTED);
}

void Player_Anim_PlayOnce(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayOnce(play, &this->skelAnime, anim);
}

void Player_Anim_PlayLoop(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayLoop(play, &this->skelAnime, anim);
}

void Player_Anim_PlayLoopAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_Anim_PlayOnceAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_Anim_ResetModelRotY(Player* this) {
    this->actor.shape.rot.y += this->skelAnime.jointTable[LIMB_INDEX_MODEL_ROT].y;
    this->skelAnime.jointTable[LIMB_INDEX_MODEL_ROT].y = 0;
}

void Player_ResetAttack(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_RELEASING_SPIN_ATTACK;
    this->meleeWeaponState = 0;
    this->meleeWeaponInfo[0].active = this->meleeWeaponInfo[1].active = this->meleeWeaponInfo[2].active = 0;
}

void Player_ResetSubCam(PlayState* play, Player* this) {
    Camera* subCam;

    if (this->subCamId != CAM_ID_NONE) {
        subCam = play->cameraPtrs[this->subCamId];
        if ((subCam != NULL) && (subCam->csId == 1100)) {
            OnePointCutscene_EndCutscene(play, this->subCamId);
            this->subCamId = CAM_ID_NONE;
        }
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_DIVING | PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER);
}

void Player_DetatchHeldActor(PlayState* play, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !Player_IsHoldingHookshot(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
    }

    if (Player_GetExplosiveHeld(this) >= 0) {
        Player_ChangeItem(play, this, PLAYER_IA_NONE);
        this->heldItemId = ITEM_NONE_FE;
    }
}

void Player_ResetAttributes(PlayState* play, Player* this) {
    if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor == NULL)) {
        if (this->interactRangeActor != NULL) {
            if (this->getItemId == GI_NONE) {
                this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
                this->interactRangeActor = NULL;
            }
        } else {
            this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
        }
    }

    Player_ResetAttack(this);
    this->attentionMode = PLAYER_ATTENTIONMODE_NONE;

    Player_ResetSubCam(play, this);
    Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                           PLAYER_STATE1_IN_FIRST_PERSON_MODE | PLAYER_STATE1_CLIMBING);
    this->stateFlags2 &=
        ~(PLAYER_STATE2_MOVING_PUSH_PULL_WALL | PLAYER_STATE2_RESTRAINED_BY_ENEMY | PLAYER_STATE2_CRAWLING);

    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;

    this->slashCounter = this->unk_844 = 0;
}

s32 Player_UnequipItem(PlayState* play, Player* this) {
    if (this->heldItemAction > PLAYER_IA_LAST_USED) {
        Player_UseItem(play, this, ITEM_NONE);
        return true;
    }

    return false;
}

void Player_DetachHeldActorAndResetAttributes(PlayState* play, Player* this) {
    Player_ResetAttributes(play, this);
    Player_DetatchHeldActor(play, this);
}

s32 Player_MashTimerThresholdExceeded(Player* this, s32 timerStep, s32 timerThreshold) {
    s16 yawDiff = this->analogStickAngle - sAnalogStickAngle;

    this->actionVar16 += timerStep + (s16)(ABS(yawDiff) * fabsf(sAnalogStickDistance) * (9.9939f / (0x10000 * 60.0f)));

    if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B)) {
        this->actionVar16 += 5;
    }

    return this->actionVar16 > timerThreshold;
}

void Player_SetOneFrameFreezeFlash(PlayState* play) {
    if (play->actorCtx.freezeFlashTimer == 0) {
        play->actorCtx.freezeFlashTimer = 1;
    }
}

void Player_RequestRumble(Player* this, s32 sourceStrength, s32 duration, s32 decreaseRate, s32 distSq) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        Rumble_Request(distSq, sourceStrength, duration, decreaseRate);
    }
}

void Player_AnimSfx_PlayVoice(Player* this, u16 sfxId) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        Player_PlaySfx(this, sfxId + this->ageProperties->voiceSfxIdOffset);
    } else {
        func_800F4190(&this->actor.projectedPos, sfxId);
    }
}

void func_808326F0(Player* this) {
    u16* entry = &D_8085361C[0];
    s32 i;

    for (i = 0; i < 4; i++) {
        Audio_StopSfxById((u16)(*entry + this->ageProperties->voiceSfxIdOffset));
        entry++;
    }
}

u16 Player_GetFloorSfx(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset;
}

void Player_AnimSfx_PlayFloor(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_GetFloorSfx(this, sfxId));
}

u16 Player_GetFloorSfxByAge(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset + this->ageProperties->surfaceSfxIdOffset;
}

void Player_AnimSfx_PlayFloorByAge(Player* this, u16 sfxId) {
    Player_PlaySfx(this, Player_GetFloorSfxByAge(this, sfxId));
}

void Player_AnimSfx_PlayFloorWalk(Player* this, f32 freqVolumeLerp) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = Player_GetFloorSfxByAge(this, NA_SE_PL_WALK_GROUND);
    }

    func_800F4010(&this->actor.projectedPos, sfxId, freqVolumeLerp);
}

void Player_AnimSfx_PlayFloorJump(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_JUMP + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = Player_GetFloorSfxByAge(this, NA_SE_PL_JUMP);
    }

    Player_PlaySfx(this, sfxId);
}

void Player_AnimSfx_PlayFloorLand(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_LAND + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = Player_GetFloorSfxByAge(this, NA_SE_PL_LAND);
    }

    Player_PlaySfx(this, sfxId);
}

void Player_PlaySfx_Noticable(Player* this, u16 sfxId) {
    Player_PlaySfx(this, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_MAKING_NOTICABLE_SFX;
}

void Player_AnimSfx_Play(Player* this, AnimSfxEntry* entry) {
    s32 data;
    s32 type;
    u32 cond;
    s32 pad;

    do {
        data = ABS(entry->flags);
        type = ANIMSFX_GET_TYPE(data);

        if (PlayerAnimation_OnFrame(&this->skelAnime, fabsf(ANIMSFX_GET_FRAME(data)))) {
            if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_GENERAL)) {
                Player_PlaySfx(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR)) {
                Player_AnimSfx_PlayFloor(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_BY_AGE)) {
                Player_AnimSfx_PlayFloorByAge(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_VOICE)) {
                Player_AnimSfx_PlayVoice(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_LAND)) {
                Player_AnimSfx_PlayFloorLand(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_WALK_HIGH)) {
                Player_AnimSfx_PlayFloorWalk(this, 6.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_JUMP)) {
                Player_AnimSfx_PlayFloorJump(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_WALK_LOW)) {
                Player_AnimSfx_PlayFloorWalk(this, 0.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_WALK_WOOD)) {
                func_800F4010(&this->actor.projectedPos, NA_SE_PL_WALK_LADDER + this->ageProperties->surfaceSfxIdOffset,
                              0.0f);
            }
        }
        cond = (entry->flags >= 0);
        entry++;
    } while (cond);
}

void Player_Anim_PlayOnceWithMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -6.0f);
}

void Player_Anim_PlayOnceAdjustedWithMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -6.0f);
}

void Player_Anim_PlayLoopWithMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_Anim_PlayOnceForOneFrame(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void Player_Anim_PlayLoopWithLongMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 Player_Anim_PlayLoopOnceFinished(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, anim);
        return true;
    } else {
        return false;
    }
}

void Player_Anim_ResetTranslation(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevYaw = this->actor.shape.rot.y;
}

void Player_Anim_ResetTranslationAgeAdjusted(Player* this) {
    Player_Anim_ResetTranslation(this);
    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void Player_Anim_ResetModelYaw(Player* this) {
    this->skelAnime.jointTable[LIMB_INDEX_MODEL_ROT].y = 0;
}

void Player_Anim_ResetMove(Player* this) {
    if (this->skelAnime.moveFlags) {
        Player_Anim_ResetModelRotY(this);
        this->skelAnime.jointTable[LIMB_INDEX_MODEL_TRANSLATION].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[LIMB_INDEX_MODEL_TRANSLATION].z = this->skelAnime.baseTransl.z;

        if (this->skelAnime.moveFlags & ANIM_FLAG_8) {
            if (this->skelAnime.moveFlags & ANIM_FLAG_UPDATE_Y) {
                this->skelAnime.jointTable[LIMB_INDEX_MODEL_TRANSLATION].y = this->skelAnime.prevTransl.y;
            }
        } else {
            this->skelAnime.jointTable[LIMB_INDEX_MODEL_TRANSLATION].y = this->skelAnime.baseTransl.y;
        }
        Player_Anim_ResetTranslation(this);
        this->skelAnime.moveFlags = 0;
    }
}

/**
 * Only used for ledge climbing
 */
void Player_AnimReplace_SetupLedgeClimb(Player* this, s32 moveFlags) {
    Vec3f pos;

    this->skelAnime.moveFlags = moveFlags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    SkelAnime_UpdateTranslation(&this->skelAnime, &pos, this->actor.shape.rot.y);

    if (moveFlags & ANIM_FLAG_1) {
        if (!LINK_IS_ADULT) {
            pos.x *= 0.64f;
            pos.z *= 0.64f;
        }
        this->actor.world.pos.x += pos.x * this->actor.scale.x;
        this->actor.world.pos.z += pos.z * this->actor.scale.z;
    }

    if (moveFlags & ANIM_FLAG_UPDATE_Y) {
        if (!(moveFlags & ANIM_FLAG_4)) {
            pos.y *= this->ageProperties->unk_08;
        }
        this->actor.world.pos.y += pos.y * this->actor.scale.y;
    }

    Player_Anim_ResetModelRotY(this);
}

void Player_AnimReplace_Setup(PlayState* play, Player* this, s32 moveFlags) {
    if (moveFlags & ANIM_FLAG_200) {
        Player_Anim_ResetTranslationAgeAdjusted(this);
    } else if ((moveFlags & ANIM_FLAG_100) || (this->skelAnime.moveFlags != 0)) {
        Player_Anim_ResetTranslation(this);
    } else {
        this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
        this->skelAnime.prevYaw = this->actor.shape.rot.y;
    }

    this->skelAnime.moveFlags = moveFlags;
    Player_SetHorizontalSpeedToZero(this);
    AnimationContext_DisableQueue(play);
}

void Player_AnimReplace_PlayOnceSetSpeed(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags,
                                         f32 playbackSpeed) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_AnimReplace_Setup(play, this, moveFlags);
}

void Player_AnimReplace_PlayOnce(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags) {
    Player_AnimReplace_PlayOnceSetSpeed(play, this, anim, moveFlags, PLAYER_ANIM_NORMAL_SPEED);
}

void Player_AnimReplace_PlayOnceAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags) {
    Player_AnimReplace_PlayOnceSetSpeed(play, this, anim, moveFlags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplace_PlayOnceAdjustedUnkFlags(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_AnimReplace_PlayOnceAdjusted(play, this, anim, ANIM_FLAG_4 | ANIM_FLAG_8 | ANIM_FLAG_NOMOVE);
}

void Player_AnimReplace_PlayLoopSetSpeed(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags,
                                         f32 playbackSpeed) {
    PlayerAnimation_PlayLoopSetSpeed(play, &this->skelAnime, anim, playbackSpeed);
    Player_AnimReplace_Setup(play, this, moveFlags);
}

void Player_AnimReplace_PlayLoop(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags) {
    Player_AnimReplace_PlayLoopSetSpeed(play, this, anim, moveFlags, PLAYER_ANIM_NORMAL_SPEED);
}

void Player_AnimReplace_PlayLoopAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim, s32 moveFlags) {
    Player_AnimReplace_PlayLoopSetSpeed(play, this, anim, moveFlags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void Player_AnimReplace_PlayLoopAdjustedUnkFlags(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_AnimReplace_PlayLoopAdjusted(play, this, anim, 0x1C);
}

// Stores four consecutive frames of analog stick input data into two buffers, one offset by cam angle and the other not
void Player_StoreAnalogStickInput(PlayState* play, Player* this) {
    s8 analogStickDirection128Parts;
    s8 analogStickDirection4Parts;

    this->analogStickDistance = sAnalogStickDistance;
    this->analogStickAngle = sAnalogStickAngle;

    // Get analog stick dist and angle, stick dist ranges from -60.0f to 60.0f on each axis
    Lib_GetAnalogInput(&sAnalogStickDistance, &sAnalogStickAngle, sControlInput);

    // Loops from 0 to 3 over and over
    sCameraOffsetAnalogStickAngle = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play)) + sAnalogStickAngle;

    this->inputFrameCounter = (this->inputFrameCounter + 1) % 4;

    if (sAnalogStickDistance < 55.0f) {
        analogStickDirection4Parts = PLAYER_ANALOG_NONE;
        analogStickDirection128Parts = -1;
    } else {
        analogStickDirection128Parts = (u16)(sAnalogStickAngle + 0x2000) >> 9;
        analogStickDirection4Parts =
            (u16)((s16)(sCameraOffsetAnalogStickAngle - this->actor.shape.rot.y) + 0x2000) >> 14;
    }

    this->analogStickDirection128Parts[this->inputFrameCounter] = analogStickDirection128Parts;
    this->analogStickDirection4Parts[this->inputFrameCounter] = analogStickDirection4Parts;
}

void Player_Anim_PlayOnceWaterAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* linkAnim) {
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, linkAnim, sPlayerUnderwaterSpeedAdjustment);
}

s32 Player_IsFreeSwimming(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_SWIMMING) && (this->currentBoots != PLAYER_BOOTS_IRON);
}

s32 Player_IsUsingBoomerang(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_USING_BOOMERANG);
}

void func_808332F4(Player* this, PlayState* play) {
    GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

    this->unk_862 = ABS(giEntry->gi);
}

static PlayerAnimationHeader* Player_GetIdleAnim(Player* this) {
    return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType);
}

s32 func_80833350(Player* this) {
    PlayerAnimationHeader** entry;
    s32 i;

    if (Player_GetIdleAnim(this) != this->skelAnime.animation) {
        for (i = 0, entry = &D_80853D7C[0][0]; i < 28; i++, entry++) {
            if (this->skelAnime.animation == *entry) {
                return i + 1;
            }
        }
        return 0;
    }

    return -1;
}

void func_808333FC(Player* this, s32 arg1) {
    if (D_80853E7C[arg1] != 0) {
        Player_AnimSfx_Play(this, D_80853E50[D_80853E7C[arg1] - 1]);
    }
}

PlayerAnimationHeader* Player_GetRunAnim(Player* this) {
    if (this->unk_890 != 0) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_FLINCH, this->modelAnimType);
    } else if (!(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
               (this->currentBoots == PLAYER_BOOTS_IRON)) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_heavy_run, this->modelAnimType);
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType);
    }
}

s32 Player_IsAimingBoomerang(Player* this) {
    return Player_IsUsingBoomerang(this) && (this->firstPersonItemTimer != 0);
}

PlayerAnimationHeader* Player_GetWaitRightAnim(Player* this) {
    if (Player_IsAimingBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_waitR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitR, this->modelAnimType);
    }
}

PlayerAnimationHeader* Player_GetWaitLeftAnim(Player* this) {
    if (Player_IsAimingBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_waitL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitL, this->modelAnimType);
    }
}

PlayerAnimationHeader* Player_GetSidewalkSlowAnim(Player* this) {
    if (Player_IsAimingFpsItem(this)) {
        return &gPlayerAnim_link_bow_side_walk;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walk, this->modelAnimType);
    }
}

PlayerAnimationHeader* Player_GetAimBoomerangRightAnim(Player* this) {
    if (Player_IsAimingBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType);
    }
}

PlayerAnimationHeader* Player_GetAimBoomerangLeftAnim(Player* this) {
    if (Player_IsAimingBoomerang(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkL, this->modelAnimType);
    }
}

void Player_SetUpperAction(Player* this, PlayerUpperActionFunc upperActionFunc) {
    this->upperActionFunc = upperActionFunc;
    this->unk_836 = 0;
    this->upperInterpWeight = 0.0f;
    func_808326F0(this);
}

void Player_SetupChangeItemAnim(PlayState* play, Player* this, s8 itemAction) {
    PlayerAnimationHeader* current = this->skelAnime.animation;
    PlayerAnimationHeader** iter = D_80853914 + this->modelAnimType;
    u32 i;

    this->stateFlags1 &= ~(PLAYER_STATE1_USING_FPS_ITEM | PLAYER_STATE1_USING_BOOMERANG);

    for (i = 0; i < PLAYER_ANIMGROUP_MAX; i++) {
        if (current == *iter) {
            break;
        }
        iter += PLAYER_ANIMTYPE_MAX;
    }

    Player_ChangeItem(play, this, itemAction);

    if (i < PLAYER_ANIMGROUP_MAX) {
        this->skelAnime.animation = GET_PLAYER_ANIM(i, this->modelAnimType);
    }
}

s8 Player_ItemToItemAction(s32 itemId) {
    if (itemId >= ITEM_NONE_FE) {
        return PLAYER_IA_NONE;
    } else if (itemId == ITEM_LAST_USED) {
        return PLAYER_IA_LAST_USED;
    } else if (itemId == ITEM_FISHING_POLE) {
        return PLAYER_IA_FISHING_POLE;
    } else {
        return sItemActions[itemId];
    }
}

void Player_InitItemAction_DoNothing(PlayState* play, Player* this) {
}

void Player_InitItemAction_DekuStick(PlayState* play, Player* this) {
    this->dekuStickLength = 1.0f;
}

void Player_InitItemAction_Hammer(PlayState* play, Player* this) {
}

void Player_InitItemAction_Aim(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_FPS_ITEM;

    if (this->heldItemAction != PLAYER_IA_SLINGSHOT) {
        this->unk_860 = -1;
    } else {
        this->unk_860 = -2;
    }
}

void Player_InitItemAction_Explosive(PlayState* play, Player* this) {
    s32 explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* spawnedActor;

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        Player_UnequipItem(play, this);
        return;
    }

    explosiveType = Player_GetExplosiveHeld(this);
    explosiveInfo = &sExplosiveInfos[explosiveType];

    spawnedActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, explosiveInfo->actorId, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
    if (spawnedActor != NULL) {
        if ((explosiveType != 0) && (play->bombchuBowlingStatus != 0)) {
            play->bombchuBowlingStatus--;
            if (play->bombchuBowlingStatus == 0) {
                play->bombchuBowlingStatus = -1;
            }
        } else {
            Inventory_ChangeAmmo(explosiveInfo->itemId, -1);
        }

        this->interactRangeActor = spawnedActor;
        this->heldActor = spawnedActor;
        this->getItemId = GI_NONE;
        this->unk_3BC.y = spawnedActor->shape.rot.y - this->actor.shape.rot.y;
        this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
    }
}

void Player_InitItemAction_Hookshot(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_FPS_ITEM;
    this->unk_860 = -3;

    this->heldActor =
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
}

void Player_InitItemAction_Boomerang(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_BOOMERANG;
}

void Player_ChangeItem(PlayState* play, Player* this, s8 itemAction) {
    this->stickFlameTimer = 0;
    this->dekuStickLength = 0.0f;
    this->spinAttackTimer = 0.0f;

    this->heldItemAction = this->itemAction = itemAction;
    this->modelGroup = this->nextModelGroup;

    this->stateFlags1 &= ~(PLAYER_STATE1_USING_FPS_ITEM | PLAYER_STATE1_USING_BOOMERANG);

    sInitItemActionFuncs[itemAction](play, this);

    Player_SetModelGroup(this, this->modelGroup);
}

void func_80833A20(Player* this, s32 newMeleeWeaponState) {
    u16 itemSfx;
    u16 voiceSfx;

    if (this->meleeWeaponState == 0) {
        if ((this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) && (gSaveContext.swordHealth > 0.0f)) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else {
            itemSfx = NA_SE_IT_SWORD_SWING;
        }

        voiceSfx = NA_SE_VO_LI_SWORD_N;
        if (this->heldItemAction == PLAYER_IA_HAMMER) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else if (this->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            itemSfx = NA_SE_NONE;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        } else if (this->slashCounter >= 3) {
            itemSfx = NA_SE_IT_SWORD_SWING_HARD;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }

        if (itemSfx != NA_SE_NONE) {
            Player_PlaySfx_Noticable(this, itemSfx);
        }

        if (!((this->meleeWeaponAnimation >= PLAYER_MWA_FLIPSLASH_START) &&
              (this->meleeWeaponAnimation <= PLAYER_MWA_JUMPSLASH_FINISH))) {
            Player_AnimSfx_PlayVoice(this, voiceSfx);
        }
    }

    this->meleeWeaponState = newMeleeWeaponState;
}

s32 Player_IsZParallelOrLockOnFriend(Player* this) {
    if (this->stateFlags1 &
        (PLAYER_STATE1_LOCK_ON_FRIEND | PLAYER_STATE1_Z_PARALLEL | PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_TryEnemyLockOn(Player* this) {
    if ((this->lockOnActor != NULL) &&
        CHECK_FLAG_ALL(this->lockOnActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_ENEMY)) {
        this->stateFlags1 |= PLAYER_STATE1_LOCK_ON_ENEMY;
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_LOCK_ON_ENEMY) {
        this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_ENEMY;
        if (this->speedXZ == 0.0f) {
            this->yaw = this->actor.shape.rot.y;
        }
    }

    return false;
}

s32 Player_IsZTargeting(Player* this) {
    return Player_IsEnemyLockOn(this) || Player_IsZParallelOrLockOnFriend(this);
}

s32 Player_TryZTargeting(Player* this) {
    return Player_TryEnemyLockOn(this) || Player_IsZParallelOrLockOnFriend(this);
}

void Player_ResetLeftRightBlendWeight(Player* this) {
    this->leftRightBlendWeightTarget = 0.0f;
    this->leftRightBlendWeight = 0.0f;
}

s32 Player_IsItemValid(Player* this, s32 itemId) {
    if ((itemId < ITEM_NONE_FE) && (Player_ItemToItemAction(itemId) == this->itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_IsMaskValid(s32 item1, s32 itemAction) {
    if ((item1 < ITEM_NONE_FE) && (Player_ItemToItemAction(item1) == itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 Player_GetItemOnButton(PlayState* play, s32 index) {
    if (index >= 4) {
        return ITEM_NONE;
    } else if (play->bombchuBowlingStatus != 0) {
        return (play->bombchuBowlingStatus > 0) ? ITEM_BOMBCHU : ITEM_NONE;
    } else if (index == 0) {
        return B_BTN_ITEM;
    } else if (index == 1) {
        return C_BTN_ITEM(0);
    } else if (index == 2) {
        return C_BTN_ITEM(1);
    } else {
        return C_BTN_ITEM(2);
    }
}

void Player_SetupUseItem(Player* this, PlayState* play) {
    s32 maskItemAction;
    s32 itemId;
    s32 i;

    if (this->currentMask != PLAYER_MASK_NONE) {
        maskItemAction = this->currentMask - 1 + PLAYER_IA_MASK_KEATON;
        if (!Player_IsMaskValid(C_BTN_ITEM(0), maskItemAction) && !Player_IsMaskValid(C_BTN_ITEM(1), maskItemAction) &&
            !Player_IsMaskValid(C_BTN_ITEM(2), maskItemAction)) {
            this->currentMask = PLAYER_MASK_NONE;
        }
    }

    if (!(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE)) &&
        !Player_IsShootingHookshot(this)) {
        if (this->itemAction >= PLAYER_IA_FISHING_POLE) {
            if (!Player_IsItemValid(this, B_BTN_ITEM) && !Player_IsItemValid(this, C_BTN_ITEM(0)) &&
                !Player_IsItemValid(this, C_BTN_ITEM(1)) && !Player_IsItemValid(this, C_BTN_ITEM(2))) {
                Player_UseItem(play, this, ITEM_NONE);
                return;
            }
        }

        for (i = 0; i < ARRAY_COUNT(sUseItemButtons); i++) {
            if (CHECK_BTN_ALL(sControlInput->press.button, sUseItemButtons[i])) {
                break;
            }
        }

        itemId = Player_GetItemOnButton(play, i);
        if (itemId >= ITEM_NONE_FE) {
            for (i = 0; i < ARRAY_COUNT(sUseItemButtons); i++) {
                if (CHECK_BTN_ALL(sControlInput->cur.button, sUseItemButtons[i])) {
                    break;
                }
            }

            itemId = Player_GetItemOnButton(play, i);
            if ((itemId < ITEM_NONE_FE) && (Player_ItemToItemAction(itemId) == this->heldItemAction)) {
                D_80853618 = true;
            }
        } else {
            this->heldItemButton = i;
            Player_UseItem(play, this, itemId);
        }
    }
}

void Player_SetupUseItemAnim(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 phi_f2;
    f32 startFrame;
    f32 endFrame;
    f32 playSpeed;
    s32 sp38;
    s8 itemAction;
    s32 nextAnimType;

    itemAction = Player_ItemToItemAction(this->heldItemId);
    Player_SetUpperAction(this, Player_UpperAction_2);

    nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
    sp38 = D_80854164[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType];
    if ((itemAction == PLAYER_IA_BOTTLE) || (itemAction == PLAYER_IA_BOOMERANG) ||
        ((itemAction == PLAYER_IA_NONE) &&
         ((this->heldItemAction == PLAYER_IA_BOTTLE) || (this->heldItemAction == PLAYER_IA_BOOMERANG)))) {
        sp38 = (itemAction == PLAYER_IA_NONE) ? -PLAYER_D_808540F4_13 : PLAYER_D_808540F4_13;
    }

    this->unk_15A = ABS(sp38);

    anim = D_808540F4[this->unk_15A].anim;
    if ((anim == &gPlayerAnim_link_normal_fighter2free) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_link_normal_free2fighter_free;
    }

    phi_f2 = Animation_GetLastFrame(anim);
    endFrame = phi_f2;

    if (sp38 >= 0) {
        playSpeed = 1.2f;
        startFrame = 0.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.2f;
        startFrame = phi_f2;
    }

    if (itemAction != PLAYER_IA_NONE) {
        playSpeed *= 2.0f;
    }

    PlayerAnimation_Change(play, &this->upperSkelAnime, anim, playSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGE_ITEM;
}

void Player_TryUsingItem(Player* this, PlayState* play) {
    if ((this->actor.category == ACTORCAT_PLAYER) && !(this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) &&
        ((this->heldItemAction == this->itemAction) || (this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD)) &&
        (gSaveContext.health != 0) && (play->csCtx.state == CS_STATE_IDLE) && (this->csMode == PLAYER_CSMODE_NONE) &&
        (play->shootingGalleryStatus == 0) && (play->activeCamId == CAM_ID_MAIN) &&
        (play->transitionTrigger != TRANS_TRIGGER_START) && (gSaveContext.timerState != TIMER_STATE_STOP)) {
        Player_SetupUseItem(this, play);
    }

    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) {
        Player_SetupUseItemAnim(this, play);
    }
}

s32 Player_GetFpsItemAmmo(PlayState* play, Player* this, s32* itemPtr, s32* typePtr) {
    if (LINK_IS_ADULT) {
        *itemPtr = ITEM_BOW;
        if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
            *typePtr = ARROW_NORMAL_HORSE;
        } else {
            *typePtr = ARROW_NORMAL + (this->heldItemAction - PLAYER_IA_BOW);
        }
    } else {
        *itemPtr = ITEM_SLINGSHOT;
        *typePtr = ARROW_SEED;
    }

    if (gSaveContext.minigameState == 1) {
        return play->interfaceCtx.hbaAmmo;
    } else if (play->shootingGalleryStatus != 0) {
        return play->shootingGalleryStatus;
    } else {
        return AMMO(*itemPtr);
    }
}

static u16 sFpsItemReadySfxIds[] = { NA_SE_IT_BOW_DRAW, NA_SE_IT_SLING_DRAW, NA_SE_IT_HOOKSHOT_READY };

static u8 sMagicArrowCosts[] = { 4, 4, 8 };

s32 Player_TryReadyFpsItemToShoot(Player* this, PlayState* play) {
    s32 itemId;
    s32 arrowType;
    s32 magicArrowType;

    if ((this->heldItemAction >= PLAYER_IA_BOW_FIRE) && (this->heldItemAction <= PLAYER_IA_BOW_0E) &&
        (gSaveContext.magicState != MAGIC_STATE_IDLE)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
    } else {
        Player_SetUpperAction(this, Player_UpperAction_ReadyFpsItemToShoot);

        this->stateFlags1 |= PLAYER_STATE1_READY_TO_SHOOT;
        this->firstPersonItemTimer = 14;

        if (this->unk_860 >= 0) {
            Player_PlaySfx(this, sFpsItemReadySfxIds[ABS(this->unk_860) - 1]);

            if (!Player_IsHoldingHookshot(this) && (Player_GetFpsItemAmmo(play, this, &itemId, &arrowType) > 0)) {
                magicArrowType = arrowType - ARROW_FIRE;

                if (this->unk_860 >= 0) {
                    if ((magicArrowType >= 0) && (magicArrowType <= 2) &&
                        !Magic_RequestChange(play, sMagicArrowCosts[magicArrowType], MAGIC_CONSUME_NOW)) {
                        arrowType = ARROW_NORMAL;
                    }

                    this->heldActor = Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);
                }
            }
        }

        return true;
    }

    return false;
}

void Player_UseItemWithNoticableSfx(PlayState* play, Player* this) {
    if (this->heldItemAction != PLAYER_IA_NONE) {
        if (Player_SwordFromIA(this, this->heldItemAction) >= 0) {
            Player_PlaySfx_Noticable(this, NA_SE_IT_SWORD_PUTAWAY);
        } else {
            Player_PlaySfx_Noticable(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    Player_UseItem(play, this, this->heldItemId);

    if (Player_SwordFromIA(this, this->heldItemAction) >= 0) {
        Player_PlaySfx_Noticable(this, NA_SE_IT_SWORD_PICKOUT);
    } else if (this->heldItemAction != PLAYER_IA_NONE) {
        Player_PlaySfx_Noticable(this, NA_SE_PL_CHANGE_ARMS);
    }
}

void Player_SetupUpperActionForHeldItem(PlayState* play, Player* this) {
    if (Player_UpperAction_2 == this->upperActionFunc) {
        Player_UseItemWithNoticableSfx(play, this);
    }

    Player_SetUpperAction(this, sUpdateItemActionFuncs[this->heldItemAction]);
    this->firstPersonItemTimer = 0;
    this->unk_6AC = 0;
    Player_DetatchHeldActor(play, this);
    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGE_ITEM;
}

static PlayerAnimationHeader* sShieldStandingRightAnims[] = {
    &gPlayerAnim_link_anchor_waitR2defense,
    &gPlayerAnim_link_anchor_waitR2defense_long,
};

static PlayerAnimationHeader* sShieldStandingLeftAnims[] = {
    &gPlayerAnim_link_anchor_waitL2defense,
    &gPlayerAnim_link_anchor_waitL2defense_long,
};

static PlayerAnimationHeader* D_808543B4[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitL,
};

static PlayerAnimationHeader* D_808543BC[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitR,
};

static PlayerAnimationHeader* D_808543C4[] = {
    &gPlayerAnim_link_normal_defense_hit,
    &gPlayerAnim_link_fighter_defense_long_hit,
};

PlayerAnimationHeader* Player_SetupShieldStanding(PlayState* play, Player* this) {
    Player_SetUpperAction(this, Player_UpperAction_ShieldStanding);
    Player_DetatchHeldActor(play, this);

    if (this->leftRightBlendWeight < 0.5f) {
        return sShieldStandingRightAnims[Player_IsHoldingTwoHandedWeapon(this)];
    } else {
        return sShieldStandingLeftAnims[Player_IsHoldingTwoHandedWeapon(this)];
    }
}

s32 Player_TryShieldingStanding(PlayState* play, Player* this) {
    PlayerAnimationHeader* shieldStandingAnim;
    f32 frame;

    if (!(this->stateFlags1 &
          (PLAYER_STATE1_HOLDING_SHIELD | PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
        (play->shootingGalleryStatus == 0) && (this->heldItemAction == this->itemAction) &&
        (this->currentShield != PLAYER_SHIELD_NONE) && !Player_IsChildWithHylianShield(this) &&
        Player_IsZTargeting(this) && CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {

        shieldStandingAnim = Player_SetupShieldStanding(play, this);
        frame = Animation_GetLastFrame(shieldStandingAnim);
        PlayerAnimation_Change(play, &this->upperSkelAnime, shieldStandingAnim, PLAYER_ANIM_NORMAL_SPEED, frame, frame,
                               ANIMMODE_ONCE, 0.0f);
        Player_PlaySfx(this, NA_SE_IT_SHIELD_POSTURE);

        return true;
    }

    return false;
}

s32 Player_UpperAction_IdleDefault(Player* this, PlayState* play) {
    if (Player_TryShieldingStanding(play, this)) {
        return true;
    }

    return false;
}

void Player_SetupShieldStandingEnd(Player* this) {
    Player_SetUpperAction(this, Player_UpperAction_ShieldStandingEnd);

    if (this->itemAction < 0) {
        Player_SetHeldItem(this);
    }

    Animation_Reverse(&this->upperSkelAnime);
    Player_PlaySfx(this, NA_SE_IT_SHIELD_REMOVE);
}

void func_808348EC(PlayState* play, Player* this) {
    struct_808540F4* ptr = &D_808540F4[this->unk_15A];
    f32 temp;

    temp = ptr->unk_04;
    temp = (this->upperSkelAnime.playSpeed < 0.0f) ? temp - 1.0f : temp;

    if (PlayerAnimation_OnFrame(&this->upperSkelAnime, temp)) {
        Player_UseItemWithNoticableSfx(play, this);
    }

    Player_TryEnemyLockOn(this);
}

s32 func_8083499C(Player* this, PlayState* play) {
    // Never passes due to Player_TryUsingItem running first, which ends up in PLAYER_STATE1_START_CHANGE_ITEM being
    // unset
    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) {
        Player_SetupUseItemAnim(this, play);
    } else {
        return false;
    }

    return true;
}

s32 Player_UpperAction_IdleWithSword(Player* this, PlayState* play) {
    if (Player_TryShieldingStanding(play, this) || func_8083499C(this, play)) {
        return true;
    }

    return false;
}

s32 Player_UpperAction_2(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->upperSkelAnime) ||
        ((Player_ItemToItemAction(this->heldItemId) == this->heldItemAction) &&
         (D_80853614 =
              (D_80853614 || ((this->modelAnimType != PLAYER_ANIMTYPE_3) && (play->shootingGalleryStatus == 0)))))) {
        Player_SetUpperAction(this, sUpdateItemActionFuncs[this->heldItemAction]);
        this->firstPersonItemTimer = 0;
        this->unk_6AC = 0;
        D_80853618 = D_80853614;
        return this->upperActionFunc(this, play);
    }

    if (func_80833350(this) != 0) {
        func_808348EC(play, this);
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
        this->unk_6AC = 0;
    } else {
        func_808348EC(play, this);
    }

    return true;
}

s32 Player_UpperAction_ShieldStanding(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->upperSkelAnime);

    if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
        Player_SetupShieldStandingEnd(this);
        return true;
    }

    this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;
    Player_SetModelsForHoldingShield(this);

    return true;
}

s32 Player_UpperAction_X(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 frame;

    if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        anim = Player_SetupShieldStanding(play, this);
        frame = Animation_GetLastFrame(anim);
        PlayerAnimation_Change(play, &this->upperSkelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, frame, frame, ANIMMODE_ONCE,
                               0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;
    Player_SetModelsForHoldingShield(this);

    return true;
}

s32 Player_UpperAction_ShieldStandingEnd(Player* this, PlayState* play) {
    D_80853614 = D_80853618;

    if (D_80853614 || PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperAction(this, sUpdateItemActionFuncs[this->heldItemAction]);
        PlayerAnimation_PlayLoop(play, &this->upperSkelAnime,
                                 GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
        this->unk_6AC = 0;
        this->upperActionFunc(this, play);
        return false;
    }

    return true;
}

s32 func_80834D2C(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;

    if (this->heldItemAction != PLAYER_IA_BOOMERANG) {
        if (!Player_TryReadyFpsItemToShoot(this, play)) {
            return false;
        }

        if (!Player_IsHoldingHookshot(this)) {
            anim = &gPlayerAnim_link_bow_bow_ready;
        } else {
            anim = &gPlayerAnim_link_hook_shot_ready;
        }
        PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
    } else {
        Player_SetUpperAction(this, Player_UpperAction_PullOutBoomerang);
        this->firstPersonItemTimer = 10;
        PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_wait2waitR);
    }

    if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_uma_anim_walk);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !Player_TryEnemyLockOn(this)) {
        Player_Anim_PlayLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
    }

    return true;
}

s32 func_80834E44(PlayState* play) {
    return (play->shootingGalleryStatus > 0) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B);
}

s32 func_80834E7C(PlayState* play) {
    return (play->shootingGalleryStatus != 0) &&
           ((play->shootingGalleryStatus < 0) ||
            CHECK_BTN_ANY(sControlInput->cur.button, BTN_A | BTN_B | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN));
}

s32 func_80834EB8(Player* this, PlayState* play) {
    if ((this->attentionMode == PLAYER_ATTENTIONMODE_NONE) || (this->attentionMode == PLAYER_ATTENTIONMODE_AIMING)) {
        if (Player_IsZTargeting(this) ||
            (Camera_CheckValidMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_AIM_ADULT) == 0)) {
            return true;
        }
        this->attentionMode = PLAYER_ATTENTIONMODE_AIMING;
    }

    return false;
}

s32 func_80834F2C(Player* this, PlayState* play) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG)) {
        if (D_80853614 || func_80834E44(play)) {
            if (func_80834D2C(this, play)) {
                return func_80834EB8(this, play);
            }
        }
    }

    return false;
}

s32 func_80834FBC(Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            Player_RequestRumble(this, 255, 10, 250, SQ(0));
            Player_PlaySfx(this, NA_SE_IT_HOOKSHOT_RECEIVE);
        }

        return true;
    }

    return false;
}

s32 Player_UpperAction_IA_Aim(Player* this, PlayState* play) {
    if (this->stickFlameTimer >= 0) {
        this->stickFlameTimer = -this->stickFlameTimer;
    }

    if ((!Player_IsHoldingHookshot(this) || func_80834FBC(this)) && !Player_TryShieldingStanding(play, this) &&
        !func_80834F2C(this, play)) {
        return false;
    }

    return true;
}

s32 func_808350A4(PlayState* play, Player* this) {
    s32 itemId;
    s32 arrowType;

    if (this->heldActor != NULL) {
        if (!Player_IsHoldingHookshot(this)) {
            Player_GetFpsItemAmmo(play, this, &itemId, &arrowType);

            if (gSaveContext.minigameState == 1) {
                play->interfaceCtx.hbaAmmo--;
            } else if (play->shootingGalleryStatus != 0) {
                play->shootingGalleryStatus--;
            } else {
                Inventory_ChangeAmmo(itemId, -1);
            }

            if (play->shootingGalleryStatus == 1) {
                play->shootingGalleryStatus = -10;
            }

            Player_RequestRumble(this, 150, 10, 150, SQ(0));
        } else {
            Player_RequestRumble(this, 255, 20, 150, SQ(0));
        }

        this->unk_A73 = 4;
        this->heldActor->parent = NULL;
        this->actor.child = NULL;
        this->heldActor = NULL;

        return true;
    }

    return false;
}

static PlayerAnimationHeader* D_808543CC[] = {
    &gPlayerAnim_link_bow_walk2ready,
    &gPlayerAnim_link_hook_walk2ready,
};

static PlayerAnimationHeader* D_808543D4[] = {
    &gPlayerAnim_link_bow_bow_wait,
    &gPlayerAnim_link_hook_wait,
};

static u16 D_808543DC[] = { NA_SE_IT_BOW_FLICK, NA_SE_IT_SLING_FLICK };

s32 Player_UpperAction_ReadyFpsItemToShoot(Player* this, PlayState* play) {
    s32 bowOrHook;

    if (!Player_IsHoldingHookshot(this)) {
        bowOrHook = 0; // bow
    } else {
        bowOrHook = 1; // hookshot
    }

    Math_ScaledStepToS(&this->upperLimbRot.z, 1200, 400);
    this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_UPPER_ROT_Z;

    if ((this->unk_836 == 0) && (func_80833350(this) == 0) &&
        (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk)) {
        PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, D_808543CC[bowOrHook]);
        this->unk_836 = -1;
    } else if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        PlayerAnimation_PlayLoop(play, &this->upperSkelAnime, D_808543D4[bowOrHook]);
        this->unk_836 = 1;
    } else if (this->unk_836 == 1) {
        this->unk_836 = 2;
    }

    if (this->firstPersonItemTimer > 10) {
        this->firstPersonItemTimer--;
    }

    func_80834EB8(this, play);

    if ((this->unk_836 > 0) && ((this->stickFlameTimer < 0) || (!D_80853618 && !func_80834E7C(play)))) {
        Player_SetUpperAction(this, Player_UpperAction_7);
        if (this->stickFlameTimer >= 0) {
            if (bowOrHook == 0) {
                if (!func_808350A4(play, this)) {
                    Player_PlaySfx(this, D_808543DC[ABS(this->stickFlameTimer) - 1]);
                }
            } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                func_808350A4(play, this);
            }
        }
        this->firstPersonItemTimer = 10;
        Player_SetHorizontalSpeedToZero(this);
    } else {
        this->stateFlags1 |= PLAYER_STATE1_READY_TO_SHOOT;
    }

    return 1;
}

s32 Player_UpperAction_7(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->upperSkelAnime);

    if (Player_IsHoldingHookshot(this) && !func_80834FBC(this)) {
        return 1;
    }

    if (!Player_TryShieldingStanding(play, this) &&
        (D_80853614 || ((this->stickFlameTimer < 0) && D_80853618) || func_80834E44(play))) {
        this->stickFlameTimer = ABS(this->stickFlameTimer);

        if (Player_TryReadyFpsItemToShoot(this, play)) {
            if (Player_IsHoldingHookshot(this)) {
                this->unk_836 = 1;
            } else {
                PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_next);
            }
        }
    } else {
        if (this->firstPersonItemTimer != 0) {
            this->firstPersonItemTimer--;
        }

        if (Player_IsZTargeting(this) || (this->attentionMode != PLAYER_ATTENTIONMODE_NONE) ||
            (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
            if (this->firstPersonItemTimer == 0) {
                this->firstPersonItemTimer++;
            }
            return 1;
        }

        if (Player_IsHoldingHookshot(this)) {
            Player_SetUpperAction(this, Player_UpperAction_IA_Aim);
        } else {
            Player_SetUpperAction(this, Player_UpperAction_8);
            PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_end);
        }

        this->firstPersonItemTimer = 0;
    }

    return 1;
}

s32 Player_UpperAction_8(Player* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperAction(this, Player_UpperAction_IA_Aim);
    }

    return 1;
}

void Player_StartZParallel(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_Z_PARALLEL;

    if (!(this->skelAnime.moveFlags & 0x80) && (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        (sShapeYawToTouchedWall < 0x2000)) {
        this->yaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->zTargetYaw = this->actor.shape.rot.y;
}

s32 Player_TryIdlingAllAndReleaseHeldActor(PlayState* play, Player* this, Actor* heldActor) {
    if (heldActor == NULL) {
        Player_DetachHeldActorAndResetAttributes(play, this);
        Player_Setup1_IdleAll(this, play);
        return true;
    }

    return false;
}

void Player_SetupCarryAboveHead(Player* this, PlayState* play) {
    if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, this->heldActor)) {
        Player_SetUpperAction(this, Player_UpperAction_CarryAboveHead);
        PlayerAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
    }
}

s32 Player_UpperAction_CarryAboveHead(Player* this, PlayState* play) {
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        Player_SetupUpperActionForHeldItem(play, this);
    }

    if (Player_TryShieldingStanding(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
            PlayerAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
        }

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.terminalVelocity = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }
        return true;
    }

    return Player_UpperAction_IdleDefault(this, play);
}

void Player_SetLeftHandDlists(Player* this, Gfx** dLists) {
    this->leftHandDLists = dLists + gSaveContext.linkAge;
}

s32 Player_UpperAction_HoldBoomerang(Player* this, PlayState* play) {
    if (Player_TryShieldingStanding(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
        Player_SetUpperAction(this, Player_UpperAction_WaitForThrownBoomerang);
    } else if (func_80834F2C(this, play)) {
        return true;
    }

    return false;
}

s32 Player_UpperAction_PullOutBoomerang(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperAction(this, Player_UpperAction_AimBoomerang);
        PlayerAnimation_PlayLoop(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_waitR);
    }

    func_80834EB8(this, play);

    return true;
}

s32 Player_UpperAction_AimBoomerang(Player* this, PlayState* play) {
    PlayerAnimationHeader* animSeg = this->skelAnime.animation;

    if ((Player_GetWaitRightAnim(this) == animSeg) || (Player_GetWaitLeftAnim(this) == animSeg) ||
        (Player_GetAimBoomerangRightAnim(this) == animSeg) || (Player_GetAimBoomerangLeftAnim(this) == animSeg)) {
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                                    this->skelAnime.jointTable);
    } else {
        PlayerAnimation_Update(play, &this->upperSkelAnime);
    }

    func_80834EB8(this, play);

    if (!D_80853618) {
        Player_SetUpperAction(this, Player_UpperAction_ThrowBoomerang);
        PlayerAnimation_PlayOnce(play, &this->upperSkelAnime,
                                 (this->leftRightBlendWeight < 0.5f) ? &gPlayerAnim_link_boom_throwR
                                                                     : &gPlayerAnim_link_boom_throwL);
    }

    return 1;
}

s32 Player_UpperAction_ThrowBoomerang(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperAction(this, Player_UpperAction_WaitForThrownBoomerang);
        this->firstPersonItemTimer = 0;
    } else if (PlayerAnimation_OnFrame(&this->upperSkelAnime, 6.0f)) {
        f32 posX = (Math_SinS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.x;
        f32 posZ = (Math_CosS(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.z;
        s32 yaw = (this->lockOnActor != NULL) ? this->actor.shape.rot.y + 14000 : this->actor.shape.rot.y;
        EnBoom* boomerang =
            (EnBoom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 30.0f, posZ,
                                 this->actor.focus.rot.x, yaw, 0, 0);

        this->boomerangActor = &boomerang->actor;
        if (boomerang != NULL) {
            boomerang->moveTo = this->lockOnActor;
            boomerang->returnTimer = 20;
            this->stateFlags1 |= PLAYER_STATE1_AWAITING_THROWN_BOOMERANG;
            if (!Player_IsEnemyLockOn(this)) {
                Player_StartZParallel(this);
            }
            this->unk_A73 = 4;
            Player_PlaySfx(this, NA_SE_IT_BOOMERANG_THROW);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return true;
}

s32 Player_UpperAction_WaitForThrownBoomerang(Player* this, PlayState* play) {
    if (Player_TryShieldingStanding(play, this)) {
        return true;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG)) {
        Player_SetUpperAction(this, Player_UpperAction_CatchBoomerang);
        PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_catch);
        Player_SetLeftHandDlists(this, gPlayerLeftHandBoomerangDLs);
        Player_PlaySfx(this, NA_SE_PL_CATCH_BOOMERANG);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
        return true;
    }

    return false;
}

s32 Player_UpperAction_CatchBoomerang(Player* this, PlayState* play) {
    if (!Player_UpperAction_HoldBoomerang(this, play) && PlayerAnimation_Update(play, &this->upperSkelAnime)) {
        Player_SetUpperAction(this, Player_UpperAction_HoldBoomerang);
    }

    return 1;
}

s32 Player_SetAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    if (actionFunc == this->actionFunc) {
        return false;
    }

    if (Player_Action_PlayOcarina == this->actionFunc) {
        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
        this->stateFlags2 &= ~(PLAYER_STATE2_OCARINA_START_READY | PLAYER_STATE2_OCARINA_ON_FOR_ACTOR);
    } else if (Player_Action_CastMagicSpell == this->actionFunc) {
        Player_ResetSubCam(play, this);
    }

    this->actionFunc = actionFunc;

    if ((this->itemAction != this->heldItemAction) &&
        (!(flags & 1) || !(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD))) {
        Player_SetHeldItem(this);
    }

    if (!(flags & 1) && !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
        Player_SetupUpperActionForHeldItem(play, this);
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_SHIELD;
    }

    Player_Anim_ResetMove(this);
    this->stateFlags1 &=
        ~(PLAYER_STATE1_END_HOOKSHOT_MOVE | PLAYER_STATE1_TALKING | PLAYER_STATE1_TAKING_DAMAGE |
          PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FALLING_INTO_GROTTO);
    this->stateFlags2 &= ~(PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING | PLAYER_STATE2_OCARINA_ON | PLAYER_STATE2_IDLING);
    this->stateFlags3 &=
        ~(PLAYER_STATE3_MIDAIR | PLAYER_STATE3_ENDING_MELEE_ATTACK | PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH);
    this->actionVar8 = 0;
    this->actionVar16 = 0;
    this->unk_6AC = 0;
    func_808326F0(this);

    return true;
}

void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 moveFlags = this->skelAnime.moveFlags;

    this->skelAnime.moveFlags = 0;
    Player_SetAction(play, this, actionFunc, flags);
    this->skelAnime.moveFlags = moveFlags;
}

void Player_SetAction_PreserveItemAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 itemAction;

    if (this->itemAction >= 0) {
        itemAction = this->itemAction;
        this->itemAction = this->heldItemAction;
        Player_SetAction(play, this, actionFunc, flags);
        this->itemAction = itemAction;
        Player_SetModels(this, Player_ModelGroupFromIA(this, this->itemAction));
    }
}

void func_80835E44(PlayState* play, s16 camSetting) {
    if (!Play_CamIsNotFixed(play)) {
        if (camSetting == CAM_SET_SCENE_TRANSITION) {
            Interface_ChangeHudVisibilityMode(HUD_VISIBILITY_NOTHING_ALT);
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), camSetting);
    }
}

void Player_TurnAroundCamera(PlayState* play, s32 arg1) {
    func_80835E44(play, CAM_SET_TURN_AROUND);
    Camera_SetCameraData(Play_GetCamera(play, CAM_ID_MAIN), 4, NULL, NULL, arg1, 0, 0);
}

void Player_PutAwayHookshot(Player* this) {
    if (Player_IsHoldingHookshot(this)) {
        Actor* heldActor = this->heldActor;

        if (heldActor != NULL) {
            Actor_Kill(heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

void Player_UseItem(PlayState* play, Player* this, s32 itemId) {
    s8 itemAction;
    s32 temp;
    s32 nextAnimType;

    itemAction = Player_ItemToItemAction(itemId);

    if (
        // Option 1
        ((this->heldItemAction == this->itemAction) &&
         (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) || (Player_MeleeWeaponFromIA(itemAction) != 0) ||
          (itemAction == PLAYER_IA_NONE))) ||
        // Option 2
        ((this->itemAction < 0) && ((Player_MeleeWeaponFromIA(itemAction) != 0) || (itemAction == PLAYER_IA_NONE)))) {

        if ((itemAction == PLAYER_IA_NONE) || !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) ||
            ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
             ((itemAction == PLAYER_IA_HOOKSHOT) || (itemAction == PLAYER_IA_LONGSHOT)))) {

            if ((play->bombchuBowlingStatus == 0) &&
                (((itemAction == PLAYER_IA_DEKU_STICK) && (AMMO(ITEM_DEKU_STICK) == 0)) ||
                 ((itemAction == PLAYER_IA_MAGIC_BEAN) && (AMMO(ITEM_MAGIC_BEAN) == 0)) ||
                 (temp = Player_ExplosiveFromIA(this, itemAction),
                  ((temp >= 0) && ((AMMO(sExplosiveInfos[temp].itemId) == 0) ||
                                   (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length >= 3)))))) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
            } else if (itemAction == PLAYER_IA_LENS_OF_TRUTH) {
                if (Magic_RequestChange(play, 0, MAGIC_CONSUME_LENS)) {
                    if (play->actorCtx.lensActive) {
                        Actor_DisableLens(play);
                    } else {
                        play->actorCtx.lensActive = true;
                    }

                    Audio_PlaySfx((play->actorCtx.lensActive) ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            } else if (itemAction == PLAYER_IA_DEKU_NUT) {
                if (AMMO(ITEM_DEKU_NUT) != 0) {
                    func_8083C61C(play, this);
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            } else if ((temp = Player_MagicSpellFromIA(this, itemAction)) >= 0) {
                if (((itemAction == PLAYER_IA_FARORES_WIND) && (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 0)) ||
                    ((gSaveContext.magicCapacity != 0) && (gSaveContext.magicState == MAGIC_STATE_IDLE) &&
                     (gSaveContext.magic >= sMagicSpellCosts[temp]))) {
                    this->itemAction = itemAction;
                    this->attentionMode = PLAYER_ATTENTIONMODE_ITEM_CUTSCENE;
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            } else if (itemAction >= PLAYER_IA_MASK_KEATON) {
                if (this->currentMask != PLAYER_MASK_NONE) {
                    this->currentMask = PLAYER_MASK_NONE;
                } else {
                    this->currentMask = itemAction - PLAYER_IA_MASK_KEATON + 1;
                }

                Player_PlaySfx_Noticable(this, NA_SE_PL_CHANGE_ARMS);
            } else if (((itemAction >= PLAYER_IA_OCARINA_FAIRY) && (itemAction <= PLAYER_IA_OCARINA_OF_TIME)) ||
                       (itemAction >= PLAYER_IA_BOTTLE_FISH)) {
                if (!Player_IsEnemyLockOn(this) ||
                    ((itemAction >= PLAYER_IA_BOTTLE_POTION_RED) && (itemAction <= PLAYER_IA_BOTTLE_FAIRY))) {
                    TitleCard_Clear(play, &play->actorCtx.titleCtx);
                    this->attentionMode = PLAYER_ATTENTIONMODE_ITEM_CUTSCENE;
                    this->itemAction = itemAction;
                }
            } else if ((itemAction != this->heldItemAction) ||
                       ((this->heldActor == NULL) && (Player_ExplosiveFromIA(this, itemAction) >= 0))) {
                this->nextModelGroup = Player_ModelGroupFromIA(this, itemAction);
                nextAnimType = gPlayerModelTypes[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];

                if ((this->heldItemAction >= 0) && (Player_MagicSpellFromIA(this, itemAction) < 0) &&
                    (itemId != this->heldItemId) &&
                    (D_80854164[gPlayerModelTypes[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType] !=
                     PLAYER_D_808540F4_0)) {
                    this->heldItemId = itemId;
                    this->stateFlags1 |= PLAYER_STATE1_START_CHANGE_ITEM;
                } else {
                    Player_PutAwayHookshot(this);
                    Player_DetatchHeldActor(play, this);
                    Player_SetupChangeItemAnim(play, this, itemAction);
                }
            } else {
                D_80853614 = D_80853618 = true;
            }
        }
    }
}

void Player_SetupDie(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    s32 cond = Player_IsFreeSwimming(this);

    Player_DetachHeldActorAndResetAttributes(play, this);

    Player_SetAction(play, this, cond ? Player_Action_SwimDrown : Player_Action_Die, 0);

    this->stateFlags1 |= PLAYER_STATE1_IN_DEATH_CUTSCENE;

    Player_Anim_PlayOnce(play, this, anim);
    if (anim == &gPlayerAnim_link_derth_rebirth) {
        this->skelAnime.endFrame = 84.0f;
    }

    Player_ClearAttentionModeAndStopMoving(this);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DOWN);

    if (this->actor.category == ACTORCAT_PLAYER) {
        Audio_SetBgmVolumeOffDuringFanfare();

        if (Inventory_ConsumeFairy(play)) {
            play->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->actionVar8 = 1;
        } else {
            play->gameOverCtx.state = GAMEOVER_DEATH_START;
            Audio_StopBgmAndFanfare(0);
            Audio_PlayFanfare(NA_BGM_GAME_OVER);
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
        }

        OnePointCutscene_Init(play, 9806, cond ? 120 : 60, &this->actor, CAM_ID_MAIN);
        Letterbox_SetSizeTarget(32);
    }
}

s32 Player_CanUseItem(Player* this) {
    return (!(Player_Action_CsIntoAction == this->actionFunc) ||
            ((this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) &&
             ((this->heldItemId == ITEM_LAST_USED) || (this->heldItemId == ITEM_NONE)))) &&
           (!(Player_UpperAction_2 == this->upperActionFunc) ||
            (Player_ItemToItemAction(this->heldItemId) == this->heldItemAction));
}

s32 Player_TryUpperAction(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->actor.parent != NULL) &&
        Player_IsHoldingHookshot(this)) {
        Player_SetAction(play, this, Player_Action_HookshotFly, 1);
        this->stateFlags3 |= PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH;
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_hook_fly_start);
        Player_AnimReplace_Setup(play, this, 0x9B);
        Player_ClearAttentionModeAndStopMoving(this);
        this->yaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        this->hoverBootsTimer = 0;
        this->rotOverrideFlags |=
            PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y | PLAYER_ROT_OVERRIDE_UPPER_ROT_X;
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
        return true;
    }

    if (Player_CanUseItem(this)) {
        Player_TryUsingItem(this, play);
        if (Player_Action_ThrowDekuNut == this->actionFunc) {
            return true;
        }
    }

    if (!this->upperActionFunc(this, play)) {
        return false;
    }

    if (this->upperInterpWeight != 0.0f) {
        if ((func_80833350(this) == 0) || (this->speedXZ != 0.0f)) {
            AnimationContext_SetCopyFalse(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                                          this->skelAnime.jointTable, D_80853410);
        }
        Math_StepToF(&this->upperInterpWeight, 0.0f, 0.25f);
        AnimationContext_SetInterp(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                   this->upperSkelAnime.jointTable, 1.0f - this->upperInterpWeight);
    } else if ((func_80833350(this) == 0) || (this->speedXZ != 0.0f)) {
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->upperSkelAnime.jointTable, D_80853410);
    } else {
        AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                    this->upperSkelAnime.jointTable);
    }

    return true;
}

s32 Player_SetupCsIntoAction(PlayState* play, Player* this, PlayerCsIntoActionFunc csIntoActionFunc) {
    this->csIntoActionFunc = csIntoActionFunc;
    Player_SetAction(play, this, Player_Action_CsIntoAction, 0);
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;
    return Player_UnequipItem(play, this);
}

void Player_UpdateYaw(Player* this, PlayState* play) {
    s16 previousYaw = this->actor.shape.rot.y;

    if (!(this->stateFlags2 & (PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE))) {
        if ((this->lockOnActor != NULL) &&
            ((play->actorCtx.targetCtx.rotZTick != 0) || (this->actor.category != ACTORCAT_PLAYER))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y,
                               Math_Vec3f_Yaw(&this->actor.world.pos, &this->lockOnActor->focus.pos), 4000);
        } else if ((this->stateFlags1 & PLAYER_STATE1_Z_PARALLEL) &&
                   !(this->stateFlags2 & (PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE))) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->zTargetYaw, 4000);
        }
    } else if (!(this->stateFlags2 & PLAYER_STATE2_NO_YAW_UPDATE)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 2000);
    }

    this->yawDiffPrevFrame = this->actor.shape.rot.y - previousYaw;
}

// Steps angle based on offset from referenceAngle, then returns any excess angle difference beyond angleMinMax
s32 Player_StepAngleWithOffset(s16* angle, s16 target, s16 step, s16 angleMinMax, s16 referenceAngle,
                               s16 angleDiffMinMax) {
    s16 angleDiff;
    s16 clampedAngleDiff;
    s16 originalAngle;

    angleDiff = clampedAngleDiff = referenceAngle - *angle;
    clampedAngleDiff = CLAMP(clampedAngleDiff, -angleDiffMinMax, angleDiffMinMax);
    *angle += (s16)(angleDiff - clampedAngleDiff);

    Math_ScaledStepToS(angle, target, step);

    originalAngle = *angle;
    if (*angle < -angleMinMax) {
        *angle = -angleMinMax;
    } else if (*angle > angleMinMax) {
        *angle = angleMinMax;
    }
    return originalAngle - *angle;
}

s32 Player_UpdateLookAngles(Player* this, s32 syncUpperRotToFocusRot) {
    s16 yawDiff;
    s16 yaw;

    yaw = this->actor.shape.rot.y;
    if (syncUpperRotToFocusRot) {
        yaw = this->actor.focus.rot.y;
        this->upperLimbRot.x = this->actor.focus.rot.x;
        this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_UPPER_ROT_X;
    } else {
        Player_StepAngleWithOffset(
            &this->upperLimbRot.x,
            Player_StepAngleWithOffset(&this->headLimbRot.x, this->actor.focus.rot.x, DEG_TO_BINANG(3.3f),
                                       DEG_TO_BINANG(54.935f), this->actor.focus.rot.x, 0),
            DEG_TO_BINANG(1.1f), DEG_TO_BINANG(21.975f), this->headLimbRot.x, DEG_TO_BINANG(54.935f));
        yawDiff = this->actor.focus.rot.y - yaw;
        Player_StepAngleWithOffset(&yawDiff, 0, DEG_TO_BINANG(1.1f), DEG_TO_BINANG(131.84f), this->upperLimbRot.y,
                                   DEG_TO_BINANG(43.95f));
        yaw = this->actor.focus.rot.y - yawDiff;
        Player_StepAngleWithOffset(&this->headLimbRot.y, yawDiff - this->upperLimbRot.y, DEG_TO_BINANG(1.1f),
                                   DEG_TO_BINANG(43.95f), yawDiff, DEG_TO_BINANG(43.95f));
        Player_StepAngleWithOffset(&this->upperLimbRot.y, yawDiff, DEG_TO_BINANG(1.1f), DEG_TO_BINANG(43.95f),
                                   this->headLimbRot.y, DEG_TO_BINANG(43.95f));
        this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_HEAD_ROT_X |
                                  PLAYER_ROT_OVERRIDE_HEAD_ROT_Y | PLAYER_ROT_OVERRIDE_UPPER_ROT_X |
                                  PLAYER_ROT_OVERRIDE_UPPER_ROT_Y;
    }

    return yaw;
}

void Player_UpdateZTarget(Player* this, PlayState* play) {
    s32 ignoreLeash = false;
    s32 zBtnPressed = CHECK_BTN_ALL(sControlInput->cur.button, BTN_Z);
    Actor* actorToLockOn;
    s32 pad;
    s32 isHoldZTarget;
    s32 isTalking;

    if (!zBtnPressed) {
        this->stateFlags1 &= ~PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csMode != PLAYER_CSMODE_NONE) ||
        (this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_IN_CUTSCENE)) ||
        (this->stateFlags3 & PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH)) {
        this->zTargetSwitchTimer = 0;
    } else if (zBtnPressed || (this->stateFlags2 & PLAYER_STATE2_USING_SWITCH_Z_TARGET) ||
               (this->forcedLockOn != NULL)) {
        if (this->zTargetSwitchTimer <= 5) {
            this->zTargetSwitchTimer = 5;
        } else {
            this->zTargetSwitchTimer--;
        }
    } else if (this->stateFlags1 & PLAYER_STATE1_Z_PARALLEL) {
        this->zTargetSwitchTimer = 0;
    } else if (this->zTargetSwitchTimer != 0) {
        this->zTargetSwitchTimer--;
    }

    if (this->zTargetSwitchTimer >= 6) {
        ignoreLeash = true;
    }

    isTalking = Player_IsTalking(play);

    if (isTalking || (this->zTargetSwitchTimer != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_AWAITING_THROWN_BOOMERANG))) {
        if (!isTalking) {
            if (!(this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) &&
                ((this->heldItemAction != PLAYER_IA_FISHING_POLE) || (this->stickFlameTimer == 0)) &&
                CHECK_BTN_ALL(sControlInput->press.button, BTN_Z)) {

                if (this->actor.category == ACTORCAT_PLAYER) {
                    actorToLockOn = play->actorCtx.targetCtx.nextLockOnActor;
                } else {
                    actorToLockOn = &GET_PLAYER(play)->actor;
                }

                isHoldZTarget = (gSaveContext.zTargetSetting != 0) || (this->actor.category != ACTORCAT_PLAYER);
                this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING;

                if ((actorToLockOn != NULL) && !(actorToLockOn->flags & ACTOR_FLAG_CANT_LOCK_ON)) {
                    if ((actorToLockOn == this->lockOnActor) && (this->actor.category == ACTORCAT_PLAYER)) {
                        actorToLockOn = play->actorCtx.targetCtx.arrowPointedActor;
                    }

                    if (this->lockOnActor != actorToLockOn) {
                        if (!isHoldZTarget) {
                            this->stateFlags2 |= PLAYER_STATE2_USING_SWITCH_Z_TARGET;
                        }
                        this->lockOnActor = actorToLockOn;
                        this->zTargetSwitchTimer = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_CAN_SPEAK_OR_CHECK | PLAYER_STATE2_NAVI_REQUESTING_TALK);
                    } else {
                        if (!isHoldZTarget) {
                            Player_Untarget(this);
                        }
                    }

                    this->stateFlags1 &= ~PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET;
                } else {
                    if (!(this->stateFlags1 & (PLAYER_STATE1_Z_PARALLEL | PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET))) {
                        Player_StartZParallel(this);
                    }
                }
            }

            if (this->lockOnActor != NULL) {
                if ((this->actor.category == ACTORCAT_PLAYER) && (this->lockOnActor != this->forcedLockOn) &&
                    Target_OutsideLeashRange(this->lockOnActor, this, ignoreLeash)) {
                    Player_Untarget(this);
                    this->stateFlags1 |= PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET;
                } else if (this->lockOnActor != NULL) {
                    this->lockOnActor->targetPriority = 40;
                }
            } else if (this->forcedLockOn != NULL) {
                this->lockOnActor = this->forcedLockOn;
            }
        }

        if (this->lockOnActor != NULL) {
            this->stateFlags1 &= ~(PLAYER_STATE1_LOCK_ON_FRIEND | PLAYER_STATE1_Z_PARALLEL);
            if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                !CHECK_FLAG_ALL(this->lockOnActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_ENEMY)) {
                this->stateFlags1 |= PLAYER_STATE1_LOCK_ON_FRIEND;
            }
        } else {
            if (this->stateFlags1 & PLAYER_STATE1_Z_PARALLEL) {
                this->stateFlags2 &= ~PLAYER_STATE2_USING_SWITCH_Z_TARGET;
            } else {
                Player_UntargetCheckFloor(this);
            }
        }
    } else {
        Player_UntargetCheckFloor(this);
    }
}

s32 Player_CalculateTargetVelocityAndYaw(PlayState* play, Player* this, f32* inputVelocity, s16* inputYaw, f32 arg4) {
    f32 baseSpeedScale;
    f32 slope;
    f32 slopeSpeedScale;
    f32 speedLimit;

    if ((this->attentionMode != PLAYER_ATTENTIONMODE_NONE) || (play->transitionTrigger == TRANS_TRIGGER_START) ||
        (this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE)) {
        *inputVelocity = 0.0f;
        *inputYaw = this->actor.shape.rot.y;
    } else {
        *inputVelocity = sAnalogStickDistance;
        *inputYaw = sAnalogStickAngle;

        if (arg4 != 0.0f) {
            *inputVelocity -= 20.0f;
            if (*inputVelocity < 0.0f) {
                *inputVelocity = 0.0f;
            } else {
                baseSpeedScale = 1.0f - Math_CosS(*inputVelocity * 450.0f);
                *inputVelocity = (SQ(baseSpeedScale) * 30.0f) + 7.0f;
            }
        } else {
            *inputVelocity *= 0.8f;
        }

        if (sAnalogStickDistance != 0.0f) {
            slope = Math_SinS(this->floorPitch);
            speedLimit = this->speedLimit;
            slopeSpeedScale = CLAMP(slope, 0.0f, 0.6f);

            if (this->shapeOffsetY != 0.0f) {
                speedLimit = speedLimit - (this->shapeOffsetY * 0.008f);
                if (speedLimit < 2.0f) {
                    speedLimit = 2.0f;
                }
            }

            *inputVelocity = (*inputVelocity * 0.14f) - (8.0f * slopeSpeedScale * slopeSpeedScale);
            *inputVelocity = CLAMP(*inputVelocity, 0.0f, speedLimit);

            return true;
        }
    }

    return false;
}

s32 Player_StepHorizontalSpeedToZero(Player* this) {
    return Math_StepToF(&this->speedXZ, 0.0f, R_PLAYER_BOOT_8 / 100.0f);
}

s32 Player_GetInputVelocityAndYaw(Player* this, f32* inputVelocity, s16* inputYaw, f32 arg3, PlayState* play) {
    if (!Player_CalculateTargetVelocityAndYaw(play, this, inputVelocity, inputYaw, arg3)) {
        *inputYaw = this->actor.shape.rot.y;

        if (this->lockOnActor != NULL) {
            if ((play->actorCtx.targetCtx.rotZTick != 0) && !(this->stateFlags2 & PLAYER_STATE2_NO_YAW_UPDATE)) {
                *inputYaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->lockOnActor->focus.pos);
            }
        } else if (Player_IsZParallelOrLockOnFriend(this)) {
            *inputYaw = this->zTargetYaw;
        }

        return false;
    }

    *inputYaw += Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
    return true;
}

static s8 sZTargetEnemySwapActionList[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 sZTargetFriendSwapActionList[] = { 13, 1, 2, 5, 3, 4, 9, 10, 11, 7, 8, -6 };
static s8 sSidewalkSlowSwapActionList[] = { 13, 1, 2, 3, 4, 9, 10, 11, 8, 7, -6 };
static s8 sBackwalkFriendSwapActionList[] = { 13, 2, 4, 9, 10, 11, 8, -7 };
static s8 sSidewalkFastSwapActionList[] = { 13, 2, 4, 9, 10, 11, 12, 8, -7 };
static s8 sTurnSwapActionList[] = { -7 };
static s8 sIdleSwapActionList[] = { 0, 11, 1, 2, 3, 5, 4, 9, 8, 7, -6 };
static s8 sRunSwapActionList[] = { 0, 11, 1, 2, 3, 12, 5, 4, 9, 8, 7, -6 };
static s8 sRunZTargetSwapActionList[] = { 13, 1, 2, 3, 12, 5, 4, 9, 10, 11, 8, 7, -6 };
static s8 sBackwalkEndSwapActionList[] = { 10, 8, -7 };
static s8 sSwimSwapActionList[] = { 0, 12, 5, -4 };

static s32 (*sPlayerSwapActionFuncs[])(Player* this, PlayState* play) = {
    Player_SwapAction_TryCUp,                    // PLAYER_SWAPACTION_CUP
    Player_SwapAction_TryOpeningDoor,            // PLAYER_SWAPACTION_DOOR
    Player_SwapAction_TryGetItem,                // PLAYER_SWAPACTION_GET_ITEM
    Player_SwapAction_TryMountingHorse,          // PLAYER_SWAPACTION_MOUNT_HORSE
    Player_SwapAction_TryTalking,                // PLAYER_SWAPACTION_TALK
    Player_SwapAction_TrySpecialWallInteraction, // PLAYER_SWAPACTION_CLIMB_PUSH_PULL
    Player_SwapAction_TryRolling,                // PLAYER_SWAPACTION_ROLL_PUT_AWAY
    Player_SwapAction_TryAttackBottleFishingRod, // PLAYER_SWAPACTION_MELEE_WEAPON
    Player_SwapAction_TryChargingSpinAttack,     // PLAYER_SWAPACTION_SPIN_ATTACK
    Player_SwapAction_TryThrowPutDown,           // PLAYER_SWAPACTION_THROW_PUT_DOWN
    Player_SwapAction_TryAButtonActions,         // PLAYER_SWAPACTION_ROLL_JUMP_SLASH
    Player_SwapAction_TryShieldingCrouched,      // PLAYER_SWAPACTION_DEFENDING
    Player_SwapAction_TryJumpToLedge,            // PLAYER_SWAPACTION_WALL_JUMP
    Player_SwapAction_TryItemCsFirstPerson,      // PLAYER_SWAPACTION_ITEM_FIRST_PERSON
};

s32 Player_TrySwappingAction(PlayState* play, Player* this, s8* swapActionList, s32 arg3) {
    s32 i;

    if (!(this->stateFlags1 &
          (PLAYER_STATE1_EXITING_SCENE | PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_IN_CUTSCENE))) {
        if (arg3) {
            D_808535E0 = Player_TryUpperAction(this, play);
            if (Player_Action_ThrowDekuNut == this->actionFunc) {
                return true;
            }
        }

        if (Player_IsShootingHookshot(this)) {
            this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_UPPER_ROT_X;
            return true;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_START_CHANGE_ITEM) && (Player_UpperAction_2 != this->upperActionFunc)) {
            while (*swapActionList >= 0) {
                if (sPlayerSwapActionFuncs[*swapActionList](this, play)) {
                    return true;
                }
                swapActionList++;
            }

            if (sPlayerSwapActionFuncs[-(*swapActionList)](this, play)) {
                return true;
            }
        }
    }

    return false;
}

typedef enum {
    /* -1 */ PLAYER_ACTION_INTERRUPT_NONE = -1,
    /*  0 */ PLAYER_ACTION_INTERRUPT_SWAP,
    /*  1 */ PLAYER_ACTION_INTERRUPT_MOVE
} PlayerActionInterrupt;

// Checks if action is interrupted within a certain number of frames from the end of the current animation
// Returns -1 is action is not interrupted at all, 0 if interrupted by a sub-action, 1 if interrupted by the player
// moving
s32 Player_GetActionInterruptState(PlayState* play, Player* this, SkelAnime* skelAnime, f32 framesFromEnd) {
    f32 inputVelocity;
    s16 inputYaw;

    if (skelAnime->curFrame >= (skelAnime->endFrame - framesFromEnd)) {
        if (Player_TrySwappingAction(play, this, sIdleSwapActionList, true)) {
            return PLAYER_ACTION_INTERRUPT_SWAP;
        }

        if (Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play)) {
            return PLAYER_ACTION_INTERRUPT_MOVE;
        }
    }

    return PLAYER_ACTION_INTERRUPT_NONE;
}

void Player_SpawnSpinAttack(PlayState* play, Player* this, s32 magicCost) {
    if (magicCost != 0) {
        this->spinAttackTimer = 0.0f;
    } else {
        this->spinAttackTimer = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (this->actor.category == ACTORCAT_PLAYER) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                    this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, 0, 0, 0,
                    Player_GetMeleeWeaponHeld(this) | magicCost);
    }
}

// Check for inputs for quickspin
s32 func_808375D8(Player* this) {
    s8 sp3C[4];
    s8* iter;
    s8* iter2;
    s8 temp1;
    s8 temp2;
    s32 i;

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) || Player_HoldsBrokenKnife(this)) {
        return 0;
    }

    iter = &this->analogStickDirection128Parts[0];
    iter2 = &sp3C[0];
    for (i = 0; i < 4; i++, iter++, iter2++) {
        if ((*iter2 = *iter) < 0) {
            return 0;
        }
        *iter2 *= 2;
    }

    temp1 = sp3C[0] - sp3C[1];
    if (ABS(temp1) < 10) {
        return 0;
    }

    iter2 = &sp3C[1];
    for (i = 1; i < 3; i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);
        if ((ABS(temp2) < 10) || (temp2 * temp1 < 0)) {
            return 0;
        }
    }

    return 1;
}

void func_80837704(PlayState* play, Player* this) {
    PlayerAnimationHeader* anim;

    if ((this->meleeWeaponAnimation >= PLAYER_MWA_RIGHT_SLASH_1H) &&
        (this->meleeWeaponAnimation <= PLAYER_MWA_RIGHT_COMBO_2H)) {
        anim = D_80854358[Player_IsHoldingTwoHandedWeapon(this)];
    } else {
        anim = D_80854350[Player_IsHoldingTwoHandedWeapon(this)];
    }

    Player_ResetAttack(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 8.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -9.0f);
    Player_SpawnSpinAttack(play, this, 2 << 8);
}

void Player_Setup1_ChargeSpinAttack(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_ChargeSpinAttack, 1);
    func_80837704(play, this);
}

static s8 D_80854480[] = {
    PLAYER_MWA_STAB_1H,        // PLAYER_ANALOG_UP
    PLAYER_MWA_RIGHT_SLASH_1H, // PLAYER_ANALOG_RIGHT
    PLAYER_MWA_RIGHT_SLASH_1H, // PLAYER_ANALOG_DOWN
    PLAYER_MWA_LEFT_SLASH_1H,  // PLAYER_ANALOG_LEFT
};
static s8 D_80854484[] = {
    PLAYER_MWA_HAMMER_FORWARD, // PLAYER_ANALOG_UP
    PLAYER_MWA_HAMMER_SIDE,    // PLAYER_ANALOG_RIGHT
    PLAYER_MWA_HAMMER_FORWARD, // PLAYER_ANALOG_DOWN
    PLAYER_MWA_HAMMER_SIDE,    // PLAYER_ANALOG_LEFT
};

s32 func_80837818(Player* this) {
    s32 analogDir = this->analogStickDirection4Parts[this->inputFrameCounter];
    s32 meleeWeaponAnim;

    if (this->heldItemAction == PLAYER_IA_HAMMER) {
        if (analogDir <= PLAYER_ANALOG_NONE) {
            analogDir = PLAYER_ANALOG_UP;
        }
        meleeWeaponAnim = D_80854484[analogDir];
        this->slashCounter = 0;
    } else {
        if (func_808375D8(this)) {
            meleeWeaponAnim = PLAYER_MWA_SPIN_ATTACK_1H;
        } else {
            if (analogDir <= PLAYER_ANALOG_NONE) {
                if (Player_IsZTargeting(this)) {
                    meleeWeaponAnim = PLAYER_MWA_FORWARD_SLASH_1H;
                } else {
                    meleeWeaponAnim = PLAYER_MWA_RIGHT_SLASH_1H;
                }
            } else {
                meleeWeaponAnim = D_80854480[analogDir];
                if (meleeWeaponAnim == PLAYER_MWA_STAB_1H) {
                    this->stateFlags2 |= PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
                    if (!Player_IsZTargeting(this)) {
                        meleeWeaponAnim = PLAYER_MWA_FORWARD_SLASH_1H;
                    }
                }
            }
            if (this->heldItemAction == PLAYER_IA_DEKU_STICK) {
                meleeWeaponAnim = PLAYER_MWA_FORWARD_SLASH_1H;
            }
        }
        if (Player_IsHoldingTwoHandedWeapon(this)) {
            meleeWeaponAnim++;
        }
    }

    return meleeWeaponAnim;
}

void func_80837918(Player* this, s32 quadIndex, u32 dmgFlags) {
    this->meleeWeaponQuads[quadIndex].info.toucher.dmgFlags = dmgFlags;

    if (dmgFlags == DMG_DEKU_STICK) {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_WOOD;
    } else {
        this->meleeWeaponQuads[quadIndex].info.toucherFlags = TOUCH_ON | TOUCH_NEAREST;
    }
}

static u32 D_80854488[][2] = {
    { DMG_SLASH_MASTER, DMG_JUMP_MASTER }, { DMG_SLASH_KOKIRI, DMG_JUMP_KOKIRI }, { DMG_SLASH_GIANT, DMG_JUMP_GIANT },
    { DMG_DEKU_STICK, DMG_JUMP_MASTER },   { DMG_HAMMER_SWING, DMG_HAMMER_JUMP },
};

void Player_SetupAttack(PlayState* play, Player* this, s32 arg2) {
    s32 pad;
    u32 dmgFlags;
    s32 temp;

    Player_SetAction(play, this, Player_Action_Attack, 0);
    this->unk_844 = 8;

    if (!((arg2 >= PLAYER_MWA_FLIPSLASH_FINISH) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH))) {
        Player_ResetAttack(this);
    }

    if ((arg2 != this->meleeWeaponAnimation) || !(this->slashCounter < 3)) {
        this->slashCounter = 0;
    }

    this->slashCounter++;
    if (this->slashCounter >= 3) {
        arg2 += 2;
    }

    this->meleeWeaponAnimation = arg2;

    Player_Anim_PlayOnceAdjusted(play, this, D_80854190[arg2].unk_00);
    if ((arg2 != PLAYER_MWA_FLIPSLASH_START) && (arg2 != PLAYER_MWA_JUMPSLASH_START)) {
        Player_AnimReplace_Setup(play, this, 0x209);
    }

    this->yaw = this->actor.shape.rot.y;

    if (Player_HoldsBrokenKnife(this)) {
        temp = 1;
    } else {
        temp = Player_GetMeleeWeaponHeld(this) - 1;
    }

    if ((arg2 >= PLAYER_MWA_FLIPSLASH_START) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH)) {
        dmgFlags = D_80854488[temp][1];
    } else {
        dmgFlags = D_80854488[temp][0];
    }

    func_80837918(this, 0, dmgFlags);
    func_80837918(this, 1, dmgFlags);
}

void Player_SetInvincibilityTimerWithDamageFlash(Player* this, s32 timer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = timer;
        this->damageFlashTimer = 0;
    }
}

void Player_SetInvincibilityTimerWithoutDamageFlash(Player* this, s32 timer) {
    if (this->invincibilityTimer > timer) {
        this->invincibilityTimer = timer;
    }
    this->damageFlashTimer = 0;
}

s32 Player_InflictDamageImpl(PlayState* play, Player* this, s32 damage) {
    if ((this->invincibilityTimer != 0) || (this->actor.category != ACTORCAT_PLAYER)) {
        return true;
    }

    return Health_ChangeBy(play, damage);
}

void func_80837B60(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    Player_AnimReplace_SetupLedgeClimb(this, 3);
}

void Player_Setup_Midair(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Midair, 0);
    Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
    this->midairVar16 = 1;
    if (this->attentionMode != PLAYER_ATTENTIONMODE_CUTSCENE) {
        this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
    }
}

static PlayerAnimationHeader* D_808544B0[] = {
    &gPlayerAnim_link_normal_front_shit, &gPlayerAnim_link_normal_front_shitR, &gPlayerAnim_link_normal_back_shit,
    &gPlayerAnim_link_normal_back_shitR, &gPlayerAnim_link_normal_front_hit,   &gPlayerAnim_link_anchor_front_hitR,
    &gPlayerAnim_link_normal_back_hit,   &gPlayerAnim_link_anchor_back_hitR,
};

void Player_ApplyDamage(PlayState* play, Player* this, s32 damageReaction, f32 arg3, f32 arg4, s16 arg5, s32 arg6) {
    PlayerAnimationHeader* sp2C = NULL;
    PlayerAnimationHeader** sp28;

    if (this->stateFlags1 & PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP) {
        func_80837B60(this);
    }

    this->unk_890 = 0;

    Player_PlaySfx(this, NA_SE_PL_DAMAGE);

    if (!Player_InflictDamageImpl(play, this, 0 - this->actor.colChkInfo.damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
            Player_Setup_Midair(this, play);
        }
        return;
    }

    Player_SetInvincibilityTimerWithDamageFlash(this, arg6);

    if (damageReaction == PLAYER_DMGREACTION_FROZEN) {
        Player_SetAction(play, this, Player_Action_FrozenInIce, 0);

        sp2C = &gPlayerAnim_link_normal_ice_down;

        Player_ClearAttentionModeAndStopMoving(this);
        Player_RequestRumble(this, 255, 10, 40, SQ(0));

        Player_PlaySfx(this, NA_SE_PL_FREEZE_S);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FREEZE);
    } else if (damageReaction == PLAYER_DMGREACTION_ELECTRIC_SHOCK) {
        Player_SetAction(play, this, Player_Action_ElectricShock, 0);

        Player_RequestRumble(this, 255, 80, 150, SQ(0));

        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_electric_shock);
        Player_ClearAttentionModeAndStopMoving(this);

        this->actionVar16 = 20;
    } else {
        arg5 -= this->actor.shape.rot.y;
        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            Player_SetAction(play, this, Player_Action_SwimDamage, 0);
            Player_RequestRumble(this, 180, 20, 50, SQ(0));

            this->speedXZ = 4.0f;
            this->actor.velocity.y = 0.0f;

            sp2C = &gPlayerAnim_link_swimer_swim_hit;

            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        } else if ((damageReaction == PLAYER_DMGREACTION_KNOCKBACK) || (damageReaction == PLAYER_DMGREACTION_FLINCH) ||
                   !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   (this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                         PLAYER_STATE1_CLIMBING))) {
            Player_SetAction(play, this, Player_Action_KnockbackFly, 0);

            this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

            Player_RequestRumble(this, 255, 20, 150, SQ(0));
            Player_ClearAttentionModeAndStopMoving(this);

            if (damageReaction == PLAYER_DMGREACTION_FLINCH) {
                this->knockbackFlyVar16 = 4;

                this->actor.speed = 3.0f;
                this->speedXZ = 3.0f;
                this->actor.velocity.y = 6.0f;

                Player_Anim_PlayOnceForOneFrame(play, this,
                                                GET_PLAYER_ANIM(PLAYER_ANIMGROUP_FLINCH, this->modelAnimType));
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
            } else {
                this->actor.speed = arg3;
                this->speedXZ = arg3;
                this->actor.velocity.y = arg4;

                if (ABS(arg5) > 0x4000) {
                    sp2C = &gPlayerAnim_link_normal_front_downA;
                } else {
                    sp2C = &gPlayerAnim_link_normal_back_downA;
                }

                if ((this->actor.category != ACTORCAT_PLAYER) && (this->actor.colChkInfo.health == 0)) {
                    Player_AnimSfx_PlayVoice(this, NA_SE_VO_BL_DOWN);
                } else {
                    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
                }
            }

            this->hoverBootsTimer = 0;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        } else {
            if ((this->speedXZ > 4.0f) && !Player_IsEnemyLockOn(this)) {
                this->unk_890 = 20;
                Player_RequestRumble(this, 120, 20, 10, SQ(0));
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
                return;
            }

            sp28 = D_808544B0;

            Player_SetAction(play, this, Player_Action_Damage, 0);
            Player_ResetLeftRightBlendWeight(this);

            if (this->actor.colChkInfo.damage < 5) {
                Player_RequestRumble(this, 120, 20, 10, SQ(0));
            } else {
                Player_RequestRumble(this, 180, 20, 100, SQ(0));
                this->speedXZ = 23.0f;
                sp28 += 4;
            }

            if (ABS(arg5) <= 0x4000) {
                sp28 += 2;
            }

            if (Player_IsEnemyLockOn(this)) {
                sp28 += 1;
            }

            sp2C = *sp28;

            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += arg5;
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (ABS(arg5) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    Player_DetachHeldActorAndResetAttributes(play, this);

    this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;

    if (sp2C != NULL) {
        Player_Anim_PlayOnceAdjusted(play, this, sp2C);
    }
}

s32 Player_GetHurtFloorType(s32 floorType) {
    s32 temp = floorType - FLOOR_TYPE_2;

    if ((temp >= 0) && (temp <= (FLOOR_TYPE_3 - FLOOR_TYPE_2))) {
        return temp;
    } else {
        return -1;
    }
}

s32 Player_IsFloorSinking(s32 floorType) {
    return (floorType == FLOOR_TYPE_4) || (floorType == FLOOR_TYPE_7) || (floorType == FLOOR_TYPE_12);
}

void Player_TryBurningDekuShield(Player* this, PlayState* play) {
    if (this->currentShield == PLAYER_SHIELD_DEKU) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_SHIELD, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 1);
        Inventory_DeleteEquipment(play, EQUIP_TYPE_SHIELD);
        Message_StartTextbox(play, 0x305F, NULL);
    }
}

void Player_StartBurning(Player* this) {
    s32 i;

    // clang-format off
    for (i = 0; i < PLAYER_BODYPART_MAX; i++) { this->flameTimers[i] = Rand_S16Offset(0, 200); }
    // clang-format on

    this->isBurning = true;
}

void Player_TryBurning(Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1) {
        Player_StartBurning(this);
    }
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
}

void Player_GiveOnceSecondInvincibility(Player* this) {
    if ((this->invincibilityTimer >= 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

s32 Player_UpdateDamage(Player* this, PlayState* play) {
    s32 pad;
    s32 sp68 = false;
    s32 sp64;

    if (this->voidRespawnCounter != 0) {
        if (!Player_InBlockingCsMode(play, this)) {
            Player_InflictDamage(play, -16);
            this->voidRespawnCounter = 0;
        }
    } else {
        sp68 = ((Player_GetHeight(this) - 8.0f) < (this->shapeOffsetY * this->actor.scale.y));

        if (sp68 || (this->actor.bgCheckFlags & BGCHECKFLAG_CRUSHED) || (sFloorType == FLOOR_TYPE_9) ||
            (this->stateFlags2 & PLAYER_STATE2_FORCE_VOID_OUT)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);

            if (sp68) {
                Play_TriggerRespawn(play);
                Scene_SetTransitionForNextEntrance(play);
            } else {
                // Special case for getting crushed in Forest Temple's Checkboard Ceiling Hall or Shadow Temple's
                // Falling Spike Trap Room, to respawn the player in a specific place
                if (((play->sceneId == SCENE_FOREST_TEMPLE) && (play->roomCtx.curRoom.num == 15)) ||
                    ((play->sceneId == SCENE_SHADOW_TEMPLE) && (play->roomCtx.curRoom.num == 10))) {
                    static SpecialRespawnInfo sCheckboardCeilingRespawn = { { 1992.0f, 403.0f, -3432.0f }, 0 };
                    static SpecialRespawnInfo sFallingSpikeTrapRespawn = { { 1200.0f, -1343.0f, 3850.0f }, 0 };
                    SpecialRespawnInfo* respawnInfo;

                    if (play->sceneId == SCENE_FOREST_TEMPLE) {
                        respawnInfo = &sCheckboardCeilingRespawn;
                    } else {
                        respawnInfo = &sFallingSpikeTrapRespawn;
                    }

                    Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
                }

                Play_TriggerVoidOut(play);
            }

            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_TAKEN_AWAY);
            play->haltAllActors = true;
            Audio_PlaySfx(NA_SE_OC_ABYSS);
        } else if ((this->specialDamageEffect != 0) &&
                   ((this->specialDamageEffect >= 2) || (this->invincibilityTimer == 0))) {
            u8 damageReactions[] = { PLAYER_DMGREACTION_FLINCH, PLAYER_DMGREACTION_KNOCKBACK,
                                     PLAYER_DMGREACTION_KNOCKBACK };

            Player_TryBurning(this);

            if (this->specialDamageEffect == 3) {
                this->shockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->damageAmount;
            Player_ApplyDamage(play, this, damageReactions[this->specialDamageEffect - 1], this->damageSpeedXZ,
                               this->damageSpeedY, this->damageYaw, 20);
        } else {
            sp64 = (this->shieldQuad.base.acFlags & AC_BOUNCED) != 0;

            //! @bug The second set of conditions here seems intended as a way for Link to "block" hits by rolling.
            // However, `Collider.atFlags` is a byte so the flag check at the end is incorrect and cannot work.
            // Additionally, `Collider.atHit` can never be set while already colliding as AC, so it's also bugged.
            // This behavior was later fixed in MM, most likely by removing both the `atHit` and `atFlags` checks.
            if (sp64 || ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                         (this->cylinder.info.atHit != NULL) && (this->cylinder.info.atHit->atFlags & 0x20000000))) {

                Player_RequestRumble(this, 180, 20, 100, SQ(0));

                if (!Player_IsChildWithHylianShield(this)) {
                    if (this->invincibilityTimer >= 0) {
                        PlayerAnimationHeader* anim;
                        s32 isShieldCrouched = (Player_Action_ShieldCrouched == this->actionFunc);

                        if (!Player_IsFreeSwimming(this)) {
                            Player_SetAction(play, this, Player_Action_ShieldDeflectAttack, 0);
                        }

                        if (!(this->actionVar8 = isShieldCrouched)) {
                            Player_SetUpperAction(this, Player_UpperAction_X);

                            if (this->leftRightBlendWeight < 0.5f) {
                                anim = D_808543BC[Player_IsHoldingTwoHandedWeapon(this)];
                            } else {
                                anim = D_808543B4[Player_IsHoldingTwoHandedWeapon(this)];
                            }
                            PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
                        } else {
                            Player_Anim_PlayOnce(play, this, D_808543C4[Player_IsHoldingTwoHandedWeapon(this)]);
                        }
                    }

                    if (!(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP |
                                               PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_CLIMBING))) {
                        this->speedXZ = -18.0f;
                        this->yaw = this->actor.shape.rot.y;
                    }
                }

                if (sp64 && (this->shieldQuad.info.acHitInfo->toucher.effect == 1)) {
                    Player_TryBurningDekuShield(this, play);
                }

                return false;
            }

            if ((this->unk_A87 != 0) || (this->invincibilityTimer > 0) ||
                (this->stateFlags1 & PLAYER_STATE1_TAKING_DAMAGE) || (this->csMode != PLAYER_CSMODE_NONE) ||
                (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) ||
                (this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {
                return false;
            }

            if (this->cylinder.base.acFlags & AC_HIT) {
                Actor* ac = this->cylinder.base.ac;
                s32 damageReaction;

                if (ac->flags & ACTOR_FLAG_PLAY_BODYHIT_SFX) {
                    Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
                }

                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    damageReaction = PLAYER_DMGREACTION_DEFAULT;
                } else if (this->actor.colChkInfo.acHitEffect == 2) {
                    damageReaction = PLAYER_DMGREACTION_FROZEN;
                } else if (this->actor.colChkInfo.acHitEffect == 3) {
                    damageReaction = PLAYER_DMGREACTION_ELECTRIC_SHOCK;
                } else if (this->actor.colChkInfo.acHitEffect == 4) {
                    damageReaction = PLAYER_DMGREACTION_KNOCKBACK;
                } else {
                    Player_TryBurning(this);
                    damageReaction = PLAYER_DMGREACTION_DEFAULT;
                }

                Player_ApplyDamage(play, this, damageReaction, 4.0f, 5.0f, Actor_WorldYawTowardActor(ac, &this->actor),
                                   20);
            } else if (this->invincibilityTimer != 0) {
                return false;
            } else {
                static u8 D_808544F4[] = { 120, 60 };
                s32 sp48 = Player_GetHurtFloorType(sFloorType);

                if (((this->actor.wallPoly != NULL) &&
                     func_80042108(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) ||
                    ((sp48 >= 0) && func_80042108(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) &&
                     (this->floorTypeTimer >= D_808544F4[sp48])) ||
                    ((sp48 >= 0) &&
                     ((this->currentTunic != PLAYER_TUNIC_GORON) || (this->floorTypeTimer >= D_808544F4[sp48])))) {
                    this->floorTypeTimer = 0;
                    this->actor.colChkInfo.damage = 4;
                    Player_ApplyDamage(play, this, PLAYER_DMGREACTION_DEFAULT, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

void Player_Setup1_Jump(Player* this, PlayerAnimationHeader* anim, f32 jumpVelocityY, PlayState* play, u16 sfxId) {
    Player_SetAction(play, this, Player_Action_Midair, 1);

    if (anim != NULL) {
        Player_Anim_PlayOnceAdjusted(play, this, anim);
    }

    this->actor.velocity.y = jumpVelocityY * sPlayerUnderwaterSpeedAdjustment;
    this->hoverBootsTimer = 0;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;

    Player_AnimSfx_PlayFloorJump(this);
    Player_AnimSfx_PlayVoice(this, sfxId);

    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
}

void Player_Setup2_Jump(Player* this, PlayerAnimationHeader* anim, f32 jumpVelocityY, PlayState* play) {
    Player_Setup1_Jump(this, anim, jumpVelocityY, play, NA_SE_VO_LI_SWORD_N);
}

s32 Player_SwapAction_TryJumpToLedge(Player* this, PlayState* play) {
    s32 sp3C;
    PlayerAnimationHeader* sp38;
    f32 sp34;
    f32 jumpVelocityY;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;
    f32 sp24;

    if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->ageProperties->unk_14 > this->yDistToLedge))) {
        sp3C = false;

        if (Player_IsFreeSwimming(this)) {
            if (this->actor.depthInWater < 50.0f) {
                if ((this->ledgeClimbType <= PLAYER_LEDGE_CLIMB_1) ||
                    (this->yDistToLedge > this->ageProperties->unk_10)) {
                    return false;
                }
            } else if ((this->currentBoots != PLAYER_BOOTS_IRON) || (this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_3)) {
                return false;
            }
        } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   ((this->ageProperties->unk_14 <= this->yDistToLedge) &&
                    (this->stateFlags1 & PLAYER_STATE1_SWIMMING))) {
            return false;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (sTouchedWallFlags & WALL_FLAG_6)) {
            if (this->ledgeClimbDelayTimer >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL;
                if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                    sp3C = true;
                }
            }
        } else if ((this->ledgeClimbDelayTimer >= 6) || CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
            sp3C = true;
        }

        if (sp3C) {
            Player_SetAction(play, this, Player_Action_JumpToLedge, 0);

            this->stateFlags1 |= PLAYER_STATE1_JUMPING;

            sp34 = this->yDistToLedge;

            if (this->ageProperties->unk_14 <= sp34) {
                sp38 = &gPlayerAnim_link_normal_250jump_start;
                this->speedXZ = 1.0f;
            } else {
                wallPolyNormalX = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                wallPolyNormalZ = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                sp24 = this->distToInteractWall + 0.5f;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING_ONTO_LEDGE;

                if (Player_IsFreeSwimming(this)) {
                    sp38 = &gPlayerAnim_link_swimer_swim_15step_up;
                    sp34 -= (60.0f * this->ageProperties->unk_08);
                    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;
                } else if (this->ageProperties->unk_18 <= sp34) {
                    sp38 = &gPlayerAnim_link_normal_150step_up;
                    sp34 -= (59.0f * this->ageProperties->unk_08);
                } else {
                    sp38 = &gPlayerAnim_link_normal_100step_up;
                    sp34 -= (41.0f * this->ageProperties->unk_08);
                }

                this->actor.shape.yOffset -= sp34 * 100.0f;

                this->actor.world.pos.x -= sp24 * wallPolyNormalX;
                this->actor.world.pos.y += this->yDistToLedge;
                this->actor.world.pos.z -= sp24 * wallPolyNormalZ;

                Player_ClearAttentionModeAndStopMoving(this);
            }

            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;

            PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, sp38, 1.3f);
            AnimationContext_DisableQueue(play);

            this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;

            return true;
        }
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->ledgeClimbType == PLAYER_LEDGE_CLIMB_1) &&
               (this->ledgeClimbDelayTimer >= 3)) {
        jumpVelocityY = (this->yDistToLedge * 0.08f) + 5.5f;
        Player_Setup2_Jump(this, &gPlayerAnim_link_normal_jump, jumpVelocityY, play);
        this->speedXZ = 2.5f;

        return true;
    }

    return false;
}

void Player_SetupMiniCutscene(PlayState* play, Player* this, f32 arg2, s16 arg3) {
    Player_SetAction(play, this, Player_Action_MiniCutscene, 0);
    Player_ResetAttributes(play, this);

    this->miniCsHasMovedToStart = true;
    this->miniCutsceneVar16 = 1;

    this->miniCsPosTarget.x = (Math_SinS(arg3) * arg2) + this->actor.world.pos.x;
    this->miniCsPosTarget.z = (Math_CosS(arg3) * arg2) + this->actor.world.pos.z;

    Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
}

void Player_SetupSwimIdle(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SwimIdle, 0);
    Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
}

void Player_SetupEnterGrotto(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_EnterGrotto, 0);

    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FALLING_INTO_GROTTO;

    Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_FREE0);
}

s32 Player_TryEnteringGrotto(PlayState* play, Player* this) {
    if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (this->stateFlags1 & PLAYER_STATE1_FALLING_INTO_GROTTO)) {
        Player_SetupEnterGrotto(play, this);
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_S);
        func_800788CC(NA_SE_OC_SECRET_WARP_IN);
        return true;
    }

    return false;
}

/**
 * The actual entrances each "return entrance" value can map to.
 * This is used by scenes that are shared between locations, like child/adult Shooting Gallery or Great Fairy Fountains.
 *
 * This 1D array is split into groups of entrances.
 * The start of each group is indexed by `sReturnEntranceGroupIndices` values.
 * The resulting groups are then indexed by the spawn value.
 *
 * The spawn value (`PlayState.spawn`) is set to a different value depending on the entrance used to enter the
 * scene, which allows these dynamic "return entrances" to link back to the previous scene.
 *
 * Note: grottos and normal fairy fountains use `ENTR_RETURN_GROTTO`
 */
s16 sReturnEntranceGroupData[] = {
    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
    /*  0 */ ENTR_DEATH_MOUNTAIN_TRAIL_4,  // from Magic Fairy Fountain
    /*  1 */ ENTR_DEATH_MOUNTAIN_CRATER_3, // from Double Magic Fairy Fountain
    /*  2 */ ENTR_HYRULE_CASTLE_2,         // from Double Defense Fairy Fountain (as adult)

    // ENTR_RETURN_2
    /*  3 */ ENTR_KAKARIKO_VILLAGE_9, // from Potion Shop in Kakariko
    /*  4 */ ENTR_MARKET_DAY_5,       // from Potion Shop in Market

    // ENTR_RETURN_BAZAAR
    /*  5 */ ENTR_KAKARIKO_VILLAGE_3,
    /*  6 */ ENTR_MARKET_DAY_6,

    // ENTR_RETURN_4
    /*  7 */ ENTR_KAKARIKO_VILLAGE_11, // from House of Skulltulas
    /*  8 */ ENTR_BACK_ALLEY_DAY_2,    // from Bombchu Shop

    // ENTR_RETURN_SHOOTING_GALLERY
    /*  9 */ ENTR_KAKARIKO_VILLAGE_10,
    /* 10 */ ENTR_MARKET_DAY_8,

    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    /* 11 */ ENTR_ZORAS_FOUNTAIN_5,  // from Farores Wind Fairy Fountain
    /* 12 */ ENTR_HYRULE_CASTLE_2,   // from Dins Fire Fairy Fountain (as child)
    /* 13 */ ENTR_DESERT_COLOSSUS_7, // from Nayrus Love Fairy Fountain
};

/**
 * The values are indices into `sReturnEntranceGroupData` marking the start of each group
 */
u8 sReturnEntranceGroupIndices[] = {
    11, // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    9,  // ENTR_RETURN_SHOOTING_GALLERY
    3,  // ENTR_RETURN_2
    5,  // ENTR_RETURN_BAZAAR
    7,  // ENTR_RETURN_4
    0,  // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
};

s32 Player_HandleExitsAndVoids(PlayState* play, Player* this, CollisionPoly* poly, u32 bgId) {
    s32 exitIndex;
    s32 temp;
    s32 sp34;
    f32 speedXZ;
    s32 yaw;

    if (this->actor.category == ACTORCAT_PLAYER) {
        exitIndex = 0;

        if (!(this->stateFlags1 & PLAYER_STATE1_IN_DEATH_CUTSCENE) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
            (this->csMode == PLAYER_CSMODE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
            (((poly != NULL) && (exitIndex = SurfaceType_GetExitIndex(&play->colCtx, poly, bgId), exitIndex != 0)) ||
             (Player_IsFloorSinking(sFloorType) && (this->floorProperty == FLOOR_PROPERTY_12)))) {

            sp34 = this->sceneExitPosY - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 &
                  (PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (sp34 < 100) && (sYDistToFloor > 100.0f)) {
                return 0;
            }

            if (exitIndex == 0) {
                Play_TriggerVoidOut(play);
                Scene_SetTransitionForNextEntrance(play);
            } else {
                play->nextEntranceIndex = play->exitList[exitIndex - 1];

                if (play->nextEntranceIndex == ENTR_RETURN_GROTTO) {
                    gSaveContext.respawnFlag = 2;
                    play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                } else if (play->nextEntranceIndex >= ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS) {
                    play->nextEntranceIndex =
                        sReturnEntranceGroupData[sReturnEntranceGroupIndices[play->nextEntranceIndex -
                                                                             ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS] +
                                                 play->spawn];
                    Scene_SetTransitionForNextEntrance(play);
                } else {
                    if (SurfaceType_GetFloorEffect(&play->colCtx, poly, bgId) == FLOOR_EFFECT_2) {
                        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = play->nextEntranceIndex;
                        Play_TriggerVoidOut(play);
                        gSaveContext.respawnFlag = -2;
                    }

                    gSaveContext.retainWeatherMode = true;
                    Scene_SetTransitionForNextEntrance(play);
                }

                play->transitionTrigger = TRANS_TRIGGER_START;
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_RIDING_HORSE | PLAYER_STATE1_IN_CUTSCENE)) &&
                !(this->stateFlags2 & PLAYER_STATE2_CRAWLING) && !Player_IsFreeSwimming(this) &&
                (temp = SurfaceType_GetFloorType(&play->colCtx, poly, bgId), (temp != FLOOR_TYPE_10)) &&
                ((sp34 < 100) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {

                if (temp == FLOOR_TYPE_11) {
                    func_800788CC(NA_SE_OC_SECRET_HOLE_OUT);
                    func_800F6964(5);
                    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
                } else {
                    speedXZ = this->speedXZ;

                    if (speedXZ < 0.0f) {
                        this->actor.world.rot.y += 0x8000;
                        speedXZ = -speedXZ;
                    }

                    if (speedXZ > R_RUN_SPEED_LIMIT / 100.0f) {
                        gSaveContext.entranceSpeed = R_RUN_SPEED_LIMIT / 100.0f;
                    } else {
                        gSaveContext.entranceSpeed = speedXZ;
                    }

                    if (sConveyorSpeed != CONVEYOR_SPEED_DISABLED) {
                        yaw = sConveyorYaw;
                    } else {
                        yaw = this->actor.world.rot.y;
                    }
                    Player_SetupMiniCutscene(play, this, 400.0f, yaw);
                }
            } else {
                if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    Player_SetHorizontalSpeedToZero(this);
                }
            }

            this->stateFlags1 |= PLAYER_STATE1_EXITING_SCENE | PLAYER_STATE1_IN_CUTSCENE;

            func_80835E44(play, CAM_SET_SCENE_TRANSITION);

            return true;
        } else {
            if (play->transitionTrigger == TRANS_TRIGGER_OFF) {

                if ((this->actor.world.pos.y < -4000.0f) ||
                    (((this->floorProperty == FLOOR_PROPERTY_5) || (this->floorProperty == FLOOR_PROPERTY_12)) &&
                     ((sYDistToFloor < 100.0f) || (this->fallDistance > 400.0f) ||
                      ((play->sceneId != SCENE_SHADOW_TEMPLE) && (this->fallDistance > 200.0f)))) ||
                    ((play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) && (this->fallDistance > 320.0f))) {

                    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        if (this->floorProperty == FLOOR_PROPERTY_5) {
                            Play_TriggerRespawn(play);
                        } else {
                            Play_TriggerVoidOut(play);
                        }
                        play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                        Audio_PlaySfx(NA_SE_OC_ABYSS);
                    } else {
                        Player_SetupEnterGrotto(play, this);
                        this->actionVar16 = 9999;
                        if (this->floorProperty == FLOOR_PROPERTY_5) {
                            this->actionVar8 = -1;
                        } else {
                            this->actionVar8 = 1;
                        }
                    }
                }

                this->sceneExitPosY = this->actor.world.pos.y;
            }
        }
    }

    return false;
}

/**
 * Gets a position relative to player's yaw.
 * An offset is applied to the provided base position in the direction of shape y rotation.
 * The resulting position is stored in `dest`
 */
void Player_GetRelativePosition(Player* this, Vec3f* base, Vec3f* offset, Vec3f* dest) {
    f32 cos = Math_CosS(this->actor.shape.rot.y);
    f32 sin = Math_SinS(this->actor.shape.rot.y);

    dest->x = base->x + ((offset->x * cos) + (offset->z * sin));
    dest->y = base->y + offset->y;
    dest->z = base->z + ((offset->z * cos) - (offset->x * sin));
}

Actor* Player_SpawnFairy(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, s32 type) {
    Vec3f pos;

    Player_GetRelativePosition(this, arg2, arg3, &pos);

    return Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, pos.x, pos.y, pos.z, 0, 0, 0, type);
}

f32 Player_PosVsFloorLineTestImpl(PlayState* play, Player* this, Vec3f* offset, Vec3f* result,
                                  CollisionPoly** floorPoly, s32* floorBgId) {
    Player_GetRelativePosition(this, &this->actor.world.pos, offset, result);

    return BgCheck_EntityRaycastDown3(&play->colCtx, floorPoly, floorBgId, result);
}

f32 Player_PosVsFloorLineTest(PlayState* play, Player* this, Vec3f* offset, Vec3f* result) {
    CollisionPoly* floorPoly;
    s32 floorBgId;

    return Player_PosVsFloorLineTestImpl(play, this, offset, result, &floorPoly, &floorBgId);
}

/**
 * Checks if a line between the player's position and the provided `offset` intersect a wall.
 *
 * Point A of the line is at player's world position offset by the height provided in `offset`.
 * Point B of the line is at player's world position offset by the entire `offset` vector.
 * Point A and B are always at the same height, meaning this is a horizontal line test.
 */
s32 Player_PosVsWallLineTest(PlayState* play, Player* this, Vec3f* offset, CollisionPoly** wallPoly, s32* bgId,
                             Vec3f* result) {
    Vec3f posA;
    Vec3f posB;

    posA.x = this->actor.world.pos.x;
    posA.y = this->actor.world.pos.y + offset->y;
    posA.z = this->actor.world.pos.z;

    Player_GetRelativePosition(this, &this->actor.world.pos, offset, &posB);

    return BgCheck_EntityLineTest1(&play->colCtx, &posA, &posB, result, wallPoly, true, false, false, true, bgId);
}

s32 Player_SwapAction_TryOpeningDoor(Player* this, PlayState* play) {
    SlidingDoorActorBase* slidingDoor;
    DoorActorBase* door;
    s32 doorDirection;
    f32 sp78;
    f32 sp74;
    Actor* doorActor;
    f32 sp6C;
    s32 pad3;
    s32 frontRoom;
    Actor* attachedActor;
    PlayerAnimationHeader* sp5C;
    CollisionPoly* groundPoly;
    Vec3f checkPos;

    if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
        (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
         ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)))) {
        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) || (Player_Action_SpawnFromDoor == this->actionFunc)) {
            doorActor = this->doorActor;

            if (this->doorType <= PLAYER_DOORTYPE_AJAR) {
                doorActor->textId = 0xD0;
                Player_SetupTalk(play, doorActor);
                return false;
            }

            doorDirection = this->doorDirection;
            sp78 = Math_CosS(doorActor->shape.rot.y);
            sp74 = Math_SinS(doorActor->shape.rot.y);

            if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                slidingDoor = (SlidingDoorActorBase*)doorActor;

                this->yaw = slidingDoor->dyna.actor.home.rot.y;
                if (doorDirection > 0) {
                    this->yaw -= 0x8000;
                }
                this->actor.shape.rot.y = this->yaw;

                if (this->speedXZ <= 0.0f) {
                    this->speedXZ = 0.1f;
                }

                Player_SetupMiniCutscene(play, this, 50.0f, this->actor.shape.rot.y);

                this->actionVar8 = 0;
                this->unk_447 = this->doorType;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->miniCsPosTarget.x = this->actor.world.pos.x + ((doorDirection * 20.0f) * sp74);
                this->miniCsPosTarget.z = this->actor.world.pos.z + ((doorDirection * 20.0f) * sp78);
                this->miniCsEndPos.x = this->actor.world.pos.x + ((doorDirection * -120.0f) * sp74);
                this->miniCsEndPos.z = this->actor.world.pos.z + ((doorDirection * -120.0f) * sp78);

                slidingDoor->isActive = true;
                Player_ClearAttentionModeAndStopMoving(this);

                if (this->doorTimer != 0) {
                    this->actionVar16 = 0;
                    Player_Anim_PlayOnceWithMorph(play, this, Player_GetIdleAnim(this));
                    this->skelAnime.endFrame = 0.0f;
                } else {
                    this->speedXZ = 0.1f;
                }

                if (slidingDoor->dyna.actor.category == ACTORCAT_DOOR) {
                    this->doorBgCamIndex =
                        play->transiActorCtx.list[GET_TRANSITION_ACTOR_INDEX(&slidingDoor->dyna.actor)]
                            .sides[(doorDirection > 0) ? 0 : 1]
                            .bgCamIndex;

                    Actor_DisableLens(play);
                }
            } else {
                // The door actor can be either EnDoor or DoorKiller.
                door = (DoorActorBase*)doorActor;

                door->openAnim = (doorDirection < 0.0f)
                                     ? (LINK_IS_ADULT ? DOOR_OPEN_ANIM_ADULT_L : DOOR_OPEN_ANIM_CHILD_L)
                                     : (LINK_IS_ADULT ? DOOR_OPEN_ANIM_ADULT_R : DOOR_OPEN_ANIM_CHILD_R);

                if (door->openAnim == DOOR_OPEN_ANIM_ADULT_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA_free, this->modelAnimType);
                } else if (door->openAnim == DOOR_OPEN_ANIM_CHILD_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA, this->modelAnimType);
                } else if (door->openAnim == DOOR_OPEN_ANIM_ADULT_R) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB_free, this->modelAnimType);
                } else {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB, this->modelAnimType);
                }

                Player_SetAction(play, this, Player_Action_OpenDoor, 0);
                Player_UnequipItem(play, this);

                if (doorDirection < 0) {
                    this->actor.shape.rot.y = doorActor->shape.rot.y;
                } else {
                    this->actor.shape.rot.y = doorActor->shape.rot.y - 0x8000;
                }

                this->yaw = this->actor.shape.rot.y;

                sp6C = (doorDirection * 22.0f);
                this->actor.world.pos.x = doorActor->world.pos.x + sp6C * sp74;
                this->actor.world.pos.z = doorActor->world.pos.z + sp6C * sp78;

                Player_Anim_PlayOnceWaterAdjusted(play, this, sp5C);

                if (this->doorTimer != 0) {
                    this->skelAnime.endFrame = 0.0f;
                }

                Player_ClearAttentionModeAndStopMoving(this);
                Player_AnimReplace_Setup(play, this, 0x28F);

                // If this door is the second half of a double door (spawned as child)
                if (doorActor->parent != NULL) {
                    doorDirection = -doorDirection;
                }

                door->playerIsOpening = true;

                // If the door actor is not DoorKiller
                if (this->doorType != PLAYER_DOORTYPE_FAKE) {
                    // The door actor is EnDoor

                    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                    Actor_DisableLens(play);

                    if (ENDOOR_GET_TYPE(doorActor) == DOOR_SCENEEXIT) {
                        checkPos.x = doorActor->world.pos.x - (sp6C * sp74);
                        checkPos.y = doorActor->world.pos.y + 10.0f;
                        checkPos.z = doorActor->world.pos.z - (sp6C * sp78);

                        BgCheck_EntityRaycastDown1(&play->colCtx, &groundPoly, &checkPos);

                        //! @bug groundPoly's bgId is not guaranteed to be BGCHECK_SCENE
                        if (Player_HandleExitsAndVoids(play, this, groundPoly, BGCHECK_SCENE)) {
                            gSaveContext.entranceSpeed = 2.0f;
                            gSaveContext.entranceSound = NA_SE_OC_DOOR_OPEN;
                        }
                    } else {
                        Camera_ChangeDoorCam(Play_GetCamera(play, CAM_ID_MAIN), doorActor,
                                             play->transiActorCtx.list[GET_TRANSITION_ACTOR_INDEX(doorActor)]
                                                 .sides[(doorDirection > 0) ? 0 : 1]
                                                 .bgCamIndex,
                                             0, 38.0f * D_808535EC, 26.0f * D_808535EC, 10.0f * D_808535EC);
                    }
                }
            }

            if ((this->doorType != PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR)) {
                frontRoom = play->transiActorCtx.list[GET_TRANSITION_ACTOR_INDEX(doorActor)]
                                .sides[(doorDirection > 0) ? 0 : 1]
                                .room;

                if ((frontRoom >= 0) && (frontRoom != play->roomCtx.curRoom.num)) {
                    func_8009728C(play, &play->roomCtx, frontRoom);
                }
            }

            doorActor->room = play->roomCtx.curRoom.num;

            if (((attachedActor = doorActor->child) != NULL) || ((attachedActor = doorActor->parent) != NULL)) {
                attachedActor->room = play->roomCtx.curRoom.num;
            }

            return true;
        }
    }

    return false;
}

void Player_Setup1_IdleLockOnEnemy(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;

    Player_SetAction(play, this, Player_Action_IdleLockOnEnemy, 1);

    if (this->leftRightBlendWeight < 0.5f) {
        anim = Player_GetWaitRightAnim(this);
        this->leftRightBlendWeight = 0.0f;
    } else {
        anim = Player_GetWaitLeftAnim(this);
        this->leftRightBlendWeight = 1.0f;
    }

    this->leftRightBlendWeightTarget = this->leftRightBlendWeight;
    Player_Anim_PlayLoop(play, this, anim);
    this->yaw = this->actor.shape.rot.y;
}

void Player_Setup1_IdleZParallelOrLockOnFriend(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_IdleZParallelOrLockOnFriend, 1);
    Player_Anim_PlayOnceWithMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void Player_Setup1_IdleAll(Player* this, PlayState* play) {
    if (Player_IsEnemyLockOn(this)) {
        Player_Setup1_IdleLockOnEnemy(this, play);
    } else if (Player_IsZParallelOrLockOnFriend(this)) {
        Player_Setup1_IdleZParallelOrLockOnFriend(this, play);
    } else {
        Player_SetupIdleWithMorph(this, play);
    }
}

void Player_Setup2_IdleAll(Player* this, PlayState* play) {
    PlayerActionFunc func;

    if (Player_IsEnemyLockOn(this)) {
        func = Player_Action_IdleLockOnEnemy;
    } else if (Player_IsZParallelOrLockOnFriend(this)) {
        func = Player_Action_IdleZParallelOrLockOnFriend;
    } else {
        func = Player_Action_Idle;
    }

    Player_SetAction(play, this, func, 1);
}

void Player_Setup3_IdleAll(Player* this, PlayState* play) {
    Player_Setup2_IdleAll(this, play);
    if (Player_IsEnemyLockOn(this)) {
        this->actionVar16 = 1;
    }
}

void Player_Setup4_IdleAll(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    Player_Setup3_IdleAll(this, play);
    Player_Anim_PlayOnceWaterAdjusted(play, this, anim);
}

s32 Player_CanLift(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL);
}

void Player_CsIntoAction_SetupLift(PlayState* play, Player* this) {
    if (Player_CanLift(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;

        if (interactActorId == ACTOR_BG_TOKI_SWD) {
            this->interactRangeActor->parent = &this->actor;
            Player_SetAction(play, this, Player_Action_StartCutsceneDelayed, 0);
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
        } else {
            PlayerAnimationHeader* anim;

            if (interactActorId == ACTOR_BG_HEAVY_BLOCK) {
                Player_SetAction(play, this, Player_Action_LiftThrowHeavyBlock, 0);
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                anim = &gPlayerAnim_link_normal_heavy_carry;
            } else if ((interactActorId == ACTOR_EN_ISHI) && ((interactRangeActor->params & 0xF) == 1)) {
                Player_SetAction(play, this, Player_Action_LiftSilverRock, 0);
                anim = &gPlayerAnim_link_silver_carry;
            } else if (((interactActorId == ACTOR_EN_BOMBF) || (interactActorId == ACTOR_EN_KUSA)) &&
                       (Player_GetStrength() <= PLAYER_STR_NONE)) {
                Player_SetAction(play, this, Player_Action_LiftFail, 0);
                this->actor.world.pos.x =
                    (Math_SinS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
                this->actor.world.pos.z =
                    (Math_CosS(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;
                this->yaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
                anim = &gPlayerAnim_link_normal_nocarry_free;
            } else {
                Player_SetAction(play, this, Player_Action_Lift, 0);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_carryB, this->modelAnimType);
            }

            Player_Anim_PlayOnce(play, this, anim);
        }
    } else {
        Player_Setup1_IdleAll(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
    }
}

void Player_CsIntoAction_SetupTalk(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_Talk, 0);

    this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;

    if (this->actor.textId != 0) {
        Message_StartTextbox(play, this->actor.textId, this->talkActor);
        this->lockOnActor = this->talkActor;
    }
}

void Player_CsIntoAction_SetupRideHorse(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_RideHorse, 0);
}

void Player_CsIntoAction_SetupGrabWall(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_GrabWall, 0);
}

void Player_CsIntoAction_SetupClimb(PlayState* play, Player* this) {
    s32 sp1C = this->actionVar16;
    s32 sp18 = this->actionVar8;

    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_ClimbWall, 0);
    this->actor.velocity.y = 0.0f;

    this->actionVar16 = sp1C;
    this->actionVar8 = sp18;
}

void Player_CsIntoAction_SetupCrawl(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_Crawl, 0);
}

void Player_CsIntoAction_SetupGetItem(PlayState* play, Player* this) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_GetItem, 0);

    this->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_CUTSCENE;

    if (this->getItemId == GI_HEART_CONTAINER_2) {
        this->getItemVar16 = 20;
    } else if (this->getItemId >= 0) {
        this->getItemVar16 = 1;
    } else {
        this->getItemId = -this->getItemId;
    }
}

s32 Player_Setup3_Jump(Player* this, PlayState* play) {
    s16 yawDiff;
    PlayerAnimationHeader* anim;
    f32 jumpVelocityY;

    yawDiff = this->yaw - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x1000) && (this->speedXZ > 4.0f)) {
        anim = &gPlayerAnim_link_normal_run_jump;
    } else {
        anim = &gPlayerAnim_link_normal_jump;
    }

    if (this->speedXZ > (R_PLAYER_BOOX_12 / 100.0f)) {
        jumpVelocityY = R_PLAYER_BOOX_13 / 100.0f;
    } else {
        jumpVelocityY = (R_PLAYER_BOOX_14 / 100.0f) + ((R_PLAYER_BOOX_15 * this->speedXZ) / 1000.0f);
    }

    Player_Setup1_Jump(this, anim, jumpVelocityY, play, NA_SE_VO_LI_AUTO_JUMP);
    this->midairVar16 = 1;

    return true;
}

void Player_SetupGrabLedge(PlayState* play, Player* this, CollisionPoly* arg2, f32 distToInteractWall,
                           PlayerAnimationHeader* arg4) {
    f32 nx = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 nz = COLPOLY_GET_NORMAL(arg2->normal.z);

    Player_SetAction(play, this, Player_Action_GrabLedge, 0);
    Player_DetachHeldActorAndResetAttributes(play, this);
    Player_Anim_PlayOnce(play, this, arg4);

    this->actor.world.pos.x -= (distToInteractWall + 1.0f) * nx;
    this->actor.world.pos.z -= (distToInteractWall + 1.0f) * nz;
    this->actor.shape.rot.y = this->yaw = Math_Atan2S(nz, nx);

    Player_ClearAttentionModeAndStopMoving(this);
    Player_Anim_ResetTranslation(this);
}

s32 Player_TryGrabbingLedge(Player* this, PlayState* play) {
    CollisionPoly* sp84;
    s32 sp80;
    Vec3f sp74;
    Vec3f sp68;
    f32 temp1;

    //! @bug `floorPitch` and `floorPitchAlt` are cleared to 0 before this function is called, because the player
    //! left the ground. The angles will always be zero and therefore will always pass these checks.
    //! The intention seems to be to prevent ledge hanging or vine grabbing when walking off of a steep enough slope.
    if ((this->actor.depthInWater < -80.0f) && (ABS(this->floorPitch) < 2730) && (ABS(this->floorPitchAlt) < 2730)) {
        sp74.x = this->actor.prevPos.x - this->actor.world.pos.x;
        sp74.z = this->actor.prevPos.z - this->actor.world.pos.z;

        temp1 = sqrtf(SQ(sp74.x) + SQ(sp74.z));
        if (temp1 != 0.0f) {
            temp1 = 5.0f / temp1;
        } else {
            temp1 = 0.0f;
        }

        sp74.x = this->actor.prevPos.x + (sp74.x * temp1);
        sp74.y = this->actor.world.pos.y;
        sp74.z = this->actor.prevPos.z + (sp74.z * temp1);

        if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp74, &sp68, &sp84, true, false, false,
                                    true, &sp80) &&
            (ABS(sp84->normal.y) < 600)) {
            f32 nx = COLPOLY_GET_NORMAL(sp84->normal.x);
            f32 ny = COLPOLY_GET_NORMAL(sp84->normal.y);
            f32 nz = COLPOLY_GET_NORMAL(sp84->normal.z);
            f32 distToInteractWall;
            s32 sp50;

            distToInteractWall = Math3D_UDistPlaneToPos(nx, ny, nz, sp84->dist, &this->actor.world.pos);

            sp50 = (sPrevFloorProperty == FLOOR_PROPERTY_6);
            if (!sp50 && (SurfaceType_GetWallFlags(&play->colCtx, sp84, sp80) & WALL_FLAG_3)) {
                sp50 = 1;
            }

            Player_SetupGrabLedge(play, this, sp84, distToInteractWall,
                                  sp50 ? &gPlayerAnim_link_normal_Fclimb_startB : &gPlayerAnim_link_normal_fall);

            if (sp50) {
                Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupClimb);

                this->yaw += 0x8000;
                this->actor.shape.rot.y = this->yaw;

                this->stateFlags1 |= PLAYER_STATE1_CLIMBING;
                Player_AnimReplace_Setup(play, this, 0x9F);

                this->actionVar16 = -1;
                this->actionVar8 = sp50;
            } else {
                this->stateFlags1 |= PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP;
                this->stateFlags1 &= ~PLAYER_STATE1_Z_PARALLEL;
            }

            Player_PlaySfx(this, NA_SE_PL_SLIPDOWN);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
            return true;
        }
    }

    return false;
}

void Player_SetupClimbLedge(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    Player_SetAction(play, this, Player_Action_ClimbLedge, 0);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, 1.3f);
}

static Vec3f D_8085451C = { 0.0f, 0.0f, 100.0f };

void func_8083AA10(Player* this, PlayState* play) {
    s32 sp5C;
    CollisionPoly* sp58;
    s32 sp54;
    WaterBox* sp50;
    Vec3f sp44;
    f32 sp40;
    f32 sp3C;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;

    if (!(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)) &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (!Player_TryEnteringGrotto(play, this)) {
            if (sPrevFloorProperty == FLOOR_PROPERTY_8) {
                this->actor.world.pos.x = this->actor.prevPos.x;
                this->actor.world.pos.z = this->actor.prevPos.z;
                return;
            }

            if (!(this->stateFlags3 & PLAYER_STATE3_MIDAIR) && !(this->skelAnime.moveFlags & 0x80) &&
                (Player_Action_Midair != this->actionFunc) && (Player_Action_FallDive != this->actionFunc)) {

                if ((sPrevFloorProperty == FLOOR_PROPERTY_7) || (this->meleeWeaponState != 0)) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    Player_SetHorizontalSpeedToZero(this);
                    return;
                }

                if (this->hoverBootsTimer != 0) {
                    this->actor.velocity.y = 1.0f;
                    sPrevFloorProperty = FLOOR_PROPERTY_9;
                    return;
                }

                sp5C = (s16)(this->yaw - this->actor.shape.rot.y);

                Player_SetAction(play, this, Player_Action_Midair, 1);
                Player_ResetAttributes(play, this);

                this->floorSfxOffset = this->prevFloorSfxOffset;

                if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_LEAVE) &&
                    !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) && (sPrevFloorProperty != FLOOR_PROPERTY_6) &&
                    (sPrevFloorProperty != FLOOR_PROPERTY_9) && (sYDistToFloor > 20.0f) &&
                    (this->meleeWeaponState == 0) && (ABS(sp5C) < 0x2000) && (this->speedXZ > 3.0f)) {

                    if ((sPrevFloorProperty == FLOOR_PROPERTY_11) &&
                        !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {

                        sp40 = Player_PosVsFloorLineTestImpl(play, this, &D_8085451C, &sp44, &sp58, &sp54);
                        sp3C = this->actor.world.pos.y;

                        if (WaterBox_GetSurface1(play, &play->colCtx, sp44.x, sp44.z, &sp3C, &sp50) &&
                            ((sp3C - sp40) > 50.0f)) {
                            Player_Setup2_Jump(this, &gPlayerAnim_link_normal_run_jump_water_fall, 6.0f, play);
                            Player_SetAction(play, this, Player_Action_FallDive, 0);
                            return;
                        }
                    }

                    Player_Setup3_Jump(this, play);
                    return;
                }

                if ((sPrevFloorProperty == FLOOR_PROPERTY_9) || (sYDistToFloor <= this->ageProperties->unk_34) ||
                    !Player_TryGrabbingLedge(this, play)) {
                    Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_landing_wait);
                    return;
                }
            }
        }
    } else {
        this->fallStartHeight = this->actor.world.pos.y;
    }
}

s32 Player_TryFirstPersonCameraMode(PlayState* play, Player* this) {
    s32 camMode;

    if (this->attentionMode == PLAYER_ATTENTIONMODE_AIMING) {
        if (Player_IsUsingFpsItem(this)) {
            if (LINK_IS_ADULT) {
                camMode = CAM_MODE_AIM_ADULT;
            } else {
                camMode = CAM_MODE_AIM_CHILD;
            }
        } else {
            camMode = CAM_MODE_AIM_BOOMERANG;
        }
    } else {
        camMode = CAM_MODE_FIRST_PERSON;
    }

    return Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), camMode);
}

s32 Player_TryStartingCutscene(PlayState* play, Player* this) {
    if (this->attentionMode == PLAYER_ATTENTIONMODE_CUTSCENE) {
        Player_SetAction(play, this, Player_Action_StartCutscene, 0);
        if (this->doorBgCamIndex != 0) {
            this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
        }
        Player_ResetAttack(this);
        return true;
    }

    return false;
}

void Player_LoadGetItemObject(Player* this, s16 objectId) {
    s32 pad;
    u32 size;

    if (objectId != OBJECT_INVALID) {
        this->giObjectLoading = true;
        osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);

        size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

        LOG_HEX("size", size, "../z_player.c", 9090);
        ASSERT(size <= 1024 * 8, "size <= 1024 * 8", "../z_player.c", 9091);

        DmaMgr_RequestAsync(&this->giObjectDmaRequest, this->giObjectSegment, gObjectTable[objectId].vromStart, size, 0,
                            &this->giObjectLoadQueue, NULL, "../z_player.c", 9099);
    }
}

void Player_SetupMagicSpell(PlayState* play, Player* this, s32 magicSpell) {
    Player_SetAction_PreserveItemAction(play, this, Player_Action_CastMagicSpell, 0);

    this->actionVar8 = magicSpell - 3;
    Magic_RequestChange(play, sMagicSpellCosts[magicSpell], MAGIC_CONSUME_WAIT_PREVIEW);

    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f);

    if (magicSpell == 5) {
        this->subCamId = OnePointCutscene_Init(play, 1100, -101, NULL, CAM_ID_MAIN);
    } else {
        Player_TurnAroundCamera(play, 10);
    }
}

void Player_SetRotToZero(Player* this) {
    this->actor.focus.rot.x = this->actor.focus.rot.z = this->headLimbRot.x = this->headLimbRot.y =
        this->headLimbRot.z = this->upperLimbRot.x = this->upperLimbRot.y = this->upperLimbRot.z = 0;

    this->actor.focus.rot.y = this->actor.shape.rot.y;
}

static u8 D_80854528[] = {
    GI_ZELDAS_LETTER,       // EXCH_ITEM_ZELDAS_LETTER
    GI_WEIRD_EGG,           // EXCH_ITEM_WEIRD_EGG
    GI_CHICKEN,             // EXCH_ITEM_CHICKEN
    GI_MAGIC_BEAN,          // EXCH_ITEM_MAGIC_BEAN
    GI_POCKET_EGG,          // EXCH_ITEM_POCKET_EGG
    GI_POCKET_CUCCO,        // EXCH_ITEM_POCKET_CUCCO
    GI_COJIRO,              // EXCH_ITEM_COJIRO
    GI_ODD_MUSHROOM,        // EXCH_ITEM_ODD_MUSHROOM
    GI_ODD_POTION,          // EXCH_ITEM_ODD_POTION
    GI_POACHERS_SAW,        // EXCH_ITEM_POACHERS_SAW
    GI_BROKEN_GORONS_SWORD, // EXCH_ITEM_BROKEN_GORONS_SWORD
    GI_PRESCRIPTION,        // EXCH_ITEM_PRESCRIPTION
    GI_EYEBALL_FROG,        // EXCH_ITEM_EYEBALL_FROG
    GI_EYE_DROPS,           // EXCH_ITEM_EYE_DROPS
    GI_CLAIM_CHECK,         // EXCH_ITEM_CLAIM_CHECK
    GI_MASK_SKULL,          // EXCH_ITEM_MASK_SKULL
    GI_MASK_SPOOKY,         // EXCH_ITEM_MASK_SPOOKY
    GI_MASK_KEATON,         // EXCH_ITEM_MASK_KEATON
    GI_MASK_BUNNY_HOOD,     // EXCH_ITEM_MASK_BUNNY_HOOD
    GI_MASK_TRUTH,          // EXCH_ITEM_MASK_TRUTH
    GI_MASK_GORON,          // EXCH_ITEM_MASK_GORON
    GI_MASK_ZORA,           // EXCH_ITEM_MASK_ZORA
    GI_MASK_GERUDO,         // EXCH_ITEM_MASK_GERUDO
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_FISH
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BLUE_FIRE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BUG
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_POE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BIG_POE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_RUTOS_LETTER
};

static PlayerAnimationHeader* D_80854548[] = {
    &gPlayerAnim_link_normal_give_other,
    &gPlayerAnim_link_bottle_read,
    &gPlayerAnim_link_normal_take_out,
};

s32 Player_SwapAction_TryItemCsFirstPerson(Player* this, PlayState* play) {
    s32 sp2C;
    s32 sp28;
    GetItemEntry* giEntry;
    Actor* talkActor;

    if ((this->attentionMode != PLAYER_ATTENTIONMODE_NONE) &&
        (Player_IsFreeSwimming(this) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
         (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE))) {

        if (!Player_TryStartingCutscene(play, this)) {
            if (this->attentionMode == PLAYER_ATTENTIONMODE_ITEM_CUTSCENE) {
                sp2C = Player_MagicSpellFromIA(this, this->itemAction);
                if (sp2C >= 0) {
                    if ((sp2C != 3) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        Player_SetupMagicSpell(play, this, sp2C);
                    } else {
                        Player_SetAction(play, this, Player_Action_FaroresWindChoice, 1);
                        this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
                        Player_TurnAroundCamera(play, 4);
                    }

                    Player_ClearAttentionModeAndStopMoving(this);
                    return true;
                }

                sp2C = this->itemAction - PLAYER_IA_ZELDAS_LETTER;
                if (
                    // Option 1:
                    (sp2C >= 0) ||
                    // Option 2:
                    (sp28 = Player_BottleFromIA(this, this->itemAction) - 1,
                     (
                         // Sub Requirement 1:
                         (sp28 >= 0) && (sp28 < 6) &&
                         // Sub Requirement 2:
                         ((this->itemAction > PLAYER_IA_BOTTLE_POE) ||
                          ((this->talkActor != NULL) && (((this->itemAction == PLAYER_IA_BOTTLE_POE) &&
                                                          (this->exchangeItemId == EXCH_ITEM_BOTTLE_POE)) ||
                                                         (this->exchangeItemId == EXCH_ITEM_BOTTLE_BLUE_FIRE))))))) {

                    if ((play->actorCtx.titleCtx.delayTimer == 0) && (play->actorCtx.titleCtx.alpha == 0)) {
                        Player_SetAction_PreserveItemAction(play, this, Player_Action_ExchangeItem, 0);

                        if (sp2C >= 0) {
                            giEntry = &sGetItemTable[D_80854528[sp2C] - 1];
                            Player_LoadGetItemObject(this, giEntry->objectId);
                        }

                        this->stateFlags1 |=
                            PLAYER_STATE1_TALKING | PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;

                        if (sp2C >= 0) {
                            sp2C = sp2C + 1;
                        } else {
                            sp2C = sp28 + 0x18;
                        }

                        talkActor = this->talkActor;

                        if ((talkActor != NULL) &&
                            ((this->exchangeItemId == sp2C) || (this->exchangeItemId == EXCH_ITEM_BOTTLE_BLUE_FIRE) ||
                             ((this->exchangeItemId == EXCH_ITEM_BOTTLE_POE) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BIG_POE)) ||
                             ((this->exchangeItemId == EXCH_ITEM_MAGIC_BEAN) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BUG))) &&

                            ((this->exchangeItemId != EXCH_ITEM_MAGIC_BEAN) ||
                             (this->itemAction == PLAYER_IA_MAGIC_BEAN))) {
                            if (this->exchangeItemId == EXCH_ITEM_MAGIC_BEAN) {
                                Inventory_ChangeAmmo(ITEM_MAGIC_BEAN, -1);
                                Player_SetAction_PreserveItemAction(play, this, Player_Action_PlantMagicBeans, 0);
                                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
                                this->plantMagicBeansVar16 = 80;
                                this->plantMagicBeansVar8 = -1;
                            }
                            talkActor->flags |= ACTOR_FLAG_TALK_REQUESTED;
                            this->lockOnActor = this->talkActor;
                        } else if (sp2C == EXCH_ITEM_BOTTLE_RUTOS_LETTER) {
                            this->actionVar8 = 1;
                            this->actor.textId = 0x4005;
                            Player_TurnAroundCamera(play, 1);
                        } else {
                            this->actionVar8 = 2;
                            this->actor.textId = 0xCF;
                            Player_TurnAroundCamera(play, 4);
                        }

                        this->actor.flags |= ACTOR_FLAG_TALK_REQUESTED;
                        this->exchangeItemId = sp2C;

                        if (this->actionVar8 < 0) {
                            Player_Anim_PlayOnceWithMorph(play, this,
                                                          GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check, this->modelAnimType));
                        } else {
                            Player_Anim_PlayOnce(play, this, D_80854548[this->actionVar8]);
                        }

                        Player_ClearAttentionModeAndStopMoving(this);
                    }
                    return true;
                }

                sp2C = Player_BottleFromIA(this, this->itemAction);
                if (sp2C >= 0) {
                    if (sp2C == 0xC) {
                        Player_SetAction_PreserveItemAction(play, this, Player_Action_ReleaseFairyFromBottle, 0);
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_bug_out);
                        Player_TurnAroundCamera(play, 3);
                    } else if ((sp2C > 0) && (sp2C < 4)) {
                        Player_SetAction_PreserveItemAction(play, this, Player_Action_DropItemFromBottle, 0);
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_bottle_fish_out);
                        Player_TurnAroundCamera(play, (sp2C == 1) ? 1 : 5);
                    } else {
                        Player_SetAction_PreserveItemAction(play, this, Player_Action_DrinkFromBottle, 0);
                        Player_Anim_PlayOnceAdjustedWithMorph(play, this, &gPlayerAnim_link_bottle_drink_demo_start);
                        Player_TurnAroundCamera(play, 2);
                    }
                } else {
                    Player_SetAction_PreserveItemAction(play, this, Player_Action_PlayOcarina, 0);
                    Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_okarina_start);
                    this->stateFlags2 |= PLAYER_STATE2_OCARINA_ON;
                    Player_TurnAroundCamera(play, (this->ocarinaActor != NULL) ? 0x5B : 0x5A);
                    if (this->ocarinaActor != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_OCARINA_ON_FOR_ACTOR;
                        Camera_SetViewParam(Play_GetCamera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->ocarinaActor);
                    }
                }
            } else if (Player_TryFirstPersonCameraMode(play, this) != CAM_MODE_NORMAL) {
                if (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
                    Player_SetAction(play, this, Player_Action_AimFirstPerson, 1);
                    this->aimFirstPersonVar16 = 13;
                    Player_SetRotToZero(this);
                }
                this->stateFlags1 |= PLAYER_STATE1_IN_FIRST_PERSON_MODE;
                Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
                Player_SetHorizontalSpeedToZero(this);
                return true;
            } else {
                this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
                Audio_PlaySfx(NA_SE_SY_ERROR);
                return false;
            }

            this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
        }

        Player_ClearAttentionModeAndStopMoving(this);
        return true;
    }

    return false;
}

s32 Player_SwapAction_TryTalking(Player* this, PlayState* play) {
    Actor* talkActor = this->talkActor;
    Actor* lockOnActor = this->lockOnActor;
    Actor* sp2C = NULL;
    s32 sp28 = 0;
    s32 sp24;

    sp24 = (lockOnActor != NULL) &&
           (CHECK_FLAG_ALL(lockOnActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_CHECK_WITH_NAVI) ||
            (lockOnActor->naviEnemyId != NAVI_ENEMY_NONE));

    if (sp24 || (this->naviTextId != 0)) {
        sp28 = (this->naviTextId < 0) && ((ABS(this->naviTextId) & 0xFF00) != 0x200);
        if (sp28 || !sp24) {
            sp2C = this->naviActor;
            if (sp28) {
                lockOnActor = NULL;
                talkActor = NULL;
            }
        } else {
            sp2C = lockOnActor;
        }
    }

    if ((talkActor != NULL) || (sp2C != NULL)) {
        if ((lockOnActor == NULL) || (lockOnActor == talkActor) || (lockOnActor == sp2C)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                ((this->heldActor != NULL) &&
                 (sp28 || (talkActor == this->heldActor) || (sp2C == this->heldActor) ||
                  ((talkActor != NULL) && (talkActor->flags & ACTOR_FLAG_IMMEDIATE_TALK))))) {
                if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                    (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) ||
                    (Player_IsFreeSwimming(this) && !(this->stateFlags2 & PLAYER_STATE2_DIVING))) {

                    if (talkActor != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_CAN_SPEAK_OR_CHECK;
                        if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) ||
                            (talkActor->flags & ACTOR_FLAG_IMMEDIATE_TALK)) {
                            sp2C = NULL;
                        } else if (sp2C == NULL) {
                            return false;
                        }
                    }

                    if (sp2C != NULL) {
                        if (!sp28) {
                            this->stateFlags2 |= PLAYER_STATE2_NAVI_REQUESTING_TALK;
                        }

                        if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) && !sp28) {
                            return false;
                        }

                        talkActor = sp2C;
                        this->talkActor = NULL;

                        if (sp28 || !sp24) {
                            sp2C->textId = ABS(this->naviTextId);
                        } else {
                            if (sp2C->naviEnemyId != NAVI_ENEMY_NONE) {
                                sp2C->textId = sp2C->naviEnemyId + 0x600;
                            }
                        }
                    }

                    this->currentMask = D_80858AA4;
                    Player_SetupTalk(play, talkActor);

                    return true;
                }
            }
        }
    }

    return false;
}

s32 func_8083B8F4(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_RIDING_HORSE)) &&
        (Camera_CheckValidMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_FIRST_PERSON) != 0)) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
            (Player_IsFreeSwimming(this) && (this->actor.depthInWater < this->ageProperties->unk_2C))) {
            this->attentionMode = PLAYER_ATTENTIONMODE_C_UP;
            return true;
        }
    }

    return false;
}

s32 Player_SwapAction_TryCUp(Player* this, PlayState* play) {
    if (this->attentionMode != PLAYER_ATTENTIONMODE_NONE) {
        Player_SwapAction_TryItemCsFirstPerson(this, play);
        return true;
    }

    if ((this->lockOnActor != NULL) &&
        (CHECK_FLAG_ALL(this->lockOnActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_CHECK_WITH_NAVI) ||
         (this->lockOnActor->naviEnemyId != NAVI_ENEMY_NONE))) {
        this->stateFlags2 |= PLAYER_STATE2_NAVI_REQUESTING_TALK;
    } else if ((this->naviTextId == 0) && !Player_IsEnemyLockOn(this) &&
               CHECK_BTN_ALL(sControlInput->press.button, BTN_CUP) &&
               (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) &&
               (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT) && !func_8083B8F4(this, play)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
    }

    return false;
}

void Player_SetupJumpSlash(PlayState* play, Player* this, s32 arg2, f32 xzSpeed, f32 yVelocity) {
    Player_SetupAttack(play, this, arg2);
    Player_SetAction(play, this, Player_Action_JumpSlash, 0);

    this->stateFlags3 |= PLAYER_STATE3_MIDAIR;

    this->yaw = this->actor.shape.rot.y;
    this->speedXZ = xzSpeed;
    this->actor.velocity.y = yVelocity;

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->hoverBootsTimer = 0;

    Player_AnimSfx_PlayFloorJump(this);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_L);
}

s32 func_8083BB20(Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) && (Player_GetMeleeWeaponHeld(this) != 0)) {
        if (D_80853614 ||
            ((this->actor.category != ACTORCAT_PLAYER) && CHECK_BTN_ALL(sControlInput->press.button, BTN_B))) {
            return true;
        }
    }

    return false;
}

s32 Player_TryJumpSlashing(Player* this, PlayState* play) {
    if (func_8083BB20(this) && (sFloorType != FLOOR_TYPE_7)) {
        Player_SetupJumpSlash(play, this, PLAYER_MWA_JUMPSLASH_START, 3.0f, 4.5f);
        return true;
    }

    return false;
}

void Player_SetupRoll(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Roll, 0);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime,
                                     GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing_roll, this->modelAnimType),
                                     1.25f * sPlayerUnderwaterSpeedAdjustment);
}

s32 Player_TryRolling(Player* this, PlayState* play) {
    if ((this->analogStickDirection4Parts[this->inputFrameCounter] == 0) && (sFloorType != FLOOR_TYPE_7)) {
        Player_SetupRoll(this, play);
        return true;
    }

    return false;
}

void Player_SetupBackflipSidehop(Player* this, PlayState* play, s32 midairVar8) {
    Player_Setup1_Jump(this, D_80853D4C[midairVar8][0], !(midairVar8 & 1) ? 5.8f : 3.5f, play, NA_SE_VO_LI_SWORD_N);

    if (midairVar8) {}

    this->midairVar16 = 1;
    this->midairVar8 = midairVar8;

    this->yaw = this->actor.shape.rot.y + (midairVar8 << 0xE);
    this->speedXZ = !(midairVar8 & 1) ? 6.0f : 8.5f;

    this->stateFlags2 |= PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING;

    Player_PlaySfx(this, ((midairVar8 << 0xE) == 0x8000) ? NA_SE_PL_ROLL : NA_SE_PL_SKIP);
}

s32 Player_SwapAction_TryAButtonActions(Player* this, PlayState* play) {
    s32 sp2C;

    if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) &&
        (play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) && (sFloorType != FLOOR_TYPE_7) &&
        (SurfaceType_GetFloorEffect(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) != FLOOR_EFFECT_1)) {
        sp2C = this->analogStickDirection4Parts[this->inputFrameCounter];

        if (sp2C <= 0) {
            if (Player_IsZTargeting(this)) {
                if (this->actor.category != ACTORCAT_PLAYER) {
                    if (sp2C < 0) {
                        Player_Setup2_Jump(this, &gPlayerAnim_link_normal_jump, R_PLAYER_BOOT_11 / 100.0f, play);
                    } else {
                        Player_SetupRoll(this, play);
                    }
                } else {
                    if ((Player_GetMeleeWeaponHeld(this) != 0) && Player_CanUseItem(this)) {
                        Player_SetupJumpSlash(play, this, PLAYER_MWA_JUMPSLASH_START, 5.0f, 5.0f);
                    } else {
                        Player_SetupRoll(this, play);
                    }
                }
                return true;
            }
        } else {
            Player_SetupBackflipSidehop(this, play, sp2C);
            return true;
        }
    }

    return false;
}

void func_8083BF50(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 sp30;

    sp30 = this->walkCurFrame - 3.0f;
    if (sp30 < 0.0f) {
        sp30 += 29.0f;
    }

    if (sp30 < 14.0f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endL, this->modelAnimType);
        sp30 = 11.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 1.375f * -sp30;
        }
        sp30 /= 11.0f;
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endR, this->modelAnimType);
        sp30 = 26.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 2 * -sp30;
        }
        sp30 /= 12.0f;
    }

    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, 4.0f * sp30);
    this->yaw = this->actor.shape.rot.y;
}

void func_8083C0B8(Player* this, PlayState* play) {
    Player_Setup2_IdleAll(this, play);
    func_8083BF50(this, play);
}

void Player_SetupIdle(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Idle, 1);
    Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

void func_8083C148(Player* this, PlayState* play) {
    if (!(this->stateFlags3 & PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH)) {
        Player_SetRotToZero(this);
        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            Player_SetupSwimIdle(play, this);
        } else {
            Player_Setup1_IdleAll(this, play);
        }
        if (this->attentionMode < PLAYER_ATTENTIONMODE_ITEM_CUTSCENE) {
            this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
        }
    }

    this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                           PLAYER_STATE1_IN_FIRST_PERSON_MODE);
}

// May put away item instead
s32 Player_SwapAction_TryRolling(Player* this, PlayState* play) {
    if (!Player_TryEnemyLockOn(this) && !D_808535E0 && !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
        CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        if (Player_TryRolling(this, play)) {
            return true;
        }
        if ((this->putAwayTimer == 0) && (this->heldItemAction >= PLAYER_IA_SWORD_MASTER)) {
            Player_UseItem(play, this, ITEM_NONE);
        } else {
            this->stateFlags2 ^= PLAYER_STATE2_NAVI_IS_ACTIVE;
        }
    }

    return false;
}

s32 Player_SwapAction_TryShieldingCrouched(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 frame;

    if ((play->shootingGalleryStatus == 0) && (this->currentShield != PLAYER_SHIELD_NONE) &&
        CHECK_BTN_ALL(sControlInput->cur.button, BTN_R) &&
        (Player_IsChildWithHylianShield(this) ||
         (!Player_IsZParallelOrLockOnFriend(this) && (this->lockOnActor == NULL)))) {

        Player_ResetAttack(this);
        Player_DetatchHeldActor(play, this);

        if (Player_SetAction(play, this, Player_Action_ShieldCrouched, 0)) {
            this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;

            if (!Player_IsChildWithHylianShield(this)) {
                Player_SetModelsForHoldingShield(this);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
            } else {
                anim = &gPlayerAnim_clink_normal_defense_ALL;
            }

            if (anim != this->skelAnime.animation) {
                if (Player_IsEnemyLockOn(this)) {
                    this->unk_86C = 1.0f;
                } else {
                    this->unk_86C = 0.0f;
                    Player_ResetLeftRightBlendWeight(this);
                }
                this->upperLimbRot.x = this->upperLimbRot.y = this->upperLimbRot.z = 0;
            }

            frame = Animation_GetLastFrame(anim);
            PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, frame, frame, ANIMMODE_ONCE,
                                   0.0f);

            if (Player_IsChildWithHylianShield(this)) {
                Player_AnimReplace_Setup(play, this, 4);
            }

            Player_PlaySfx(this, NA_SE_IT_SHIELD_POSTURE);
        }

        return true;
    }

    return false;
}

s32 func_8083C484(Player* this, f32* inputVelocity, s16* inputYaw) {
    s16 yaw = this->yaw - *inputYaw;

    if (ABS(yaw) > DEG_TO_BINANG(135.0f)) {
        if (Player_StepHorizontalSpeedToZero(this)) {
            *inputVelocity = 0.0f;
            *inputYaw = this->yaw;
        } else {
            return true;
        }
    }

    return false;
}

void func_8083C50C(Player* this) {
    if ((this->unk_844 > 0) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        this->unk_844 = -this->unk_844;
    }
}

s32 Player_SwapAction_TryChargingSpinAttack(Player* this, PlayState* play) {
    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) && (Player_GetMeleeWeaponHeld(this) != 0) &&
            (this->unk_844 == 1) && (this->heldItemAction != PLAYER_IA_DEKU_STICK)) {
            if ((this->heldItemAction != PLAYER_IA_SWORD_BIGGORON) || (gSaveContext.swordHealth > 0.0f)) {
                Player_Setup1_ChargeSpinAttack(play, this);
                return true;
            }
        }
    } else {
        func_8083C50C(this);
    }

    return false;
}

s32 func_8083C61C(PlayState* play, Player* this) {
    if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (AMMO(ITEM_DEKU_NUT) != 0)) {
        Player_SetAction(play, this, Player_Action_ThrowDekuNut, 0);
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_light_bom);
        this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
        return true;
    }

    return false;
}

typedef struct {
    /* 0x0 */ PlayerAnimationHeader* unk_00;
    /* 0x4 */ PlayerAnimationHeader* unk_04;
    /* 0x8 */ u8 unk_08;
    /* 0x9 */ u8 unk_09;
} BottleSwingAnimInfo; // size = 0xC

typedef enum BottleSwingAnimation {
    /* 0 */ BOTTLE_SWING_ANIM_0,
    /* 1 */ BOTTLE_SWING_ANIM_1,
    /* 2 */ BOTTLE_SWING_ANIM_MAX
} BottleSwingAnimation;

static BottleSwingAnimInfo sBottleSwingAnims[BOTTLE_SWING_ANIM_MAX] = {
    { &gPlayerAnim_link_bottle_bug_miss, &gPlayerAnim_link_bottle_bug_in, 2, 3 },   // BOTTLE_SWING_ANIM_0
    { &gPlayerAnim_link_bottle_fish_miss, &gPlayerAnim_link_bottle_fish_in, 5, 3 }, // BOTTLE_SWING_ANIM_1
};

s32 Player_TrySwingingBottleAndCastingFishingRod(PlayState* play, Player* this) {
    Vec3f sp24;

    if (D_80853614) {
        if (Player_GetBottleHeld(this) >= 0) {
            Player_SetAction(play, this, Player_Action_SwingBottle, 0);

            if (this->actor.depthInWater > 12.0f) {
                this->actionVar16 = 1;
            }

            Player_Anim_PlayOnceAdjusted(play, this, sBottleSwingAnims[this->actionVar16].unk_00);

            Player_PlaySfx(this, NA_SE_IT_SWORD_SWING);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_AUTO_JUMP);
            return true;
        }

        if (this->heldItemAction == PLAYER_IA_FISHING_POLE) {
            sp24 = this->actor.world.pos;
            sp24.y += 50.0f;

            if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.world.pos.z > 1300.0f) ||
                BgCheck_SphVsFirstPoly(&play->colCtx, &sp24, 20.0f)) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                return false;
            }

            Player_SetAction(play, this, Player_Action_CastFishingRod, 0);
            this->stickFlameTimer = 1;
            Player_SetHorizontalSpeedToZero(this);
            Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_fishing_throw);
            return true;
        } else {
            return false;
        }
    }

    return false;
}

void Player_SetupRun(Player* this, PlayState* play) {
    PlayerActionFunc func;

    if (Player_IsZTargeting(this)) {
        func = Player_Action_RunZTarget;
    } else {
        func = Player_Action_Run;
    }

    Player_SetAction(play, this, func, 1);
    Player_Anim_PlayLoopWithMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType));

    this->walkFloorPitch = 0;
    this->unk_864 = this->walkCurFrame = 0.0f;
}

void Player_SetupRunTowardsYaw(Player* this, PlayState* play, s16 yaw) {
    this->actor.shape.rot.y = this->yaw = yaw;
    Player_SetupRun(this, play);
}

s32 func_8083C910(PlayState* play, Player* this, f32 arg2) {
    WaterBox* waterBox;
    f32 waterBoxYSurface = this->actor.world.pos.y;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterBoxYSurface,
                             &waterBox)) {
        waterBoxYSurface -= this->actor.world.pos.y;
        if (waterBoxYSurface >= this->ageProperties->unk_24) {
            Player_SetAction(play, this, Player_Action_SwimSpawn, 0);
            Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim);
            this->stateFlags1 |= PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE;
            this->swimSpawnVar16 = 20;
            this->speedXZ = 2.0f;
            Player_SetBootData(play, this);
            return false;
        }
    }

    Player_SetupMiniCutscene(play, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    return true;
}

void Player_InitMode_D(PlayState* play, Player* this) {
    if (func_8083C910(play, this, 180.0f)) {
        this->miniCutsceneVar16 = -20;
    }
}

void Player_InitMode_Other(PlayState* play, Player* this) {
    this->speedXZ = 2.0f;
    gSaveContext.entranceSpeed = 2.0f;

    if (func_8083C910(play, this, 120.0f)) {
        this->miniCutsceneVar16 = -15;
    }
}

void Player_InitMode_F(PlayState* play, Player* this) {
    if (gSaveContext.entranceSpeed < 0.1f) {
        gSaveContext.entranceSpeed = 0.1f;
    }

    this->speedXZ = gSaveContext.entranceSpeed;

    if (func_8083C910(play, this, 800.0f)) {
        this->miniCutsceneVar16 = -80 / this->speedXZ;
        if (this->miniCutsceneVar16 < -20) {
            this->miniCutsceneVar16 = -20;
        }
    }
}

void Player_SetupBackwalkFriend(Player* this, s16 inputYaw, PlayState* play) {
    Player_SetAction(play, this, Player_Action_BackwalkFriend, 1);
    PlayerAnimation_CopyJointToMorph(play, &this->skelAnime);
    this->walkCurFrame = 0.0f;
    this->unk_864 = 0.0f;
    this->yaw = inputYaw;
}

void Player_SetupSidewalkSlow(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_SidewalkSlow, 1);
    Player_Anim_PlayLoopWithMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType));
}

void Player_SetupBackwalkEnemy(Player* this, s16 inputYaw, PlayState* play) {
    Player_SetAction(play, this, Player_Action_BackwalkEnemy, 1);
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_walk, 2.2f, 0.0f,
                           Animation_GetLastFrame(&gPlayerAnim_link_anchor_back_walk), ANIMMODE_ONCE, -6.0f);
    this->speedXZ = 8.0f;
    this->yaw = inputYaw;
}

void Player_SetupSidewalkFast(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_SidewalkFast, 1);
    Player_Anim_PlayLoopWithMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType));
    this->walkCurFrame = 0.0f;
}

void Player_SetupBackwalkEndEnemy(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_BackwalkEndEnemy, 1);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_brake, 2.0f);
}

void Player_SetupTurn(PlayState* play, Player* this, s16 yaw) {
    this->yaw = yaw;
    Player_SetAction(play, this, Player_Action_Turn, 1);
    this->unk_87E = 1200;
    this->unk_87E *= sPlayerUnderwaterSpeedAdjustment;
    PlayerAnimation_Change(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_45_turn, this->modelAnimType), 1.0f,
                           0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void Player_SetupIdleWithLeftRightAnim(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;

    Player_SetAction(play, this, Player_Action_Idle, 1);

    if (this->leftRightBlendWeight < 0.5f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitR2wait, this->modelAnimType);
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitL2wait, this->modelAnimType);
    }
    Player_Anim_PlayOnce(play, this, anim);

    this->yaw = this->actor.shape.rot.y;
}

void Player_Setup2_IdleLockOnEnemy(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_IdleLockOnEnemy, 1);
    Player_Anim_PlayOnceWithMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait2waitR, this->modelAnimType));
    this->IdleLockOnEnemyVar16 = true;
}

void func_8083CF10(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        Player_SetupRun(this, play);
    } else {
        Player_SetupIdleWithLeftRightAnim(this, play);
    }
}

void Player_EndMiniCutscene(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        Player_SetupRun(this, play);
    } else {
        Player_Setup1_IdleAll(this, play);
    }
}

s32 Player_TrySpawningSplash(PlayState* play, Player* this, f32 arg2, s32 splashScale) {
    f32 sp3C = fabsf(arg2);
    WaterBox* sp38;
    f32 sp34;
    Vec3f splashPos;
    s32 splashType;

    if (sp3C > 2.0f) {
        splashPos.x = this->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        splashPos.z = this->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        sp34 = this->actor.world.pos.y;
        if (WaterBox_GetSurface1(play, &play->colCtx, splashPos.x, splashPos.z, &sp34, &sp38)) {
            if ((sp34 - this->actor.world.pos.y) < 100.0f) {
                splashType = (sp3C <= 10.0f) ? 0 : 1;
                splashPos.y = sp34;
                EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, splashType, splashScale);
                return true;
            }
        }
    }

    return false;
}

void Player_JumpOutOfWater(PlayState* play, Player* this, f32 arg2) {
    this->stateFlags1 |= PLAYER_STATE1_JUMPING;
    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;

    Player_ResetSubCam(play, this);

    if (Player_TrySpawningSplash(play, this, arg2, 500)) {
        Player_PlaySfx(this, NA_SE_EV_JUMP_OUT_WATER);
    }

    Player_SetBootData(play, this);
}

s32 Player_TrySwimDivingOrGetItem(PlayState* play, Player* this, Input* input) {
    if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
        if ((input == NULL) ||
            (CHECK_BTN_ALL(input->press.button, BTN_A) && (ABS(this->shapePitchOffset) < DEG_TO_BINANG(65.918f)) &&
             (this->currentBoots != PLAYER_BOOTS_IRON))) {

            Player_SetAction(play, this, Player_Action_SwimDive, 0);
            Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_swimer_swim_deep_start);

            this->shapePitchOffset = 0;
            this->stateFlags2 |= PLAYER_STATE2_DIVING;
            this->actor.velocity.y = 0.0f;

            if (input != NULL) {
                this->stateFlags2 |= PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER;
                Player_PlaySfx(this, NA_SE_PL_DIVE_BUBBLE);
            }

            return true;
        }
    }

    if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || (this->stateFlags2 & PLAYER_STATE2_DIVING)) {
        if (this->actor.velocity.y > 0.0f) {
            if (this->actor.depthInWater < this->ageProperties->unk_30) {

                this->stateFlags2 &= ~PLAYER_STATE2_DIVING;

                if (input != NULL) {
                    Player_SetAction(play, this, Player_Action_SwimGetItem, 1);

                    if (this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) {
                        this->stateFlags1 |=
                            PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    }

                    this->actionVar16 = 2;
                }

                Player_ResetSubCam(play, this);
                Player_Anim_PlayOnceWithMorph(play, this,
                                              (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)
                                                  ? &gPlayerAnim_link_swimer_swim_get
                                                  : &gPlayerAnim_link_swimer_swim_deep_end);

                if (Player_TrySpawningSplash(play, this, this->actor.velocity.y, 500)) {
                    Player_PlaySfx(this, NA_SE_PL_FACE_UP);
                }

                return true;
            }
        }
    }

    return false;
}

void func_8083D330(PlayState* play, Player* this) {
    Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
    this->shapePitchOffset = 16000;
    this->actionVar16 = 1;
}

void func_8083D36C(PlayState* play, Player* this) {
    if ((this->currentBoots != PLAYER_BOOTS_IRON) || !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_DetachHeldActorAndResetAttributes(play, this);

        if ((this->currentBoots != PLAYER_BOOTS_IRON) && (this->stateFlags2 & PLAYER_STATE2_DIVING)) {
            this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
            Player_TrySwimDivingOrGetItem(play, this, NULL);
            this->actionVar8 = 1;
        } else if (Player_Action_FallDive == this->actionFunc) {
            Player_SetAction(play, this, Player_Action_SwimDive, 0);
            func_8083D330(play, this);
        } else {
            Player_SetAction(play, this, Player_Action_SwimIdle, 1);
            Player_Anim_PlayOnceWithMorph(play, this,
                                          (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)
                                              ? &gPlayerAnim_link_swimer_wait2swim_wait
                                              : &gPlayerAnim_link_swimer_land2swim_wait);
        }
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->actor.depthInWater < this->ageProperties->unk_2C)) {
        if (Player_TrySpawningSplash(play, this, this->actor.velocity.y, 500)) {
            Player_PlaySfx(this, NA_SE_EV_DIVE_INTO_WATER);

            if (this->fallDistance > 800.0f) {
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
            }
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_SWIMMING;
    this->stateFlags2 |= PLAYER_STATE2_DIVING;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);
    this->unk_854 = 0.0f;

    Player_SetBootData(play, this);
}

void Player_UpdateUnderwater(PlayState* play, Player* this) {
    if (this->actor.depthInWater < this->ageProperties->unk_2C) {
        Audio_SetBaseFilter(0);
        this->underwaterTimer = 0;
    } else {
        Audio_SetBaseFilter(0x20);
        if (this->underwaterTimer < 300) {
            this->underwaterTimer++;
        }
    }

    if ((Player_Action_JumpToLedge == this->actionFunc) || (Player_Action_ClimbLedge == this->actionFunc)) {
        return;
    }

    if (this->actor.depthInWater > this->ageProperties->unk_2C) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) ||
            (!((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) &&
             (Player_Action_SwimDamage != this->actionFunc) && (Player_Action_SwimDrown != this->actionFunc) &&
             (Player_Action_SwimIdle != this->actionFunc) && (Player_Action_SwimMove != this->actionFunc) &&
             (Player_Action_SwimZTarget != this->actionFunc) && (Player_Action_SwimDive != this->actionFunc) &&
             (Player_Action_SwimGetItem != this->actionFunc) && (Player_Action_SwimSpawn != this->actionFunc))) {
            func_8083D36C(play, this);
        }
    } else if ((this->stateFlags1 & PLAYER_STATE1_SWIMMING) &&
               (this->actor.depthInWater < this->ageProperties->unk_24)) {
        if ((this->skelAnime.moveFlags == 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            Player_SetupTurn(play, this, this->actor.shape.rot.y);
        }
        Player_JumpOutOfWater(play, this, this->actor.velocity.y);
    }
}

void func_8083D6EC(PlayState* play, Player* this) {
    Vec3f ripplePos;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;

    this->actor.terminalVelocity = -20.0f;
    this->actor.gravity = R_PLAYER_BOOT_10 / 100.0f;

    if (Player_IsFloorSinking(sFloorType)) {
        temp1 = fabsf(this->speedXZ) * 20.0f;
        temp3 = 0.0f;

        if (sFloorType == FLOOR_TYPE_4) {
            if (this->shapeOffsetY > 1300.0f) {
                temp2 = this->shapeOffsetY;
            } else {
                temp2 = 1300.0f;
            }
            if (this->currentBoots == PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp1 *= 0.3f;
            }
        } else {
            temp2 = 20000.0f;
            if (this->currentBoots != PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if ((sFloorType == FLOOR_TYPE_7) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                temp1 = 0;
            }
        }

        if (this->currentBoots != PLAYER_BOOTS_HOVER) {
            temp3 = (temp2 - this->shapeOffsetY) * 0.02f;
            temp3 = CLAMP(temp3, 0.0f, 300.0f);
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp3 += temp3;
            }
        }

        this->shapeOffsetY += temp3 - temp1;
        this->shapeOffsetY = CLAMP(this->shapeOffsetY, 0.0f, temp2);

        this->actor.gravity -= this->shapeOffsetY * 0.004f;
    } else {
        this->shapeOffsetY = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        if (this->actor.depthInWater < 50.0f) {
            temp4 = fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].x - this->unk_A88.x) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->unk_A88.y) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].z - this->unk_A88.z);
            if (temp4 > 4.0f) {
                temp4 = 4.0f;
            }
            this->unk_854 += temp4;

            if (this->unk_854 > 15.0f) {
                this->unk_854 = 0.0f;

                ripplePos.x = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.x;
                ripplePos.y = this->actor.world.pos.y + this->actor.depthInWater;
                ripplePos.z = (Rand_ZeroOne() * 10.0f) + this->actor.world.pos.z;
                EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 0);

                if ((this->speedXZ > 4.0f) && !Player_IsFreeSwimming(this) &&
                    ((this->actor.world.pos.y + this->actor.depthInWater) <
                     this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    Player_TrySpawningSplash(play, this, 20.0f,
                                             (fabsf(this->speedXZ) * 50.0f) + (this->actor.depthInWater * 5.0f));
                }
            }
        }

        if (this->actor.depthInWater > 40.0f) {
            s32 numBubbles = 0;
            s32 i;

            if ((this->actor.velocity.y > -1.0f) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (Rand_ZeroOne() < 0.2f) {
                    numBubbles = 1;
                }
            } else {
                numBubbles = this->actor.velocity.y * -2.0f;
            }

            for (i = 0; i < numBubbles; i++) {
                EffectSsBubble_Spawn(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 Player_LookAtTargetActor(Player* this, s32 syncUpperRotToFocusRot) {
    Actor* lockOnActor = this->lockOnActor;
    Vec3f sp30;
    s16 sp2E;
    s16 sp2C;

    sp30.x = this->actor.world.pos.x;
    sp30.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    sp30.z = this->actor.world.pos.z;
    sp2E = Math_Vec3f_Pitch(&sp30, &lockOnActor->focus.pos);
    sp2C = Math_Vec3f_Yaw(&sp30, &lockOnActor->focus.pos);
    Math_SmoothStepToS(&this->actor.focus.rot.y, sp2C, 4, 10000, 0);
    Math_SmoothStepToS(&this->actor.focus.rot.x, sp2E, 4, 10000, 0);
    this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y;

    return Player_UpdateLookAngles(this, syncUpperRotToFocusRot);
}

static Vec3f D_8085456C = { 0.0f, 100.0f, 40.0f };

void Player_SetLookAngle(Player* this, PlayState* play) {
    s16 sp46;
    s16 temp2;
    f32 temp1;
    Vec3f sp34;

    if (this->lockOnActor != NULL) {
        if (Player_IsAimingFpsItem(this) || Player_IsAimingBoomerang(this)) {
            Player_LookAtTargetActor(this, 1);
        } else {
            Player_LookAtTargetActor(this, 0);
        }
        return;
    }

    if (sFloorType == FLOOR_TYPE_11) {
        Math_SmoothStepToS(&this->actor.focus.rot.x, -20000, 10, 4000, 800);
    } else {
        sp46 = 0;
        temp1 = Player_PosVsFloorLineTest(play, this, &D_8085456C, &sp34);
        if (temp1 > BGCHECK_Y_MIN) {
            temp2 = Math_Atan2S(40.0f, this->actor.world.pos.y - temp1);
            sp46 = CLAMP(temp2, -4000, 4000);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        Math_SmoothStepToS(&this->actor.focus.rot.x, sp46, 14, 4000, 30);
    }

    Player_UpdateLookAngles(this, Player_IsAimingFpsItem(this) || Player_IsAimingBoomerang(this));
}

void func_8083DDC8(Player* this, PlayState* play) {
    s16 temp1;
    s16 temp2;

    if (!Player_IsAimingFpsItem(this) && !Player_IsAimingBoomerang(this) && (this->speedXZ > 5.0f)) {
        temp1 = this->speedXZ * 200.0f;
        temp2 = (s16)(this->yaw - this->actor.shape.rot.y) * this->speedXZ * 0.1f;
        temp1 = CLAMP(temp1, -4000, 4000);
        temp2 = CLAMP(-temp2, -4000, 4000);
        Math_ScaledStepToS(&this->upperLimbRot.x, temp1, 900);
        this->headLimbRot.x = -(f32)this->upperLimbRot.x * 0.5f;
        Math_ScaledStepToS(&this->headLimbRot.z, temp2, 300);
        Math_ScaledStepToS(&this->upperLimbRot.z, temp2, 200);
        this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_HEAD_ROT_X | PLAYER_ROT_OVERRIDE_HEAD_ROT_Z |
                                  PLAYER_ROT_OVERRIDE_UPPER_ROT_X | PLAYER_ROT_OVERRIDE_UPPER_ROT_Z;
    } else {
        Player_SetLookAngle(this, play);
    }
}

void func_8083DF68(Player* this, f32 inputVelocity, s16 inputYaw) {
    Math_AsymStepToF(&this->speedXZ, inputVelocity, R_PLAYER_BOOT_0 / 100.0f, 1.5f);
    Math_ScaledStepToS(&this->yaw, inputYaw, REG(27));
}

void func_8083DFE0(Player* this, f32* inputVelocity, s16* inputYaw) {
    s16 yawDiff = this->yaw - *inputYaw;

    if (this->meleeWeaponState == 0) {
        this->speedXZ = CLAMP(this->speedXZ, -(R_RUN_SPEED_LIMIT / 100.0f), (R_RUN_SPEED_LIMIT / 100.0f));
    }

    if (ABS(yawDiff) > DEG_TO_BINANG(135.0f)) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = *inputYaw;
        }
    } else {
        Math_AsymStepToF(&this->speedXZ, *inputVelocity, 0.05f, 0.1f);
        Math_ScaledStepToS(&this->yaw, *inputYaw, 200);
    }
}

typedef struct {
    /* 0x0 */ PlayerAnimationHeader* anim;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ f32 unk_08;
} struct_80854578; // size = 0xC

static struct_80854578 D_80854578[] = {
    { &gPlayerAnim_link_uma_left_up, 35.17f, 6.6099997f },
    { &gPlayerAnim_link_uma_right_up, -34.16f, 7.91f },
};

s32 Player_SwapAction_TryMountingHorse(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 unk_04;
    f32 unk_08;
    f32 sp38;
    f32 sp34;
    s32 temp;

    if ((rideActor != NULL) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
        sp38 = Math_CosS(rideActor->actor.shape.rot.y);
        sp34 = Math_SinS(rideActor->actor.shape.rot.y);

        Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupRideHorse);

        this->stateFlags1 |= PLAYER_STATE1_RIDING_HORSE;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;

        if (this->mountSide < 0) {
            temp = 0;
        } else {
            temp = 1;
        }

        unk_04 = D_80854578[temp].unk_04;
        unk_08 = D_80854578[temp].unk_08;
        this->actor.world.pos.x =
            rideActor->actor.world.pos.x + rideActor->riderPos.x + ((unk_04 * sp38) + (unk_08 * sp34));
        this->actor.world.pos.z =
            rideActor->actor.world.pos.z + rideActor->riderPos.z + ((unk_08 * sp38) - (unk_04 * sp34));

        this->rideOffsetY = rideActor->actor.world.pos.y - this->actor.world.pos.y;
        this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        Actor_MountHorse(play, this, &rideActor->actor);
        Player_Anim_PlayOnce(play, this, D_80854578[temp].anim);
        Player_AnimReplace_Setup(play, this, 0x9B);
        this->actor.parent = this->rideActor;
        Player_ClearAttentionModeAndStopMoving(this);
        Actor_DisableLens(play);
        return true;
    }

    return false;
}

void Player_GetSlopeDirection(CollisionPoly* floorPoly, Vec3f* slopeNormal, s16* downwardSlopeYaw) {
    slopeNormal->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    slopeNormal->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    slopeNormal->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    *downwardSlopeYaw = Math_Atan2S(slopeNormal->z, slopeNormal->x);
}

static PlayerAnimationHeader* sSlopeSlipAnims[] = {
    &gPlayerAnim_link_normal_down_slope_slip,
    &gPlayerAnim_link_normal_up_slope_slip,
};

s32 Player_HandleSlopes(PlayState* play, Player* this, CollisionPoly* floorPoly) {
    s32 pad;
    s16 playerVelYaw;
    Vec3f slopeNormal;
    s16 downwardSlopeYaw;
    f32 slopeSlowdownSpeed;
    f32 slopeSlowdownSpeedStep;
    s16 velYawToDownwardSlope;

    if (!Player_InBlockingCsMode(play, this) && (Player_Action_SlipOnSlope != this->actionFunc) &&
        (SurfaceType_GetFloorEffect(&play->colCtx, floorPoly, this->actor.floorBgId) == FLOOR_EFFECT_1)) {

        // Get direction of movement relative to the downward direction of the slope
        playerVelYaw = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
        Player_GetSlopeDirection(floorPoly, &slopeNormal, &downwardSlopeYaw);
        velYawToDownwardSlope = downwardSlopeYaw - playerVelYaw;

        if (ABS(velYawToDownwardSlope) > 0x3E80) { // 87.9 degrees
            // moving parallel or upwards on the slope, player does not slip but does slow down
            slopeSlowdownSpeed = (1.0f - slopeNormal.y) * 40.0f;
            slopeSlowdownSpeedStep = SQ(slopeSlowdownSpeed) * 0.015f;

            if (slopeSlowdownSpeedStep < 1.2f) {
                slopeSlowdownSpeedStep = 1.2f;
            }

            // slows down speed as player is climbing a slope
            this->pushedYaw = downwardSlopeYaw;
            Math_StepToF(&this->pushedSpeed, slopeSlowdownSpeed, slopeSlowdownSpeedStep);
        } else {
            // moving downward on the slope, causing player to slip
            Player_SetAction(play, this, Player_Action_SlipOnSlope, 0);
            Player_DetachHeldActorAndResetAttributes(play, this);

            if (sFloorPitchShape >= 0) {
                this->actionVar8 = 1;
            }

            Player_Anim_PlayLoopWithMorph(play, this, sSlopeSlipAnims[this->actionVar8]);
            this->speedXZ = sqrtf(SQ(this->actor.velocity.x) + SQ(this->actor.velocity.z));
            this->yaw = playerVelYaw;
            return true;
        }
    }

    return false;
}

// unknown data (unused)
static s32 D_80854598[] = {
    0xFFDB0871, 0xF8310000, 0x00940470, 0xF3980000, 0xFFB504A9, 0x0C9F0000, 0x08010402,
};

void func_8083E4C4(PlayState* play, Player* this, GetItemEntry* giEntry) {
    s32 dropType = giEntry->field & 0x1F;

    if (!(giEntry->field & 0x80)) {
        Item_DropCollectible(play, &this->actor.world.pos, dropType | 0x8000);
        if ((dropType != ITEM00_BOMBS_A) && (dropType != ITEM00_ARROWS_SMALL) && (dropType != ITEM00_ARROWS_MEDIUM) &&
            (dropType != ITEM00_ARROWS_LARGE) && (dropType != ITEM00_RUPEE_GREEN) && (dropType != ITEM00_RUPEE_BLUE) &&
            (dropType != ITEM00_RUPEE_RED) && (dropType != ITEM00_RUPEE_PURPLE) && (dropType != ITEM00_RUPEE_ORANGE)) {
            Item_Give(play, giEntry->itemId);
        }
    } else {
        Item_Give(play, giEntry->itemId);
    }

    Audio_PlaySfx((this->getItemId < 0) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
}

s32 Player_SwapAction_TryGetItem(Player* this, PlayState* play) {
    Actor* interactedActor;

    if (iREG(67) ||
        (((interactedActor = this->interactRangeActor) != NULL) && TitleCard_Clear(play, &play->actorCtx.titleCtx))) {
        if (iREG(67) || (this->getItemId > GI_NONE)) {
            if (iREG(67)) {
                this->getItemId = iREG(68);
            }

            if (this->getItemId < GI_MAX) {
                GetItemEntry* giEntry = &sGetItemTable[this->getItemId - 1];

                if ((interactedActor != &this->actor) && !iREG(67)) {
                    interactedActor->parent = &this->actor;
                }

                iREG(67) = false;

                if ((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) ||
                    (play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY)) {
                    Player_DetatchHeldActor(play, this);
                    Player_LoadGetItemObject(this, giEntry->objectId);

                    if (!(this->stateFlags2 & PLAYER_STATE2_DIVING) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                        Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupGetItem);
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemB);
                        Player_TurnAroundCamera(play, 9);
                    }

                    this->stateFlags1 |=
                        PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                    Player_ClearAttentionModeAndStopMoving(this);
                    return 1;
                }

                func_8083E4C4(play, this, giEntry);
                this->getItemId = GI_NONE;
            }
        } else if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A) &&
                   !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
            if (this->getItemId != GI_NONE) {
                GetItemEntry* giEntry = &sGetItemTable[-this->getItemId - 1];
                EnBox* chest = (EnBox*)interactedActor;

                if (giEntry->itemId != ITEM_NONE) {
                    if (((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) && (giEntry->field & 0x40)) ||
                        ((Item_CheckObtainability(giEntry->itemId) != ITEM_NONE) && (giEntry->field & 0x20))) {
                        this->getItemId = -GI_RUPEE_BLUE;
                        giEntry = &sGetItemTable[GI_RUPEE_BLUE - 1];
                    }
                }

                Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupGetItem);
                this->stateFlags1 |=
                    PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_IN_CUTSCENE;
                Player_LoadGetItemObject(this, giEntry->objectId);
                this->actor.world.pos.x =
                    chest->dyna.actor.world.pos.x - (Math_SinS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->actor.world.pos.z =
                    chest->dyna.actor.world.pos.z - (Math_CosS(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->yaw = this->actor.shape.rot.y = chest->dyna.actor.shape.rot.y;
                Player_ClearAttentionModeAndStopMoving(this);

                if ((giEntry->itemId != ITEM_NONE) && (giEntry->gi >= 0) &&
                    (Item_CheckObtainability(giEntry->itemId) == ITEM_NONE)) {
                    Player_Anim_PlayOnceAdjusted(play, this, this->ageProperties->unk_98);
                    Player_AnimReplace_Setup(play, this, 0x28F);
                    chest->unk_1F4 = 1;
                    Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_SLOW_CHEST_CS);
                } else {
                    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_box_kick);
                    chest->unk_1F4 = -1;
                }

                return true;
            }

            if ((this->heldActor == NULL) || Player_IsHoldingHookshot(this)) {
                if ((interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                    s32 sp24 = this->itemAction;

                    this->itemAction = PLAYER_IA_NONE;
                    this->modelAnimType = PLAYER_ANIMTYPE_0;
                    this->heldItemAction = this->itemAction;
                    Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupLift);

                    if (sp24 == PLAYER_IA_SWORD_MASTER) {
                        this->nextModelGroup = Player_ModelGroupFromIA(this, PLAYER_IA_LAST_USED);
                        Player_ChangeItem(play, this, PLAYER_IA_LAST_USED);
                    } else {
                        Player_UseItem(play, this, ITEM_LAST_USED);
                    }
                } else {
                    s32 strength = Player_GetStrength();

                    if ((interactedActor->id == ACTOR_EN_ISHI) && ((interactedActor->params & 0xF) == 1) &&
                        (strength < PLAYER_STR_SILVER_G)) {
                        return false;
                    }

                    Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupLift);
                }

                Player_ClearAttentionModeAndStopMoving(this);
                this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
                return true;
            }
        }
    }

    return false;
}

void Player_SetupThrow(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Throw, 1);
    Player_Anim_PlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_throw, this->modelAnimType));
}

// Determines whether a held actor should be dropped or thrown: false implies droppable.
s32 Player_CanThrowActor(Player* this, Actor* actor) {
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_ALWAYS_THROW) &&
        ((this->speedXZ < 1.1f) || (actor->id == ACTOR_EN_BOM_CHU))) {
        return false;
    }

    return true;
}

s32 Player_SwapAction_TryThrowPutDown(Player* this, PlayState* play) {
    if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor != NULL) &&
        CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
        if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, this->heldActor)) {
            if (!Player_CanThrowActor(this, this->heldActor)) {
                Player_SetAction(play, this, Player_Action_PutDown, 1);
                Player_Anim_PlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_put, this->modelAnimType));
            } else {
                Player_SetupThrow(this, play);
            }
        }
        return true;
    }

    return false;
}

s32 func_8083EC18(Player* this, PlayState* play, u32 wallFlags) {
    if (this->yDistToLedge >= 79.0f) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) || (this->currentBoots == PLAYER_BOOTS_IRON) ||
            (this->actor.depthInWater < this->ageProperties->unk_2C)) {
            s32 sp8C = (wallFlags & WALL_FLAG_3) ? 2 : 0;

            if ((sp8C != 0) || (wallFlags & WALL_FLAG_1) ||
                SurfaceType_CheckWallFlag2(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                f32 phi_f20;
                CollisionPoly* wallPoly = this->actor.wallPoly;
                f32 sp80;
                f32 sp7C;
                f32 phi_f12;
                f32 phi_f14;

                phi_f20 = phi_f12 = 0.0f;

                if (sp8C != 0) {
                    sp80 = this->actor.world.pos.x;
                    sp7C = this->actor.world.pos.z;
                } else {
                    Vec3f sp50[3];
                    s32 i;
                    f32 sp48;
                    Vec3f* sp44 = &sp50[0];
                    s32 pad;

                    CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, sp50);

                    sp80 = phi_f12 = sp44->x;
                    sp7C = phi_f14 = sp44->z;
                    phi_f20 = sp44->y;
                    for (i = 1; i < 3; i++) {
                        sp44++;
                        if (sp80 > sp44->x) {
                            sp80 = sp44->x;
                        } else if (phi_f12 < sp44->x) {
                            phi_f12 = sp44->x;
                        }

                        if (sp7C > sp44->z) {
                            sp7C = sp44->z;
                        } else if (phi_f14 < sp44->z) {
                            phi_f14 = sp44->z;
                        }

                        if (phi_f20 > sp44->y) {
                            phi_f20 = sp44->y;
                        }
                    }

                    sp80 = (sp80 + phi_f12) * 0.5f;
                    sp7C = (sp7C + phi_f14) * 0.5f;

                    phi_f12 = ((this->actor.world.pos.x - sp80) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                              ((this->actor.world.pos.z - sp7C) * COLPOLY_GET_NORMAL(wallPoly->normal.x));
                    sp48 = this->actor.world.pos.y - phi_f20;

                    phi_f20 = ((f32)(s32)((sp48 / 15.000000223517418) + 0.5) * 15.000000223517418) - sp48;
                    phi_f12 = fabsf(phi_f12);
                }

                if (phi_f12 < 8.0f) {
                    f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                    f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                    f32 sp34 = this->distToInteractWall;
                    PlayerAnimationHeader* sp30;

                    Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupClimb);
                    this->stateFlags1 |= PLAYER_STATE1_CLIMBING;
                    this->stateFlags1 &= ~PLAYER_STATE1_SWIMMING;

                    if ((sp8C != 0) || (wallFlags & WALL_FLAG_1)) {
                        if ((this->actionVar8 = sp8C) != 0) {
                            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                                sp30 = &gPlayerAnim_link_normal_Fclimb_startA;
                            } else {
                                sp30 = &gPlayerAnim_link_normal_Fclimb_hold2upL;
                            }
                            sp34 = (this->ageProperties->wallCheckRadius - 1.0f) - sp34;
                        } else {
                            sp30 = this->ageProperties->unk_A4;
                            sp34 = sp34 - 1.0f;
                        }
                        this->actionVar16 = -2;
                        this->actor.world.pos.y += phi_f20;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                    } else {
                        sp30 = this->ageProperties->unk_A8;
                        this->actionVar16 = -4;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw;
                    }

                    this->actor.world.pos.x = (sp34 * wallPolyNormalX) + sp80;
                    this->actor.world.pos.z = (sp34 * wallPolyNormalZ) + sp7C;
                    Player_ClearAttentionModeAndStopMoving(this);
                    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
                    Player_Anim_PlayOnce(play, this, sp30);
                    Player_AnimReplace_Setup(play, this, 0x9F);

                    return true;
                }
            }
        }
    }

    return false;
}

void Player_SetupClimbEnd(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    Player_SetAction_PreserveMoveFlags(play, this, Player_Action_ClimbEnd, 0);
    PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, anim, (4.0f / 3.0f));
}

/**
 * @return true if Player chooses to enter crawlspace
 */
s32 Player_TryEnteringCrawlspace(Player* this, PlayState* play, u32 wallFlags) {
    CollisionPoly* wallPoly;
    Vec3f wallVertices[3];
    f32 xVertex1;
    f32 xVertex2;
    f32 zVertex1;
    f32 zVertex2;
    s32 i;

    if (!LINK_IS_ADULT && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING) && (wallFlags & WALL_FLAG_CRAWLSPACE)) {
        wallPoly = this->actor.wallPoly;
        CollisionPoly_GetVerticesByBgId(wallPoly, this->actor.wallBgId, &play->colCtx, wallVertices);

        // Determines min and max vertices for x & z (edges of the crawlspace hole)
        xVertex1 = xVertex2 = wallVertices[0].x;
        zVertex1 = zVertex2 = wallVertices[0].z;
        for (i = 1; i < 3; i++) {
            if (xVertex1 > wallVertices[i].x) {
                // Update x min
                xVertex1 = wallVertices[i].x;
            } else if (xVertex2 < wallVertices[i].x) {
                // Update x max
                xVertex2 = wallVertices[i].x;
            }
            if (zVertex1 > wallVertices[i].z) {
                // Update z min
                zVertex1 = wallVertices[i].z;
            } else if (zVertex2 < wallVertices[i].z) {
                // Update z max
                zVertex2 = wallVertices[i].z;
            }
        }

        // XZ Center of the crawlspace hole
        xVertex1 = (xVertex1 + xVertex2) * 0.5f;
        zVertex1 = (zVertex1 + zVertex2) * 0.5f;

        // Perpendicular (sideways) XZ-Distance from player pos to crawlspace line
        // Uses y-component of crossproduct formula for the distance from a point to a line
        xVertex2 = ((this->actor.world.pos.x - xVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                   ((this->actor.world.pos.z - zVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

        if (fabsf(xVertex2) < 8.0f) {
            // Give do-action prompt to "Enter on A" for the crawlspace
            this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_ENTER;

            if (CHECK_BTN_ALL(sControlInput->press.button, BTN_A)) {
                // Enter Crawlspace
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 distToInteractWall = this->distToInteractWall;

                Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupCrawl);
                this->stateFlags2 |= PLAYER_STATE2_CRAWLING;
                this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                this->actor.world.pos.x = xVertex1 + (distToInteractWall * wallPolyNormalX);
                this->actor.world.pos.z = zVertex1 + (distToInteractWall * wallPolyNormalZ);
                Player_ClearAttentionModeAndStopMoving(this);
                this->actor.prevPos = this->actor.world.pos;
                Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_child_tunnel_start);
                Player_AnimReplace_Setup(play, this, 0x9D);

                return true;
            }
        }
    }

    return false;
}

s32 Player_ProcessSpecialWallInteraction(PlayState* play, Player* this, f32 yOffset, f32 xzDistToWall,
                                         f32 xzCheckOffsetFar, f32 xzCheckOffsetNear) {
    CollisionPoly* wallPoly;
    s32 wallBgId;
    Vec3f checkPosNear;
    Vec3f checkPosFar;
    Vec3f posResult;
    f32 yawCos;
    f32 yawSin;
    s32 yawTarget;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;

    yawCos = Math_CosS(this->actor.shape.rot.y);
    yawSin = Math_SinS(this->actor.shape.rot.y);

    checkPosNear.x = this->actor.world.pos.x + (xzCheckOffsetNear * yawSin);
    checkPosNear.z = this->actor.world.pos.z + (xzCheckOffsetNear * yawCos);
    checkPosFar.x = this->actor.world.pos.x + (xzCheckOffsetFar * yawSin);
    checkPosFar.z = this->actor.world.pos.z + (xzCheckOffsetFar * yawCos);
    checkPosFar.y = checkPosNear.y = this->actor.world.pos.y + yOffset;

    if (BgCheck_EntityLineTest1(&play->colCtx, &checkPosNear, &checkPosFar, &posResult, &this->actor.wallPoly, true,
                                false, false, true, &wallBgId)) {
        wallPoly = this->actor.wallPoly;

        this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;
        this->actor.wallBgId = wallBgId;

        sTouchedWallFlags = SurfaceType_GetWallFlags(&play->colCtx, wallPoly, wallBgId);

        wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
        yawTarget = Math_Atan2S(-wallPolyNormalZ, -wallPolyNormalX);
        Math_ScaledStepToS(&this->actor.shape.rot.y, yawTarget, 800);

        this->yaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = posResult.x - (Math_SinS(this->actor.shape.rot.y) * xzDistToWall);
        this->actor.world.pos.z = posResult.z - (Math_CosS(this->actor.shape.rot.y) * xzDistToWall);

        return true;
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;

    return false;
}

s32 Player_ProcessGrabPushPullWallInteraction(PlayState* play, Player* this) {
    return Player_ProcessSpecialWallInteraction(play, this, 26.0f, this->ageProperties->wallCheckRadius + 5.0f, 30.0f,
                                                0.0f);
}

/**
 * Two exit walls are placed at each end of the crawlspace, separate to the two entrance walls used to enter the
 * crawlspace. These front and back exit walls are futher into the crawlspace than the front and
 * back entrance walls. When player interacts with either of these two interior exit walls, start the leaving-crawlspace
 * cutscene and return true. Else, return false
 */
s32 Player_TryLeavingCrawlspace(Player* this, PlayState* play) {
    s16 yawToWall;

    if ((this->speedXZ != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
        (sTouchedWallFlags & WALL_FLAG_CRAWLSPACE)) {

        // The exit wallYaws will always point inward on the crawlline
        // Interacting with the exit wall in front will have a yaw diff of 0x8000
        // Interacting with the exit wall behind will have a yaw diff of 0
        yawToWall = this->actor.shape.rot.y - this->actor.wallYaw;
        if (this->speedXZ < 0.0f) {
            yawToWall += 0x8000;
        }

        if (ABS(yawToWall) > 0x4000) {
            Player_SetAction(play, this, Player_Action_LeaveCrawlspace, 0);

            if (this->speedXZ > 0.0f) {
                // Leaving a crawlspace forwards
                this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
                Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_child_tunnel_end);
                Player_AnimReplace_Setup(play, this, 0x9D);
                OnePointCutscene_Init(play, 9601, 999, NULL, CAM_ID_MAIN);
            } else {
                // Leaving a crawlspace backwards
                this->actor.shape.rot.y = this->actor.wallYaw;
                PlayerAnimation_Change(
                    play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start, -PLAYER_ANIM_NORMAL_SPEED,
                    Animation_GetLastFrame(&gPlayerAnim_link_child_tunnel_start), 0.0f, ANIMMODE_ONCE, 0.0f);
                Player_AnimReplace_Setup(play, this, 0x9D);
                OnePointCutscene_Init(play, 9602, 999, NULL, CAM_ID_MAIN);
            }

            this->yaw = this->actor.shape.rot.y;
            Player_SetHorizontalSpeedToZero(this);

            return true;
        }
    }

    return false;
}

void func_8083F72C(Player* this, PlayerAnimationHeader* anim, PlayState* play) {
    if (!Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupGrabWall)) {
        Player_SetAction(play, this, Player_Action_GrabWall, 0);
    }

    Player_Anim_PlayOnce(play, this, anim);
    Player_ClearAttentionModeAndStopMoving(this);

    this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
}

s32 Player_SwapAction_TrySpecialWallInteraction(Player* this, PlayState* play) {
    DynaPolyActor* wallPolyActor;

    if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sShapeYawToTouchedWall < 0x3000)) {

        if (((this->speedXZ > 0.0f) && func_8083EC18(this, play, sTouchedWallFlags)) ||
            Player_TryEnteringCrawlspace(this, play, sTouchedWallFlags)) {
            return true;
        }

        if (!Player_IsFreeSwimming(this) &&
            ((this->speedXZ == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL)) &&
            (sTouchedWallFlags & WALL_FLAG_6) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->yDistToLedge >= 39.0f)) {

            this->stateFlags2 |= PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL;

            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId)) != NULL)) {

                    if (wallPolyActor->actor.id == ACTOR_BG_HEAVY_BLOCK) {
                        if (Player_GetStrength() < PLAYER_STR_GOLD_G) {
                            return false;
                        }

                        Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupLift);
                        this->stateFlags1 |= PLAYER_STATE1_HOLDING_ACTOR;
                        this->interactRangeActor = &wallPolyActor->actor;
                        this->getItemId = GI_NONE;
                        this->yaw = this->actor.wallYaw + 0x8000;
                        Player_ClearAttentionModeAndStopMoving(this);

                        return true;
                    }

                    this->unk_3C4 = &wallPolyActor->actor;
                } else {
                    this->unk_3C4 = NULL;
                }

                func_8083F72C(this, &gPlayerAnim_link_normal_push_wait, play);

                return true;
            }
        }
    }

    return false;
}

s32 func_8083F9D0(PlayState* play, Player* this) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        ((this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) ||
         CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
        DynaPolyActor* wallPolyActor = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
        }

        if (&wallPolyActor->actor == this->unk_3C4) {
            if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
                return true;
            } else {
                return false;
            }
        }
    }

    Player_Setup2_IdleAll(this, play);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_push_wait_end);
    this->stateFlags2 &= ~PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    return true;
}

void Player_SetupPush(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Push, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_push_start);
}

void Player_SetupPull(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Pull, 0);
    this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    Player_Anim_PlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_start, this->modelAnimType));
}

void Player_LetGoWhileClimbing(Player* this, PlayState* play) {
    this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING | PLAYER_STATE1_SWIMMING);
    Player_Setup_Midair(this, play);
    this->speedXZ = -0.4f;
}

s32 Player_TryLettingGoWhileClimbing(Player* this, PlayState* play) {
    if (!CHECK_BTN_ALL(sControlInput->press.button, BTN_A) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        ((sTouchedWallFlags & WALL_FLAG_3) || (sTouchedWallFlags & WALL_FLAG_1) ||
         SurfaceType_CheckWallFlag2(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId))) {
        return false;
    }

    Player_LetGoWhileClimbing(this, play);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_AUTO_JUMP);
    return true;
}

s32 Player_GetZLockOnEnemyMoveDirection(Player* this, f32 inputVelocity, s16 inputYaw) {
    f32 targetYawDiff = (s16)(inputYaw - this->actor.shape.rot.y);
    f32 yawRatio;

    if (this->lockOnActor != NULL) {
        Player_LookAtTargetActor(this, Player_IsAimingFpsItem(this) || Player_IsAimingBoomerang(this));
    }

    yawRatio = fabsf(targetYawDiff) / 0x8000;

    if (inputVelocity > ((SQ(yawRatio) * 50.0f) + 6.0f)) {
        return 1;
    } else if (inputVelocity > (((1.0f - yawRatio) * 10.0f) + 6.8f)) {
        return -1;
    }

    return 0;
}

// Updates focus and look angles, then returns direction to move in?
s32 Player_GetZParallelMoveDirection(Player* this, f32* inputVelocity, s16* inputYaw, PlayState* play) {
    s16 sp2E = *inputYaw - this->zTargetYaw;
    u16 sp2C = ABS(sp2E);

    if ((Player_IsAimingFpsItem(this) || Player_IsAimingBoomerang(this)) && (this->lockOnActor == NULL)) {
        *inputVelocity *= Math_SinS(sp2C);

        if (*inputVelocity != 0.0f) {
            *inputYaw = (((sp2E >= 0) ? 1 : -1) * 0x4000) + this->actor.shape.rot.y;
        } else {
            *inputYaw = this->actor.shape.rot.y;
        }

        if (this->lockOnActor != NULL) {
            Player_LookAtTargetActor(this, 1);
        } else {
            Math_SmoothStepToS(&this->actor.focus.rot.x, sControlInput->rel.stick_y * 240.0f, 14, 4000, 30);
            Player_UpdateLookAngles(this, true);
        }
    } else {
        if (this->lockOnActor != NULL) {
            return Player_GetZLockOnEnemyMoveDirection(this, *inputVelocity, *inputYaw);
        }

        Player_SetLookAngle(this, play);

        if ((*inputVelocity != 0.0f) && (sp2C < 6000)) {
            return 1;
        } else if (*inputVelocity > Math_SinS((0x4000 - (sp2C >> 1))) * 200.0f) {
            return -1;
        }
    }

    return 0;
}

s32 Player_GetPushPullDirection(Player* this, f32* arg1, s16* arg2) {
    s16 temp1 = *arg2 - this->actor.shape.rot.y;
    u16 temp2 = ABS(temp1);
    f32 temp3 = Math_CosS(temp2);

    *arg1 *= temp3;

    if (*arg1 != 0.0f) {
        if (temp3 > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

s32 Player_GetSpinAttackMoveDirection(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    Player_SetLookAngle(this, play);

    if ((*arg1 != 0.0f) || (ABS(this->yawDiffPrevFrame) > 400)) {
        s16 temp1 = *arg2 - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
        u16 temp2 = (ABS(temp1) - 0x2000) & 0xFFFF;

        if ((temp2 < 0x4000) || (this->yawDiffPrevFrame != 0)) {
            return -1;
        } else {
            return 1;
        }
    }

    return 0;
}

void func_80840138(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->leftRightBlendWeightTarget = 0.0f;
        } else {
            this->leftRightBlendWeightTarget = 1.0f;
        }
    }

    Math_StepToF(&this->leftRightBlendWeight, this->leftRightBlendWeightTarget, 0.3f);
}

void func_808401B0(PlayState* play, Player* this) {
    PlayerAnimation_BlendToJoint(play, &this->skelAnime, Player_GetWaitRightAnim(this), this->walkCurFrame,
                                 Player_GetWaitLeftAnim(this), this->walkCurFrame, this->leftRightBlendWeight,
                                 this->blendTable);
}

s32 Player_IsFrameTargetInStep(f32 curFrame, f32 frameStep, f32 maxFrame, f32 targetFrame) {
    f32 framesTotarget;

    if ((targetFrame == 0.0f) && (frameStep > 0.0f)) {
        targetFrame = maxFrame;
    }

    framesTotarget = (curFrame + frameStep) - targetFrame;

    if (((framesTotarget * frameStep) >= 0.0f) && (((framesTotarget - frameStep) * frameStep) < 0.0f)) {
        return true;
    }

    return false;
}

void Player_UpdateWalkFrame(Player* this, f32 frameStep) {
    f32 updateScale = R_UPDATE_RATE * 0.5f;

    frameStep *= updateScale;
    if (frameStep < -7.25) {
        frameStep = -7.25;
    } else if (frameStep > 7.25f) {
        frameStep = 7.25f;
    }

    if (1) {}

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (this->hoverBootsTimer != 0)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG);
    } else if (Player_IsFrameTargetInStep(this->walkCurFrame, frameStep, 29.0f, 10.0f) ||
               Player_IsFrameTargetInStep(this->walkCurFrame, frameStep, 29.0f, 24.0f)) {
        Player_AnimSfx_PlayFloorWalk(this, this->speedXZ);
        if (this->speedXZ > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_MAKING_NOTICABLE_SFX;
        }
    }

    this->walkCurFrame += frameStep;

    if (this->walkCurFrame < 0.0f) {
        this->walkCurFrame += 29.0f;
    } else if (this->walkCurFrame >= 29.0f) {
        this->walkCurFrame -= 29.0f;
    }
}

void Player_Action_IdleLockOnEnemy(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 temp1;
    u32 temp2;
    s16 temp3;
    s32 temp4;

    if (this->stateFlags3 & PLAYER_STATE3_ENDING_MELEE_ATTACK) {
        if (Player_GetMeleeWeaponHeld(this) != 0) {
            this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;
        } else {
            this->stateFlags3 &= ~PLAYER_STATE3_ENDING_MELEE_ATTACK;
        }
    }

    if (this->IdleLockOnEnemyVar16) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Anim_ResetMove(this);
            Player_Anim_PlayLoop(play, this, Player_GetWaitRightAnim(this));
            this->IdleLockOnEnemyVar16 = false;
            this->stateFlags3 &= ~PLAYER_STATE3_ENDING_MELEE_ATTACK;
        }
        Player_ResetLeftRightBlendWeight(this);
    } else {
        func_808401B0(play, this);
    }

    Player_StepHorizontalSpeedToZero(this);

    if (Player_TrySwappingAction(play, this, sZTargetEnemySwapActionList, true)) {
        return;
    }

    if (!Player_TryEnemyLockOn(this) &&
        (!Player_IsZParallelOrLockOnFriend(this) || (Player_UpperAction_ShieldStanding != this->upperActionFunc))) {
        func_8083CF10(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    temp1 = Player_GetZLockOnEnemyMoveDirection(this, inputVelocity, inputYaw);

    if (temp1 > 0) {
        Player_SetupRunTowardsYaw(this, play, inputYaw);
        return;
    }

    if (temp1 < 0) {
        Player_SetupBackwalkEnemy(this, inputYaw, play);
        return;
    }

    if (inputVelocity > 4.0f) {
        Player_SetupSidewalkFast(this, play);
        return;
    }

    Player_UpdateWalkFrame(this, (this->speedXZ * 0.3f) + 1.0f);
    func_80840138(this, inputVelocity, inputYaw);

    temp2 = this->walkCurFrame;
    if ((temp2 < 6) || ((temp2 - 0xE) < 6)) {
        Math_StepToF(&this->speedXZ, 0.0f, 1.5f);
        return;
    }

    temp3 = inputYaw - this->yaw;
    temp4 = ABS(temp3);

    if (temp4 > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.5f)) {
            this->yaw = inputYaw;
        }
        return;
    }

    Math_AsymStepToF(&this->speedXZ, inputVelocity * 0.3f, 2.0f, 1.5f);

    if (!(this->stateFlags3 & PLAYER_STATE3_ENDING_MELEE_ATTACK)) {
        Math_ScaledStepToS(&this->yaw, inputYaw, temp4 * 0.1f);
    }
}

void Player_Action_IdleZParallelOrLockOnFriend(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;
    s16 temp2;
    s32 temp3;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
    }

    Player_StepHorizontalSpeedToZero(this);

    if (Player_TrySwappingAction(play, this, sZTargetFriendSwapActionList, true)) {
        return;
    }

    if (Player_TryEnemyLockOn(this)) {
        Player_Setup2_IdleLockOnEnemy(this, play);
        return;
    }

    if (!Player_IsZParallelOrLockOnFriend(this)) {
        Player_SetAction_PreserveMoveFlags(play, this, Player_Action_Idle, 1);
        this->yaw = this->actor.shape.rot.y;
        return;
    }

    if (Player_UpperAction_ShieldStanding == this->upperActionFunc) {
        Player_Setup2_IdleLockOnEnemy(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
    moveDir = Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play);

    if (moveDir > 0) {
        Player_SetupRunTowardsYaw(this, play, inputYaw);
        return;
    }

    if (moveDir < 0) {
        Player_SetupBackwalkFriend(this, inputYaw, play);
        return;
    }

    if (inputVelocity > 4.9f) {
        Player_SetupSidewalkFast(this, play);
        Player_ResetLeftRightBlendWeight(this);
        return;
    }
    if (inputVelocity != 0.0f) {
        Player_SetupSidewalkSlow(this, play);
        return;
    }

    temp2 = inputYaw - this->actor.shape.rot.y;
    temp3 = ABS(temp2);

    if (temp3 > 800) {
        Player_SetupTurn(play, this, inputYaw);
    }
}

void Player_ChooseIdleAnim(PlayState* play, Player* this) {
    PlayerAnimationHeader* anim;
    PlayerAnimationHeader** animPtr;
    s32 heathIsCritical;
    s32 sp38;
    s32 sp34;

    if ((this->lockOnActor != NULL) ||
        (!(heathIsCritical = Health_IsCritical()) && ((this->unk_6AC = (this->unk_6AC + 1) & 1) != 0))) {
        this->stateFlags2 &= ~PLAYER_STATE2_IDLING;
        anim = Player_GetIdleAnim(this);
    } else {
        this->stateFlags2 |= PLAYER_STATE2_IDLING;
        if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
            anim = Player_GetIdleAnim(this);
        } else {
            sp38 = play->roomCtx.curRoom.behaviorType2;
            if (heathIsCritical) {
                if (this->unk_6AC >= 0) {
                    sp38 = 7;
                    this->unk_6AC = -1;
                } else {
                    sp38 = 8;
                }
            } else {
                sp34 = Rand_ZeroOne() * 5.0f;
                if (sp34 < 4) {
                    if (((sp34 != 0) && (sp34 != 3)) || ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                                                         ((sp34 == 3) || (Player_GetMeleeWeaponHeld(this) != 0)))) {
                        if ((sp34 == 0) && Player_IsHoldingTwoHandedWeapon(this)) {
                            sp34 = 4;
                        }
                        sp38 = sp34 + 9;
                    }
                }
            }
            animPtr = &D_80853D7C[sp38][0];
            if (this->modelAnimType != PLAYER_ANIMTYPE_1) {
                animPtr = &D_80853D7C[sp38][1];
            }
            anim = *animPtr;
        }
    }

    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED * sPlayerUnderwaterSpeedAdjustment,
                           0.0f, Animation_GetLastFrame(anim), ANIMMODE_ONCE, -6.0f);
}

void Player_Action_Idle(Player* this, PlayState* play) {
    s32 sp44 = func_80833350(this);
    s32 isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);
    f32 inputVelocity;
    s16 inputYaw;
    s16 yawDiff;

    if (sp44 > 0) {
        func_808333FC(this, sp44 - 1);
    }

    if (isAnimDone) {
        if (this->idleVar16 != 0) {
            if (DECR(this->idleVar16) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
            this->skelAnime.jointTable[0].y = (this->skelAnime.jointTable[0].y + ((this->idleVar16 & 1) * 0x50)) - 0x28;
        } else {
            Player_Anim_ResetMove(this);
            Player_ChooseIdleAnim(play, this);
        }
    }

    Player_StepHorizontalSpeedToZero(this);

    if (this->idleVar16 != 0) {
        return;
    }

    if (Player_TrySwappingAction(play, this, sIdleSwapActionList, true)) {
        return;
    }

    if (Player_TryEnemyLockOn(this)) {
        Player_Setup2_IdleLockOnEnemy(this, play);
        return;
    }

    if (Player_IsZParallelOrLockOnFriend(this)) {
        Player_Setup1_IdleZParallelOrLockOnFriend(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play);

    if (inputVelocity != 0.0f) {
        Player_SetupRunTowardsYaw(this, play, inputYaw);
        return;
    }

    yawDiff = inputYaw - this->actor.shape.rot.y;
    if (ABS(yawDiff) > DEG_TO_BINANG(4.4f)) {
        Player_SetupTurn(play, this, inputYaw);
        return;
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, inputYaw, DEG_TO_BINANG(6.595f));
    this->yaw = this->actor.shape.rot.y;

    if (Player_GetIdleAnim(this) == this->skelAnime.animation) {
        Player_SetLookAngle(this, play);
    }
}

void Player_Action_SidewalkSlow(Player* this, PlayState* play) {
    f32 frames;
    f32 coeff;
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;
    s16 temp2;
    s32 temp3;
    s32 direction;

    this->skelAnime.mode = 0;
    PlayerAnimation_SetUpdateFunction(&this->skelAnime);

    this->skelAnime.animation = Player_GetSidewalkSlowAnim(this);

    if (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk) {
        frames = 24.0f;
        coeff = -(R_PLAYER_BOOT_16 / 100.0f);
    } else {
        frames = 29.0f;
        coeff = R_PLAYER_BOOT_16 / 100.0f;
    }

    this->skelAnime.animLength = frames;
    this->skelAnime.endFrame = frames - 1.0f;

    if ((s16)(this->yaw - this->actor.shape.rot.y) >= 0) {
        direction = 1;
    } else {
        direction = -1;
    }

    this->skelAnime.playSpeed = direction * (this->speedXZ * coeff);

    PlayerAnimation_Update(play, &this->skelAnime);

    if (PlayerAnimation_OnFrame(&this->skelAnime, 0.0f) || PlayerAnimation_OnFrame(&this->skelAnime, frames * 0.5f)) {
        Player_AnimSfx_PlayFloorWalk(this, this->speedXZ);
    }

    if (Player_TrySwappingAction(play, this, sSidewalkSlowSwapActionList, true)) {
        return;
    }

    if (Player_TryEnemyLockOn(this)) {
        Player_Setup2_IdleLockOnEnemy(this, play);
        return;
    }

    if (!Player_IsZParallelOrLockOnFriend(this)) {
        Player_SetupIdleWithMorph(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
    moveDir = Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play);

    if (moveDir > 0) {
        Player_SetupRunTowardsYaw(this, play, inputYaw);
        return;
    }

    if (moveDir < 0) {
        Player_SetupBackwalkFriend(this, inputYaw, play);
        return;
    }

    if (inputVelocity > 4.9f) {
        Player_SetupSidewalkFast(this, play);
        Player_ResetLeftRightBlendWeight(this);
        return;
    }

    if ((inputVelocity == 0.0f) && (this->speedXZ == 0.0f)) {
        Player_Setup1_IdleZParallelOrLockOnFriend(this, play);
        return;
    }

    temp2 = inputYaw - this->yaw;
    temp3 = ABS(temp2);

    if (temp3 > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.5f)) {
            this->yaw = inputYaw;
        }
        return;
    }

    Math_AsymStepToF(&this->speedXZ, inputVelocity * 0.4f, 1.5f, 1.5f);
    Math_ScaledStepToS(&this->yaw, inputYaw, temp3 * 0.1f);
}

void func_80841138(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;
        Player_UpdateWalkFrame(this, R_PLAYER_BOOT_4 / 1000.0f);
        PlayerAnimation_LoadToJoint(play, &this->skelAnime,
                                    GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                    this->walkCurFrame);
        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }
        temp1 = this->unk_864;
    } else {
        temp2 = this->speedXZ - (REG(48) / 100.0f);
        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            Player_UpdateWalkFrame(this, (R_PLAYER_BOOT_4 / 1000.0f) + ((R_PLAYER_BOOT_5 / 1000.0f) * this->speedXZ));
            PlayerAnimation_LoadToJoint(play, &this->skelAnime,
                                        GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                        this->walkCurFrame);
        } else {
            temp1 = (R_PLAYER_BOOT_6 / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                Player_UpdateWalkFrame(this,
                                       (R_PLAYER_BOOT_4 / 1000.0f) + ((R_PLAYER_BOOT_5 / 1000.0f) * this->speedXZ));
            } else {
                temp1 = 1.0f;
                Player_UpdateWalkFrame(this, 1.2f + ((R_PLAYER_BOOT_7 / 1000.0f) * temp2));
            }
            PlayerAnimation_LoadToMorph(play, &this->skelAnime,
                                        GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType),
                                        this->walkCurFrame);
            PlayerAnimation_LoadToJoint(play, &this->skelAnime, &gPlayerAnim_link_normal_back_run,
                                        this->walkCurFrame * (16.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        PlayerAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void Player_SetupBackwalkHaltFriend(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_BackwalkHaltFriend, 1);
    Player_Anim_PlayOnceWithMorph(play, this, &gPlayerAnim_link_normal_back_brake);
}

s32 func_80841458(Player* this, f32* arg1, s16* inputYaw, PlayState* play) {
    if (this->speedXZ > 6.0f) {
        Player_SetupBackwalkHaltFriend(this, play);
        return true;
    }

    if (*arg1 != 0.0f) {
        if (Player_StepHorizontalSpeedToZero(this)) {
            *arg1 = 0.0f;
            *inputYaw = this->yaw;
        } else {
            return true;
        }
    }

    return false;
}

void Player_Action_BackwalkFriend(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;
    s16 targetYawDiff;

    func_80841138(this, play);

    if (Player_TrySwappingAction(play, this, sBackwalkFriendSwapActionList, true)) {
        return;
    }

    if (!Player_TryZTargeting(this)) {
        Player_SetupRunTowardsYaw(this, play, this->yaw);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
    moveDir = Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play);

    if (moveDir >= 0) {
        if (!func_80841458(this, &inputVelocity, &inputYaw, play)) {
            if (moveDir != 0) {
                Player_SetupRun(this, play);
            } else if (inputVelocity > 4.9f) {
                Player_SetupSidewalkFast(this, play);
            } else {
                Player_SetupSidewalkSlow(this, play);
            }
        }
    } else {
        targetYawDiff = inputYaw - this->yaw;

        Math_AsymStepToF(&this->speedXZ, inputVelocity * 1.5f, 1.5f, 2.0f);
        Math_ScaledStepToS(&this->yaw, inputYaw, targetYawDiff * 0.1f);

        if ((inputVelocity == 0.0f) && (this->speedXZ == 0.0f)) {
            Player_Setup1_IdleZParallelOrLockOnFriend(this, play);
        }
    }
}

void Player_SetupBackwalkEndHaltFriend(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_BackwalkEndHaltFriend, 1);
    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_back_brake_end);
}

void Player_Action_BackwalkHaltFriend(Player* this, PlayState* play) {
    s32 isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);
    f32 inputVelocity;
    s16 inputYaw;

    Player_StepHorizontalSpeedToZero(this);

    if (Player_TrySwappingAction(play, this, sBackwalkFriendSwapActionList, true)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (this->speedXZ == 0.0f) {
        this->yaw = this->actor.shape.rot.y;

        if (Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play) > 0) {
            Player_SetupRun(this, play);
        } else if ((inputVelocity != 0.0f) || isAnimDone) {
            Player_SetupBackwalkEndHaltFriend(this, play);
        }
    }
}

void Player_Action_BackwalkEndHaltFriend(Player* this, PlayState* play) {
    s32 isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_TrySwappingAction(play, this, sBackwalkFriendSwapActionList, true)) {
        return;
    }

    if (isAnimDone) {
        Player_Setup1_IdleZParallelOrLockOnFriend(this, play);
    }
}

void Player_UpdateSidewalkFastAnim(PlayState* play, Player* this) {
    f32 curFrame;
    PlayerAnimationHeader* sidewalkLeftAnim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkL, this->modelAnimType);
    PlayerAnimationHeader* sidewalkRightAnim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType);

    this->skelAnime.animation = sidewalkLeftAnim;

    Player_UpdateWalkFrame(this, (R_PLAYER_BOOT_1 / 1000.0f) + ((R_PLAYER_BOOT_2 / 1000.0f) * this->speedXZ));

    curFrame = this->walkCurFrame * (16.0f / 29.0f);
    PlayerAnimation_BlendToJoint(play, &this->skelAnime, sidewalkRightAnim, curFrame, sidewalkLeftAnim, curFrame,
                                 this->leftRightBlendWeight, this->blendTable);
}

void Player_Action_SidewalkFast(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;
    s16 temp2;
    s32 temp3;

    Player_UpdateSidewalkFastAnim(play, this);

    if (Player_TrySwappingAction(play, this, sSidewalkFastSwapActionList, true)) {
        return;
    }

    if (!Player_TryZTargeting(this)) {
        Player_SetupRun(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (Player_IsZParallelOrLockOnFriend(this)) {
        moveDir = Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play);
    } else {
        moveDir = Player_GetZLockOnEnemyMoveDirection(this, inputVelocity, inputYaw);
    }

    if (moveDir > 0) {
        Player_SetupRun(this, play);
        return;
    }

    if (moveDir < 0) {
        if (Player_IsZParallelOrLockOnFriend(this)) {
            Player_SetupBackwalkFriend(this, inputYaw, play);
        } else {
            Player_SetupBackwalkEnemy(this, inputYaw, play);
        }
        return;
    }

    if ((this->speedXZ < 3.6f) && (inputVelocity < 4.0f)) {
        if (!Player_IsEnemyLockOn(this) && Player_IsZParallelOrLockOnFriend(this)) {
            Player_SetupSidewalkSlow(this, play);
        } else {
            Player_Setup1_IdleAll(this, play);
        }
        return;
    }

    func_80840138(this, inputVelocity, inputYaw);

    temp2 = inputYaw - this->yaw;
    temp3 = ABS(temp2);

    if (temp3 > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 3.0f) != 0) {
            this->yaw = inputYaw;
        }
        return;
    }

    inputVelocity *= 0.9f;
    Math_AsymStepToF(&this->speedXZ, inputVelocity, 2.0f, 3.0f);
    Math_ScaledStepToS(&this->yaw, inputYaw, temp3 * 0.1f);
}

void Player_Action_Turn(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_IsHoldingTwoHandedWeapon(this)) {
        AnimationContext_SetLoadFrame(play, Player_GetIdleAnim(this), 0, this->skelAnime.limbCount,
                                      this->skelAnime.morphTable);
        AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                     this->skelAnime.morphTable, D_80853410);
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play);

    if (Player_TrySwappingAction(play, this, sTurnSwapActionList, true)) {
        return;
    }

    if (inputVelocity != 0.0f) {
        this->actor.shape.rot.y = inputYaw;
        Player_SetupRun(this, play);
    } else if (Math_ScaledStepToS(&this->actor.shape.rot.y, inputYaw, this->unk_87E)) {
        Player_SetupIdle(this, play);
    }

    this->yaw = this->actor.shape.rot.y;
}

void Player_BlendRunAnims(Player* this, s32 blendType, PlayState* play) {
    PlayerAnimationHeader* anim;
    s16 floorPitch;
    f32 blendWeight;

    if (ABS(sFloorPitchShape) < DEG_TO_BINANG(20.0f)) {
        floorPitch = 0;
    } else {
        floorPitch = CLAMP(sFloorPitchShape, -DEG_TO_BINANG(60.0f), DEG_TO_BINANG(60.0f));
    }

    Math_ScaledStepToS(&this->walkFloorPitch, floorPitch, DEG_TO_BINANG(2.1973f));

    if ((this->modelAnimType == PLAYER_ANIMTYPE_3) || ((this->walkFloorPitch == 0) && (this->shapeOffsetY <= 0.0f))) {
        if (blendType == 0) {
            PlayerAnimation_LoadToJoint(play, &this->skelAnime,
                                        GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                        this->walkCurFrame);
        } else {
            PlayerAnimation_LoadToMorph(play, &this->skelAnime,
                                        GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                        this->walkCurFrame);
        }
        return;
    }

    if (this->walkFloorPitch != 0) {
        blendWeight = (f32)this->walkFloorPitch / DEG_TO_BINANG(60.0f);
    } else {
        blendWeight = this->shapeOffsetY * 0.0006f;
    }

    blendWeight *= fabsf(this->speedXZ) * 0.5f;

    if (blendWeight > 1.0f) {
        blendWeight = 1.0f;
    }

    if (blendWeight < 0.0f) {
        anim = &gPlayerAnim_link_normal_climb_down;
        blendWeight = -blendWeight;
    } else {
        anim = &gPlayerAnim_link_normal_climb_up;
    }

    if (blendType == 0) {
        PlayerAnimation_BlendToJoint(play, &this->skelAnime,
                                     GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType), this->walkCurFrame,
                                     anim, this->walkCurFrame, blendWeight, this->blendTable);
    } else {
        PlayerAnimation_BlendToMorph(play, &this->skelAnime,
                                     GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType), this->walkCurFrame,
                                     anim, this->walkCurFrame, blendWeight, this->blendTable);
    }
}

void Player_UpdateRunAnim(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        temp1 = R_UPDATE_RATE * 0.5f;

        Player_UpdateWalkFrame(this, R_PLAYER_BOOT_4 / 1000.0f);
        PlayerAnimation_LoadToJoint(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                    this->walkCurFrame);

        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }

        temp1 = this->unk_864;
    } else {
        temp2 = this->speedXZ - (REG(48) / 100.0f);

        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            Player_UpdateWalkFrame(this, (R_PLAYER_BOOT_4 / 1000.0f) + ((R_PLAYER_BOOT_5 / 1000.0f) * this->speedXZ));

            Player_BlendRunAnims(this, 0, play);
        } else {
            temp1 = (R_PLAYER_BOOT_6 / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                Player_UpdateWalkFrame(this,
                                       (R_PLAYER_BOOT_4 / 1000.0f) + ((R_PLAYER_BOOT_5 / 1000.0f) * this->speedXZ));
            } else {
                temp1 = 1.0f;
                Player_UpdateWalkFrame(this, 1.2f + ((R_PLAYER_BOOT_7 / 1000.0f) * temp2));
            }

            Player_BlendRunAnims(this, 1, play);

            PlayerAnimation_LoadToJoint(play, &this->skelAnime, Player_GetRunAnim(this),
                                        this->walkCurFrame * (20.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        PlayerAnimation_InterpJointMorph(play, &this->skelAnime, 1.0f - temp1);
    }
}

void Player_Action_Run(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    Player_UpdateRunAnim(this, play);

    if (Player_TrySwappingAction(play, this, sRunSwapActionList, true)) {
        return;
    }

    if (Player_TryZTargeting(this)) {
        Player_SetupRun(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play);

    if (func_8083C484(this, &inputVelocity, &inputYaw)) {
        return;
    }

    func_8083DF68(this, inputVelocity, inputYaw);
    func_8083DDC8(this, play);

    if ((this->speedXZ == 0.0f) && (inputVelocity == 0.0f)) {
        func_8083C0B8(this, play);
    }
}

void Player_Action_RunZTarget(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    Player_UpdateRunAnim(this, play);

    if (Player_TrySwappingAction(play, this, sRunZTargetSwapActionList, true)) {
        return;
    }

    if (!Player_TryZTargeting(this)) {
        Player_SetupRun(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (func_8083C484(this, &inputVelocity, &inputYaw)) {
        return;
    }

    if ((Player_IsZParallelOrLockOnFriend(this) && (inputVelocity != 0.0f) &&
         (Player_GetZParallelMoveDirection(this, &inputVelocity, &inputYaw, play) <= 0)) ||
        (!Player_IsZParallelOrLockOnFriend(this) &&
         (Player_GetZLockOnEnemyMoveDirection(this, inputVelocity, inputYaw) <= 0))) {
        Player_Setup1_IdleAll(this, play);
        return;
    }

    func_8083DF68(this, inputVelocity, inputYaw);
    func_8083DDC8(this, play);

    if ((this->speedXZ == 0) && (inputVelocity == 0)) {
        Player_Setup1_IdleAll(this, play);
    }
}

void Player_Action_BackwalkEnemy(Player* this, PlayState* play) {
    s32 isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);
    f32 inputVelocity;
    s16 inputYaw;

    if (Player_TrySwappingAction(play, this, sSidewalkFastSwapActionList, true)) {
        return;
    }

    if (!Player_TryZTargeting(this)) {
        Player_SetupRun(this, play);
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
        Player_StepHorizontalSpeedToZero(this);

        if ((this->skelAnime.curFrame > 10.0f) &&
            (Player_GetZLockOnEnemyMoveDirection(this, inputVelocity, inputYaw) < 0)) {
            Player_SetupBackwalkEnemy(this, inputYaw, play);
            return;
        }

        if (isAnimDone) {
            Player_SetupBackwalkEndEnemy(this, play);
        }
    }
}

void Player_Action_BackwalkEndEnemy(Player* this, PlayState* play) {
    s32 isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);
    f32 inputVelocity;
    s16 inputYaw;

    Player_StepHorizontalSpeedToZero(this);

    if (Player_TrySwappingAction(play, this, sBackwalkEndSwapActionList, true)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (this->speedXZ == 0.0f) {
        this->yaw = this->actor.shape.rot.y;

        if (Player_GetZLockOnEnemyMoveDirection(this, inputVelocity, inputYaw) > 0) {
            Player_SetupRun(this, play);
            return;
        }

        if ((inputVelocity != 0.0f) || isAnimDone) {
            Player_Setup1_IdleAll(this, play);
        }
    }
}

void func_8084260C(Vec3f* src, Vec3f* dest, f32 arg2, f32 arg3, f32 arg4) {
    dest->x = (Rand_ZeroOne() * arg3) + src->x;
    dest->y = (Rand_ZeroOne() * arg4) + (src->y + arg2);
    dest->z = (Rand_ZeroOne() * arg3) + src->z;
}

static Vec3f D_808545B4 = { 0.0f, 0.0f, 0.0f };
static Vec3f D_808545C0 = { 0.0f, 0.0f, 0.0f };

s32 func_8084269C(PlayState* play, Player* this) {
    Vec3f sp2C;

    if ((this->floorSfxOffset == SURFACE_SFX_OFFSET_DIRT) || (this->floorSfxOffset == SURFACE_SFX_OFFSET_SAND)) {
        func_8084260C(&this->actor.shape.feetPos[FOOT_LEFT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_LEFT].y, 7.0f, 5.0f);
        func_800286CC(play, &sp2C, &D_808545B4, &D_808545C0, 50, 30);
        func_8084260C(&this->actor.shape.feetPos[FOOT_RIGHT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_RIGHT].y, 7.0f, 5.0f);
        func_800286CC(play, &this->actor.shape.feetPos[FOOT_RIGHT], &D_808545B4, &D_808545C0, 50, 30);
        return true;
    }

    return false;
}

void Player_Action_PlantMagicBeans(Player* this, PlayState* play) {
    Player_Anim_PlayLoopOnceFinished(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_wait, this->modelAnimType));

    if (DECR(this->plantMagicBeansVar16) == 0) {
        if (!Player_SwapAction_TryItemCsFirstPerson(this, play)) {
            Player_Setup4_IdleAll(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_end, this->modelAnimType), play);
        }

        this->actor.flags &= ~ACTOR_FLAG_TALK_REQUESTED;
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
    }
}

s32 func_8084285C(Player* this, f32 arg1, f32 arg2, f32 arg3) {
    if ((arg1 <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= arg3)) {
        func_80833A20(this, (arg2 <= this->skelAnime.curFrame) ? 1 : -1);
        return true;
    }

    Player_ResetAttack(this);
    return false;
}

s32 Player_TryCrouchStab(Player* this, PlayState* play) {
    if (!Player_IsChildWithHylianShield(this) && (Player_GetMeleeWeaponHeld(this) != 0) && D_80853614) {
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_defense_kiru);
        this->actionVar8 = 1;
        this->meleeWeaponAnimation = PLAYER_MWA_STAB_1H;
        this->yaw = this->actor.shape.rot.y + this->upperLimbRot.y;
        return true;
    }

    return false;
}

s32 Player_TryInteracting(Player* this, PlayState* play) {
    return Player_SwapAction_TryItemCsFirstPerson(this, play) || Player_SwapAction_TryTalking(this, play) ||
           Player_SwapAction_TryGetItem(this, play);
}

void Player_RequestQuake(PlayState* play, s32 speed, s32 y, s32 duration) {
    s32 quakeIndex = Quake_Request(Play_GetCamera(play, CAM_ID_MAIN), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, speed);
    Quake_SetPerturbations(quakeIndex, y, 0, 0, 0);
    Quake_SetDuration(quakeIndex, duration);
}

void Player_RequestQuakeAndRumble(PlayState* play, Player* this) {
    Player_RequestQuake(play, 27767, 7, 20);
    play->actorCtx.unk_02 = 4;
    Player_RequestRumble(this, 255, 20, 150, SQ(0));
    Player_PlaySfx(this, NA_SE_IT_HAMMER_HIT);
}

void Player_ConsumeDekuStick(PlayState* play, Player* this) {
    Inventory_ChangeAmmo(ITEM_DEKU_STICK, -1);
    Player_UseItem(play, this, ITEM_NONE);
}

s32 Player_TryBreakingDekuStick(PlayState* play, Player* this) {
    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->dekuStickLength > 0.5f)) {
        if (AMMO(ITEM_DEKU_STICK) != 0) {
            EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND], this->actor.shape.rot.y + 0x8000);
            this->dekuStickLength = 0.5f;
            Player_ConsumeDekuStick(play, this);
            Player_PlaySfx(this, NA_SE_IT_WOODSTICK_BROKEN);
        }

        return true;
    }

    return false;
}

s32 Player_TryBreakingGiantsKnife(PlayState* play, Player* this) {
    if (this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) {
        if (!gSaveContext.bgsFlag && (gSaveContext.swordHealth > 0.0f)) {
            if ((gSaveContext.swordHealth -= 1.0f) <= 0.0f) {
                EffectSsStick_Spawn(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                    this->actor.shape.rot.y + 0x8000);
                Inventory_BreakGiantsKnife(play);
                Player_PlaySfx(this, NA_SE_IT_MAJIN_SWORD_BROKEN);
            }
        }

        return true;
    }

    return false;
}

void Player_TryBreakingMeleeWeapons(PlayState* play, Player* this) {
    Player_TryBreakingDekuStick(play, this);
    Player_TryBreakingGiantsKnife(play, this);
}

static PlayerAnimationHeader* sRecoilAnims[] = {
    &gPlayerAnim_link_fighter_rebound,
    &gPlayerAnim_link_fighter_rebound_long,
    &gPlayerAnim_link_fighter_reboundR,
    &gPlayerAnim_link_fighter_rebound_longR,
};

void Player_TryRecoiling(PlayState* play, Player* this) {
    s32 pad;
    s32 sp28;

    if (Player_Action_ShieldCrouched != this->actionFunc) {
        Player_ResetAttributes(play, this);
        Player_SetAction(play, this, Player_Action_Recoil, 0);

        if (Player_IsEnemyLockOn(this)) {
            sp28 = 2;
        } else {
            sp28 = 0;
        }

        Player_Anim_PlayOnceAdjusted(play, this, sRecoilAnims[Player_IsHoldingTwoHandedWeapon(this) + sp28]);
    }

    Player_RequestRumble(this, 180, 20, 100, SQ(0));
    this->speedXZ = -18.0f;
    Player_TryBreakingMeleeWeapons(play, this);
}

s32 Player_ProcessAttackCollision(PlayState* play, Player* this) {
    f32 weaponCheckDistScale;
    CollisionPoly* groundPoly;
    s32 bgId;
    Vec3f weaponBaseCheckPos;
    Vec3f sp5C;
    Vec3f tipBaseDiff;
    s32 temp1;
    s32 surfaceMaterial;

    if (this->meleeWeaponState > 0) {
        if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            if (!(this->meleeWeaponQuads[0].base.atFlags & AT_BOUNCED) &&
                !(this->meleeWeaponQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {

                    weaponCheckDistScale = Math_Vec3f_DistXYZAndStoreDiff(&this->meleeWeaponInfo[0].tip,
                                                                          &this->meleeWeaponInfo[0].base, &tipBaseDiff);
                    if (weaponCheckDistScale != 0.0f) {
                        weaponCheckDistScale = (weaponCheckDistScale + 10.0f) / weaponCheckDistScale;
                    }

                    weaponBaseCheckPos.x = this->meleeWeaponInfo[0].tip.x + (tipBaseDiff.x * weaponCheckDistScale);
                    weaponBaseCheckPos.y = this->meleeWeaponInfo[0].tip.y + (tipBaseDiff.y * weaponCheckDistScale);
                    weaponBaseCheckPos.z = this->meleeWeaponInfo[0].tip.z + (tipBaseDiff.z * weaponCheckDistScale);

                    if (BgCheck_EntityLineTest1(&play->colCtx, &weaponBaseCheckPos, &this->meleeWeaponInfo[0].tip,
                                                &sp5C, &groundPoly, true, false, false, true, &bgId) &&
                        !SurfaceType_IsIgnoredByEntities(&play->colCtx, groundPoly, bgId) &&
                        (SurfaceType_GetFloorType(&play->colCtx, groundPoly, bgId) != FLOOR_TYPE_6) &&
                        !Actor_TryCollisionWithJabuSurface(play, &this->actor, groundPoly, bgId, &sp5C)) {

                        if (this->heldItemAction == PLAYER_IA_HAMMER) {
                            Player_SetOneFrameFreezeFlash(play);
                            Player_RequestQuakeAndRumble(play, this);
                            Player_TryRecoiling(play, this);
                            return true;
                        }

                        if (this->speedXZ >= 0.0f) {
                            surfaceMaterial = SurfaceType_GetMaterial(&play->colCtx, groundPoly, bgId);

                            if (surfaceMaterial == SURFACE_MATERIAL_WOOD) {
                                CollisionCheck_SpawnShieldParticlesWood(play, &sp5C, &this->actor.projectedPos);
                            } else {
                                CollisionCheck_SpawnShieldParticles(play, &sp5C);
                                if (surfaceMaterial == SURFACE_MATERIAL_DIRT_SOFT) {
                                    Player_PlaySfx(this, NA_SE_IT_WALL_HIT_SOFT);
                                } else {
                                    Player_PlaySfx(this, NA_SE_IT_WALL_HIT_HARD);
                                }
                            }

                            Player_TryBreakingMeleeWeapons(play, this);
                            this->speedXZ = -14.0f;
                            Player_RequestRumble(this, 180, 20, 100, SQ(0));
                        }
                    }
                }
            } else {
                Player_TryRecoiling(play, this);
                Player_SetOneFrameFreezeFlash(play);
                return true;
            }
        }

        temp1 = (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT);

        if (temp1) {
            if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
                Actor* at = this->meleeWeaponQuads[temp1 ? 1 : 0].base.at;

                if ((at != NULL) && (at->id != ACTOR_EN_KANBAN)) {
                    Player_SetOneFrameFreezeFlash(play);
                }
            }

            if (!Player_TryBreakingDekuStick(play, this) && (this->heldItemAction != PLAYER_IA_HAMMER)) {
                Player_TryBreakingGiantsKnife(play, this);

                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    Player_ApplyDamage(play, this, PLAYER_DMGREACTION_ELECTRIC_SHOCK, 0.0f, 0.0f,
                                       this->actor.shape.rot.y, 20);
                    return true;
                }
            }
        }
    }

    return false;
}

void Player_Action_ShieldCrouched(Player* this, PlayState* play) {
    f32 sp54;
    f32 sp50;
    s16 sp4E;
    s16 sp4C;
    s16 sp4A;
    s16 sp48;
    s16 sp46;
    f32 sp40;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!Player_IsChildWithHylianShield(this)) {
            Player_Anim_PlayLoop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_wait, this->modelAnimType));
        }
        this->shieldAimCrouchedVar16 = true;
        this->shieldAimCrouchedVar8 = 0;
    }

    if (!Player_IsChildWithHylianShield(this)) {
        this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;
        Player_TryUpperAction(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_SHIELD;
    }

    Player_StepHorizontalSpeedToZero(this);

    if (this->shieldAimCrouchedVar16) {
        sp54 = sControlInput->rel.stick_y * 100;
        sp50 = sControlInput->rel.stick_x * -120;
        sp4E = this->actor.shape.rot.y - Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

        sp40 = Math_CosS(sp4E);
        sp4C = (Math_SinS(sp4E) * sp50) + (sp54 * sp40);
        sp40 = Math_CosS(sp4E);
        sp4A = (sp50 * sp40) - (Math_SinS(sp4E) * sp54);

        if (sp4C > 3500) {
            sp4C = 3500;
        }

        sp48 = ABS(sp4C - this->actor.focus.rot.x) * 0.25f;
        if (sp48 < 100) {
            sp48 = 100;
        }

        sp46 = ABS(sp4A - this->upperLimbRot.y) * 0.25f;
        if (sp46 < 50) {
            sp46 = 50;
        }

        Math_ScaledStepToS(&this->actor.focus.rot.x, sp4C, sp48);
        this->upperLimbRot.x = this->actor.focus.rot.x;
        Math_ScaledStepToS(&this->upperLimbRot.y, sp4A, sp46);

        if (this->shieldAimCrouchedVar8 != 0) {
            if (!Player_ProcessAttackCollision(play, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    func_80833A20(this, 1);
                }
            } else {
                this->shieldAimCrouchedVar16 = true;
                this->shieldAimCrouchedVar8 = 0;
            }
        } else if (!Player_TryInteracting(this, play)) {
            if (Player_SwapAction_TryShieldingCrouched(this, play)) {
                Player_TryCrouchStab(this, play);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_SHIELD;
                Player_ResetAttack(this);

                if (Player_IsChildWithHylianShield(this)) {
                    Player_Setup3_IdleAll(this, play);
                    PlayerAnimation_Change(
                        play, &this->skelAnime, &gPlayerAnim_clink_normal_defense_ALL, PLAYER_ANIM_NORMAL_SPEED,
                        Animation_GetLastFrame(&gPlayerAnim_clink_normal_defense_ALL), 0.0f, ANIMMODE_ONCE, 0.0f);
                    Player_AnimReplace_Setup(play, this, 4);
                } else {
                    if (this->itemAction < 0) {
                        Player_SetHeldItem(this);
                    }
                    Player_Setup4_IdleAll(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_end, this->modelAnimType),
                                          play);
                }

                Player_PlaySfx(this, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        } else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;
    Player_SetModelsForHoldingShield(this);

    this->rotOverrideFlags |=
        PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_UPPER_ROT_X | PLAYER_ROT_OVERRIDE_UPPER_ROT_Y;
}

void Player_Action_ShieldDeflectAttack(Player* this, PlayState* play) {
    s32 actionInterruptState;
    PlayerAnimationHeader* anim;
    f32 frames;

    Player_StepHorizontalSpeedToZero(this);

    if (this->shieldDeflectAttackVar8 == 0) {
        D_808535E0 = Player_TryUpperAction(this, play);
        if ((Player_UpperAction_ShieldStanding == this->upperActionFunc) ||
            (Player_GetActionInterruptState(play, this, &this->upperSkelAnime, 4.0f) >= PLAYER_ACTION_INTERRUPT_MOVE)) {
            Player_SetAction(play, this, Player_Action_IdleLockOnEnemy, 1);
        }
    } else {
        actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 4.0f);
        if (actionInterruptState != PLAYER_ACTION_INTERRUPT_SWAP) {
            if ((actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE) ||
                PlayerAnimation_Update(play, &this->skelAnime)) {
                Player_SetAction(play, this, Player_Action_ShieldCrouched, 1);
                this->stateFlags1 |= PLAYER_STATE1_HOLDING_SHIELD;
                Player_SetModelsForHoldingShield(this);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
                frames = Animation_GetLastFrame(anim);
                PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_NORMAL_SPEED, frames, frames,
                                       ANIMMODE_ONCE, 0.0f);
            }
        }
    }
}

void Player_Action_Damage(Player* this, PlayState* play) {
    s32 actionInterruptState;

    Player_StepHorizontalSpeedToZero(this);

    actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 16.0f);
    if (actionInterruptState != PLAYER_ACTION_INTERRUPT_SWAP) {
        if (PlayerAnimation_Update(play, &this->skelAnime) || (actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE)) {
            Player_Setup1_IdleAll(this, play);
        }
    }
}

void Player_Action_KnockbackFly(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;

    Player_GiveOnceSecondInvincibility(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && (this->knockbackFlyVar16 == 0) &&
        (this->specialDamageEffect != 0)) {
        s16 temp = this->actor.shape.rot.y - this->damageYaw;

        this->yaw = this->actor.shape.rot.y = this->damageYaw;
        this->speedXZ = this->damageSpeedXZ;

        if (ABS(temp) > 0x4000) {
            this->actor.shape.rot.y = this->damageYaw + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->knockbackFlyVar16 != 0) {
            this->knockbackFlyVar16--;
            if (this->knockbackFlyVar16 == 0) {
                Player_SetupIdleWithMorph(this, play);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) ||
                   (!(this->cylinder.base.acFlags & AC_HIT) && (this->specialDamageEffect == 0))) {
            if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
                this->knockbackFlyVar16++;
            } else {
                Player_SetAction(play, this, Player_Action_KnockbackDown, 0);
                this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;
            }

            Player_Anim_PlayOnce(play, this,
                                 (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_downB
                                                                        : &gPlayerAnim_link_normal_back_downB);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Player_AnimSfx_PlayFloor(this, NA_SE_PL_BOUND);
    }
}

void Player_Action_KnockbackDown(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;

    Player_GiveOnceSecondInvincibility(this);
    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->speedXZ == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            this->knockbackDownVar16++;
        } else {
            Player_SetAction(play, this, Player_Action_KnockbackGetUp, 0);
            this->stateFlags1 |= PLAYER_STATE1_TAKING_DAMAGE;
        }

        Player_Anim_PlayOnceAdjusted(play, this,
                                     (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_down_wake
                                                                            : &gPlayerAnim_link_normal_back_down_wake);
        this->yaw = this->actor.shape.rot.y;
    }
}

static AnimSfxEntry sKnockbackGetUpAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 20, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 30, NA_SE_NONE, STOP),
};

void Player_Action_KnockbackGetUp(Player* this, PlayState* play) {
    s32 actionInterruptState;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    Player_GiveOnceSecondInvincibility(this);

    if (this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
        PlayerAnimation_Update(play, &this->skelAnime);
    } else {
        actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 16.0f);
        if (actionInterruptState != PLAYER_ACTION_INTERRUPT_SWAP) {
            if (PlayerAnimation_Update(play, &this->skelAnime) ||
                (actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE)) {
                Player_Setup1_IdleAll(this, play);
            }
        }
    }

    Player_AnimSfx_Play(this, sKnockbackGetUpAnimSfx);
}

static Vec3f D_808545E4 = { 0.0f, 0.0f, 5.0f };

void func_80843AE8(PlayState* play, Player* this) {
    if (this->actionVar16 != 0) {
        if (this->actionVar16 > 0) {
            this->actionVar16--;
            if (this->actionVar16 == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    PlayerAnimation_Change(
                        play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_wait, PLAYER_ANIM_NORMAL_SPEED, 0.0f,
                        Animation_GetLastFrame(&gPlayerAnim_link_swimer_swim_wait), ANIMMODE_ONCE, -16.0f);
                } else {
                    PlayerAnimation_Change(
                        play, &this->skelAnime, &gPlayerAnim_link_derth_rebirth, PLAYER_ANIM_NORMAL_SPEED, 99.0f,
                        Animation_GetLastFrame(&gPlayerAnim_link_derth_rebirth), ANIMMODE_ONCE, 0.0f);
                }
                gSaveContext.healthAccumulator = 0x140;
                this->actionVar16 = -1;
            }
        } else if (gSaveContext.healthAccumulator == 0) {
            this->stateFlags1 &= ~PLAYER_STATE1_IN_DEATH_CUTSCENE;
            if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                Player_SetupSwimIdle(play, this);
            } else {
                Player_SetupIdleWithMorph(this, play);
            }
            this->unk_A87 = 20;
            Player_SetInvincibilityTimerWithoutDamageFlash(this, -20);
            Audio_SetBgmVolumeOnDuringFanfare();
        }
    } else if (this->actionVar8 != 0) {
        this->actionVar16 = 60;
        Player_SpawnFairy(play, this, &this->actor.world.pos, &D_808545E4, FAIRY_REVIVE_DEATH);
        Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
        OnePointCutscene_Init(play, 9908, 125, &this->actor, CAM_ID_MAIN);
    } else if (play->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        play->gameOverCtx.state = GAMEOVER_DEATH_DELAY_MENU;
    }
}

static AnimSfxEntry D_808545F0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 60, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 140, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 164, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 170, NA_SE_NONE, STOP),
};

void Player_Action_Die(Player* this, PlayState* play) {
    if (this->currentTunic != PLAYER_TUNIC_GORON) {
        if ((play->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3) || (sFloorType == FLOOR_TYPE_9) ||
            ((Player_GetHurtFloorType(sFloorType) >= 0) &&
             !func_80042108(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            Player_StartBurning(this);
        }
    }

    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actor.category == ACTORCAT_PLAYER) {
            func_80843AE8(play, this);
        }
        return;
    }

    if (this->skelAnime.animation == &gPlayerAnim_link_derth_rebirth) {
        Player_AnimSfx_Play(this, D_808545F0);
    } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_electric_shock_end) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 88.0f)) {
            Player_AnimSfx_PlayFloor(this, NA_SE_PL_BOUND);
        }
    }
}

void func_80843E14(Player* this, u16 sfxId) {
    Player_AnimSfx_PlayVoice(this, sfxId);

    if ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)) {
        Actor_PlaySfx(this->heldActor, NA_SE_VO_RT_FALL);
    }
}

typedef struct {
    /* 0x00 */ s8 damage;
    /* 0x01 */ u8 rumbleStrength;
    /* 0x02 */ u8 rumbleDuration;
    /* 0x03 */ u8 rumbleDecreaseRate;
    /* 0x04 */ u16 sfxId;
} FallImpactInfo; // size = 0x06

static FallImpactInfo D_80854600[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

s32 func_80843E64(PlayState* play, Player* this) {
    s32 fallDistance;

    if ((sFloorType == FLOOR_TYPE_6) || (sFloorType == FLOOR_TYPE_9)) {
        fallDistance = 0;
    } else {
        fallDistance = this->fallDistance;
    }

    Math_StepToF(&this->speedXZ, 0.0f, 1.0f);

    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);

    if (fallDistance >= 400) {
        s32 impactIndex;
        FallImpactInfo* impactInfo;

        if (this->fallDistance < 800) {
            impactIndex = 0;
        } else {
            impactIndex = 1;
        }

        impactInfo = &D_80854600[impactIndex];

        if (Player_InflictDamage(play, impactInfo->damage)) {
            return -1;
        }

        Player_SetInvincibilityTimerWithDamageFlash(this, 40);
        Player_RequestQuake(play, 32967, 2, 30);
        Player_RequestRumble(this, impactInfo->rumbleStrength, impactInfo->rumbleDuration,
                             impactInfo->rumbleDecreaseRate, SQ(0));
        Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
        Player_AnimSfx_PlayVoice(this, impactInfo->sfxId);

        return impactIndex + 1;
    }

    if (fallDistance > 200) {
        fallDistance *= 2;

        if (fallDistance > 255) {
            fallDistance = 255;
        }

        Player_RequestRumble(this, (u8)fallDistance, (u8)(fallDistance * 0.1f), (u8)fallDistance, SQ(0));

        if (sFloorType == FLOOR_TYPE_6) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    Player_AnimSfx_PlayFloorLand(this);

    return 0;
}

void func_8084409C(PlayState* play, Player* this, f32 speedXZ, f32 velocityY) {
    Actor* heldActor = this->heldActor;

    if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = speedXZ;
        heldActor->velocity.y = velocityY;
        Player_SetupUpperActionForHeldItem(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_Action_Midair(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    if (gSaveContext.respawn[RESPAWN_MODE_TOP].data > 40) {
        this->actor.gravity = 0.0f;
    } else if (Player_IsEnemyLockOn(this)) {
        this->actor.gravity = -1.2f;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
            Actor* heldActor = this->heldActor;

            if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
                func_8084409C(play, this, this->speedXZ + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        PlayerAnimation_Update(play, &this->skelAnime);

        if (!(this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING)) {
            func_8083DFE0(this, &inputVelocity, &inputYaw);
        }

        Player_TryUpperAction(this, play);

        if (!(this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) || (this->midairVar8 != 2)) {
            if (Player_TryJumpSlashing(this, play)) {
                return;
            }
        }

        if (this->actor.velocity.y < 0.0f) {
            if (this->midairVar16 >= 0) {
                if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->midairVar16 == 0) ||
                    (this->fallDistance > 0)) {
                    if ((sYDistToFloor > 800.0f) || (this->stateFlags1 & PLAYER_STATE1_END_HOOKSHOT_MOVE)) {
                        func_80843E14(this, NA_SE_VO_LI_FALL_S);
                        this->stateFlags1 &= ~PLAYER_STATE1_END_HOOKSHOT_MOVE;
                    }

                    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_landing,
                                           PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 8.0f);
                    this->midairVar16 = -1;
                }
            } else {
                if ((this->midairVar16 == -1) && (this->fallDistance > 120.0f) && (sYDistToFloor > 280.0f)) {
                    this->midairVar16 = -2;
                    func_80843E14(this, NA_SE_VO_LI_FALL_L);
                }

                if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
                    !(this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_HOLDING_ACTOR | PLAYER_STATE1_SWIMMING)) &&
                    (this->speedXZ > 0.0f)) {
                    if ((this->yDistToLedge >= 150.0f) &&
                        (this->analogStickDirection4Parts[this->inputFrameCounter] == 0)) {
                        func_8083EC18(this, play, sTouchedWallFlags);
                    } else if ((this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) && (this->yDistToLedge < 150.0f) &&
                               (((this->actor.world.pos.y - this->actor.floorHeight) + this->yDistToLedge) >
                                (70.0f * this->ageProperties->unk_08))) {
                        AnimationContext_DisableQueue(play);
                        if (this->stateFlags1 & PLAYER_STATE1_END_HOOKSHOT_MOVE) {
                            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                        } else {
                            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
                        }
                        this->actor.world.pos.y += this->yDistToLedge;
                        Player_SetupGrabLedge(play, this, this->actor.wallPoly, this->distToInteractWall,
                                              GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_hold, this->modelAnimType));
                        this->actor.shape.rot.y = this->yaw += 0x8000;
                        this->stateFlags1 |= PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP;
                    }
                }
            }
        }
        return;
    }

    {
        PlayerAnimationHeader* anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType);
        s32 sp3C;

        if (this->stateFlags2 & PLAYER_STATE2_BACKFLIPPING_OR_SIDEHOPPING) {
            if (Player_IsEnemyLockOn(this)) {
                anim = D_80853D4C[this->midairVar8][2];
            } else {
                anim = D_80853D4C[this->midairVar8][1];
            }
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_run_jump) {
            anim = &gPlayerAnim_link_normal_run_jump_end;
        } else if (Player_IsEnemyLockOn(this)) {
            anim = &gPlayerAnim_link_anchor_landingR;
            Player_ResetLeftRightBlendWeight(this);
        } else if (this->fallDistance <= 80) {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType);
        } else if ((this->fallDistance < 800) && (this->analogStickDirection4Parts[this->inputFrameCounter] == 0) &&
                   !(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
            Player_SetupRoll(this, play);
            return;
        }

        sp3C = func_80843E64(play, this);

        if (sp3C > 0) {
            Player_Setup4_IdleAll(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType), play);
            this->skelAnime.endFrame = 8.0f;
            if (sp3C == 1) {
                this->midairVar16 = 10;
            } else {
                this->midairVar16 = 20;
            }
        } else if (sp3C == 0) {
            Player_Setup4_IdleAll(this, anim, play);
        }
    }
}

static AnimSfxEntry D_8085460C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 1, NA_SE_VO_LI_SWORD_N, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_BY_AGE, 6, NA_SE_PL_WALK_GROUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 6, NA_SE_PL_ROLL, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 18, NA_SE_NONE, STOP),
};

void Player_Action_Roll(Player* this, PlayState* play) {
    Actor* cylinderOc;
    s32 actionInterruptState;
    s32 isAnimDone;
    DynaPolyActor* wallPolyActor;
    s32 pad;
    f32 inputVelocity;
    s16 inputYaw;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    cylinderOc = NULL;
    isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);

    if (PlayerAnimation_OnFrame(&this->skelAnime, 8.0f)) {
        Player_SetInvincibilityTimerWithoutDamageFlash(this, -10);
    }

    if (Player_TryInteracting(this, play)) {
        return;
    }

    if (this->rollVar16) {
        Math_StepToF(&this->speedXZ, 0.0f, 2.0f);

        actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 5.0f);
        if ((actionInterruptState != PLAYER_ACTION_INTERRUPT_SWAP)) {
            if ((actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE) || isAnimDone) {
                Player_Setup3_IdleAll(this, play);
            }
        }
        return;
    }

    if (this->speedXZ >= 7.0f) {
        if (((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sWorldYawToTouchedWall < 0x2000)) ||
            ((this->cylinder.base.ocFlags1 & OC1_HIT) &&
             (cylinderOc = this->cylinder.base.oc,
              ((cylinderOc->id == ACTOR_EN_WOOD02) &&
               (ABS((s16)(this->actor.world.rot.y - cylinderOc->yawTowardsPlayer)) > DEG_TO_BINANG(135.0f)))))) {

            if (cylinderOc != NULL) {
                cylinderOc->home.rot.y = 1;
            } else if (this->actor.wallBgId != BGCHECK_SCENE) {
                wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);
                if ((wallPolyActor != NULL) && (wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2)) {
                    wallPolyActor->actor.home.rot.z = 1;
                }
            }

            Player_Anim_PlayOnce(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_hip_down, this->modelAnimType));
            this->speedXZ = -this->speedXZ;
            Player_RequestQuake(play, 33267, 3, 12);
            Player_RequestRumble(this, 255, 20, 150, SQ(0));
            Player_PlaySfx(this, NA_SE_PL_BODY_HIT);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
            this->rollVar16 = true;
            return;
        }
    }

    if ((this->skelAnime.curFrame < 15.0f) || !Player_SwapAction_TryAttackBottleFishingRod(this, play)) {
        if (this->skelAnime.curFrame >= 20.0f) {
            Player_Setup3_IdleAll(this, play);
            return;
        }

        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play);

        inputVelocity *= 1.5f;
        if ((inputVelocity < 3.0f) || (this->analogStickDirection4Parts[this->inputFrameCounter] != 0)) {
            inputVelocity = 3.0f;
        }

        func_8083DF68(this, inputVelocity, this->actor.shape.rot.y);

        if (func_8084269C(play, this)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_ROLL_DUST - SFX_FLAG);
        }

        Player_AnimSfx_Play(this, D_8085460C);
    }
}

void Player_Action_FallDive(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_run_jump_water_fall_wait);
    }

    Math_StepToF(&this->speedXZ, 0.0f, 0.05f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.colChkInfo.damage = 0x10;
        Player_ApplyDamage(play, this, PLAYER_DMGREACTION_KNOCKBACK, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
    }
}

void Player_Action_JumpSlash(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    this->actor.gravity = -1.2f;
    PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_ProcessAttackCollision(play, this)) {
        return;
    }

    func_8084285C(this, 6.0f, 7.0f, 99.0f);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
        func_8083DFE0(this, &inputVelocity, &this->yaw);
        return;
    }

    if (func_80843E64(play, this) >= 0) {
        this->meleeWeaponAnimation += 2;
        Player_SetupAttack(play, this, this->meleeWeaponAnimation);
        this->slashCounter = 3;
        Player_AnimSfx_PlayFloorLand(this);
    }
}

s32 Player_TryReleasingSpinAttack(Player* this, PlayState* play) {
    s32 temp;

    if (Player_TryStartingCutscene(play, this)) {
        this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
    } else {
        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
            if ((this->spinAttackTimer >= 0.85f) || func_808375D8(this)) {
                temp = D_80854384[Player_IsHoldingTwoHandedWeapon(this)];
            } else {
                temp = D_80854380[Player_IsHoldingTwoHandedWeapon(this)];
            }

            Player_SetupAttack(play, this, temp);
            Player_SetInvincibilityTimerWithoutDamageFlash(this, -8);

            this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
            if (this->analogStickDirection4Parts[this->inputFrameCounter] == 0) {
                this->stateFlags2 |= PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
            }
        } else {
            return false;
        }
    }

    return true;
}

void Player_SetupChargeSpinAttackWalk(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_ChargeSpinAttackWalk, 1);
}

void Player_SetupChargeSpinAttackSidewalk(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_ChargeSpinAttackSidewalk, 1);
}

void Player_CancelSpinAttackCharge(Player* this, PlayState* play) {
    Player_Setup2_IdleAll(this, play);
    Player_ResetAttack(this);
    Player_Anim_PlayOnceWithMorph(play, this, D_80854368[Player_IsHoldingTwoHandedWeapon(this)]);
    this->yaw = this->actor.shape.rot.y;
}

void Player_Setup2_ChargeSpinAttack(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_ChargeSpinAttack, 1);
    this->walkCurFrame = 0.0f;
    Player_Anim_PlayLoop(play, this, sSpinAttackChargeAnims[Player_IsHoldingTwoHandedWeapon(this)]);
    this->chargeSpinAttackVar16 = 1;
}

void Player_UpdateSpinAttackTimer(Player* this) {
    Math_StepToF(&this->spinAttackTimer, 1.0f, 0.02f);
}

void Player_Action_ChargeSpinAttack(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_StartZParallel(this);
        this->stateFlags1 &= ~PLAYER_STATE1_Z_PARALLEL;
        Player_Anim_PlayLoop(play, this, sSpinAttackChargeAnims[Player_IsHoldingTwoHandedWeapon(this)]);
        this->chargeSpinAttackVar16 = -1;
    }

    Player_StepHorizontalSpeedToZero(this);

    if (Player_TryInteracting(this, play)) {
        return;
    }

    if (this->chargeSpinAttackVar16 == 0) {
        return;
    }

    Player_UpdateSpinAttackTimer(this);

    if (this->chargeSpinAttackVar16 < 0) {
        if (this->spinAttackTimer >= 0.1f) {
            this->slashCounter = 0;
            this->chargeSpinAttackVar16 = 1;
        } else if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
            Player_CancelSpinAttackCharge(this, play);
        }
        return;
    }

    if (Player_TryReleasingSpinAttack(this, play)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    moveDir = Player_GetSpinAttackMoveDirection(this, &inputVelocity, &inputYaw, play);
    if (moveDir > 0) {
        Player_SetupChargeSpinAttackWalk(this, play);
    } else if (moveDir < 0) {
        Player_SetupChargeSpinAttackSidewalk(this, play);
    }
}

void Player_Action_ChargeSpinAttackWalk(Player* this, PlayState* play) {
    s16 curYawDiff;
    s32 absCurYawDiff;
    f32 absLinearVelocity;
    f32 blendWeight;
    f32 inputVelocity;
    s16 inputYaw;
    s32 moveDir;
    s16 targetYawDiff;
    s32 absTargetYawDiff;

    curYawDiff = this->yaw - this->actor.shape.rot.y;
    absCurYawDiff = ABS(curYawDiff);

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    absLinearVelocity = fabsf(this->speedXZ);
    blendWeight = absLinearVelocity * 1.5f;
    blendWeight = CLAMP_MIN(blendWeight, 1.5f);
    blendWeight = ((absCurYawDiff < 0x4000) ? -1.0f : 1.0f) * blendWeight;

    Player_UpdateWalkFrame(this, blendWeight);

    blendWeight = CLAMP(absLinearVelocity * 0.5f, 0.5f, 1.0f);

    PlayerAnimation_BlendToJoint(play, &this->skelAnime, sSpinAttackChargeAnims[Player_IsHoldingTwoHandedWeapon(this)],
                                 0.0f, sSpinAttackChargeWalkAnims[Player_IsHoldingTwoHandedWeapon(this)],
                                 this->walkCurFrame * (21.0f / 29.0f), blendWeight, this->blendTable);

    if (Player_TryInteracting(this, play)) {
        return;
    }

    if (Player_TryReleasingSpinAttack(this, play)) {
        return;
    }

    Player_UpdateSpinAttackTimer(this);
    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    moveDir = Player_GetSpinAttackMoveDirection(this, &inputVelocity, &inputYaw, play);

    if (moveDir < 0) {
        Player_SetupChargeSpinAttackSidewalk(this, play);
        return;
    }

    if (moveDir == 0) {
        inputVelocity = 0.0f;
        inputYaw = this->yaw;
    }

    targetYawDiff = inputYaw - this->yaw;
    absTargetYawDiff = ABS(targetYawDiff);

    if (absTargetYawDiff > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = inputYaw;
        }
        return;
    }

    Math_AsymStepToF(&this->speedXZ, inputVelocity * 0.2f, 1.0f, 0.5f);
    Math_ScaledStepToS(&this->yaw, inputYaw, absTargetYawDiff * 0.1f);

    if ((inputVelocity == 0.0f) && (this->speedXZ == 0.0f)) {
        Player_Setup2_ChargeSpinAttack(this, play);
    }
}

void Player_Action_ChargeSpinAttackSidewalk(Player* this, PlayState* play) {
    f32 sp5C = fabsf(this->speedXZ);
    f32 sp58;
    f32 inputVelocity;
    s16 inputYaw;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp5C == 0.0f) {
        sp5C = ABS(this->yawDiffPrevFrame) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }
        Player_UpdateWalkFrame(this, ((this->yawDiffPrevFrame >= 0) ? 1 : -1) * sp5C);
    } else {
        sp58 = sp5C * 1.5f;
        if (sp58 < 1.5f) {
            sp58 = 1.5f;
        }
        Player_UpdateWalkFrame(this, sp58);
    }

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    PlayerAnimation_BlendToJoint(play, &this->skelAnime, sSpinAttackChargeAnims[Player_IsHoldingTwoHandedWeapon(this)],
                                 0.0f, D_80854378[Player_IsHoldingTwoHandedWeapon(this)],
                                 this->walkCurFrame * (21.0f / 29.0f), sp58, this->blendTable);

    if (Player_TryInteracting(this, play) || Player_TryReleasingSpinAttack(this, play)) {
        return;
    }

    Player_UpdateSpinAttackTimer(this);
    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    temp4 = Player_GetSpinAttackMoveDirection(this, &inputVelocity, &inputYaw, play);

    if (temp4 > 0) {
        Player_SetupChargeSpinAttackWalk(this, play);
        return;
    }

    if (temp4 == 0) {
        inputVelocity = 0.0f;
        inputYaw = this->yaw;
    }

    temp5 = inputYaw - this->yaw;
    sp44 = ABS(temp5);

    if (sp44 > 0x4000) {
        if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = inputYaw;
        }
        return;
    }

    Math_AsymStepToF(&this->speedXZ, inputVelocity * 0.2f, 1.0f, 0.5f);
    Math_ScaledStepToS(&this->yaw, inputYaw, sp44 * 0.1f);

    if ((inputVelocity == 0.0f) && (this->speedXZ == 0.0f) && (sp5C == 0.0f)) {
        Player_Setup2_ChargeSpinAttack(this, play);
    }
}

void Player_Action_JumpToLedge(Player* this, PlayState* play) {
    s32 isAnimDone;
    f32 jumpVelocityY;
    s32 actionInterruptState;
    f32 temp3;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);

    if (this->skelAnime.animation == &gPlayerAnim_link_normal_250jump_start) {
        this->speedXZ = 1.0f;

        if (PlayerAnimation_OnFrame(&this->skelAnime, 8.0f)) {
            jumpVelocityY = this->yDistToLedge;

            if (jumpVelocityY > this->ageProperties->unk_0C) {
                jumpVelocityY = this->ageProperties->unk_0C;
            }

            if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                jumpVelocityY *= 0.085f;
            } else {
                jumpVelocityY *= 0.072f;
            }

            if (!LINK_IS_ADULT) {
                jumpVelocityY += 1.0f;
            }

            Player_Setup1_Jump(this, NULL, jumpVelocityY, play, NA_SE_VO_LI_AUTO_JUMP);
            this->midairVar16 = -1;
            return;
        }
    } else {
        actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 4.0f);

        if (actionInterruptState == PLAYER_ACTION_INTERRUPT_SWAP) {
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        if (isAnimDone || (actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE)) {
            Player_SetupIdle(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_JUMPING);
            return;
        }

        temp3 = 0.0f;

        if (this->skelAnime.animation == &gPlayerAnim_link_swimer_swim_15step_up) {
            if (PlayerAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                Player_JumpOutOfWater(play, this, 10.0f);
            }
            temp3 = 50.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_150step_up) {
            temp3 = 30.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) {
            temp3 = 16.0f;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, temp3)) {
            Player_AnimSfx_PlayFloorLand(this);
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) || (this->skelAnime.curFrame > 5.0f)) {
            if (this->jumpToLedgeVar16 == 0) {
                Player_AnimSfx_PlayFloorJump(this);
                this->jumpToLedgeVar16 = 1;
            }
            Math_StepToF(&this->actor.shape.yOffset, 0.0f, 150.0f);
        }
    }
}

void Player_Action_CsIntoAction(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (
        // Option 1:
        ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->heldActor != NULL) &&
         (this->getItemId == GI_NONE)) ||
        // Option 2:
        !Player_TryUpperAction(this, play)) {
        this->csIntoActionFunc(play, this);
    }
}

s32 Player_CutsceneMove(PlayState* play, Player* this, CsCmdActorCue* cue, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->speedXZ == 0.0f)) {
        return PlayerAnimation_Update(play, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 sp34 = R_UPDATE_RATE * 0.5f;
        f32 selfDistX = cue->endPos.x - this->actor.world.pos.x;
        f32 selfDistZ = cue->endPos.z - this->actor.world.pos.z;
        f32 sp28 = sqrtf(SQ(selfDistX) + SQ(selfDistZ)) / sp34;
        s32 sp24 = (cue->endFrame - play->csCtx.curFrame) + 1;

        arg4 = Math_Atan2S(selfDistZ, selfDistX);

        if (arg5 == 1) {
            f32 distX = cue->endPos.x - cue->startPos.x;
            f32 distZ = cue->endPos.z - cue->startPos.z;
            s32 temp = (((sqrtf(SQ(distX) + SQ(distZ)) / sp34) / (cue->endFrame - cue->startFrame)) / 1.5f) * 4.0f;

            if (temp >= sp24) {
                arg4 = this->actor.shape.rot.y;
                arg3 = 0.0f;
            } else {
                arg3 = sp28 / ((sp24 - temp) + 1);
            }
        } else {
            arg3 = sp28 / sp24;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    Player_UpdateRunAnim(this, play);
    func_8083DF68(this, arg3, arg4);

    if ((arg3 == 0.0f) && (this->speedXZ == 0.0f)) {
        func_8083BF50(this, play);
    }

    return 0;
}

// Returns remaining distance until within range
s32 Player_CutsceneMoveToPos(PlayState* play, Player* this, f32* targetVelocity, s32 endRange) {
    f32 dx = this->miniCsPosTarget.x - this->actor.world.pos.x;
    f32 dz = this->miniCsPosTarget.z - this->actor.world.pos.z;
    s32 xzDist = sqrtf(SQ(dx) + SQ(dz));
    s16 yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->miniCsPosTarget);

    if (xzDist < endRange) {
        *targetVelocity = 0.0f;
        yaw = this->actor.shape.rot.y;
    }

    if (Player_CutsceneMove(play, this, NULL, *targetVelocity, yaw, 2)) {
        return 0;
    }

    return xzDist;
}

s32 func_80845C68(PlayState* play, s32 arg1) {
    if (arg1 == 0) {
        Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
    }
    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

void Player_Action_MiniCutscene(Player* this, PlayState* play) {
    f32 sp3C;
    s32 temp;
    f32 sp34;
    s32 sp30;
    s32 pad;

    if (!Player_SwapAction_TryItemCsFirstPerson(this, play)) {
        if (this->miniCutsceneVar16 == 0) {
            PlayerAnimation_Update(play, &this->skelAnime);

            if (DECR(this->doorTimer) == 0) {
                this->speedXZ = 0.1f;
                this->miniCutsceneVar16 = 1;
            }
        } else if (!this->miniCsHasMovedToStart) {
            // Never run?
            sp3C = 5.0f * sPlayerUnderwaterSpeedAdjustment;

            if (Player_CutsceneMoveToPos(play, this, &sp3C, -1) < 30) {
                this->miniCsHasMovedToStart = true;
                this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;

                this->miniCsPosTarget.x = this->miniCsEndPos.x;
                this->miniCsPosTarget.z = this->miniCsEndPos.z;
            }
        } else {
            sp34 = 5.0f;
            sp30 = 20;

            if (this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) {
                sp34 = gSaveContext.entranceSpeed;

                if (sConveyorSpeed != CONVEYOR_SPEED_DISABLED) {
                    this->miniCsPosTarget.x = (Math_SinS(sConveyorYaw) * 400.0f) + this->actor.world.pos.x;
                    this->miniCsPosTarget.z = (Math_CosS(sConveyorYaw) * 400.0f) + this->actor.world.pos.z;
                }
            } else if (this->miniCutsceneVar16 < 0) {
                this->miniCutsceneVar16++;

                sp34 = gSaveContext.entranceSpeed;
                sp30 = -1;
            }

            temp = Player_CutsceneMoveToPos(play, this, &sp34, sp30);

            if ((this->miniCutsceneVar16 == 0) ||
                ((temp == 0) && (this->speedXZ == 0.0f) &&
                 (Play_GetCamera(play, CAM_ID_MAIN)->stateFlags & CAM_STATE_CAM_FUNC_FINISH))) {

                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
                func_80845C68(play, gSaveContext.respawn[RESPAWN_MODE_DOWN].data);

                if (!Player_SwapAction_TryTalking(this, play)) {
                    Player_EndMiniCutscene(this, play);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        Player_TryUpperAction(this, play);
    }
}

void Player_Action_OpenDoor(Player* this, PlayState* play) {
    s32 isAnimDone;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    isAnimDone = PlayerAnimation_Update(play, &this->skelAnime);

    Player_TryUpperAction(this, play);

    if (isAnimDone) {
        if (this->actionVar16 == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->actionVar16 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            Player_SetupIdle(this, play);
            if (play->roomCtx.prevRoom.num >= 0) {
                func_80097534(play, &play->roomCtx);
            }
            Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
            Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
        }
        return;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) && PlayerAnimation_OnFrame(&this->skelAnime, 15.0f)) {
        play->playerSetupIdle(this, play);
    }
}

void Player_Action_Lift(Player* this, PlayState* play) {
    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Setup1_IdleAll(this, play);
        Player_SetupCarryAboveHead(this, play);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, interactRangeActor)) {
            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            interactRangeActor->bgCheckFlags &=
                ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_WALL |
                  BGCHECKFLAG_CEILING | BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH | BGCHECKFLAG_GROUND_STRICT);
            this->unk_3BC.y = interactRangeActor->shape.rot.y - this->actor.shape.rot.y;
        }
        return;
    }

    Math_ScaledStepToS(&this->unk_3BC.y, 0, 4000);
}

static AnimSfxEntry D_8085461C[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 49, NA_SE_VO_LI_SWORD_L, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 230, NA_SE_VO_LI_SWORD_N, STOP),
};

void Player_Action_LiftThrowHeavyBlock(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->actionVar16++ > 20)) {
        if (!Player_SwapAction_TryItemCsFirstPerson(this, play)) {
            Player_Setup4_IdleAll(this, &gPlayerAnim_link_normal_heavy_carry_end, play);
        }
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 41.0f)) {
        BgHeavyBlock* heavyBlock = (BgHeavyBlock*)this->interactRangeActor;

        this->heldActor = &heavyBlock->dyna.actor;
        this->actor.child = &heavyBlock->dyna.actor;
        heavyBlock->dyna.actor.parent = &this->actor;
        func_8002DBD0(&heavyBlock->dyna.actor, &heavyBlock->unk_164, &this->leftHandPos);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 229.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->speed = Math_SinS(heldActor->shape.rot.x) * 40.0f;
        heldActor->velocity.y = Math_CosS(heldActor->shape.rot.x) * 40.0f;
        heldActor->gravity = -2.0f;
        heldActor->terminalVelocity = -30.0f;
        Player_DetatchHeldActor(play, this);
        return;
    }

    Player_AnimSfx_Play(this, D_8085461C);
}

void Player_Action_LiftSilverRock(Player* this, PlayState* play) {
    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_silver_wait);
        this->actionVar16 = 1;
        return;
    }

    if (this->actionVar16 == 0) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            return;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, 25.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_L);
            return;
        }

    } else if (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)) {
        Player_SetAction(play, this, Player_Action_ThrowSilverRock, 1);
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_silver_throw);
    }
}

void Player_Action_ThrowSilverRock(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Setup1_IdleAll(this, play);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = 10.0f;
        heldActor->velocity.y = 20.0f;
        Player_SetupUpperActionForHeldItem(play, this);
        Player_PlaySfx(this, NA_SE_PL_THROW);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }
}

void Player_Action_LiftFail(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_nocarry_free_wait);
        this->actionVar16 = 15;
        return;
    }

    if (this->actionVar16 != 0) {
        this->actionVar16--;
        if (this->actionVar16 == 0) {
            Player_Setup4_IdleAll(this, &gPlayerAnim_link_normal_nocarry_free_end, play);
            this->stateFlags1 &= ~PLAYER_STATE1_HOLDING_ACTOR;
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

void Player_Action_PutDown(Player* this, PlayState* play) {
    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Setup1_IdleAll(this, play);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!Player_TryIdlingAllAndReleaseHeldActor(play, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speed = 0.0f;
            Player_SetupUpperActionForHeldItem(play, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU) {
                func_8083B8F4(this, play);
            }
        }
    }
}

void Player_Action_Throw(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) &&
         Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.018f, play))) {
        Player_Setup1_IdleAll(this, play);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        func_8084409C(play, this, this->speedXZ + 8.0f, 12.0f);
    }
}

static ColliderCylinderInit D_80854624 = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static ColliderQuadInit D_80854650 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000100, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit D_808546A0 = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xDFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void func_8084663C(Actor* thisx, PlayState* play) {
}

void Player_InitMode_0(PlayState* play, Player* this) {
    this->actor.update = func_8084663C;
    this->actor.draw = NULL;
}

void Player_InitMode_BlueWarp(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SpawnFromBlueWarp, 0);
    if ((play->sceneId == SCENE_LAKE_HYLIA) && IS_CUTSCENE_LAYER) {
        this->spawnFromBlueWarpVar8 = 1;
    }
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, PLAYER_ANIM_ADJUSTED_SPEED,
                           0.0f, 24.0f, ANIMMODE_ONCE, 0.0f);
    this->actor.world.pos.y += 800.0f;
}

static u8 D_808546F0[] = { ITEM_SWORD_MASTER, ITEM_SWORD_KOKIRI };

void Player_PullMasterSwordFromPedistal(PlayState* play, Player* this, s32 arg2) {
    s32 itemId = D_808546F0[(void)0, gSaveContext.linkAge];
    s32 itemAction = sItemActions[itemId];

    Player_PutAwayHookshot(this);
    Player_DetatchHeldActor(play, this);

    this->heldItemId = itemId;
    this->nextModelGroup = Player_ModelGroupFromIA(this, itemAction);

    Player_ChangeItem(play, this, itemAction);
    Player_SetupUpperActionForHeldItem(play, this);

    if (arg2 != 0) {
        Player_PlaySfx(this, NA_SE_IT_SWORD_PICKOUT);
    }
}

static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

void Player_InitMode_AgeSwap(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SpawnFromAgeSwap, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    Math_Vec3f_Copy(&this->actor.world.pos, &D_808546F4);
    this->yaw = this->actor.shape.rot.y = -0x8000;
    PlayerAnimation_Change(play, &this->skelAnime, this->ageProperties->unk_A0, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f,
                           ANIMMODE_ONCE, 0.0f);
    Player_AnimReplace_Setup(play, this, 0x28F);
    if (LINK_IS_ADULT) {
        Player_PullMasterSwordFromPedistal(play, this, 0);
    }
    this->spawnFromAgeSwapVar16 = 20;
}

void Player_InitMode_Door(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SpawnFromDoor, 0);
    Player_AnimReplace_Setup(play, this, 0x9B);
}

void Player_InitMode_Grotto(PlayState* play, Player* this) {
    Player_Setup2_Jump(this, &gPlayerAnim_link_normal_jump, 12.0f, play);
    Player_SetAction(play, this, Player_Action_SpawnFromGrotto, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    this->fallStartHeight = this->actor.world.pos.y;
    OnePointCutscene_Init(play, 5110, 40, &this->actor, CAM_ID_MAIN);
}

void Player_InitMode_Knockback(PlayState* play, Player* this) {
    Player_ApplyDamage(play, this, PLAYER_DMGREACTION_KNOCKBACK, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void Player_InitMode_WarpSong(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SpawnFromWarpSong, 0);
    this->actor.draw = NULL;
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

static s16 sMagicSpellActors[] = { ACTOR_MAGIC_WIND, ACTOR_MAGIC_DARK, ACTOR_MAGIC_FIRE };

Actor* Player_SpawnMagicSpellActor(PlayState* play, Player* this, s32 magicSpellIndex) {
    return Actor_Spawn(&play->actorCtx, play, sMagicSpellActors[magicSpellIndex], this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void Player_InitMode_FaroresWind(PlayState* play, Player* this) {
    this->actor.draw = NULL;
    Player_SetAction(play, this, Player_Action_SpawnFromFaroresWind, 0);
    this->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 500, ICHAIN_STOP),
};

static EffectBlureInit2 D_8085470C = {
    0, 8, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, 4,
    0, 2, 0, { 0, 0, 0, 0 },         { 0, 0, 0, 0 },
};

static Vec3s D_80854730 = { -57, 3377, 0 };

void Player_InitCommon(Player* this, PlayState* play, FlexSkeletonHeader* skelHeader) {
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->meleeWeaponEffectIndex = TOTAL_EFFECT_COUNT;
    this->yaw = this->actor.world.rot.y;
    Player_SetupUpperActionForHeldItem(play, this);

    SkelAnime_InitLink(play, &this->skelAnime, skelHeader, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType),
                       9, this->jointTable, this->morphTable, PLAYER_LIMB_MAX);
    this->skelAnime.baseTransl = D_80854730;
    SkelAnime_InitLink(play, &this->upperSkelAnime, skelHeader, Player_GetIdleAnim(this), 9, this->upperJointTable,
                       this->upperMorphTable, PLAYER_LIMB_MAX);
    this->upperSkelAnime.baseTransl = D_80854730;

    Effect_Add(play, &this->meleeWeaponEffectIndex, EFFECT_BLURE2, 0, 0, &D_8085470C);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, this->ageProperties->unk_04);
    this->subCamId = CAM_ID_NONE;

    Collider_InitCylinder(play, &this->cylinder);
    Collider_SetCylinder(play, &this->cylinder, &this->actor, &D_80854624);
    Collider_InitQuad(play, &this->meleeWeaponQuads[0]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[0], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->meleeWeaponQuads[1]);
    Collider_SetQuad(play, &this->meleeWeaponQuads[1], &this->actor, &D_80854650);
    Collider_InitQuad(play, &this->shieldQuad);
    Collider_SetQuad(play, &this->shieldQuad, &this->actor, &D_808546A0);
}

static void (*D_80854738[])(PlayState* play, Player* this) = {
    /* 0x0 */ Player_InitMode_0,
    /* 0x1 */ Player_InitMode_AgeSwap, // From time travel
    /* 0x2 */ Player_InitMode_BlueWarp,
    /* 0x3 */ Player_InitMode_Door,
    /* 0x4 */ Player_InitMode_Grotto,
    /* 0x5 */ Player_InitMode_WarpSong,
    /* 0x6 */ Player_InitMode_FaroresWind,
    /* 0x7 */ Player_InitMode_Knockback,
    /* 0x8 */ Player_InitMode_Other,
    /* 0x9 */ Player_InitMode_Other,
    /* 0xA */ Player_InitMode_Other,
    /* 0xB */ Player_InitMode_Other,
    /* 0xC */ Player_InitMode_Other,
    /* 0xD */ Player_InitMode_D,
    /* 0xE */ Player_InitMode_Other,
    /* 0xF */ Player_InitMode_F,
};

static Vec3f D_80854778 = { 0.0f, 50.0f, 0.0f };

void Player_Init(Actor* thisx, PlayState* play2) {
    Player* this = (Player*)thisx;
    PlayState* play = play2;
    SceneTableEntry* scene = play->loadedScene;
    u32 titleFileSize;
    s32 initMode;
    s32 respawnFlag;
    s32 respawnMode;

    play->shootingGalleryStatus = play->bombchuBowlingStatus = 0;

    play->playerInit = Player_InitCommon;
    play->playerUpdate = Player_UpdateCommon;
    play->isPlayerDroppingFish = Player_IsDroppingFish;
    play->startPlayerFishing = Player_StartFishing;
    play->tryGrabbingPlayer = Player_TryGrabbingPlayer;
    play->startPlayerCutscene = Player_SetupCutsceneWithCsMode;
    play->playerSetupIdle = Player_SetupIdleWithMorph;
    play->damagePlayer = Player_InflictDamage;
    play->talkWithPlayer = Player_SetupTalk;

    thisx->room = -1;
    this->ageProperties = &sAgeProperties[gSaveContext.linkAge];
    this->itemAction = this->heldItemAction = -1;
    this->heldItemId = ITEM_NONE;

    Player_UseItem(play, this, ITEM_NONE);
    Player_SetEquipmentData(play, this);
    this->prevBoots = this->currentBoots;
    Player_InitCommon(this, play, gPlayerSkelHeaders[((void)0, gSaveContext.linkAge)]);
    this->giObjectSegment = (void*)(((uintptr_t)ZeldaArena_MallocDebug(0x3008, "../z_player.c", 17175) + 8) & ~0xF);

    respawnFlag = gSaveContext.respawnFlag;

    if (respawnFlag != 0) {
        if (respawnFlag == -3) {
            thisx->params = gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams;
        } else {
            if ((respawnFlag == 1) || (respawnFlag == -1)) {
                this->voidRespawnCounter = -2;
            }

            if (respawnFlag < 0) {
                respawnMode = RESPAWN_MODE_DOWN;
            } else {
                respawnMode = respawnFlag - 1;
                Math_Vec3f_Copy(&thisx->world.pos, &gSaveContext.respawn[respawnMode].pos);
                Math_Vec3f_Copy(&thisx->home.pos, &thisx->world.pos);
                Math_Vec3f_Copy(&thisx->prevPos, &thisx->world.pos);
                this->fallStartHeight = thisx->world.pos.y;
                this->yaw = thisx->shape.rot.y = gSaveContext.respawn[respawnMode].yaw;
                thisx->params = gSaveContext.respawn[respawnMode].playerParams;
            }

            play->actorCtx.flags.tempSwch = gSaveContext.respawn[respawnMode].tempSwchFlags & 0xFFFFFF;
            play->actorCtx.flags.tempCollect = gSaveContext.respawn[respawnMode].tempCollectFlags;
        }
    }

    if ((respawnFlag == 0) || (respawnFlag < -1)) {
        titleFileSize = scene->titleFile.vromEnd - scene->titleFile.vromStart;
        if ((titleFileSize != 0) && gSaveContext.showTitleCard) {
            if (!IS_CUTSCENE_LAYER &&
                (gEntranceTable[((void)0, gSaveContext.entranceIndex) + ((void)0, gSaveContext.sceneLayer)].field &
                 ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG) &&
                ((play->sceneId != SCENE_DODONGOS_CAVERN) || GET_EVENTCHKINF(EVENTCHKINF_B0)) &&
                ((play->sceneId != SCENE_BOMBCHU_SHOP) || GET_EVENTCHKINF(EVENTCHKINF_25))) {
                TitleCard_InitPlaceName(play, &play->actorCtx.titleCtx, this->giObjectSegment, 160, 120, 144, 24, 20);
            }
        }
        gSaveContext.showTitleCard = true;
    }

    if (func_80845C68(play, (respawnFlag == 2) ? 1 : 0) == 0) {
        gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = (thisx->params & 0xFF) | 0xD00;
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;

    if (play->sceneId <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE) {
        gSaveContext.infTable[INFTABLE_1AX_INDEX] |= gBitFlags[play->sceneId];
    }

    initMode = (thisx->params & 0xF00) >> 8;
    if ((initMode == 5) || (initMode == 6)) {
        if (gSaveContext.cutsceneIndex >= 0xFFF0) {
            initMode = 13;
        }
    }

    D_80854738[initMode](play, this);

    if (initMode != 0) {
        if ((gSaveContext.gameMode == GAMEMODE_NORMAL) || (gSaveContext.gameMode == GAMEMODE_END_CREDITS)) {
            this->naviActor = Player_SpawnFairy(play, this, &thisx->world.pos, &D_80854778, FAIRY_NAVI);
            if (gSaveContext.dogParams != 0) {
                gSaveContext.dogParams |= 0x8000;
            }
        }
    }

    if (gSaveContext.nayrusLoveTimer != 0) {
        gSaveContext.magicState = MAGIC_STATE_METER_FLASH_1;
        Player_SpawnMagicSpellActor(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (gSaveContext.entranceSound != 0) {
        Actor_PlaySfx(&this->actor, ((void)0, gSaveContext.entranceSound));
        gSaveContext.entranceSound = 0;
    }

    Map_SavePlayerInitialInfo(play);
    MREG(64) = 0;
}

void Player_StepToZero(s16* pValue) {
    s16 step;

    step = (ABS(*pValue) * 100.0f) / 1000.0f;
    step = CLAMP(step, 400, 4000);

    Math_ScaledStepToS(pValue, 0, step);
}

void Player_StepRotToZero(Player* this) {
    s16 focusYawDiff;

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y)) {
        focusYawDiff = this->actor.focus.rot.y - this->actor.shape.rot.y;
        Player_StepToZero(&focusYawDiff);
        this->actor.focus.rot.y = this->actor.shape.rot.y + focusYawDiff;
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_FOCUS_ROT_X)) {
        Player_StepToZero(&this->actor.focus.rot.x);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_HEAD_ROT_X)) {
        Player_StepToZero(&this->headLimbRot.x);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_UPPER_ROT_X)) {
        Player_StepToZero(&this->upperLimbRot.x);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_FOCUS_ROT_Z)) {
        Player_StepToZero(&this->actor.focus.rot.z);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_HEAD_ROT_Y)) {
        Player_StepToZero(&this->headLimbRot.y);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_HEAD_ROT_Z)) {
        Player_StepToZero(&this->headLimbRot.z);
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_UPPER_ROT_Y)) {
        if (this->unk_6B0 != 0) {
            Player_StepToZero(&this->unk_6B0);
        } else {
            Player_StepToZero(&this->upperLimbRot.y);
        }
    }

    if (!(this->rotOverrideFlags & PLAYER_ROT_OVERRIDE_UPPER_ROT_Z)) {
        Player_StepToZero(&this->upperLimbRot.z);
    }

    this->rotOverrideFlags = 0;
}

static f32 D_80854784[] = { 120.0f, 240.0f, 360.0f };

static u8 sDiveDoActions[] = { DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4,
                               DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8 };

void Player_UpdateDoAction(PlayState* play, Player* this) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) && (this->actor.category == ACTORCAT_PLAYER)) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 sp24;
        s32 sp20 = this->analogStickDirection4Parts[this->inputFrameCounter];
        s32 sp1C = Player_IsFreeSwimming(this);
        s32 doAction = DO_ACTION_NONE;

        if (!Player_InBlockingCsMode(play, this)) {
            if (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE) {
                doAction = DO_ACTION_RETURN;
            } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->stickFlameTimer != 0)) {
                if (this->stickFlameTimer == 2) {
                    doAction = DO_ACTION_REEL;
                }
            } else if ((Player_Action_PlayOcarina != this->actionFunc) &&
                       !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) ||
                     ((heldActor != NULL) && (heldActor->id == ACTOR_EN_RU1)))) {
                    doAction = DO_ACTION_OPEN;
                } else if ((!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) || (heldActor == NULL)) &&
                           (interactRangeActor != NULL) &&
                           ((!sp1C && (this->getItemId == GI_NONE)) ||
                            ((this->getItemId < 0) && !(this->stateFlags1 & PLAYER_STATE1_SWIMMING)))) {
                    if (this->getItemId < 0) {
                        doAction = DO_ACTION_OPEN;
                    } else if ((interactRangeActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                        doAction = DO_ACTION_DROP;
                    } else {
                        doAction = DO_ACTION_GRAB;
                    }
                } else if (!sp1C && (this->stateFlags2 & PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL)) {
                    doAction = DO_ACTION_GRAB;
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL) ||
                           (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->rideActor != NULL))) {
                    doAction = DO_ACTION_CLIMB;
                } else if ((this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
                           !EN_HORSE_CHECK_4((EnHorse*)this->rideActor) &&
                           (Player_Action_DismountHorse != this->actionFunc)) {
                    if ((this->stateFlags2 & PLAYER_STATE2_CAN_SPEAK_OR_CHECK) && (this->talkActor != NULL)) {
                        if (this->talkActor->category == ACTORCAT_NPC) {
                            doAction = DO_ACTION_SPEAK;
                        } else {
                            doAction = DO_ACTION_CHECK;
                        }
                    } else if (!Player_IsAimingFpsItem(this) &&
                               !(this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
                        doAction = DO_ACTION_FASTER;
                    }
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_SPEAK_OR_CHECK) && (this->talkActor != NULL)) {
                    if (this->talkActor->category == ACTORCAT_NPC) {
                        doAction = DO_ACTION_SPEAK;
                    } else {
                        doAction = DO_ACTION_CHECK;
                    }
                } else if ((this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING)) ||
                           ((this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
                            (this->stateFlags2 & PLAYER_STATE2_CAN_DISMOUNT_HORSE))) {
                    doAction = DO_ACTION_DOWN;
                } else if (this->stateFlags2 & PLAYER_STATE2_DO_ACTION_ENTER) {
                    doAction = DO_ACTION_ENTER;
                } else if ((this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) && (this->getItemId == GI_NONE) &&
                           (heldActor != NULL)) {
                    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                        if (Player_CanThrowActor(this, heldActor) == 0) {
                            doAction = DO_ACTION_DROP;
                        } else {
                            doAction = DO_ACTION_THROW;
                        }
                    }
                } else if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING) && Player_CanLift(this) &&
                           (this->getItemId < GI_MAX)) {
                    doAction = DO_ACTION_GRAB;
                } else if (this->stateFlags2 & PLAYER_STATE2_ENABLE_DIVE_CAMERA_AND_TIMER) {
                    sp24 = (D_80854784[CUR_UPG_VALUE(UPG_SCALE)] - this->actor.depthInWater) / 40.0f;
                    sp24 = CLAMP(sp24, 0, 7);
                    doAction = sDiveDoActions[sp24];
                } else if (sp1C && !(this->stateFlags2 & PLAYER_STATE2_DIVING)) {
                    doAction = DO_ACTION_DIVE;
                } else if (!sp1C && (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) || Player_IsZTargeting(this) ||
                                     !Player_IsChildWithHylianShield(this))) {
                    if ((!(this->stateFlags1 & PLAYER_STATE1_CLIMBING_ONTO_LEDGE) && (sp20 <= 0) &&
                         (Player_IsEnemyLockOn(this) ||
                          ((sFloorType != FLOOR_TYPE_7) &&
                           (Player_IsZParallelOrLockOnFriend(this) ||
                            ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                             !(this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) && (sp20 == 0))))))) {
                        doAction = DO_ACTION_ATTACK;
                    } else if ((play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_2) &&
                               Player_IsZTargeting(this) && (sp20 > 0)) {
                        doAction = DO_ACTION_JUMP;
                    } else if ((this->heldItemAction >= PLAYER_IA_SWORD_MASTER) ||
                               ((this->stateFlags2 & PLAYER_STATE2_NAVI_IS_ACTIVE) &&
                                (play->actorCtx.targetCtx.nextLockOnActor == NULL))) {
                        doAction = DO_ACTION_PUTAWAY;
                    }
                }
            }
        }

        if (doAction != DO_ACTION_PUTAWAY) {
            this->putAwayTimer = 20;
        } else if (this->putAwayTimer != 0) {
            doAction = DO_ACTION_NONE;
            this->putAwayTimer--;
        }

        Interface_SetDoAction(play, doAction);

        if (this->stateFlags2 & PLAYER_STATE2_NAVI_REQUESTING_TALK) {
            if (this->lockOnActor != NULL) {
                Interface_SetNaviCall(play, DO_ACTION_NAVI_1);
            } else {
                Interface_SetNaviCall(play, DO_ACTION_NAVI_0);
            }
            Interface_SetNaviCall(play, DO_ACTION_NAVI_1);
        } else {
            Interface_SetNaviCall(play, DO_ACTION_NAVI_2);
        }
    }
}

s32 Player_UpdateHoverBoots(Player* this) {
    s32 cond;

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && (this->hoverBootsTimer != 0)) {
        this->hoverBootsTimer--;
    } else {
        this->hoverBootsTimer = 0;
    }

    cond = (this->currentBoots == PLAYER_BOOTS_HOVER) &&
           ((this->actor.depthInWater >= 0.0f) || (Player_GetHurtFloorType(sFloorType) >= 0) ||
            Player_IsFloorSinking(sFloorType));

    if (cond && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->hoverBootsTimer != 0)) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (!cond) {
            this->hoverBootsTimer = 19;
        }
        return false;
    }

    sFloorType = FLOOR_TYPE_0;
    this->floorPitch = this->floorPitchAlt = sFloorPitchShape = 0;

    return true;
}

static Vec3f sInteractWallCheckOffset = { 0.0f, 18.0f, 0.0f };

/**
 * Peforms various tasks related to scene collision.
 *
 * This includes:
 * - Update BgCheckInfo, parameters adjusted due to various state flags
 * - Update floor type, floor property and floor sfx offset
 * - Update conveyor, reverb and light settings according to the current floor poly
 * - Handle exits and voids
 * - Update information relating to the "interact wall"
 * - Update information for ledge climbing
 * - Update hover boots
 * - Calculate floor poly angles
 *
 */
void Player_ProcessSceneCollision(PlayState* play, Player* this) {
    u8 nextLedgeClimbType = PLAYER_LEDGE_CLIMB_NONE;
    CollisionPoly* floorPoly;
    Vec3f spB4;
    f32 spB0;
    f32 spAC;
    f32 spA8;
    u32 flags;

    sPrevFloorProperty = this->floorProperty;

    if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
        spB0 = 10.0f;
        spAC = 15.0f;
        spA8 = 30.0f;
    } else {
        spB0 = this->ageProperties->wallCheckRadius;
        spAC = 26.0f;
        spA8 = this->ageProperties->ceilingCheckHeight;
    }

    if (this->stateFlags1 & (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_FALLING_INTO_GROTTO)) {
        if (this->stateFlags1 & PLAYER_STATE1_FALLING_INTO_GROTTO) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            flags = UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else if ((this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                   ((this->sceneExitPosY - (s32)this->actor.world.pos.y) >= 100)) {
            flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                   ((Player_Action_OpenDoor == this->actionFunc) || (Player_Action_MiniCutscene == this->actionFunc))) {
            this->actor.bgCheckFlags &= ~(BGCHECKFLAG_WALL | BGCHECKFLAG_PLAYER_WALL_INTERACT);
            flags = UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else {
            flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                    UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        }
    } else {
        flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
    }

    if (this->stateFlags3 & PLAYER_STATE3_IGNORE_CEILING_FLOOR_AND_WATER) {
        flags &= ~(UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    }

    if (flags & UPDBGCHECKINFO_FLAG_2) {
        this->stateFlags3 |= PLAYER_STATE3_CHECKING_FLOOR_AND_WATER_COLLISION;
    }

    Math_Vec3f_Copy(&spB4, &this->actor.world.pos);
    Actor_UpdateBgCheckInfo(play, &this->actor, spAC, spB0, spA8, flags);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
        this->actor.velocity.y = 0.0f;
    }

    sYDistToFloor = this->actor.world.pos.y - this->actor.floorHeight;
    sConveyorSpeed = CONVEYOR_SPEED_DISABLED;

    floorPoly = this->actor.floorPoly;

    if (floorPoly != NULL) {
        this->floorProperty = SurfaceType_GetFloorProperty(&play->colCtx, floorPoly, this->actor.floorBgId);
        this->prevFloorSfxOffset = this->floorSfxOffset;

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            if (this->actor.depthInWater < 20.0f) {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_WATER_SHALLOW;
            } else {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_WATER_DEEP;
            }
        } else {
            if (this->stateFlags2 & PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND) {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_SAND;
            } else {
                this->floorSfxOffset = SurfaceType_GetSfxOffset(&play->colCtx, floorPoly, this->actor.floorBgId);
            }
        }

        if (this->actor.category == ACTORCAT_PLAYER) {
            Audio_SetCodeReverb(SurfaceType_GetEcho(&play->colCtx, floorPoly, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                Environment_ChangeLightSetting(
                    play, SurfaceType_GetLightSetting(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                DynaPoly_SetPlayerAbove(&play->colCtx, this->actor.floorBgId);
            }
        }

        sConveyorSpeed = SurfaceType_GetConveyorSpeed(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (sConveyorSpeed != CONVEYOR_SPEED_DISABLED) {
            sIsFloorConveyor = SurfaceType_IsFloorConveyor(&play->colCtx, floorPoly, this->actor.floorBgId);

            if ((!sIsFloorConveyor && (this->actor.depthInWater > 20.0f) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                (sIsFloorConveyor && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                sConveyorYaw = CONVEYOR_DIRECTION_TO_BINANG(
                    SurfaceType_GetConveyorDirection(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                sConveyorSpeed = CONVEYOR_SPEED_DISABLED;
            }
        }
    }

    Player_HandleExitsAndVoids(play, this, floorPoly, this->actor.floorBgId);

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        CollisionPoly* wallPoly;
        s32 wallBgId;
        s16 sp9A;
        s32 pad;

        sInteractWallCheckOffset.y = 18.0f;
        sInteractWallCheckOffset.z = this->ageProperties->wallCheckRadius + 10.0f;

        if (!(this->stateFlags2 & PLAYER_STATE2_CRAWLING) &&
            Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &wallPoly, &wallBgId,
                                     &sInteractWallCheckResult)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;

            if (this->actor.wallPoly != wallPoly) {
                this->actor.wallPoly = wallPoly;
                this->actor.wallBgId = wallBgId;
                this->actor.wallYaw = Math_Atan2S(wallPoly->normal.z, wallPoly->normal.x);
            }
        }

        sp9A = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);

        sTouchedWallFlags = SurfaceType_GetWallFlags(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);

        sShapeYawToTouchedWall = ABS(sp9A);

        sp9A = this->yaw - (s16)(this->actor.wallYaw + 0x8000);

        sWorldYawToTouchedWall = ABS(sp9A);

        spB0 = sWorldYawToTouchedWall * 0.00008f;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || spB0 >= 1.0f) {
            this->speedLimit = R_RUN_SPEED_LIMIT / 100.0f;
        } else {
            spAC = (R_RUN_SPEED_LIMIT / 100.0f * spB0);
            this->speedLimit = spAC;
            if (spAC < 0.1f) {
                this->speedLimit = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (sShapeYawToTouchedWall < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if (ABS(wallPoly->normal.y) < 600) {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalY = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 yDistToLedge;
                CollisionPoly* groundPoly;
                CollisionPoly* sp78;
                s32 sp74;
                Vec3f sp68;
                f32 sp64;
                f32 sp60;
                s32 temp3;

                this->distToInteractWall = Math3D_UDistPlaneToPos(wallPolyNormalX, wallPolyNormalY, wallPolyNormalZ,
                                                                  wallPoly->dist, &this->actor.world.pos);

                spB0 = this->distToInteractWall + 10.0f;
                sp68.x = this->actor.world.pos.x - (spB0 * wallPolyNormalX);
                sp68.z = this->actor.world.pos.z - (spB0 * wallPolyNormalZ);
                sp68.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                sp64 = BgCheck_EntityRaycastDown1(&play->colCtx, &groundPoly, &sp68);
                yDistToLedge = sp64 - this->actor.world.pos.y;
                this->yDistToLedge = yDistToLedge;

                if ((this->yDistToLedge < 18.0f) ||
                    BgCheck_EntityCheckCeiling(&play->colCtx, &sp60, &this->actor.world.pos,
                                               (sp64 - this->actor.world.pos.y) + 20.0f, &sp78, &sp74, &this->actor)) {
                    this->yDistToLedge = LEDGE_DIST_MAX;
                } else {
                    sInteractWallCheckOffset.y = (sp64 + 5.0f) - this->actor.world.pos.y;

                    if (Player_PosVsWallLineTest(play, this, &sInteractWallCheckOffset, &sp78, &sp74,
                                                 &sInteractWallCheckResult) &&
                        (temp3 = this->actor.wallYaw - Math_Atan2S(sp78->normal.z, sp78->normal.x),
                         ABS(temp3) < 0x4000) &&
                        !SurfaceType_CheckWallFlag1(&play->colCtx, sp78, sp74)) {
                        this->yDistToLedge = LEDGE_DIST_MAX;
                    } else if (SurfaceType_CheckWallFlag0(&play->colCtx, wallPoly, this->actor.wallBgId) == 0) {
                        if (this->ageProperties->unk_1C <= this->yDistToLedge) {
                            if (ABS(groundPoly->normal.y) > 28000) {
                                if (this->ageProperties->unk_14 <= this->yDistToLedge) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_4;
                                } else if (this->ageProperties->unk_18 <= this->yDistToLedge) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_3;
                                } else {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_2;
                                }
                            }
                        } else {
                            nextLedgeClimbType = PLAYER_LEDGE_CLIMB_1;
                        }
                    }
                }
            }
        }
    } else {
        this->speedLimit = R_RUN_SPEED_LIMIT / 100.0f;
        this->ledgeClimbDelayTimer = 0;
        this->yDistToLedge = 0.0f;
    }

    if (nextLedgeClimbType == this->ledgeClimbType) {
        if ((this->speedXZ != 0.0f) && (this->ledgeClimbDelayTimer < 100)) {
            this->ledgeClimbDelayTimer++;
        }
    } else {
        this->ledgeClimbType = nextLedgeClimbType;
        this->ledgeClimbDelayTimer = 0;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        sFloorType = SurfaceType_GetFloorType(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (!Player_UpdateHoverBoots(this)) {
            f32 floorPolyNormalX;
            f32 invFloorPolyNormalY;
            f32 floorPolyNormalZ;
            f32 sp4C;
            s32 pad2;
            f32 sp44;
            s32 pad3;

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                DynaPoly_SetPlayerOnTop(&play->colCtx, this->actor.floorBgId);
            }

            floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            invFloorPolyNormalY = 1.0f / COLPOLY_GET_NORMAL(floorPoly->normal.y);
            floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

            sp4C = Math_SinS(this->yaw);
            sp44 = Math_CosS(this->yaw);

            this->floorPitch =
                Math_Atan2S(1.0f, (-(floorPolyNormalX * sp4C) - (floorPolyNormalZ * sp44)) * invFloorPolyNormalY);
            this->floorPitchAlt =
                Math_Atan2S(1.0f, (-(floorPolyNormalX * sp44) - (floorPolyNormalZ * sp4C)) * invFloorPolyNormalY);

            sp4C = Math_SinS(this->actor.shape.rot.y);
            sp44 = Math_CosS(this->actor.shape.rot.y);

            sFloorPitchShape =
                Math_Atan2S(1.0f, (-(floorPolyNormalX * sp4C) - (floorPolyNormalZ * sp44)) * invFloorPolyNormalY);

            Player_HandleSlopes(play, this, floorPoly);
        }
    } else {
        Player_UpdateHoverBoots(this);
    }

    if (this->prevFloorType == sFloorType) {
        this->floorTypeTimer++;
    } else {
        this->prevFloorType = sFloorType;
        this->floorTypeTimer = 0;
    }
}

void Player_UpdateCamAndSeqModes(PlayState* play, Player* this) {
    u8 seqMode;
    s32 pad;
    Actor* lockOnActor;
    s32 camMode;

    if (this->actor.category == ACTORCAT_PLAYER) {
        seqMode = SEQ_MODE_DEFAULT;

        if (this->csMode != PLAYER_CSMODE_NONE) {
            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_NORMAL);
        } else if (!(this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE)) {
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH)) {
                camMode = CAM_MODE_HOOKSHOT_FLY;
                Camera_SetViewParam(Play_GetCamera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->actor.parent);
            } else if (Player_Action_KnockbackFly == this->actionFunc) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags2 & PLAYER_STATE2_ENABLE_PUSH_PULL_CAM) {
                camMode = CAM_MODE_PUSH_PULL;
            } else if ((lockOnActor = this->lockOnActor) != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK_REQUESTED)) {
                    camMode = CAM_MODE_TALK;
                } else if (this->stateFlags1 & PLAYER_STATE1_LOCK_ON_FRIEND) {
                    if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
                        camMode = CAM_MODE_FOLLOW_BOOMERANG;
                    } else {
                        camMode = CAM_MODE_Z_TARGET_FRIENDLY;
                    }
                } else {
                    camMode = CAM_MODE_Z_TARGET_UNFRIENDLY;
                }
                Camera_SetViewParam(Play_GetCamera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, lockOnActor);
            } else if (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
                camMode = CAM_MODE_CHARGE;
            } else if (this->stateFlags1 & PLAYER_STATE1_AWAITING_THROWN_BOOMERANG) {
                camMode = CAM_MODE_FOLLOW_BOOMERANG;
                Camera_SetViewParam(Play_GetCamera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->boomerangActor);
            } else if (this->stateFlags1 &
                       (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE)) {
                if (Player_IsZParallelOrLockOnFriend(this)) {
                    camMode = CAM_MODE_Z_LEDGE_HANG;
                } else {
                    camMode = CAM_MODE_LEDGE_HANG;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_Z_PARALLEL | PLAYER_STATE1_Z_PARALLEL_FROM_UNTARGET)) {
                if (Player_IsAimingFpsItem(this) || Player_IsAimingBoomerang(this)) {
                    camMode = CAM_MODE_Z_AIM;
                } else if (this->stateFlags1 & PLAYER_STATE1_CLIMBING) {
                    camMode = CAM_MODE_Z_WALL_CLIMB;
                } else {
                    camMode = CAM_MODE_Z_PARALLEL;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_JUMPING | PLAYER_STATE1_CLIMBING)) {
                if ((Player_Action_JumpToLedge == this->actionFunc) || (this->stateFlags1 & PLAYER_STATE1_CLIMBING)) {
                    camMode = CAM_MODE_WALL_CLIMB;
                } else {
                    camMode = CAM_MODE_JUMP;
                }
            } else if (this->stateFlags1 & PLAYER_STATE1_FREEFALLING) {
                camMode = CAM_MODE_FREE_FALL;
            } else if ((this->meleeWeaponState != 0) && (this->meleeWeaponAnimation >= PLAYER_MWA_FORWARD_SLASH_1H) &&
                       (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H)) {
                camMode = CAM_MODE_STILL;
            } else {
                camMode = CAM_MODE_NORMAL;
                if ((this->speedXZ == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) || (this->rideActor->speed == 0.0f))) {
                    // not moving
                    seqMode = SEQ_MODE_STILL;
                }
            }

            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), camMode);
        } else {
            // First person mode
            seqMode = SEQ_MODE_STILL;
        }

        if (play->actorCtx.targetCtx.bgmEnemy != NULL) {
            seqMode = SEQ_MODE_ENEMY;
            Audio_SetBgmEnemyVolume(sqrtf(play->actorCtx.targetCtx.bgmEnemy->xyzDistToPlayerSq));
        }

        if (play->sceneId != SCENE_FISHING_POND) {
            Audio_SetSequenceMode(seqMode);
        }
    }
}

static Vec3f sStickFlameVelocity = { 0.0f, 0.5f, 0.0f };
static Vec3f sStickFlameAccel = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 sStickFlamePrimColor = { 255, 255, 100, 255 };
static Color_RGBA8 sStickFlameEnvColor = { 255, 50, 0, 0 };

void Player_UpdateDekuStick(PlayState* play, Player* this) {
    f32 newDekuStickLength;

    if (this->dekuStickLength == 0.0f) {
        Player_UseItem(play, this, ITEM_NONE);
        return;
    }

    newDekuStickLength = 1.0f;
    if (DECR(this->stickFlameTimer) == 0) {
        Inventory_ChangeAmmo(ITEM_DEKU_STICK, -1);
        this->stickFlameTimer = 1;
        newDekuStickLength = 0.0f;
        this->dekuStickLength = newDekuStickLength;
    } else if (this->stickFlameTimer > 200) {
        newDekuStickLength = (210 - this->stickFlameTimer) / 10.0f;
    } else if (this->stickFlameTimer < 20) {
        newDekuStickLength = this->stickFlameTimer / 20.0f;
        this->dekuStickLength = newDekuStickLength;
    }

    // Spawn flame effect
    func_8002836C(play, &this->meleeWeaponInfo[0].tip, &sStickFlameVelocity, &sStickFlameAccel, &sStickFlamePrimColor,
                  &sStickFlameEnvColor, newDekuStickLength * 200.0f, 0, 8);
}

void Player_UpdateElectricShocking(PlayState* play, Player* this) {
    Vec3f shockPos;
    Vec3f* randBodyPart;
    s32 shockScale;

    this->shockTimer--;
    this->unk_892 += this->shockTimer;

    if (this->unk_892 > 20) {
        shockScale = this->shockTimer * 2;
        this->unk_892 -= 20;

        if (shockScale > 40) {
            shockScale = 40;
        }

        randBodyPart = this->bodyPartsPos + (s32)Rand_ZeroFloat(PLAYER_BODYPART_MAX - 0.1f);
        shockPos.x = (Rand_CenteredFloat(5.0f) + randBodyPart->x) - this->actor.world.pos.x;
        shockPos.y = (Rand_CenteredFloat(5.0f) + randBodyPart->y) - this->actor.world.pos.y;
        shockPos.z = (Rand_CenteredFloat(5.0f) + randBodyPart->z) - this->actor.world.pos.z;

        EffectSsFhgFlash_SpawnShock(play, &this->actor, &shockPos, shockScale, FHGFLASH_SHOCK_PLAYER);
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

void Player_UpdateBurning(PlayState* play, Player* this) {
    s32 spawnedFlame;
    u8* timerPtr;
    s32 timerStep;
    f32 flameScale;
    f32 flameIntensity;
    s32 dmgCooldown;
    s32 i;
    s32 timerExtraStep;
    s32 timerBaseStep;

    if (this->currentTunic == PLAYER_TUNIC_GORON) {
        timerBaseStep = 20;
    } else {
        timerBaseStep = (s32)(this->speedXZ * 0.4f) + 1;
    }

    spawnedFlame = false;
    timerPtr = this->flameTimers;

    if (this->stateFlags2 & PLAYER_STATE2_MAKING_NOTICABLE_SFX) {
        timerExtraStep = 100;
    } else {
        timerExtraStep = 0;
    }

    Player_TryBurningDekuShield(this, play);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        timerStep = timerExtraStep + timerBaseStep;

        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        } else {
            spawnedFlame = true;
            *timerPtr -= timerStep;

            if (*timerPtr > 20.0f) {
                flameIntensity = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(flameIntensity, 0.19999999f, 0.2f);
            } else {
                flameScale = *timerPtr * 0.01f;
            }

            flameIntensity = (*timerPtr - 25.0f) * 0.02f;
            flameIntensity = CLAMP(flameIntensity, 0.0f, 1.0f);
            EffectSsFireTail_SpawnFlameOnPlayer(play, flameScale, i, flameIntensity);
        }
    }

    if (spawnedFlame) {
        Player_PlaySfx(this, NA_SE_EV_TORCH - SFX_FLAG);

        if (play->sceneId == SCENE_SPIRIT_TEMPLE_BOSS) {
            dmgCooldown = 0;
        } else {
            dmgCooldown = 7;
        }

        if ((dmgCooldown & play->gameplayFrames) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        this->isBurning = false;
    }
}

/**
 * Rumbles the controller when close to a secret.
 */
void Player_DetectSecrets(Player* this) {
    if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        f32 step = (SQ(200.0f) * 5.0f) - (this->closestSecretDistSq * 5.0f);

        if (step < 0.0f) {
            step = 0.0f;
        }

        this->secretRumbleCharge += step;
        if (this->secretRumbleCharge > SQ(2000.0f)) {
            this->secretRumbleCharge = 0.0f;
            Player_RequestRumble(this, 120, 20, 10, SQ(0));
        }
    }
}

static s8 D_808547C4[PLAYER_CUEID_MAX] = {
    PLAYER_CSMODE_NONE, // PLAYER_CUEID_NONE
    PLAYER_CSMODE_3,    // PLAYER_CUEID_1
    PLAYER_CSMODE_3,    // PLAYER_CUEID_2
    PLAYER_CSMODE_5,    // PLAYER_CUEID_3
    PLAYER_CSMODE_4,    // PLAYER_CUEID_4
    PLAYER_CSMODE_8,    // PLAYER_CUEID_5
    PLAYER_CSMODE_9,    // PLAYER_CUEID_6
    PLAYER_CSMODE_13,   // PLAYER_CUEID_7
    PLAYER_CSMODE_14,   // PLAYER_CUEID_8
    PLAYER_CSMODE_15,   // PLAYER_CUEID_9
    PLAYER_CSMODE_16,   // PLAYER_CUEID_10
    PLAYER_CSMODE_17,   // PLAYER_CUEID_11
    PLAYER_CSMODE_18,   // PLAYER_CUEID_12
    -PLAYER_CSMODE_22,  // PLAYER_CUEID_13
    PLAYER_CSMODE_23,   // PLAYER_CUEID_14
    PLAYER_CSMODE_24,   // PLAYER_CUEID_15
    PLAYER_CSMODE_25,   // PLAYER_CUEID_16
    PLAYER_CSMODE_26,   // PLAYER_CUEID_17
    PLAYER_CSMODE_27,   // PLAYER_CUEID_18
    PLAYER_CSMODE_28,   // PLAYER_CUEID_19
    PLAYER_CSMODE_29,   // PLAYER_CUEID_20
    PLAYER_CSMODE_31,   // PLAYER_CUEID_21
    PLAYER_CSMODE_32,   // PLAYER_CUEID_22
    PLAYER_CSMODE_33,   // PLAYER_CUEID_23
    PLAYER_CSMODE_34,   // PLAYER_CUEID_24
    -PLAYER_CSMODE_35,  // PLAYER_CUEID_25
    PLAYER_CSMODE_30,   // PLAYER_CUEID_26
    PLAYER_CSMODE_36,   // PLAYER_CUEID_27
    PLAYER_CSMODE_38,   // PLAYER_CUEID_28
    -PLAYER_CSMODE_39,  // PLAYER_CUEID_29
    -PLAYER_CSMODE_40,  // PLAYER_CUEID_30
    -PLAYER_CSMODE_41,  // PLAYER_CUEID_31
    PLAYER_CSMODE_42,   // PLAYER_CUEID_32
    PLAYER_CSMODE_43,   // PLAYER_CUEID_33
    PLAYER_CSMODE_45,   // PLAYER_CUEID_34
    PLAYER_CSMODE_46,   // PLAYER_CUEID_35
    PLAYER_CSMODE_NONE, // PLAYER_CUEID_36
    PLAYER_CSMODE_NONE, // PLAYER_CUEID_37
    PLAYER_CSMODE_NONE, // PLAYER_CUEID_38
    PLAYER_CSMODE_67,   // PLAYER_CUEID_39
    PLAYER_CSMODE_48,   // PLAYER_CUEID_40
    PLAYER_CSMODE_47,   // PLAYER_CUEID_41
    -PLAYER_CSMODE_50,  // PLAYER_CUEID_42
    PLAYER_CSMODE_51,   // PLAYER_CUEID_43
    -PLAYER_CSMODE_52,  // PLAYER_CUEID_44
    -PLAYER_CSMODE_53,  // PLAYER_CUEID_45
    PLAYER_CSMODE_54,   // PLAYER_CUEID_46
    PLAYER_CSMODE_55,   // PLAYER_CUEID_47
    PLAYER_CSMODE_56,   // PLAYER_CUEID_48
    PLAYER_CSMODE_57,   // PLAYER_CUEID_49
    PLAYER_CSMODE_58,   // PLAYER_CUEID_50
    PLAYER_CSMODE_59,   // PLAYER_CUEID_51
    PLAYER_CSMODE_60,   // PLAYER_CUEID_52
    PLAYER_CSMODE_61,   // PLAYER_CUEID_53
    PLAYER_CSMODE_62,   // PLAYER_CUEID_54
    PLAYER_CSMODE_63,   // PLAYER_CUEID_55
    PLAYER_CSMODE_64,   // PLAYER_CUEID_56
    -PLAYER_CSMODE_65,  // PLAYER_CUEID_57
    -PLAYER_CSMODE_66,  // PLAYER_CUEID_58
    PLAYER_CSMODE_68,   // PLAYER_CUEID_59
    PLAYER_CSMODE_11,   // PLAYER_CUEID_60
    PLAYER_CSMODE_69,   // PLAYER_CUEID_61
    PLAYER_CSMODE_70,   // PLAYER_CUEID_62
    PLAYER_CSMODE_71,   // PLAYER_CUEID_63
    PLAYER_CSMODE_8,    // PLAYER_CUEID_64
    PLAYER_CSMODE_8,    // PLAYER_CUEID_65
    PLAYER_CSMODE_72,   // PLAYER_CUEID_66
    PLAYER_CSMODE_73,   // PLAYER_CUEID_67
    PLAYER_CSMODE_78,   // PLAYER_CUEID_68
    PLAYER_CSMODE_79,   // PLAYER_CUEID_69
    PLAYER_CSMODE_80,   // PLAYER_CUEID_70
    PLAYER_CSMODE_89,   // PLAYER_CUEID_71
    PLAYER_CSMODE_90,   // PLAYER_CUEID_72
    PLAYER_CSMODE_91,   // PLAYER_CUEID_73
    PLAYER_CSMODE_92,   // PLAYER_CUEID_74
    PLAYER_CSMODE_77,   // PLAYER_CUEID_75
    PLAYER_CSMODE_19,   // PLAYER_CUEID_76
    PLAYER_CSMODE_94,   // PLAYER_CUEID_77
};

static Vec3f D_80854814 = { 0.0f, 0.0f, 200.0f };

static f32 sWaterConveyorSpeeds[CONVEYOR_SPEED_MAX - 1] = {
    2.0f, // CONVEYOR_SPEED_SLOW
    4.0f, // CONVEYOR_SPEED_MEDIUM
    7.0f, // CONVEYOR_SPEED_FAST
};
static f32 sFloorConveyorSpeeds[CONVEYOR_SPEED_MAX - 1] = {
    0.5f, // CONVEYOR_SPEED_SLOW
    1.0f, // CONVEYOR_SPEED_MEDIUM
    3.0f, // CONVEYOR_SPEED_FAST
};

void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    s32 pad;

    sControlInput = input;

    if (this->voidRespawnCounter < 0) {
        this->voidRespawnCounter++;
        if (this->voidRespawnCounter == 0) {
            this->voidRespawnCounter = 1;
            Audio_PlaySfx(NA_SE_OC_REVENGE);
        }
    }

    Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.home.pos);

    if (this->unk_A73 != 0) {
        this->unk_A73--;
    }

    if (this->unk_88E != 0) {
        this->unk_88E--;
    }

    if (this->unk_A87 != 0) {
        this->unk_A87--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    } else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->unk_890 != 0) {
        this->unk_890--;
    }

    Player_UpdateDoAction(play, this);
    Player_UpdateZTarget(this, play);

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->stickFlameTimer != 0)) {
        Player_UpdateDekuStick(play, this);
    } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->stickFlameTimer < 0)) {
        this->stickFlameTimer++;
    }

    if (this->shockTimer != 0) {
        Player_UpdateElectricShocking(play, this);
    }

    if (this->isBurning) {
        Player_UpdateBurning(play, this);
    }

    if ((this->stateFlags3 & PLAYER_STATE3_RESTORE_NAYRUS_LOVE) && (gSaveContext.nayrusLoveTimer != 0) &&
        (gSaveContext.magicState == MAGIC_STATE_IDLE)) {
        gSaveContext.magicState = MAGIC_STATE_METER_FLASH_1;
        Player_SpawnMagicSpellActor(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (this->stateFlags2 & PLAYER_STATE2_PAUSE_MOST_UPDATING) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Player_SetHorizontalSpeedToZero(this);
            Actor_MoveXZGravity(&this->actor);
        }

        Player_ProcessSceneCollision(play, this);
    } else {
        f32 temp_f0;
        f32 phi_f12;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    Player_ResetSubCam(play, this);
                    if (this->actor.depthInWater > this->ageProperties->unk_2C) {
                        this->stateFlags2 |= PLAYER_STATE2_DIVING;
                    }
                }
            } else {
                if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
                    if ((this->prevBoots == PLAYER_BOOTS_IRON) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                        func_8083D36C(play, this);
                        this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
                    }
                }
            }

            this->prevBoots = this->currentBoots;
        }

        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
            this->actor.parent = this->rideActor;
            Player_CsIntoAction_SetupRideHorse(play, this);
            this->stateFlags1 |= PLAYER_STATE1_RIDING_HORSE;
            Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_uma_wait_1);
            Player_AnimReplace_Setup(play, this, 0x9B);
            this->rideHorseVar16 = 99;
        }

        if (this->unk_844 == 0) {
            this->slashCounter = 0;
        } else if (this->unk_844 < 0) {
            this->unk_844++;
        } else {
            this->unk_844--;
        }

        Math_ScaledStepToS(&this->shapePitchOffset, 0, 400);
        func_80032CB4(this->unk_3A8, 20, 80, 6);

        this->actor.shape.face = this->unk_3A8[0] + ((play->gameplayFrames & 32) ? 0 : 3);

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Player_UpdateBunnyEars(this);
        }

        if (Player_IsUsingFpsItem(this)) {
            func_8084FF7C(this);
        }

        if (!(this->skelAnime.moveFlags & 0x80)) {
            if (((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (sFloorType == FLOOR_TYPE_5) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((this->currentBoots == PLAYER_BOOTS_HOVER) &&
                 !(this->stateFlags1 & (PLAYER_STATE1_SWIMMING | PLAYER_STATE1_IN_CUTSCENE)))) {
                f32 speedXZTarget = this->speedXZ;
                s16 yawTarget = this->yaw;
                s16 yawDiff = this->actor.world.rot.y - yawTarget;
                s32 pad;

                if ((ABS(yawDiff) > DEG_TO_BINANG(135.0f)) && (this->actor.speed != 0.0f)) {
                    speedXZTarget = 0.0f;
                    yawTarget += 0x8000;
                }

                if (Math_StepToF(&this->actor.speed, speedXZTarget, 0.35f) && (speedXZTarget == 0.0f)) {
                    this->actor.world.rot.y = this->yaw;
                }

                if (this->speedXZ != 0.0f) {
                    s32 step;

                    step = (fabsf(this->speedXZ) * 700.0f) - (fabsf(this->actor.speed) * 100.0f);
                    step = CLAMP(step, 0, 1350);

                    Math_ScaledStepToS(&this->actor.world.rot.y, yawTarget, step);
                }

                if ((this->speedXZ == 0.0f) && (this->actor.speed != 0.0f)) {
                    func_800F4138(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speed);
                }
            } else {
                this->actor.speed = this->speedXZ;
                this->actor.world.rot.y = this->yaw;
            }

            Actor_UpdateVelocityXZGravity(&this->actor);

            if ((this->pushedSpeed != 0.0f) && !Player_InCsMode(play) &&
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                       PLAYER_STATE1_CLIMBING)) &&
                (Player_Action_JumpToLedge != this->actionFunc) && (Player_Action_CastMagicSpell != this->actionFunc)) {
                this->actor.velocity.x += this->pushedSpeed * Math_SinS(this->pushedYaw);
                this->actor.velocity.z += this->pushedSpeed * Math_CosS(this->pushedYaw);
            }

            Actor_UpdatePos(&this->actor);
            Player_ProcessSceneCollision(play, this);
        } else {
            sFloorType = FLOOR_TYPE_0;
            this->floorProperty = FLOOR_PROPERTY_0;

            if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE) &&
                (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE)) {
                EnHorse* rideActor = (EnHorse*)this->rideActor;
                CollisionPoly* sp5C;
                s32 sp58;
                Vec3f sp4C;

                if (!(rideActor->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    Player_PosVsFloorLineTestImpl(play, this, &D_80854814, &sp4C, &sp5C, &sp58);
                } else {
                    sp5C = rideActor->actor.floorPoly;
                    sp58 = rideActor->actor.floorBgId;
                }

                if ((sp5C != NULL)) {
                    if (Player_HandleExitsAndVoids(play, this, sp5C, sp58)) {
                        if (DREG(25) != 0) {
                            DREG(25) = 0;
                        } else {
                            AREG(6) = 1;
                        }
                    }
                }
            }

            sConveyorSpeed = CONVEYOR_SPEED_DISABLED;
            this->pushedSpeed = 0.0f;
        }

        // This block applies the bg conveyor to pushedSpeed
        if ((sConveyorSpeed != CONVEYOR_SPEED_DISABLED) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            f32 conveyorSpeed;

            // converts 1-index to 0-index
            sConveyorSpeed--;

            if (!sIsFloorConveyor) {
                conveyorSpeed = sWaterConveyorSpeeds[sConveyorSpeed];

                if (!(this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
                    conveyorSpeed *= 0.25f;
                }
            } else {
                conveyorSpeed = sFloorConveyorSpeeds[sConveyorSpeed];
            }

            Math_StepToF(&this->pushedSpeed, conveyorSpeed, conveyorSpeed * 0.1f);

            Math_ScaledStepToS(&this->pushedYaw, sConveyorYaw,
                               ((this->stateFlags1 & PLAYER_STATE1_SWIMMING) ? 400.0f : 800.0f) * conveyorSpeed);
        } else if (this->pushedSpeed != 0.0f) {
            Math_StepToF(&this->pushedSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_SWIMMING) ? 0.5f : 1.0f);
        }

        if (!Player_InBlockingCsMode(play, this) && !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
            Player_UpdateUnderwater(play, this);

            if ((this->actor.category == ACTORCAT_PLAYER) && (gSaveContext.health == 0)) {
                if (this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                         PLAYER_STATE1_CLIMBING)) {
                    Player_ResetAttributes(play, this);
                    Player_Setup_Midair(this, play);
                } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                           (this->stateFlags1 & PLAYER_STATE1_SWIMMING)) {
                    Player_SetupDie(play, this,
                                    Player_IsFreeSwimming(this) ? &gPlayerAnim_link_swimer_swim_down
                                    : (this->shockTimer != 0)   ? &gPlayerAnim_link_normal_electric_shock_end
                                                                : &gPlayerAnim_link_derth_rebirth);
                }
            } else {
                if ((this->actor.parent == NULL) && ((play->transitionTrigger == TRANS_TRIGGER_START) ||
                                                     (this->unk_A87 != 0) || !Player_UpdateDamage(this, play))) {
                    func_8083AA10(this, play);
                } else {
                    this->fallStartHeight = this->actor.world.pos.y;
                }
                Player_DetectSecrets(this);
            }
        }

        if ((play->csCtx.state != CS_STATE_IDLE) && (this->csMode != PLAYER_CSMODE_6) &&
            !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) &&
            !(this->stateFlags2 & PLAYER_STATE2_RESTRAINED_BY_ENEMY) && (this->actor.category == ACTORCAT_PLAYER)) {
            CsCmdActorCue* cue = play->csCtx.playerCue;

            if ((cue != NULL) && (D_808547C4[cue->id] != PLAYER_CSMODE_NONE)) {
                func_8002DF54(play, NULL, PLAYER_CSMODE_6);
                Player_SetHorizontalSpeedToZero(this);
            } else if ((this->csMode == PLAYER_CSMODE_NONE) && !(this->stateFlags2 & PLAYER_STATE2_DIVING) &&
                       (play->csCtx.state != CS_STATE_STOP)) {
                func_8002DF54(play, NULL, PLAYER_CSMODE_49);
                Player_SetHorizontalSpeedToZero(this);
            }
        }

        if (this->csMode != PLAYER_CSMODE_NONE) {
            if ((this->csMode != PLAYER_CSMODE_7) ||
                !(this->stateFlags1 & (PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE |
                                       PLAYER_STATE1_CLIMBING | PLAYER_STATE1_TAKING_DAMAGE))) {
                this->attentionMode = PLAYER_ATTENTIONMODE_CUTSCENE;
            } else if (Player_Action_StartCutscene != this->actionFunc) {
                Player_CsAction_53(play, this, NULL);
            }
        } else {
            this->prevCsMode = PLAYER_CSMODE_NONE;
        }

        func_8083D6EC(play, this);

        if ((this->lockOnActor == NULL) && (this->naviTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_CAN_SPEAK_OR_CHECK | PLAYER_STATE2_NAVI_REQUESTING_TALK);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_SWINGING_BOTTLE | PLAYER_STATE1_READY_TO_SHOOT |
                               PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_HOLDING_SHIELD);
        this->stateFlags2 &=
            ~(PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_CAN_CLIMB_PUSH_PULL_WALL |
              PLAYER_STATE2_MAKING_NOTICABLE_SFX | PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON |
              PLAYER_STATE2_NO_YAW_UPDATE | PLAYER_STATE2_ENABLE_PUSH_PULL_CAM | PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND |
              PLAYER_STATE2_IDLE_WHILE_CLIMBING | PLAYER_STATE2_FROZEN_IN_ICE | PLAYER_STATE2_DO_ACTION_ENTER |
              PLAYER_STATE2_CAN_DISMOUNT_HORSE | PLAYER_STATE2_DRAW_REFLECTION);
        this->stateFlags3 &= ~PLAYER_STATE3_CHECKING_FLOOR_AND_WATER_COLLISION;

        Player_StepRotToZero(this);
        Player_StoreAnalogStickInput(play, this);

        if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
            sPlayerUnderwaterSpeedAdjustment = 0.5f;
        } else {
            sPlayerUnderwaterSpeedAdjustment = 1.0f;
        }

        D_808535EC = 1.0f / sPlayerUnderwaterSpeedAdjustment;
        D_80853614 = D_80853618 = 0;
        D_80858AA4 = this->currentMask;

        if (!(this->stateFlags3 & PLAYER_STATE3_PAUSE_ACTION)) {
            this->actionFunc(this, play);
        }

        Player_UpdateCamAndSeqModes(play, this);

        if (this->skelAnime.moveFlags & 8) {
            AnimationContext_SetMoveActor(play, &this->actor, &this->skelAnime,
                                          (this->skelAnime.moveFlags & 4) ? 1.0f : this->ageProperties->unk_08);
        }

        Player_UpdateYaw(this, play);

        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK_REQUESTED)) {
            this->talkActorDistance = 0.0f;
        } else {
            this->talkActor = NULL;
            this->talkActorDistance = FLT_MAX;
            this->exchangeItemId = EXCH_ITEM_NONE;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = DEG_TO_BINANG(135.0f);
        }

        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->naviTextId = 0;

        if (!(this->stateFlags2 & PLAYER_STATE2_OCARINA_ON_FOR_ACTOR)) {
            this->ocarinaActor = NULL;
        }

        this->stateFlags2 &= ~PLAYER_STATE2_OCARINA_START_OVERRIDE;
        this->closestSecretDistSq = FLT_MAX;

        temp_f0 = this->actor.world.pos.y - this->actor.prevPos.y;

        this->doorType = PLAYER_DOORTYPE_NONE;
        this->specialDamageEffect = 0;
        this->forcedLockOn = NULL;

        phi_f12 =
            ((this->bodyPartsPos[PLAYER_BODYPART_L_FOOT].y + this->bodyPartsPos[PLAYER_BODYPART_R_FOOT].y) * 0.5f) +
            temp_f0;
        temp_f0 += this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 10.0f;

        this->cylinder.dim.height = temp_f0 - phi_f12;

        if (this->cylinder.dim.height < 0) {
            phi_f12 = temp_f0;
            this->cylinder.dim.height = -this->cylinder.dim.height;
        }

        this->cylinder.dim.yShift = phi_f12 - this->actor.world.pos.y;

        if (this->stateFlags1 & PLAYER_STATE1_HOLDING_SHIELD) {
            this->cylinder.dim.height = this->cylinder.dim.height * 0.8f;
        }

        Collider_UpdateCylinder(&this->actor, &this->cylinder);

        if (!(this->stateFlags2 & PLAYER_STATE2_FROZEN_IN_ICE)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP |
                                       PLAYER_STATE1_CLIMBING_ONTO_LEDGE | PLAYER_STATE1_RIDING_HORSE))) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder.base);
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_TAKING_DAMAGE)) &&
                (this->invincibilityTimer <= 0)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder.base);

                if (this->invincibilityTimer < 0) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
        }

        AnimationContext_SetNextQueue(play);
    }

    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    Math_Vec3f_Copy(&this->unk_A88, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    if (this->stateFlags1 &
        (PLAYER_STATE1_IN_DEATH_CUTSCENE | PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = 50;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_PAUSE_ACTION;

    Collider_ResetCylinderAC(play, &this->cylinder.base);

    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[0].base);
    Collider_ResetQuadAT(play, &this->meleeWeaponQuads[1].base);

    Collider_ResetQuadAC(play, &this->shieldQuad.base);
    Collider_ResetQuadAT(play, &this->shieldQuad.base);
}

static Vec3f D_80854838 = { 0.0f, 0.0f, -30.0f };

void Player_Update(Actor* thisx, PlayState* play) {
    static Vec3f sDogSpawnPos;
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 pad;
    Input sp44;
    Actor* dog;

    if (Player_TryDebugNoClip(this, play)) {
        if (gSaveContext.dogParams < 0) {
            if (Object_GetIndex(&play->objectCtx, OBJECT_DOG) < 0) {
                gSaveContext.dogParams = 0;
            } else {
                gSaveContext.dogParams &= 0x7FFF;
                Player_GetRelativePosition(this, &this->actor.world.pos, &D_80854838, &sDogSpawnPos);
                dogParams = gSaveContext.dogParams;

                dog = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DOG, sDogSpawnPos.x, sDogSpawnPos.y, sDogSpawnPos.z,
                                  0, this->actor.shape.rot.y, 0, dogParams | 0x8000);
                if (dog != NULL) {
                    dog->room = 0;
                }
            }
        }

        if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
            this->interactRangeActor = NULL;
        }

        if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
            Player_DetatchHeldActor(play, this);
        }

        if (this->stateFlags1 & (PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_IN_CUTSCENE)) {
            bzero(&sp44, sizeof(sp44));
        } else {
            sp44 = play->state.input[0];
            if (this->unk_88E != 0) {
                sp44.cur.button &= ~(BTN_A | BTN_B | BTN_CUP);
                sp44.press.button &= ~(BTN_A | BTN_B | BTN_CUP);
            }
        }

        Player_UpdateCommon(this, play, &sp44);
    }

    MREG(52) = this->actor.world.pos.x;
    MREG(53) = this->actor.world.pos.y;
    MREG(54) = this->actor.world.pos.z;
    MREG(55) = this->actor.world.rot.y;
}

typedef struct {
    /* 0x0 */ Vec3s rot;
    /* 0x6 */ Vec3s angVel;
} BunnyEarKinematics; // size = 0xC

static BunnyEarKinematics sBunnyEarKinematics;

static Vec3s D_80858AD8[25];

static Gfx* sMaskDlists[PLAYER_MASK_MAX - 1] = {
    gLinkChildKeatonMaskDL, gLinkChildSkullMaskDL, gLinkChildSpookyMaskDL, gLinkChildBunnyHoodDL,
    gLinkChildGoronMaskDL,  gLinkChildZoraMaskDL,  gLinkChildGerudoMaskDL, gLinkChildMaskOfTruthDL,
};

static Vec3s D_80854864 = { 0, 0, 0 };

void Player_DrawGameplay(PlayState* play, Player* this, s32 lod, Gfx* cullDList, OverrideLimbDrawOpa overrideLimbDraw) {
    static s32 D_8085486C = 255;

    OPEN_DISPS(play->state.gfxCtx, "../z_player.c", 19228);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    Player_DrawImpl(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
                    this->currentTunic, this->currentBoots, this->actor.shape.face, overrideLimbDraw,
                    Player_PostLimbDrawGameplay, this);

    if ((overrideLimbDraw == Player_OverrideLimbDrawGameplayDefault) && (this->currentMask != PLAYER_MASK_NONE)) {
        Mtx* bunnyEarMtx = Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Mtx));

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Vec3s earRot;

            gSPSegment(POLY_OPA_DISP++, 0x0B, bunnyEarMtx);

            // Right ear
            earRot.x = sBunnyEarKinematics.rot.y + 0x3E2;
            earRot.y = sBunnyEarKinematics.rot.z + 0xDBE;
            earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, -240.0f, &earRot);
            Matrix_ToMtx(bunnyEarMtx++, "../z_player.c", 19273);

            // Left ear
            earRot.x = sBunnyEarKinematics.rot.y - 0x3E2;
            earRot.y = -sBunnyEarKinematics.rot.z - 0xDBE;
            earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
            Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f, 240.0f, &earRot);
            Matrix_ToMtx(bunnyEarMtx, "../z_player.c", 19279);
        }

        gSPDisplayList(POLY_OPA_DISP++, sMaskDlists[this->currentMask - 1]);
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        !(this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) && (this->hoverBootsTimer != 0)) {
        s32 sp5C;
        s32 hoverBootsTimer = this->hoverBootsTimer;

        if (this->hoverBootsTimer < 19) {
            if (hoverBootsTimer >= 15) {
                D_8085486C = (19 - hoverBootsTimer) * 51.0f;
            } else if (hoverBootsTimer < 19) {
                sp5C = hoverBootsTimer;

                if (sp5C > 9) {
                    sp5C = 9;
                }

                D_8085486C = (-sp5C * 4) + 36;
                D_8085486C = D_8085486C * D_8085486C;
                D_8085486C = (s32)((Math_CosS(D_8085486C) * 100.0f) + 100.0f) + 55.0f;
                D_8085486C = D_8085486C * (sp5C * (1.0f / 9.0f));
            }

            Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 2.0f,
                                         this->actor.world.pos.z, &D_80854864);
            Matrix_Scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, "../z_player.c", 19317),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 16, 32, 1, 0,
                                        (play->gameplayFrames * -15) % 128, 16, 32));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, D_8085486C);
            gDPSetEnvColor(POLY_XLU_DISP++, 120, 90, 30, 128);
            gSPDisplayList(POLY_XLU_DISP++, gHoverBootsCircleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_player.c", 19328);
}

void Player_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_player.c", 19346);

    if (!(this->stateFlags2 & PLAYER_STATE2_DISABLE_DRAW)) {
        OverrideLimbDrawOpa overrideLimbDraw = Player_OverrideLimbDrawGameplayDefault;
        s32 lod;
        s32 pad;

        if ((this->csMode != PLAYER_CSMODE_NONE) || (Player_IsEnemyLockOn(this) && 0) ||
            (this->actor.projectedPos.z < 160.0f)) {
            lod = 0;
        } else {
            lod = 1;
        }

        func_80093C80(play);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        if (this->invincibilityTimer > 0) {
            this->damageFlashTimer += CLAMP(50 - this->invincibilityTimer, 8, 40);
            POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP, 255, 0, 0, 0, 0,
                                        4000 - (s32)(Math_CosS(this->damageFlashTimer * 0x100) * 2000.0f));
        }

        func_8002EBCC(&this->actor, play, 0);
        func_8002ED80(&this->actor, play, 0);

        if (this->attentionMode != PLAYER_ATTENTIONMODE_NONE) {
            Vec3f projectedHeadPos;

            SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->actor.focus.pos, &projectedHeadPos);
            if (projectedHeadPos.z < -4.0f) {
                overrideLimbDraw = Player_OverrideLimbDrawGameplayFirstPerson;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
            if (this->actor.projectedPos.z < 0.0f) {
                // Player is behind the camera
                overrideLimbDraw = Player_OverrideLimbDrawGameplayCrawling;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_DRAW_REFLECTION) {
            f32 sp78 = BINANG_TO_RAD_ALT2((u16)(play->gameplayFrames * 600));
            f32 sp74 = BINANG_TO_RAD_ALT2((u16)(play->gameplayFrames * 1000));

            Matrix_Push();
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_SetTranslateRotateYXZ(
                this->actor.world.pos.x,
                (this->actor.floorHeight + (this->actor.floorHeight - this->actor.world.pos.y)) +
                    (this->actor.shape.yOffset * this->actor.scale.y),
                this->actor.world.pos.z, &this->actor.shape.rot);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateX(sp78, MTXMODE_APPLY);
            Matrix_RotateY(sp74, MTXMODE_APPLY);
            Matrix_Scale(1.1f, 0.95f, 1.05f, MTXMODE_APPLY);
            Matrix_RotateY(-sp74, MTXMODE_APPLY);
            Matrix_RotateX(-sp78, MTXMODE_APPLY);
            Player_DrawGameplay(play, this, lod, gCullFrontDList, overrideLimbDraw);
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_Pop();
        }

        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

        Player_DrawGameplay(play, this, lod, gCullBackDList, overrideLimbDraw);

        if (this->invincibilityTimer > 0) {
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }

        if (this->stateFlags2 & PLAYER_STATE2_FROZEN_IN_ICE) {
            f32 scale = (this->actionVar8 >> 1) * 22.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (0 - play->gameplayFrames) % 128, 32,
                                        32, 1, 0, (play->gameplayFrames * -2) % 128, 32, 32));

            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, "../z_player.c", 19459),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->unk_862 > 0) {
            Player_DrawGetItem(play, this);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_player.c", 19473);
}

void Player_Destroy(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    Effect_Delete(play, this->meleeWeaponEffectIndex);

    Collider_DestroyCylinder(play, &this->cylinder);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[0]);
    Collider_DestroyQuad(play, &this->meleeWeaponQuads[1]);
    Collider_DestroyQuad(play, &this->shieldQuad);

    Magic_Reset(play);

    gSaveContext.linkAge = play->linkAgeOnLoad;
}

s16 func_8084ABD8(PlayState* play, Player* this, s32 arg2, s16 arg3) {
    s32 temp1;
    s16 temp2;
    s16 temp3;

    if (!Player_IsAimingFpsItem(this) && !Player_IsAimingBoomerang(this) && (arg2 == 0)) {
        temp2 = sControlInput->rel.stick_y * 240.0f;
        Math_SmoothStepToS(&this->actor.focus.rot.x, temp2, 14, 4000, 30);

        temp2 = sControlInput->rel.stick_x * -16.0f;
        temp2 = CLAMP(temp2, -3000, 3000);
        this->actor.focus.rot.y += temp2;
    } else {
        temp1 = (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) ? 3500 : 14000;
        temp3 = ((sControlInput->rel.stick_y >= 0) ? 1 : -1) *
                (s32)((1.0f - Math_CosS(sControlInput->rel.stick_y * 200)) * 1500.0f);
        this->actor.focus.rot.x += temp3;
        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -temp1, temp1);

        temp1 = 19114;
        temp2 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((sControlInput->rel.stick_x >= 0) ? 1 : -1) *
                (s32)((1.0f - Math_CosS(sControlInput->rel.stick_x * 200)) * -1500.0f);
        temp2 += temp3;
        this->actor.focus.rot.y = CLAMP(temp2, -temp1, temp1) + this->actor.shape.rot.y;
    }

    this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y;
    return Player_UpdateLookAngles(this, (play->shootingGalleryStatus != 0) || Player_IsAimingFpsItem(this) ||
                                             Player_IsAimingBoomerang(this)) -
           arg3;
}

void Player_UpdateSwimVelocityYaw(Player* this, f32* linearVelocity, f32 inputVelocity, s16 inputYaw) {
    f32 step = this->skelAnime.curFrame - 10.0f;
    f32 speedLimit = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;

    if (*linearVelocity > speedLimit) {
        *linearVelocity = speedLimit;
    }

    if ((step > 0.0f) && (step < 10.0f)) {
        step *= 6.0f;
    } else {
        step = 0.0f;
        inputVelocity = 0.0f;
    }

    Math_AsymStepToF(linearVelocity, inputVelocity * 0.8f, step, (fabsf(*linearVelocity) * 0.02f) + 0.05f);
    Math_ScaledStepToS(&this->yaw, inputYaw, 1600);
}

#define IRON_BOOTS_MAX_SINK_SPEED -3.0f
#define IRON_BOOTS_SINK_ACCEL -0.2f

void Player_ApplyBuoyancy(Player* this) {
    f32 accel;
    f32 maxVelocity = -5.0f;
    f32 buoyancyDepthInWater = this->ageProperties->buoyancyDepthInWater;
    f32 depthInWater;
    f32 extraAccel;

    if (this->actor.velocity.y < 0.0f) {
        buoyancyDepthInWater += 1.0f;
    }

    if (this->actor.depthInWater < buoyancyDepthInWater) {
        // Above buoyancy equilibrium point. Accelerate down
        if (this->actor.velocity.y <= 0.0f) {
            extraAccel = 0.0f;
        } else {
            extraAccel = this->actor.velocity.y * 0.5f;
        }
        accel = -0.1f - extraAccel;
    } else {
        // Below buoyancy equilibrium point
        if (!(this->stateFlags1 & PLAYER_STATE1_IN_DEATH_CUTSCENE) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
            (this->actor.velocity.y >= IRON_BOOTS_MAX_SINK_SPEED)) {
            // Force accelerate down
            accel = IRON_BOOTS_SINK_ACCEL;
        } else {
            // Accelerate up
            maxVelocity = 2.0f;
            if (this->actor.velocity.y >= 0.0f) {
                extraAccel = 0.0f;
            } else {
                extraAccel = this->actor.velocity.y * -0.3f;
            }
            accel = extraAccel + 0.1f;
        }

        depthInWater = this->actor.depthInWater;
        if (depthInWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_DIVING;
        }
    }

    this->actor.velocity.y += accel;

    if (((this->actor.velocity.y - maxVelocity) * accel) > 0) {
        this->actor.velocity.y = maxVelocity;
    }

    this->actor.gravity = 0.0f;
}

void Player_UpdateSwimAnim(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 playSpeed;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
        playSpeed = 1.0f;
    } else {
        playSpeed = 0.5f;
    }

    playSpeed *= arg3;

    if (playSpeed < 1.0f) {
        playSpeed = 1.0f;
    }

    this->skelAnime.playSpeed = playSpeed;
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_Action_AimFirstPerson(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_SWIMMING) {
        Player_ApplyBuoyancy(this);
        Player_UpdateSwimVelocityYaw(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
    } else {
        Player_StepHorizontalSpeedToZero(this);
    }

    if ((this->attentionMode == PLAYER_ATTENTIONMODE_AIMING) &&
        (Player_IsUsingFpsItem(this) || Player_IsUsingBoomerang(this))) {
        Player_TryUpperAction(this, play);
    }

    if ((this->csMode != PLAYER_CSMODE_NONE) || (this->attentionMode == PLAYER_ATTENTIONMODE_NONE) ||
        (this->attentionMode >= PLAYER_ATTENTIONMODE_ITEM_CUTSCENE) || Player_TryEnemyLockOn(this) ||
        (this->lockOnActor != NULL) || (Player_TryFirstPersonCameraMode(play, this) == CAM_MODE_NORMAL) ||
        (((this->attentionMode == PLAYER_ATTENTIONMODE_AIMING) &&
          (CHECK_BTN_ANY(sControlInput->press.button, BTN_A | BTN_B | BTN_R) ||
           Player_IsZParallelOrLockOnFriend(this) ||
           (!Player_IsAimingFpsItem(this) && !Player_IsAimingBoomerang(this)))) ||
         ((this->attentionMode == PLAYER_ATTENTIONMODE_C_UP) &&
          CHECK_BTN_ANY(sControlInput->press.button,
                        BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN)))) {
        func_8083C148(this, play);
        Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
    } else if ((DECR(this->aimFirstPersonVar16) == 0) || (this->attentionMode != PLAYER_ATTENTIONMODE_AIMING)) {
        if (Player_IsShootingHookshot(this)) {
            this->rotOverrideFlags |=
                PLAYER_ROT_OVERRIDE_FOCUS_ROT_X | PLAYER_ROT_OVERRIDE_FOCUS_ROT_Y | PLAYER_ROT_OVERRIDE_UPPER_ROT_X;
        } else {
            this->actor.shape.rot.y = func_8084ABD8(play, this, 0, 0);
        }
    }

    this->yaw = this->actor.shape.rot.y;
}

s32 Player_TryPlayingShootingGallery(PlayState* play, Player* this) {
    if (play->shootingGalleryStatus != 0) {
        Player_DetachHeldActorAndResetAttributes(play, this);
        Player_SetAction(play, this, Player_Action_PlayShootingGallery, 0);

        if (!Player_IsUsingFpsItem(this) || Player_IsHoldingHookshot(this)) {
            Player_UseItem(play, this, 3);
        }

        this->stateFlags1 |= PLAYER_STATE1_IN_FIRST_PERSON_MODE;
        Player_Anim_PlayOnce(play, this, Player_GetIdleAnim(this));
        Player_SetHorizontalSpeedToZero(this);
        Player_SetRotToZero(this);
        return true;
    }

    return false;
}

void Player_SetItemActionToOcarina(Player* this) {
    this->itemAction =
        (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) ? PLAYER_IA_OCARINA_FAIRY : PLAYER_IA_OCARINA_OF_TIME;
}

s32 Player_TryOcarinaAfterTextbox(PlayState* play, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_OCARINA_AFTER_TEXTBOX) {
        this->stateFlags3 &= ~PLAYER_STATE3_OCARINA_AFTER_TEXTBOX;
        Player_SetItemActionToOcarina(this);
        this->attentionMode = PLAYER_ATTENTIONMODE_ITEM_CUTSCENE;
        Player_SwapAction_TryItemCsFirstPerson(this, play);
        return true;
    }

    return false;
}

void Player_Action_Talk(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    Player_TryUpperAction(this, play);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_REQUESTED;

        if (!CHECK_FLAG_ALL(this->talkActor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_ENEMY)) {
            this->stateFlags2 &= ~PLAYER_STATE2_USING_SWITCH_Z_TARGET;
        }

        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));

        if (!Player_TryOcarinaAfterTextbox(play, this) && !Player_TryPlayingShootingGallery(play, this) &&
            !Player_TryStartingCutscene(play, this)) {
            if ((this->talkActor != this->interactRangeActor) || !Player_SwapAction_TryGetItem(this, play)) {
                if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
                    s32 rideHorseVar16 = this->actionVar16;

                    Player_CsIntoAction_SetupRideHorse(play, this);
                    this->rideHorseVar16 = rideHorseVar16;
                } else if (Player_IsFreeSwimming(this)) {
                    Player_SetupSwimIdle(play, this);
                } else {
                    Player_SetupIdleWithMorph(this, play);
                }
            }
        }

        this->unk_88E = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
        Player_Action_RideHorse(this, play);
    } else if (Player_IsFreeSwimming(this)) {
        Player_Action_SwimIdle(this, play);
    } else if (!Player_IsEnemyLockOn(this) && PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->skelAnime.moveFlags != 0) {
            Player_Anim_ResetMove(this);
            if ((this->talkActor->category == ACTORCAT_NPC) && (this->heldItemAction != PLAYER_IA_FISHING_POLE)) {
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_talk_free);
            } else {
                Player_Anim_PlayLoop(play, this, Player_GetIdleAnim(this));
            }
        } else {
            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_talk_free_wait);
        }
    }

    if (this->lockOnActor != NULL) {
        this->yaw = this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void Player_Action_GrabWall(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 pushPullDir;

    this->stateFlags2 |=
        PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_NO_YAW_UPDATE | PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;
    Player_ProcessGrabPushPullWallInteraction(play, this);

    if (!PlayerAnimation_Update(play, &this->skelAnime)) {
        return;
    }

    if (func_8083F9D0(play, this)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    pushPullDir = Player_GetPushPullDirection(this, &inputVelocity, &inputYaw);
    if (pushPullDir > 0) {
        Player_SetupPush(this, play);
    } else if (pushPullDir < 0) {
        Player_SetupPull(this, play);
    }
}

void func_8084B840(PlayState* play, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

        if (dynaPolyActor != NULL) {
            func_8002DFA4(dynaPolyActor, arg2, this->actor.world.rot.y);
        }
    }
}

static AnimSfxEntry D_80854870[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 3, NA_SE_PL_SLIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 21, NA_SE_PL_SLIP, STOP),
};

void Player_Action_Push(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s32 pushPullDir;

    this->stateFlags2 |=
        PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_NO_YAW_UPDATE | PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;

    if (Player_Anim_PlayLoopOnceFinished(play, this, &gPlayerAnim_link_normal_pushing)) {
        this->pushVar16 = true;
    } else if (!this->pushVar16) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 11.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_PUSH);
        }
    }

    Player_AnimSfx_Play(this, D_80854870);
    Player_ProcessGrabPushPullWallInteraction(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
        pushPullDir = Player_GetPushPullDirection(this, &inputVelocity, &inputYaw);
        if (pushPullDir < 0) {
            Player_SetupPull(this, play);
        } else if (pushPullDir == 0) {
            func_8083F72C(this, &gPlayerAnim_link_normal_push_end, play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
        func_8084B840(play, this, 2.0f);
        this->speedXZ = 2.0f;
    }
}

static AnimSfxEntry D_80854878[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 4, NA_SE_PL_SLIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 24, NA_SE_PL_SLIP, STOP),
};

static Vec3f D_80854880 = { 0.0f, 26.0f, -40.0f };

void Player_Action_Pull(Player* this, PlayState* play) {
    PlayerAnimationHeader* anim;
    f32 inputVelocity;
    s16 inputYaw;
    s32 pushPullDir;
    Vec3f sp5C;
    f32 temp2;
    CollisionPoly* sp54;
    s32 sp50;
    Vec3f sp44;
    Vec3f sp38;

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pulling, this->modelAnimType);
    this->stateFlags2 |=
        PLAYER_STATE2_CAN_GRAB_PUSH_PULL_WALL | PLAYER_STATE2_NO_YAW_UPDATE | PLAYER_STATE2_ENABLE_PUSH_PULL_CAM;

    if (Player_Anim_PlayLoopOnceFinished(play, this, anim)) {
        this->actionVar16 = 1;
    } else {
        if (this->actionVar16 == 0) {
            if (PlayerAnimation_OnFrame(&this->skelAnime, 11.0f)) {
                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_PUSH);
            }
        } else {
            Player_AnimSfx_Play(this, D_80854878);
        }
    }

    Player_ProcessGrabPushPullWallInteraction(play, this);

    if (!func_8083F9D0(play, this)) {
        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
        pushPullDir = Player_GetPushPullDirection(this, &inputVelocity, &inputYaw);
        if (pushPullDir > 0) {
            Player_SetupPush(this, play);
        } else if (pushPullDir == 0) {
            func_8083F72C(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_end, this->modelAnimType), play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_MOVING_PUSH_PULL_WALL) {
        temp2 = Player_PosVsFloorLineTest(play, this, &D_80854880, &sp5C) - this->actor.world.pos.y;
        if (fabsf(temp2) < 20.0f) {
            sp44.x = this->actor.world.pos.x;
            sp44.z = this->actor.world.pos.z;
            sp44.y = sp5C.y;
            if (!BgCheck_EntityLineTest1(&play->colCtx, &sp44, &sp5C, &sp38, &sp54, true, false, false, true, &sp50)) {
                func_8084B840(play, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_MOVING_PUSH_PULL_WALL;
    }
}

void Player_Action_GrabLedge(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    PlayerAnimationHeader* anim;
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        // clang-format off
        anim = (this->actionVar8 > 0) ? &gPlayerAnim_link_normal_fall_wait : GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_wait, this->modelAnimType); Player_Anim_PlayLoop(play, this, anim);
        // clang-format on
    } else if (this->actionVar8 == 0) {
        if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
            temp = 11.0f;
        } else {
            temp = 1.0f;
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, temp)) {
            Player_AnimSfx_PlayFloor(this, NA_SE_PL_WALK_GROUND);
            if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
                this->actionVar8 = 1;
            } else {
                this->actionVar8 = -1;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->yaw, 0x800);

    if (this->actionVar8 != 0) {
        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
        if (this->analogStickDirection128Parts[this->inputFrameCounter] >= 0) {
            if (this->actionVar8 > 0) {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_fall_up, this->modelAnimType);
            } else {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_up, this->modelAnimType);
            }
            Player_SetupClimbLedge(this, anim, play);
            return;
        }

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) || (this->actor.shape.feetFloorFlag != 0)) {
            func_80837B60(this);
            if (this->actionVar8 < 0) {
                this->speedXZ = -0.8f;
            } else {
                this->speedXZ = 0.8f;
            }
            Player_Setup_Midair(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE);
        }
    }
}

void Player_Action_ClimbLedge(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_SetupLedgeClimb(this, 1);
        Player_SetupIdle(this, play);
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        Player_AnimSfx_PlayFloorLand(this);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        this->stateFlags1 &= ~(PLAYER_STATE1_HANGING_FROM_LEDGE_SLIP | PLAYER_STATE1_CLIMBING_ONTO_LEDGE);
        Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_CLIMB_END);
    }
}

void func_8084BEE4(Player* this) {
    Player_PlaySfx(this, (this->actionVar8 != 0) ? NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_VINE
                                                 : NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD);
}

void Player_Action_ClimbWall(Player* this, PlayState* play) {
    static Vec3f D_8085488C = { 0.0f, 0.0f, 26.0f };
    s32 stickAdjY;
    s32 stickAdjX;
    f32 animPlaySpeed;
    f32 animDir;
    Vec3f sp6C;
    s32 sp68;
    Vec3f sp5C;
    f32 temp_f0;
    PlayerAnimationHeader* verticalClimbAnim;
    PlayerAnimationHeader* horizontalClimbAnim;

    stickAdjY = sControlInput->rel.stick_y;
    stickAdjX = sControlInput->rel.stick_x;

    this->fallStartHeight = this->actor.world.pos.y;
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    if ((this->actionVar8 != 0) && (ABS(stickAdjY) < ABS(stickAdjX))) {
        animPlaySpeed = ABS(stickAdjX) * 0.0325f;
        stickAdjY = 0;
    } else {
        animPlaySpeed = ABS(stickAdjY) * 0.05f;
        stickAdjX = 0;
    }

    if (animPlaySpeed < 1.0f) {
        animPlaySpeed = 1.0f;
    } else if (animPlaySpeed > 3.35f) {
        animPlaySpeed = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        animDir = 1.0f;
    } else {
        animDir = -1.0f;
    }

    this->skelAnime.playSpeed = animDir * animPlaySpeed;

    if (this->actionVar16 >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            DynaPolyActor* wallPolyActor = DynaPoly_GetActor(&play->colCtx, this->actor.wallBgId);

            if (wallPolyActor != NULL) {
                Math_Vec3f_Diff(&wallPolyActor->actor.world.pos, &wallPolyActor->actor.prevPos, &sp6C);
                Math_Vec3f_Sum(&this->actor.world.pos, &sp6C, &this->actor.world.pos);
            }
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 6.0f, this->ageProperties->ceilingCheckHeight,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
        Player_ProcessSpecialWallInteraction(play, this, 26.0f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    if ((this->actionVar16 < 0) || !Player_TryLettingGoWhileClimbing(this, play)) {
        if (PlayerAnimation_Update(play, &this->skelAnime) != 0) {
            if (this->actionVar16 < 0) {
                this->actionVar16 = ABS(this->actionVar16) & 1;
                return;
            }

            if (stickAdjY != 0) {
                sp68 = this->actionVar8 + this->actionVar16;

                if (stickAdjY > 0) {
                    D_8085488C.y = this->ageProperties->unk_40;
                    temp_f0 = Player_PosVsFloorLineTest(play, this, &D_8085488C, &sp5C);

                    if (this->actor.world.pos.y < temp_f0) {
                        if (this->actionVar8 != 0) {
                            this->actor.world.pos.y = temp_f0;
                            this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
                            Player_SetupGrabLedge(play, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                                  &gPlayerAnim_link_normal_jump_climb_up_free);
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            Player_SetupClimbLedge(this, &gPlayerAnim_link_normal_jump_climb_up_free, play);
                            this->stateFlags1 |= PLAYER_STATE1_CLIMBING_ONTO_LEDGE;
                        } else {
                            Player_SetupClimbEnd(this, this->ageProperties->unk_CC[this->actionVar16], play);
                        }
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp68];
                        Player_Anim_PlayOnce(play, this, this->ageProperties->unk_AC[sp68]);
                    }
                } else {
                    if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                        if (this->actionVar8 != 0) {
                            Player_LetGoWhileClimbing(this, play);
                        } else {
                            if (this->actionVar16 != 0) {
                                this->skelAnime.prevTransl = this->ageProperties->unk_44;
                            }
                            Player_SetupClimbEnd(this, this->ageProperties->unk_C4[this->actionVar16], play);
                            this->actionVar16 = 1;
                        }
                    } else {
                        sp68 ^= 1;
                        this->skelAnime.prevTransl = this->ageProperties->unk_62[sp68];
                        verticalClimbAnim = this->ageProperties->unk_AC[sp68];
                        PlayerAnimation_Change(play, &this->skelAnime, verticalClimbAnim, -PLAYER_ANIM_NORMAL_SPEED,
                                               Animation_GetLastFrame(verticalClimbAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
                    }
                }
                this->actionVar16 ^= 1;
            } else if ((this->actionVar8 != 0) && (stickAdjX != 0)) {
                horizontalClimbAnim = this->ageProperties->climbHorizontalAnims[this->actionVar16];

                if (stickAdjX > 0) {
                    // Climb left
                    this->skelAnime.prevTransl = this->ageProperties->climbLeftSpeed[this->actionVar16];
                    Player_Anim_PlayOnce(play, this, horizontalClimbAnim);
                } else {
                    // Climb right
                    this->skelAnime.prevTransl = this->ageProperties->unk_86[this->actionVar16];
                    PlayerAnimation_Change(play, &this->skelAnime, horizontalClimbAnim, -PLAYER_ANIM_NORMAL_SPEED,
                                           Animation_GetLastFrame(horizontalClimbAnim), 0.0f, ANIMMODE_ONCE, 0.0f);
                }
            } else {
                this->stateFlags2 |= PLAYER_STATE2_IDLE_WHILE_CLIMBING;
            }

            return;
        }
    }

    if (this->actionVar16 < 0) {
        if (((this->actionVar16 == -2) &&
             (PlayerAnimation_OnFrame(&this->skelAnime, 14.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 29.0f))) ||
            ((this->actionVar16 == -4) &&
             (PlayerAnimation_OnFrame(&this->skelAnime, 22.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 35.0f) ||
              PlayerAnimation_OnFrame(&this->skelAnime, 49.0f) || PlayerAnimation_OnFrame(&this->skelAnime, 55.0f)))) {
            func_8084BEE4(this);
        }
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, (this->skelAnime.playSpeed > 0.0f) ? 20.0f : 0.0f)) {
        func_8084BEE4(this);
    }
}

static f32 D_80854898[] = { 10.0f, 20.0f };
static f32 D_808548A0[] = { 40.0f, 50.0f };

static AnimSfxEntry D_808548A8[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 10, NA_SE_PL_WALK_LADDER, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_PL_WALK_LADDER, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 30, NA_SE_PL_WALK_LADDER, STOP),
};

void Player_Action_ClimbEnd(Player* this, PlayState* play) {
    s32 actionInterruptState;
    f32* sp38;
    CollisionPoly* groundPoly;
    s32 bgId;
    Vec3f sp24;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    actionInterruptState = Player_GetActionInterruptState(play, this, &this->skelAnime, 4.0f);

    if (actionInterruptState == PLAYER_ACTION_INTERRUPT_SWAP) {
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }

    if ((actionInterruptState >= PLAYER_ACTION_INTERRUPT_MOVE) || PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_SetupIdle(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }

    sp38 = D_80854898;

    if (this->actionVar16 != 0) {
        Player_AnimSfx_Play(this, D_808548A8);
        sp38 = D_808548A0;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, sp38[0]) || PlayerAnimation_OnFrame(&this->skelAnime, sp38[1])) {
        sp24.x = this->actor.world.pos.x;
        sp24.y = this->actor.world.pos.y + 20.0f;
        sp24.z = this->actor.world.pos.z;
        if (BgCheck_EntityRaycastDown3(&play->colCtx, &groundPoly, &bgId, &sp24) != 0.0f) {
            //! @bug should use `SurfaceType_GetSfxOffset` instead of `SurfaceType_GetMaterial`.
            // Most material and sfxOffsets share identical enum values,
            // so this will mostly result in the correct sfx played, but not in all cases, such as carpet and ice.
            this->floorSfxOffset = SurfaceType_GetMaterial(&play->colCtx, groundPoly, bgId);
            Player_AnimSfx_PlayFloorLand(this);
        }
    }
}

static AnimSfxEntry sEnterCrawlspaceAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 40, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 48, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 56, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 64, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 72, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 80, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 88, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 96, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 104, NA_SE_NONE, STOP),
};

/**
 * Update player's animation while entering the crawlspace.
 * Once inside, stop all player animations and update player's movement.
 */
void Player_Action_Crawl(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_EXITING_SCENE)) {
            // While inside a crawlspace, player's skeleton does not move
            if (this->skelAnime.moveFlags != 0) {
                this->skelAnime.moveFlags = 0;
                return;
            }

            if (!Player_TryLeavingCrawlspace(this, play)) {
                // Move forward and back while inside the crawlspace
                this->speedXZ = sControlInput->rel.stick_y * 0.03f;
            }
        }
        return;
    }

    // Still entering crawlspace
    Player_AnimSfx_Play(this, sEnterCrawlspaceAnimSfx);
}

static AnimSfxEntry sLeaveCrawlspaceAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 10, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 18, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 26, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 34, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 52, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 60, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 68, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 76, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 84, NA_SE_NONE, STOP),
};

/**
 * Update player's animation while leaving the crawlspace.
 */
void Player_Action_LeaveCrawlspace(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        // Player is finished exiting the crawlspace and control is returned
        Player_SetupIdle(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_CRAWLING;
        return;
    }

    // Continue animation of leaving crawlspace
    Player_AnimSfx_Play(this, sLeaveCrawlspaceAnimSfx);
}

static Vec3f D_808548FC[] = {
    { 40.0f, 0.0f, 0.0f },
    { -40.0f, 0.0f, 0.0f },
};

static Vec3f D_80854914[] = {
    { 60.0f, 20.0f, 0.0f },
    { -60.0f, 20.0f, 0.0f },
};

static Vec3f D_8085492C[] = {
    { 60.0f, -20.0f, 0.0f },
    { -60.0f, -20.0f, 0.0f },
};

s32 Player_CanDismountHorse(PlayState* play, Player* this, s32 dismountSide, f32* arg3) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 sp50;
    f32 sp4C;
    Vec3f sp40;
    Vec3f sp34;
    CollisionPoly* sp30;
    s32 sp2C;

    sp50 = rideActor->actor.world.pos.y + 20.0f;
    sp4C = rideActor->actor.world.pos.y - 20.0f;

    *arg3 = Player_PosVsFloorLineTest(play, this, &D_808548FC[dismountSide], &sp40);

    return (sp4C < *arg3) && (*arg3 < sp50) &&
           !Player_PosVsWallLineTest(play, this, &D_80854914[dismountSide], &sp30, &sp2C, &sp34) &&
           !Player_PosVsWallLineTest(play, this, &D_8085492C[dismountSide], &sp30, &sp2C, &sp34);
}

s32 Player_TryDismountingHorse(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 dismountSide;
    f32 floorPosY;

    if (this->rideHorseVar16 < 0) {
        this->rideHorseVar16 = 99;
    } else {
        dismountSide = (this->mountSide < 0) ? 0 : 1;
        if (!Player_CanDismountHorse(play, this, dismountSide, &floorPosY)) {
            dismountSide ^= 1;
            if (!Player_CanDismountHorse(play, this, dismountSide, &floorPosY)) {
                return 0;
            } else {
                this->mountSide = -this->mountSide;
            }
        }

        if ((play->csCtx.state == CS_STATE_IDLE) && (play->transitionMode == TRANS_MODE_OFF) &&
            (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor))) {
            this->stateFlags2 |= PLAYER_STATE2_CAN_DISMOUNT_HORSE;

            if (EN_HORSE_CHECK_1(rideActor) ||
                (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(sControlInput->press.button, BTN_A))) {
                rideActor->actor.child = NULL;
                Player_SetAction_PreserveMoveFlags(play, this, Player_Action_DismountHorse, 0);
                this->rideOffsetY = floorPosY - rideActor->actor.world.pos.y;
                Player_Anim_PlayOnce(play, this,
                                     (this->mountSide < 0) ? &gPlayerAnim_link_uma_left_down
                                                           : &gPlayerAnim_link_uma_right_down);
                return true;
            }
        }
    }

    return false;
}

void Player_BounceOnHorse(Player* this, f32 offsetY, f32 frame) {
    f32 rideOffsetY;
    f32 dir;

    if ((this->rideOffsetY != 0.0f) && (this->skelAnime.curFrame >= frame)) {
        if (offsetY < fabsf(this->rideOffsetY)) {
            if (this->rideOffsetY >= 0.0f) {
                dir = 1;
            } else {
                dir = -1;
            }
            rideOffsetY = dir * offsetY;
        } else {
            rideOffsetY = this->rideOffsetY;
        }
        this->actor.world.pos.y += rideOffsetY;
        this->rideOffsetY -= rideOffsetY;
    }
}

static PlayerAnimationHeader* D_80854944[] = {
    &gPlayerAnim_link_uma_anim_stop,
    &gPlayerAnim_link_uma_anim_stand,
    &gPlayerAnim_link_uma_anim_walk,
    &gPlayerAnim_link_uma_anim_slowrun,
    &gPlayerAnim_link_uma_anim_fastrun,
    &gPlayerAnim_link_uma_anim_jump100,
    &gPlayerAnim_link_uma_anim_jump200,
    NULL,
    NULL,
};

static PlayerAnimationHeader* D_80854968[] = {
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_slowrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    NULL,
    NULL,
};

static PlayerAnimationHeader* D_8085498C[] = {
    &gPlayerAnim_link_uma_wait_3,
    &gPlayerAnim_link_uma_wait_1,
    &gPlayerAnim_link_uma_wait_2,
};

static u8 D_80854998[2][2] = {
    { 32, 58 },
    { 25, 42 },
};

static Vec3s D_8085499C = { -69, 7146, -266 };

static AnimSfxEntry D_808549A4[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 48, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 58, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 68, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 92, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 110, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 126, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 132, NA_SE_PL_CALM_PAT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 136, NA_SE_PL_CALM_PAT, STOP),
};

void Player_Action_RideHorse(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    u8* arr;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;

    Player_BounceOnHorse(this, 1.0f, 10.0f);

    if (this->rideHorseVar16 == 0) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_link_uma_wait_1;
            this->rideHorseVar16 = 99;
            return;
        }

        arr = D_80854998[(this->mountSide < 0) ? 0 : 1];

        if (PlayerAnimation_OnFrame(&this->skelAnime, arr[0])) {
            Player_PlaySfx(this, NA_SE_PL_CLIMB_CLIFF);
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, arr[1])) {
            Actor_SetCameraHorseSetting(play, this);
            Player_PlaySfx(this, NA_SE_PL_SIT_ON_HORSE);
        }

        return;
    }

    Actor_SetCameraHorseSetting(play, this);
    this->skelAnime.prevTransl = D_8085499C;

    if ((rideActor->animIndex != this->rideHorseVar16) &&
        ((rideActor->animIndex >= 2) || (this->rideHorseVar16 >= 2))) {
        if ((this->rideHorseVar16 = rideActor->animIndex) < 2) {
            f32 rand = Rand_ZeroOne();
            s32 temp = 0;

            this->rideHorseVar16 = 1;

            if (rand < 0.1f) {
                temp = 2;
            } else if (rand < 0.2f) {
                temp = 1;
            }
            Player_Anim_PlayOnce(play, this, D_8085498C[temp]);
        } else {
            this->skelAnime.animation = D_80854944[this->rideHorseVar16 - 2];
            Animation_SetMorph(play, &this->skelAnime, 8.0f);
            if (this->rideHorseVar16 < 4) {
                Player_SetupUpperActionForHeldItem(play, this);
                this->actionVar8 = 0;
            }
        }
    }

    if (this->rideHorseVar16 == 1) {
        if (D_808535E0 || Player_IsTalking(play)) {
            Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_uma_wait_3);
        } else if (PlayerAnimation_Update(play, &this->skelAnime)) {
            this->rideHorseVar16 = 99;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_uma_wait_1) {
            Player_AnimSfx_Play(this, D_808549A4);
        }
    } else {
        this->skelAnime.curFrame = rideActor->curFrame;
        PlayerAnimation_AnimateFrame(play, &this->skelAnime);
    }

    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.morphTable,
                                this->skelAnime.jointTable);

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csMode != PLAYER_CSMODE_NONE)) {
        if (this->csMode == PLAYER_CSMODE_7) {
            this->csMode = PLAYER_CSMODE_NONE;
        }
        this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
        this->actionVar8 = 0;
    } else if ((this->rideHorseVar16 < 2) || (this->rideHorseVar16 >= 4)) {
        D_808535E0 = Player_TryUpperAction(this, play);
        if (D_808535E0) {
            this->actionVar8 = 0;
        }
    }

    this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
    this->actor.world.pos.y = (rideActor->actor.world.pos.y + rideActor->riderPos.y) - 27.0f;
    this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

    this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

    if ((this->csMode != PLAYER_CSMODE_NONE) ||
        (!Player_IsTalking(play) && ((rideActor->actor.speed != 0.0f) || !Player_SwapAction_TryTalking(this, play)) &&
         !Player_SwapAction_TryRolling(this, play))) {
        if (!D_808535E0) {
            if (this->actionVar8 != 0) {
                if (PlayerAnimation_Update(play, &this->upperSkelAnime)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->actionVar8 = 0;
                }

                if (this->upperSkelAnime.animation == &gPlayerAnim_link_uma_stop_muti) {
                    if (PlayerAnimation_OnFrame(&this->upperSkelAnime, 23.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyAll(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                this->upperSkelAnime.jointTable);
                } else {
                    if (PlayerAnimation_OnFrame(&this->upperSkelAnime, 10.0f)) {
                        Player_PlaySfx(this, NA_SE_IT_LASH);
                        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_LASH);
                    }

                    AnimationContext_SetCopyTrue(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                 this->upperSkelAnime.jointTable, D_80853410);
                }
            } else {
                PlayerAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_link_uma_stop_muti;
                } else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->rideHorseVar16 >= 2) && (this->rideHorseVar16 != 99)) {
                        anim = D_80854968[this->rideHorseVar16 - 2];
                    }
                }

                if (anim != NULL) {
                    PlayerAnimation_PlayOnce(play, &this->upperSkelAnime, anim);
                    this->actionVar8 = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_IN_FIRST_PERSON_MODE) {
            if ((Player_TryFirstPersonCameraMode(play, this) == CAM_MODE_NORMAL) ||
                CHECK_BTN_ANY(sControlInput->press.button, BTN_A) || Player_IsZTargeting(this)) {
                this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
                this->stateFlags1 &= ~PLAYER_STATE1_IN_FIRST_PERSON_MODE;
            } else {
                this->upperLimbRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                this->upperLimbRot.y += 5000;
                this->unk_6B0 = -5000;
            }
            return;
        }

        if ((this->csMode != PLAYER_CSMODE_NONE) ||
            (!Player_TryDismountingHorse(this, play) && !Player_SwapAction_TryItemCsFirstPerson(this, play))) {
            if (this->lockOnActor != NULL) {
                if (Player_IsAimingFpsItem(this) != 0) {
                    this->upperLimbRot.y = Player_LookAtTargetActor(this, 1) - this->actor.shape.rot.y;
                    this->upperLimbRot.y = CLAMP(this->upperLimbRot.y, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->upperLimbRot.y;
                    this->upperLimbRot.y += 5000;
                    this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_UPPER_ROT_Y;
                } else {
                    Player_LookAtTargetActor(this, 0);
                }
            } else {
                if (Player_IsAimingFpsItem(this) != 0) {
                    this->upperLimbRot.y = func_8084ABD8(play, this, 1, -5000) - this->actor.shape.rot.y;
                    this->upperLimbRot.y += 5000;
                    this->unk_6B0 = -5000;
                }
            }
        }
    }
}

static AnimSfxEntry sDismountHorseAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 0, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 10, NA_SE_PL_GET_OFF_HORSE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 25, NA_SE_PL_SLIPDOWN, STOP),
};

void Player_Action_DismountHorse(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE;
    Player_BounceOnHorse(this, 1.0f, 10.0f);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        EnHorse* rideActor = (EnHorse*)this->rideActor;

        Player_SetupIdle(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_RIDING_HORSE;
        this->actor.parent = NULL;
        AREG(6) = 0;

        if (Flags_GetEventChkInf(EVENTCHKINF_EPONA_OBTAINED) || (DREG(1) != 0)) {
            gSaveContext.horseData.pos.x = rideActor->actor.world.pos.x;
            gSaveContext.horseData.pos.y = rideActor->actor.world.pos.y;
            gSaveContext.horseData.pos.z = rideActor->actor.world.pos.z;
            gSaveContext.horseData.angle = rideActor->actor.shape.rot.y;
        }
    } else {
        Camera_ChangeSetting(Play_GetCamera(play, CAM_ID_MAIN), CAM_SET_NORMAL0);

        if (this->mountSide < 0) {
            sDismountHorseAnimSfx[0].flags = ANIMSFX_FLAGS(ANIMSFX_TYPE_FLOOR_LAND, 40, CONTINUE);
        } else {
            sDismountHorseAnimSfx[0].flags = ANIMSFX_FLAGS(ANIMSFX_TYPE_FLOOR_LAND, 29, CONTINUE);
        }
        Player_AnimSfx_Play(this, sDismountHorseAnimSfx);
    }
}

static AnimSfxEntry sSwimAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 0, NA_SE_PL_SWIM, STOP),
};

void Player_UpdateSwimMovementWithAnimSfx(Player* this, f32* linearVelocity, f32 inputVelocity, s16 inputYaw) {
    Player_UpdateSwimVelocityYaw(this, linearVelocity, inputVelocity, inputYaw);
    Player_AnimSfx_Play(this, sSwimAnimSfx);
}

void Player_SetupSwimMove(PlayState* play, Player* this, s16 inputYaw) {
    Player_SetAction(play, this, Player_Action_SwimMove, 0);
    this->actor.shape.rot.y = this->yaw = inputYaw;
    Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_SetupSwimZTarget(PlayState* play, Player* this) {
    Player_SetAction(play, this, Player_Action_SwimZTarget, 0);
    Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_Action_SwimIdle(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    Player_Anim_PlayLoopOnceFinished(play, this, &gPlayerAnim_link_swimer_swim_wait);
    Player_ApplyBuoyancy(this);

    if (Player_IsTalking(play)) {
        return;
    }

    if (Player_TrySwappingAction(play, this, sSwimSwapActionList, true)) {
        return;
    }

    if (Player_TrySwimDivingOrGetItem(play, this, sControlInput)) {
        return;
    }

    if (this->attentionMode != PLAYER_ATTENTIONMODE_C_UP) {
        this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
    }

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        inputVelocity = 0.0f;
        inputYaw = this->actor.shape.rot.y;

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            Player_Setup4_IdleAll(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType), play);
            Player_AnimSfx_PlayFloorLand(this);
        }
    } else {
        Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

        if (inputVelocity != 0.0f) {
            s16 yawDiff = this->actor.shape.rot.y - inputYaw;

            if ((ABS(yawDiff) > DEG_TO_BINANG(135.0f)) && !Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
                return;
            }

            if (Player_TryZTargeting(this)) {
                Player_SetupSwimZTarget(play, this);
            } else {
                Player_SetupSwimMove(play, this, inputYaw);
            }
        }
    }

    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, inputVelocity, inputYaw);
}

void Player_Action_SwimSpawn(Player* this, PlayState* play) {
    if (Player_SwapAction_TryItemCsFirstPerson(this, play)) {
        return;
    }

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    Player_UpdateSwimAnim(play, this, NULL, this->speedXZ);
    Player_ApplyBuoyancy(this);

    if (DECR(this->swimSpawnVar16) == 0) {
        Player_SetupSwimIdle(play, this);
    }
}

void Player_Action_SwimMove(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;
    s16 inputYawDiff;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    Player_UpdateSwimAnim(play, this, sControlInput, this->speedXZ);
    Player_ApplyBuoyancy(this);

    if (Player_TrySwappingAction(play, this, sSwimSwapActionList, true)) {
        return;
    }

    if (Player_TrySwimDivingOrGetItem(play, this, sControlInput)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    inputYawDiff = this->actor.shape.rot.y - inputYaw;
    if ((inputVelocity == 0.0f) || (ABS(inputYawDiff) > DEG_TO_BINANG(135.0f)) ||
        (this->currentBoots == PLAYER_BOOTS_IRON)) {
        Player_SetupSwimIdle(play, this);
    } else if (Player_TryZTargeting(this)) {
        Player_SetupSwimZTarget(play, this);
    }

    Player_UpdateSwimMovementWithAnimSfx(this, &this->speedXZ, inputVelocity, inputYaw);
}

s32 func_8084D980(PlayState* play, Player* this, f32* inputVelocity, s16* inputYaw) {
    PlayerAnimationHeader* anim;
    s16 yawDiff = this->yaw - *inputYaw;
    s32 moveDir;

    if (ABS(yawDiff) > DEG_TO_BINANG(135.0f)) {
        anim = &gPlayerAnim_link_swimer_swim_wait;

        if (Math_StepToF(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = *inputYaw;
        } else {
            *inputVelocity = 0.0f;
            *inputYaw = this->yaw;
        }
    } else {
        moveDir = Player_GetZParallelMoveDirection(this, inputVelocity, inputYaw, play);

        if (moveDir > 0) {
            anim = &gPlayerAnim_link_swimer_swim;
        } else if (moveDir < 0) {
            anim = &gPlayerAnim_link_swimer_back_swim;
        } else if ((yawDiff = this->actor.shape.rot.y - *inputYaw) > 0) {
            anim = &gPlayerAnim_link_swimer_Rside_swim;
        } else {
            anim = &gPlayerAnim_link_swimer_Lside_swim;
        }
    }

    if (anim != this->skelAnime.animation) {
        Player_Anim_PlayLoopWithLongMorph(play, this, anim);
        return true;
    }

    return false;
}

void Player_Action_SwimZTarget(Player* this, PlayState* play) {
    f32 inputVelocity;
    s16 inputYaw;

    Player_UpdateSwimAnim(play, this, sControlInput, this->speedXZ);
    Player_ApplyBuoyancy(this);

    if (Player_TrySwappingAction(play, this, sSwimSwapActionList, true)) {
        return;
    }

    if (Player_TrySwimDivingOrGetItem(play, this, sControlInput)) {
        return;
    }

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);

    if (inputVelocity == 0.0f) {
        Player_SetupSwimIdle(play, this);
    } else if (!Player_TryZTargeting(this)) {
        Player_SetupSwimMove(play, this, inputYaw);
    } else {
        func_8084D980(play, this, &inputVelocity, &inputYaw);
    }

    Player_UpdateSwimMovementWithAnimSfx(this, &this->speedXZ, inputVelocity, inputYaw);
}

void func_8084DBC4(PlayState* play, Player* this, f32 arg2) {
    f32 inputVelocity;
    s16 inputYaw;

    Player_GetInputVelocityAndYaw(this, &inputVelocity, &inputYaw, 0.0f, play);
    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, inputVelocity * 0.5f, inputYaw);
    Player_UpdateSwimVelocityYaw(this, &this->actor.velocity.y, arg2, this->yaw);
}

void Player_Action_SwimDive(Player* this, PlayState* play) {
    f32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;
    this->actor.gravity = 0.0f;
    Player_TryUpperAction(this, play);

    if (Player_SwapAction_TryItemCsFirstPerson(this, play)) {
        return;
    }

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        Player_SetupSwimIdle(play, this);
        return;
    }

    if (this->actionVar8 == 0) {
        if (this->actionVar16 == 0) {
            if (PlayerAnimation_Update(play, &this->skelAnime) ||
                ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(sControlInput->cur.button, BTN_A))) {
                func_8083D330(play, this);
            } else if (PlayerAnimation_OnFrame(&this->skelAnime, 20.0f) != 0) {
                this->actor.velocity.y = -2.0f;
            }

            Player_StepHorizontalSpeedToZero(this);
            return;
        }

        Player_UpdateSwimAnim(play, this, sControlInput, this->actor.velocity.y);
        this->shapePitchOffset = 16000;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A) && !Player_SwapAction_TryGetItem(this, play) &&
            !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->actor.depthInWater < D_80854784[CUR_UPG_VALUE(UPG_SCALE)])) {
            func_8084DBC4(play, this, -2.0f);
        } else {
            this->actionVar8++;
            Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
        }
        return;
    }

    if (this->actionVar8 == 1) {
        PlayerAnimation_Update(play, &this->skelAnime);
        Player_ApplyBuoyancy(this);

        if (this->shapePitchOffset < 10000) {
            this->actionVar8++;
            this->actionVar16 = this->actor.depthInWater;
            Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim);
        }
        return;
    }

    if (Player_TrySwimDivingOrGetItem(play, this, sControlInput)) {
        return;
    }

    sp2C = (this->actionVar16 * 0.018f) + 4.0f;

    if (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR) {
        sControlInput = NULL;
    }

    Player_UpdateSwimAnim(play, this, sControlInput, fabsf(this->actor.velocity.y));
    Math_ScaledStepToS(&this->shapePitchOffset, -10000, 800);

    if (sp2C > 8.0f) {
        sp2C = 8.0f;
    }

    func_8084DBC4(play, this, sp2C);
}

void func_8084DF6C(PlayState* play, Player* this) {
    this->unk_862 = 0;
    this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR);
    this->getItemId = GI_NONE;
    Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
}

void func_8084DFAC(PlayState* play, Player* this) {
    func_8084DF6C(play, this);
    Player_Anim_ResetModelRotY(this);
    Player_SetupIdle(this, play);
    this->yaw = this->actor.shape.rot.y;
}

s32 func_8084DFF4(PlayState* play, Player* this) {
    GetItemEntry* giEntry;
    s32 temp1;
    s32 temp2;

    if (this->getItemId == GI_NONE) {
        return 1;
    }

    if (this->actionVar8 == 0) {
        giEntry = &sGetItemTable[this->getItemId - 1];
        this->actionVar8 = 1;

        Message_StartTextbox(play, giEntry->textId, &this->actor);
        Item_Give(play, giEntry->itemId);

        if (((this->getItemId >= GI_RUPEE_GREEN) && (this->getItemId <= GI_RUPEE_RED)) ||
            ((this->getItemId >= GI_RUPEE_PURPLE) && (this->getItemId <= GI_RUPEE_GOLD)) ||
            ((this->getItemId >= GI_RUPEE_GREEN_LOSE) && (this->getItemId <= GI_RUPEE_PURPLE_LOSE)) ||
            (this->getItemId == GI_RECOVERY_HEART)) {
            Audio_PlaySfxGeneral(NA_SE_SY_GET_BOXITEM, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        } else {
            if ((this->getItemId == GI_HEART_CONTAINER_2) || (this->getItemId == GI_HEART_CONTAINER) ||
                ((this->getItemId == GI_HEART_PIECE) &&
                 ((gSaveContext.inventory.questItems & 0xF0000000) == (4 << QUEST_HEART_PIECE_COUNT)))) {
                temp1 = NA_BGM_HEART_GET | 0x900;
            } else {
                temp1 = temp2 = (this->getItemId == GI_HEART_PIECE) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
            }
            Audio_PlayFanfare(temp1);
        }
    } else {
        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
            if (this->getItemId == GI_SILVER_GAUNTLETS) {
                play->nextEntranceIndex = ENTR_DESERT_COLOSSUS_0;
                play->transitionTrigger = TRANS_TRIGGER_START;
                gSaveContext.nextCutsceneIndex = 0xFFF1;
                play->transitionType = TRANS_TYPE_SANDSTORM_END;
                this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
                Player_SetupCutsceneWithCsMode(play, NULL, PLAYER_CSMODE_8);
            }
            this->getItemId = GI_NONE;
        }
    }

    return 0;
}

void Player_Action_SwimGetItem(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) || func_8084DFF4(play, this)) {
            func_8084DF6C(play, this);
            Player_SetupSwimIdle(play, this);
            Player_ResetSubCam(play, this);
        }
    } else {
        if ((this->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) && PlayerAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            func_808332F4(this, play);
            Player_ResetSubCam(play, this);
            Player_TurnAroundCamera(play, 8);
        } else if (PlayerAnimation_OnFrame(&this->skelAnime, 5.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    Player_ApplyBuoyancy(this);
    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void Player_Action_SwimDamage(Player* this, PlayState* play) {
    Player_ApplyBuoyancy(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_SetupSwimIdle(play, this);
    }

    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void Player_Action_SwimDrown(Player* this, PlayState* play) {
    Player_ApplyBuoyancy(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_80843AE8(play, this);
    }

    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

static s16 sWarpSongEntrances[] = {
    ENTR_SACRED_FOREST_MEADOW_2,
    ENTR_DEATH_MOUNTAIN_CRATER_4,
    ENTR_LAKE_HYLIA_8,
    ENTR_DESERT_COLOSSUS_5,
    ENTR_GRAVEYARD_7,
    ENTR_TEMPLE_OF_TIME_7,
};

void Player_Action_PlayOcarina(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_normal_okarina_swing);
        this->playOcarinaVar16 = true;
        if (this->stateFlags2 & (PLAYER_STATE2_OCARINA_START_OVERRIDE | PLAYER_STATE2_OCARINA_ON_FOR_ACTOR)) {
            this->stateFlags2 |= PLAYER_STATE2_OCARINA_START_READY;
        } else {
            Message_StartOcarina(play, OCARINA_ACTION_FREE_PLAY);
        }
        return;
    }

    if (!this->playOcarinaVar16) {
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));

        if ((this->talkActor != NULL) && (this->talkActor == this->ocarinaActor)) {
            Player_SetupTalk(play, this->talkActor);
        } else if (this->naviTextId < 0) {
            this->talkActor = this->naviActor;
            this->naviActor->textId = -this->naviTextId;
            Player_SetupTalk(play, this->talkActor);
        } else if (!Player_SwapAction_TryItemCsFirstPerson(this, play)) {
            Player_Setup4_IdleAll(this, &gPlayerAnim_link_normal_okarina_end, play);
        }

        this->stateFlags2 &= ~(PLAYER_STATE2_OCARINA_START_OVERRIDE | PLAYER_STATE2_OCARINA_START_READY |
                               PLAYER_STATE2_OCARINA_ON_FOR_ACTOR);
        this->ocarinaActor = NULL;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = sWarpSongEntrances[play->msgCtx.lastPlayedSong];
        gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x5FF;
        gSaveContext.respawn[RESPAWN_MODE_RETURN].data = play->msgCtx.lastPlayedSong;

        this->csMode = PLAYER_CSMODE_NONE;
        this->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;

        Player_SetupCutsceneWithCsMode(play, NULL, PLAYER_CSMODE_8);
        play->mainCamera.stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;

        this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
        this->stateFlags2 |= PLAYER_STATE2_OCARINA_ON;

        if (Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, DEMOKANKYO_WARP_OUT) ==
            NULL) {
            Environment_WarpSongLeave(play);
        }

        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
    }
}

void Player_Action_ThrowDekuNut(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Setup4_IdleAll(this, &gPlayerAnim_link_normal_light_bom_end, play);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 3.0f)) {
        Inventory_ChangeAmmo(ITEM_DEKU_NUT, -1);
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
                    this->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 4000,
                    this->actor.shape.rot.y, 0, ARROW_NUT);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
    }

    Player_StepHorizontalSpeedToZero(this);
}

static AnimSfxEntry D_808549E0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_JUMP, 87, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 87, NA_SE_VO_LI_CLIMB_END, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 69, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 123, NA_SE_NONE, STOP),
};

void Player_Action_GetItem(Player* this, PlayState* play) {
    s32 cond;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->getItemVar16 != 0) {
            if (this->getItemVar16 >= 2) {
                this->getItemVar16--;
            }

            if (func_8084DFF4(play, this) && (this->getItemVar16 == 1)) {
                cond = ((this->talkActor != NULL) && (this->exchangeItemId < 0)) ||
                       (this->stateFlags3 & PLAYER_STATE3_OCARINA_AFTER_TEXTBOX);

                if (cond || (gSaveContext.healthAccumulator == 0)) {
                    if (cond) {
                        func_8084DF6C(play, this);
                        this->exchangeItemId = EXCH_ITEM_NONE;

                        if (!Player_TryOcarinaAfterTextbox(play, this)) {
                            Player_SetupTalk(play, this->talkActor);
                        }
                    } else {
                        func_8084DFAC(play, this);
                    }
                }
            }
        } else {
            Player_Anim_ResetMove(this);

            if (this->getItemId == GI_ICE_TRAP) {
                this->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_HOLDING_ACTOR);

                if (this->getItemId != GI_ICE_TRAP) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                                this->actor.world.pos.y + 100.0f, this->actor.world.pos.z, 0, 0, 0, 0);
                    Player_SetupIdle(this, play);
                } else {
                    this->actor.colChkInfo.damage = 0;
                    Player_ApplyDamage(play, this, PLAYER_DMGREACTION_FROZEN, 0.0f, 0.0f, 0, 20);
                }
                return;
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_box_kick) {
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemB);
            } else {
                Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_get_itemA);
            }

            this->getItemVar16 = 2;
            Player_TurnAroundCamera(play, 9);
        }
    } else {
        if (this->getItemVar16 == 0) {
            if (!LINK_IS_ADULT) {
                Player_AnimSfx_Play(this, D_808549E0);
            }
            return;
        }

        if (this->skelAnime.animation == &gPlayerAnim_link_demo_get_itemB) {
            Math_ScaledStepToS(&this->actor.shape.rot.y, Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000, 4000);
        }

        if (PlayerAnimation_OnFrame(&this->skelAnime, 21.0f)) {
            func_808332F4(this, play);
        }
    }
}

static AnimSfxEntry D_808549F0[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 60, NA_SE_IT_MASTER_SWORD_SWING, STOP),
};

void func_8084E988(Player* this) {
    Player_AnimSfx_Play(this, D_808549F0);
}

static AnimSfxEntry D_808549F4[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 5, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 15, NA_SE_NONE, STOP),
};

void Player_Action_SpawnFromAgeSwap(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar8 == 0) {
            if (DECR(this->spawnFromAgeSwapVar16) == 0) {
                this->actionVar8 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            Player_SetupIdle(this, play);
        }
    } else {
        if (LINK_IS_ADULT && PlayerAnimation_OnFrame(&this->skelAnime, 158.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_N);
            return;
        }

        if (!LINK_IS_ADULT) {
            Player_AnimSfx_Play(this, D_808549F4);
        } else {
            func_8084E988(this);
        }
    }
}

#define BOTTLE_DRINK_EFFECT_HEAL_STRONG (1 << 0)
#define BOTTLE_DRINK_EFFECT_FILL_MAGIC (1 << 1)
#define BOTTLE_DRINK_EFFECT_HEAL_WEAK (1 << 2)

static u8 sBottleDrinkEffects[] = {
    BOTTLE_DRINK_EFFECT_HEAL_STRONG,                                  // PLAYER_IA_BOTTLE_POTION_RED
    BOTTLE_DRINK_EFFECT_HEAL_STRONG | BOTTLE_DRINK_EFFECT_FILL_MAGIC, // PLAYER_IA_BOTTLE_POTION_BLUE
    BOTTLE_DRINK_EFFECT_FILL_MAGIC,                                   // PLAYER_IA_BOTTLE_POTION_GREEN
    BOTTLE_DRINK_EFFECT_HEAL_WEAK,                                    // PLAYER_IA_BOTTLE_MILK
    BOTTLE_DRINK_EFFECT_HEAL_WEAK,                                    // PLAYER_IA_BOTTLE_MILK_HALF
};

void Player_Action_DrinkFromBottle(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar16 == 0) {
            if (this->itemAction == PLAYER_IA_BOTTLE_POE) {
                s32 health = Rand_S16Offset(-1, 3);

                if (health == 0) {
                    health = 3;
                }

                if ((health < 0) && (gSaveContext.health <= 0x10)) {
                    health = 3;
                }

                if (health < 0) {
                    Health_ChangeBy(play, -0x10);
                } else {
                    gSaveContext.healthAccumulator = health * 0x10;
                }
            } else {
                s32 drinkEffects = sBottleDrinkEffects[this->itemAction - PLAYER_IA_BOTTLE_POTION_RED];

                if (drinkEffects & BOTTLE_DRINK_EFFECT_HEAL_STRONG) {
                    gSaveContext.healthAccumulator = 0x140;
                }

                if (drinkEffects & BOTTLE_DRINK_EFFECT_FILL_MAGIC) {
                    Magic_Fill(play);
                }

                if (drinkEffects & BOTTLE_DRINK_EFFECT_HEAL_WEAK) {
                    gSaveContext.healthAccumulator = 0x50;
                }
            }

            Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_link_bottle_drink_demo_wait);
            this->actionVar16 = 1;
            return;
        }

        Player_SetupIdle(this, play);
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
    } else if (this->actionVar16 == 1) {
        if ((gSaveContext.healthAccumulator == 0) && (gSaveContext.magicState != MAGIC_STATE_FILL)) {
            Player_Anim_PlayOnceAdjustedWithMorph(play, this, &gPlayerAnim_link_bottle_drink_demo_end);
            this->actionVar16 = 2;
            Player_UpdateBottleHeld(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        }
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
    } else if ((this->actionVar16 == 2) && PlayerAnimation_OnFrame(&this->skelAnime, 29.0f)) {
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_BREATH_DRINK);
    }
}

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ u8 itemId;
    /* 0x03 */ u8 itemAction;
    /* 0x04 */ u8 textId;
} BottleCatchInfo; // size = 0x06

static BottleCatchInfo sBottleCatchInfos[] = {
    { ACTOR_EN_ELF, ITEM_BOTTLE_FAIRY, PLAYER_IA_BOTTLE_FAIRY, 0x46 },
    { ACTOR_EN_FISH, ITEM_BOTTLE_FISH, PLAYER_IA_BOTTLE_FISH, 0x47 },
    { ACTOR_EN_ICE_HONO, ITEM_BOTTLE_BLUE_FIRE, PLAYER_IA_BOTTLE_FIRE, 0x5D },
    { ACTOR_EN_INSECT, ITEM_BOTTLE_BUG, PLAYER_IA_BOTTLE_BUG, 0x7A },
};

void Player_Action_SwingBottle(Player* this, PlayState* play) {
    BottleSwingAnimInfo* sp24;
    BottleCatchInfo* catchInfo;
    s32 temp;
    s32 i;

    sp24 = &sBottleSwingAnims[this->actionVar16];
    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar8 != 0) {
            if (this->actionVar16 == 0) {
                Message_StartTextbox(play, sBottleCatchInfos[this->actionVar8 - 1].textId, &this->actor);
                Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
                this->actionVar16 = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->actionVar8 = 0;
                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
            }
        } else {
            Player_SetupIdle(this, play);
        }
    } else {
        if (this->actionVar8 == 0) {
            temp = this->skelAnime.curFrame - sp24->unk_08;

            if (temp >= 0) {
                if (sp24->unk_09 >= temp) {
                    if (this->actionVar16 != 0) {
                        if (temp == 0) {
                            Player_PlaySfx(this, NA_SE_IT_SCOOP_UP_WATER);
                        }
                    }

                    if (this->interactRangeActor != NULL) {
                        catchInfo = &sBottleCatchInfos[0];
                        for (i = 0; i < ARRAY_COUNT(sBottleCatchInfos); i++, catchInfo++) {
                            if (this->interactRangeActor->id == catchInfo->actorId) {
                                break;
                            }
                        }

                        if (i < ARRAY_COUNT(sBottleCatchInfos)) {
                            this->actionVar8 = i + 1;
                            this->actionVar16 = 0;
                            this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                            this->interactRangeActor->parent = &this->actor;
                            Player_UpdateBottleHeld(play, this, catchInfo->itemId, ABS(catchInfo->itemAction));
                            Player_Anim_PlayOnceAdjusted(play, this, sp24->unk_04);
                            Player_TurnAroundCamera(play, 4);
                        }
                    }
                }
            }
        }
    }

    //! @bug If the animation is changed at any point above (such as by Player_SetupIdle() or
    //! Player_Anim_PlayOnceAdjusted()), it will change the curFrame to 0. This causes this flag to be set for one
    //! frame, at a time when it does not look like Player is swinging the bottle.
    if (this->skelAnime.curFrame <= 7.0f) {
        this->stateFlags1 |= PLAYER_STATE1_SWINGING_BOTTLE;
    }
}

static Vec3f D_80854A1C = { 0.0f, 0.0f, 5.0f };

void Player_Action_ReleaseFairyFromBottle(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_SetupIdle(this, play);
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 37.0f)) {
        Player_SpawnFairy(play, this, &this->leftHandPos, &D_80854A1C, FAIRY_REVIVE_BOTTLE);
        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        Player_PlaySfx(this, NA_SE_EV_BOTTLE_CAP_OPEN);
        Player_PlaySfx(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    } else if (PlayerAnimation_OnFrame(&this->skelAnime, 47.0f)) {
        gSaveContext.healthAccumulator = 0x140;
    }
}

typedef struct {
    /* 0x0 */ s16 actorId;
    /* 0x2 */ s16 actorParams;
} BottleDropInfo; // size = 0x4

void Player_Action_DropItemFromBottle(Player* this, PlayState* play) {
    static BottleDropInfo sBottleDropInfo[] = {
        { ACTOR_EN_FISH, FISH_DROPPED },
        { ACTOR_EN_ICE_HONO, 0 },
        { ACTOR_EN_INSECT, INSECT_TYPE_FIRST_DROPPED },
    };
    static AnimSfxEntry sBottleDropAnimSfx[] = {
        ANIMSFX(ANIMSFX_TYPE_VOICE, 38, NA_SE_VO_LI_AUTO_JUMP, CONTINUE),
        ANIMSFX(ANIMSFX_TYPE_GENERAL, 40, NA_SE_EV_BOTTLE_CAP_OPEN, STOP),
    };

    Player_StepHorizontalSpeedToZero(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_SetupIdle(this, play);
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
        return;
    }

    if (PlayerAnimation_OnFrame(&this->skelAnime, 76.0f)) {
        BottleDropInfo* dropInfo = &sBottleDropInfo[this->itemAction - PLAYER_IA_BOTTLE_FISH];

        Actor_Spawn(&play->actorCtx, play, dropInfo->actorId,
                    (Math_SinS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.x, this->leftHandPos.y,
                    (Math_CosS(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.z, 0x4000, this->actor.shape.rot.y,
                    0, dropInfo->actorParams);

        Player_UpdateBottleHeld(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        return;
    }

    Player_AnimSfx_Play(this, sBottleDropAnimSfx);
}

static AnimSfxEntry sExchangeItemAnimSfx[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 30, NA_SE_PL_PUT_OUT_ITEM, STOP),
};

void Player_Action_ExchangeItem(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar16 < 0) {
            Player_SetupIdle(this, play);
        } else if (this->exchangeItemId == EXCH_ITEM_NONE) {
            Actor* talkActor = this->talkActor;

            this->unk_862 = 0;
            if (talkActor->textId != 0xFFFF) {
                this->actor.flags |= ACTOR_FLAG_TALK_REQUESTED;
            }

            Player_SetupTalk(play, talkActor);
        } else {
            GetItemEntry* giEntry = &sGetItemTable[D_80854528[this->exchangeItemId - 1] - 1];

            if (this->itemAction >= PLAYER_IA_ZELDAS_LETTER) {
                this->unk_862 = ABS(giEntry->gi);
            }

            if (this->actionVar16 == 0) {
                Message_StartTextbox(play, this->actor.textId, &this->actor);

                if ((this->itemAction == PLAYER_IA_CHICKEN) || (this->itemAction == PLAYER_IA_POCKET_CUCCO)) {
                    Player_PlaySfx(this, NA_SE_EV_CHICKEN_CRY_M);
                }

                this->actionVar16 = 1;
            } else if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->actor.flags &= ~ACTOR_FLAG_TALK_REQUESTED;
                this->unk_862 = 0;

                if (this->actionVar8 == 1) {
                    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_bottle_read_end);
                    this->actionVar16 = -1;
                } else {
                    Player_SetupIdle(this, play);
                }

                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
            }
        }
    } else if (this->actionVar16 >= 0) {
        Player_AnimSfx_Play(this, sExchangeItemAnimSfx);
    }

    if ((this->actionVar8 == 0) && (this->lockOnActor != NULL)) {
        this->yaw = this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void Player_Action_Grabbed(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_normal_re_dead_attack_wait);
    }

    if (Player_MashTimerThresholdExceeded(this, 0, 100)) {
        Player_Setup1_IdleAll(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
    }
}

void Player_Action_SlipOnSlope(Player* this, PlayState* play) {
    CollisionPoly* floorPoly;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    s16 downwardSlopeYaw;
    s16 sp44;
    Vec3f slopeNormal;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON | PLAYER_STATE2_NO_YAW_UPDATE;
    PlayerAnimation_Update(play, &this->skelAnime);
    func_8084269C(play, this);
    func_800F4138(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speed);

    if (Player_SwapAction_TryItemCsFirstPerson(this, play)) {
        return;
    }

    floorPoly = this->actor.floorPoly;

    if (floorPoly == NULL) {
        Player_Setup_Midair(this, play);
        return;
    }

    Player_GetSlopeDirection(floorPoly, &slopeNormal, &downwardSlopeYaw);

    sp44 = downwardSlopeYaw;
    if (this->actionVar8 != 0) {
        sp44 = downwardSlopeYaw + 0x8000;
    }

    if (this->speedXZ < 0) {
        downwardSlopeYaw += 0x8000;
    }

    sp50 = (1.0f - slopeNormal.y) * 40.0f;
    sp50 = CLAMP(sp50, 0, 10.0f);
    sp4C = (sp50 * sp50) * 0.015f;
    sp48 = slopeNormal.y * 0.01f;

    if (SurfaceType_GetFloorEffect(&play->colCtx, floorPoly, this->actor.floorBgId) != FLOOR_EFFECT_1) {
        sp50 = 0;
        sp48 = slopeNormal.y * 10.0f;
    }

    if (sp4C < 1.0f) {
        sp4C = 1.0f;
    }

    if (Math_AsymStepToF(&this->speedXZ, sp50, sp4C, sp48) && (sp50 == 0)) {
        PlayerAnimationHeader* anim;

        if (this->actionVar8 == 0) {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_down_slope_slip_end, this->modelAnimType);
        } else {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_up_slope_slip_end, this->modelAnimType);
        }
        Player_Setup4_IdleAll(this, anim, play);
    }

    Math_SmoothStepToS(&this->yaw, downwardSlopeYaw, 10, 4000, 800);
    Math_ScaledStepToS(&this->actor.shape.rot.y, sp44, 2000);
}

void Player_Action_StartCutsceneDelayed(Player* this, PlayState* play) {
    if ((DECR(this->startCutsceneDelayedVar16) == 0)) {
        if (Player_TryStartingCutscene(play, this)) {
            Player_CsAction_DrawPlayer(play, this, NULL);
            Player_SetAction(play, this, Player_Action_StartCutscene, 0);
            Player_Action_StartCutscene(this, play);
        }
    }
}

void Player_Action_SpawnFromWarpSong(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_StartCutsceneDelayed, 0);
    this->startCutsceneDelayedVar16 = 40;
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, DEMOKANKYO_WARP_IN);
}

void Player_Action_SpawnFromBlueWarp(Player* this, PlayState* play) {
    s32 pad;

    if ((this->spawnFromBlueWarpVar8 != 0) && (play->csCtx.curFrame <= 304)) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    } else if (sYDistToFloor < 150.0f) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            if (this->spawnFromBlueWarpVar16 == 0) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    Player_AnimSfx_PlayFloorLand(this);
                    this->spawnFromBlueWarpVar16 = 1;
                }
            } else {
                if ((play->sceneId == SCENE_KOKIRI_FOREST) && Player_TryStartingCutscene(play, this)) {
                    return;
                }
                Player_SetupIdleWithMorph(this, play);
            }
        }
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if ((play->sceneId == SCENE_CHAMBER_OF_THE_SAGES) && Player_TryStartingCutscene(play, this)) {
        return;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.playerCue != NULL)) {
        f32 sp28 = this->actor.world.pos.y;

        Player_CsAction_SetStartPosAndYaw(play, this, play->csCtx.playerCue);
        this->actor.world.pos.y = sp28;
    }
}

void Player_Action_EnterGrotto(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);

    if ((this->enterGrottoDelayTransTimer++ > 8) && (play->transitionTrigger == TRANS_TRIGGER_OFF)) {
        if (this->actionVar8 != 0) {
            if (play->sceneId == SCENE_ICE_CAVERN) {
                Play_TriggerRespawn(play);
                play->nextEntranceIndex = ENTR_ICE_CAVERN_0;
            } else if (this->actionVar8 < 0) {
                Play_TriggerRespawn(play);
            } else {
                Play_TriggerVoidOut(play);
            }

            play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
            Audio_PlaySfx(NA_SE_OC_ABYSS);
        } else {
            play->transitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = 0xFF;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void Player_Action_SpawnFromDoor(Player* this, PlayState* play) {
    Player_SwapAction_TryOpeningDoor(this, play);
}

void Player_Action_SpawnFromGrotto(Player* this, PlayState* play) {
    this->actor.gravity = -1.0f;

    PlayerAnimation_Update(play, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        Player_Setup_Midair(this, play);
    } else if (this->actor.velocity.y < 6.0f) {
        Math_StepToF(&this->speedXZ, 3.0f, 0.5f);
    }
}

void Player_Action_PlayShootingGallery(Player* this, PlayState* play) {
    this->attentionMode = PLAYER_ATTENTIONMODE_AIMING;

    Player_TryFirstPersonCameraMode(play, this);
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_TryUpperAction(this, play);

    this->upperLimbRot.y = func_8084ABD8(play, this, 1, 0) - this->actor.shape.rot.y;
    this->rotOverrideFlags |= PLAYER_ROT_OVERRIDE_UPPER_ROT_Y;

    if (play->shootingGalleryStatus < 0) {
        play->shootingGalleryStatus++;
        if (play->shootingGalleryStatus == 0) {
            func_8083C148(this, play);
        }
    }
}

void Player_Action_FrozenInIce(Player* this, PlayState* play) {
    if (this->frozenInIceVar8 >= 0) {
        if (this->frozenInIceVar8 < 6) {
            this->frozenInIceVar8++;
        }

        if (Player_MashTimerThresholdExceeded(this, 1, 100)) {
            this->frozenInIceVar8 = -1;
            EffectSsIcePiece_SpawnBurst(play, &this->actor.world.pos, this->actor.scale.x);
            Player_PlaySfx(this, NA_SE_PL_ICE_BROKEN);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_FROZEN_IN_ICE;
        }

        if ((play->gameplayFrames % 4) == 0) {
            Player_InflictDamage(play, -1);
        }
    } else {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Setup1_IdleAll(this, play);
            Player_SetInvincibilityTimerWithoutDamageFlash(this, -20);
        }
    }
}

void Player_Action_ElectricShock(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_GiveOnceSecondInvincibility(this);

    if (((this->actionVar16 % 25) != 0) || Player_InflictDamageImpl(play, this, -1)) {
        if (DECR(this->actionVar16) == 0) {
            Player_Setup1_IdleAll(this, play);
        }
    }

    this->shockTimer = 40;
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG + this->ageProperties->voiceSfxIdOffset);
}

s32 Player_TryDebugNoClip(Player* this, PlayState* play) {
    sControlInput = &play->state.input[0];

    if ((CHECK_BTN_ALL(sControlInput->cur.button, BTN_A | BTN_L | BTN_R) &&
         CHECK_BTN_ALL(sControlInput->press.button, BTN_B)) ||
        (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DRIGHT))) {

        D_808535D0 ^= 1;

        if (D_808535D0) {
            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_Z_AIM);
        }
    }

    if (D_808535D0) {
        f32 speed;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        DebugCamera_ScreenText(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sControlInput->cur.button, BTN_DUP | BTN_DLEFT | BTN_DDOWN | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + 0x4000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - 0x4000;
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        Player_SetHorizontalSpeedToZero(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.x = 0.0f;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DLEFT)) {
            Flags_SetTempClear(play, play->roomCtx.curRoom.num);
        }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return false;
    }

    return true;
}

void func_8084FF7C(Player* this) {
    this->spinAttackTimer += this->dekuStickLength;
    this->dekuStickLength -= this->spinAttackTimer * 5.0f;
    this->dekuStickLength *= 0.3f;

    if (ABS(this->dekuStickLength) < 0.00001f) {
        this->dekuStickLength = 0.0f;
        if (ABS(this->spinAttackTimer) < 0.00001f) {
            this->spinAttackTimer = 0.0f;
        }
    }
}

/**
 * Updates the Bunny Hood's floppy ears' rotation and velocity.
 */
void Player_UpdateBunnyEars(Player* this) {
    Vec3s force;
    s16 angle;

    // Damping: decay by 1/8 the previous value each frame
    sBunnyEarKinematics.angVel.x -= sBunnyEarKinematics.angVel.x >> 3;
    sBunnyEarKinematics.angVel.y -= sBunnyEarKinematics.angVel.y >> 3;

    // Elastic restorative force
    sBunnyEarKinematics.angVel.x += -sBunnyEarKinematics.rot.x >> 2;
    sBunnyEarKinematics.angVel.y += -sBunnyEarKinematics.rot.y >> 2;

    // Forcing from motion relative to shape frame
    angle = this->actor.world.rot.y - this->actor.shape.rot.y;
    force.x = (s32)(this->actor.speed * -200.0f * Math_CosS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;
    force.y = (s32)(this->actor.speed * 100.0f * Math_SinS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;

    sBunnyEarKinematics.angVel.x += force.x >> 2;
    sBunnyEarKinematics.angVel.y += force.y >> 2;

    // Clamp both angular velocities to [-6000, 6000]
    if (sBunnyEarKinematics.angVel.x > 6000) {
        sBunnyEarKinematics.angVel.x = 6000;
    } else if (sBunnyEarKinematics.angVel.x < -6000) {
        sBunnyEarKinematics.angVel.x = -6000;
    }
    if (sBunnyEarKinematics.angVel.y > 6000) {
        sBunnyEarKinematics.angVel.y = 6000;
    } else if (sBunnyEarKinematics.angVel.y < -6000) {
        sBunnyEarKinematics.angVel.y = -6000;
    }

    // Add angular velocity to rotations
    sBunnyEarKinematics.rot.x += sBunnyEarKinematics.angVel.x;
    sBunnyEarKinematics.rot.y += sBunnyEarKinematics.angVel.y;

    // swivel ears outwards if bending backwards
    if (sBunnyEarKinematics.rot.x < 0) {
        sBunnyEarKinematics.rot.z = sBunnyEarKinematics.rot.x >> 1;
    } else {
        sBunnyEarKinematics.rot.z = 0;
    }
}

s32 Player_SwapAction_TryAttackBottleFishingRod(Player* this, PlayState* play) {
    if (!Player_TrySwingingBottleAndCastingFishingRod(play, this)) {
        if (func_8083BB20(this)) {
            s32 sp24 = func_80837818(this);

            Player_SetupAttack(play, this, sp24);

            if (sp24 >= PLAYER_MWA_SPIN_ATTACK_1H) {
                this->stateFlags2 |= PLAYER_STATE2_RELEASING_SPIN_ATTACK;
                Player_SpawnSpinAttack(play, this, 0);
                return true;
            }
        } else {
            return false;
        }
    }

    return true;
}

static Vec3f D_80854A40 = { 0.0f, 40.0f, 45.0f };

void Player_Action_Attack(Player* this, PlayState* play) {
    struct_80854190* sp44 = &D_80854190[this->meleeWeaponAnimation];

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    if (Player_ProcessAttackCollision(play, this)) {
        return;
    }

    func_8084285C(this, 0.0f, sp44->unk_0C, sp44->unk_0D);

    if ((this->stateFlags2 & PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK) &&
        (this->heldItemAction != PLAYER_IA_HAMMER) && PlayerAnimation_OnFrame(&this->skelAnime, 0.0f)) {
        this->speedXZ = 15.0f;
        this->stateFlags2 &= ~PLAYER_STATE2_ENABLE_FORWARD_SLIDE_FROM_ATTACK;
    }

    if (this->speedXZ > 12.0f) {
        func_8084269C(play, this);
    }

    Math_StepToF(&this->speedXZ, 0.0f, 5.0f);
    func_8083C50C(this);

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (!Player_SwapAction_TryAttackBottleFishingRod(this, play)) {
            u8 sp43 = this->skelAnime.moveFlags;
            PlayerAnimationHeader* sp3C;

            if (Player_IsEnemyLockOn(this)) {
                sp3C = sp44->unk_08;
            } else {
                sp3C = sp44->unk_04;
            }

            Player_ResetAttack(this);
            this->skelAnime.moveFlags = 0;

            if ((sp3C == &gPlayerAnim_link_fighter_Lpower_jump_kiru_end) &&
                (this->modelAnimType != PLAYER_ANIMTYPE_3)) {
                sp3C = &gPlayerAnim_link_fighter_power_jump_kiru_end;
            }

            Player_Setup4_IdleAll(this, sp3C, play);

            this->skelAnime.moveFlags = sp43;
            this->stateFlags3 |= PLAYER_STATE3_ENDING_MELEE_ATTACK;
        }
    } else if (this->heldItemAction == PLAYER_IA_HAMMER) {
        if ((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) ||
            (this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {
            static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
            Vec3f shockwavePos;
            f32 sp2C;

            shockwavePos.y = Player_PosVsFloorLineTest(play, this, &D_80854A40, &shockwavePos);
            sp2C = this->actor.world.pos.y - shockwavePos.y;

            Math_ScaledStepToS(&this->actor.focus.rot.x, Math_Atan2S(45.0f, sp2C), 800);
            Player_UpdateLookAngles(this, true);

            if ((((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) &&
                  PlayerAnimation_OnFrame(&this->skelAnime, 7.0f)) ||
                 ((this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH) &&
                  PlayerAnimation_OnFrame(&this->skelAnime, 2.0f))) &&
                (sp2C > -40.0f) && (sp2C < 40.0f)) {
                Player_RequestQuakeAndRumble(play, this);
                EffectSsBlast_SpawnWhiteShockwave(play, &shockwavePos, &zeroVec, &zeroVec);
            }
        }
    }
}

void Player_Action_Recoil(Player* this, PlayState* play) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_StepHorizontalSpeedToZero(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        Player_Setup2_IdleAll(this, play);
    }
}

void Player_Action_FaroresWindChoice(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    PlayerAnimation_Update(play, &this->skelAnime);
    Player_TryUpperAction(this, play);

    if (this->actionVar16 == 0) {
        Message_StartTextbox(play, 0x3B, &this->actor);
        this->actionVar16 = 1;
        return;
    }

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

        if (play->msgCtx.choiceIndex == 0) {
            gSaveContext.respawnFlag = 3;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_TOP].entranceIndex;
            play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
            Interface_SetSubTimerToFinalSecond(play);
            return;
        }

        if (play->msgCtx.choiceIndex == 1) {
            gSaveContext.respawn[RESPAWN_MODE_TOP].data = -respawnData;
            gSaveContext.fw.set = 0;
            Audio_PlaySfx_AtPos(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_VANISH);
        }

        Player_SetupIdleWithMorph(this, play);
        Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
    }
}

void Player_Action_SpawnFromFaroresWind(Player* this, PlayState* play) {
    s32 respawnData = gSaveContext.respawn[RESPAWN_MODE_TOP].data;

    if (this->actionVar16 > 20) {
        this->actor.draw = Player_Draw;
        this->actor.world.pos.y += 60.0f;
        Player_Setup_Midair(this, play);
        return;
    }

    if (this->actionVar16++ == 20) {
        gSaveContext.respawn[RESPAWN_MODE_TOP].data = respawnData + 1;
        Audio_PlaySfx_AtPos(&gSaveContext.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_WARP);
    }
}

static PlayerAnimationHeader* D_80854A58[] = {
    &gPlayerAnim_link_magic_kaze1,
    &gPlayerAnim_link_magic_honoo1,
    &gPlayerAnim_link_magic_tamashii1,
};

static PlayerAnimationHeader* D_80854A64[] = {
    &gPlayerAnim_link_magic_kaze2,
    &gPlayerAnim_link_magic_honoo2,
    &gPlayerAnim_link_magic_tamashii2,
};

static PlayerAnimationHeader* D_80854A70[] = {
    &gPlayerAnim_link_magic_kaze3,
    &gPlayerAnim_link_magic_honoo3,
    &gPlayerAnim_link_magic_tamashii3,
};

static u8 D_80854A7C[] = { 70, 10, 10 };

static AnimSfxEntry D_80854A80[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_PL_SKIP, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 20, NA_SE_VO_LI_SWORD_N, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 26, NA_SE_NONE, STOP),
};

static AnimSfxEntry D_80854A8C[][2] = {
    {
        ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 20, NA_SE_NONE, CONTINUE),
        ANIMSFX(ANIMSFX_TYPE_VOICE, 30, NA_SE_VO_LI_MAGIC_FROL, STOP),
    },
    {
        ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 20, NA_SE_NONE, CONTINUE),
        ANIMSFX(ANIMSFX_TYPE_VOICE, 44, NA_SE_VO_LI_MAGIC_NALE, STOP),
    },
    {
        ANIMSFX(ANIMSFX_TYPE_VOICE, 20, NA_SE_VO_LI_MAGIC_ATTACK, CONTINUE),
        ANIMSFX(ANIMSFX_TYPE_GENERAL, 20, NA_SE_IT_SWORD_SWING_HARD, STOP),
    },
};

void Player_Action_CastMagicSpell(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar8 < 0) {
            if ((this->itemAction == PLAYER_IA_NAYRUS_LOVE) || (gSaveContext.magicState == MAGIC_STATE_IDLE)) {
                Player_Setup2_IdleAll(this, play);
                Camera_SetFinishedFlag(Play_GetCamera(play, CAM_ID_MAIN));
            }
        } else {
            if (this->actionVar16 == 0) {
                PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A58[this->actionVar8], 0.83f);

                if (Player_SpawnMagicSpellActor(play, this, this->actionVar8) != NULL) {
                    this->stateFlags1 |= PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE;
                    if ((this->actionVar8 != 0) || (gSaveContext.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        gSaveContext.magicState = MAGIC_STATE_CONSUME_SETUP;
                    }
                } else {
                    Magic_Reset(play);
                }
            } else {
                PlayerAnimation_PlayLoopSetSpeed(play, &this->skelAnime, D_80854A64[this->actionVar8], 0.83f);

                if (this->actionVar8 == 0) {
                    this->actionVar16 = -10;
                }
            }

            this->actionVar16++;
        }
    } else {
        if (this->actionVar16 < 0) {
            this->actionVar16++;

            if (this->actionVar16 == 0) {
                gSaveContext.respawn[RESPAWN_MODE_TOP].data = 1;
                Play_SetupRespawnPoint(play, RESPAWN_MODE_TOP, 0x6FF);
                gSaveContext.fw.set = 1;
                gSaveContext.fw.pos.x = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.x;
                gSaveContext.fw.pos.y = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y;
                gSaveContext.fw.pos.z = gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.z;
                gSaveContext.fw.yaw = gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw;
                gSaveContext.fw.playerParams = 0x6FF;
                gSaveContext.fw.entranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
                gSaveContext.fw.roomIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex;
                gSaveContext.fw.tempSwchFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags;
                gSaveContext.fw.tempCollectFlags = gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags;
                this->actionVar16 = 2;
            }
        } else if (this->actionVar8 >= 0) {
            if (this->actionVar16 == 0) {
                Player_AnimSfx_Play(this, D_80854A80);
            } else if (this->actionVar16 == 1) {
                Player_AnimSfx_Play(this, D_80854A8C[this->actionVar8]);
                if ((this->actionVar8 == 2) && PlayerAnimation_OnFrame(&this->skelAnime, 30.0f)) {
                    this->stateFlags1 &= ~(PLAYER_STATE1_SKIP_OTHER_ACTORS_UPDATE | PLAYER_STATE1_IN_CUTSCENE);
                }
            } else if (D_80854A7C[this->actionVar8] < this->actionVar16++) {
                PlayerAnimation_PlayOnceSetSpeed(play, &this->skelAnime, D_80854A70[this->actionVar8], 0.83f);
                this->yaw = this->actor.shape.rot.y;
                this->actionVar8 = -1;
            }
        }
    }

    Player_StepHorizontalSpeedToZero(this);
}

void Player_Action_HookshotFly(Player* this, PlayState* play) {
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_NO_YAW_UPDATE_EXCEPT_LOCK_ON;

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_hook_fly_wait);
    }

    Math_Vec3f_Sum(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (func_80834FBC(this)) {
        Math_Vec3f_Copy(&this->actor.prevPos, &this->actor.world.pos);
        Player_ProcessSceneCollision(play, this);

        temp = this->actor.world.pos.y - this->actor.floorHeight;
        if (temp > 20.0f) {
            temp = 20.0f;
        }

        this->actor.world.rot.x = this->actor.shape.rot.x = 0;
        this->actor.world.pos.y -= temp;
        this->speedXZ = 1.0f;
        this->actor.velocity.y = 0.0f;
        Player_Setup_Midair(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_DIVING;
        this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        this->stateFlags1 |= PLAYER_STATE1_END_HOOKSHOT_MOVE;
        return;
    }

    if ((this->skelAnime.animation != &gPlayerAnim_link_hook_fly_start) || (4.0f <= this->skelAnime.curFrame)) {
        this->actor.gravity = 0.0f;
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        Player_RequestRumble(this, 100, 2, 100, SQ(0));
    }
}

void Player_Action_CastFishingRod(Player* this, PlayState* play) {
    if ((this->actionVar16 != 0) && ((this->spinAttackTimer != 0.0f) || (this->dekuStickLength != 0.0f))) {
        f32 updateScale = R_UPDATE_RATE * 0.5f;

        this->skelAnime.curFrame += this->skelAnime.playSpeed * updateScale;
        if (this->skelAnime.curFrame >= this->skelAnime.animLength) {
            this->skelAnime.curFrame -= this->skelAnime.animLength;
        }

        PlayerAnimation_BlendToJoint(play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
                                     (this->spinAttackTimer < 0.0f) ? &gPlayerAnim_link_fishing_reel_left
                                                                    : &gPlayerAnim_link_fishing_reel_right,
                                     5.0f, fabsf(this->spinAttackTimer), this->blendTable);
        PlayerAnimation_BlendToMorph(play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
                                     (this->dekuStickLength < 0.0f) ? &gPlayerAnim_link_fishing_reel_up
                                                                    : &gPlayerAnim_link_fishing_reel_down,
                                     5.0f, fabsf(this->dekuStickLength), D_80858AD8);
        PlayerAnimation_InterpJointMorph(play, &this->skelAnime, 0.5f);
    } else if (PlayerAnimation_Update(play, &this->skelAnime)) {
        this->stickFlameTimer = 2;
        Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_fishing_wait);
        this->actionVar16 = 1;
    }

    Player_StepHorizontalSpeedToZero(this);

    if (this->stickFlameTimer == 0) {
        Player_SetupIdleWithMorph(this, play);
    } else if (this->stickFlameTimer == 3) {
        Player_SetAction(play, this, Player_Action_ReleaseFishFromFishingRod, 0);
        Player_Anim_PlayOnceWithMorph(play, this, &gPlayerAnim_link_fishing_fish_catch);
    }
}

void Player_Action_ReleaseFishFromFishingRod(Player* this, PlayState* play) {
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->stickFlameTimer == 0)) {
        Player_Setup4_IdleAll(this, &gPlayerAnim_link_fishing_fish_catch_end, play);
    }
}

typedef void (*PlayerCsAnim)(PlayState*, Player*, void*);
typedef void (*PlayerCsAction)(PlayState*, Player*, CsCmdActorCue*);

typedef enum {
    /*   -1 */ PLAYER_CSTYPE_ACTION = -1,
    /* 0x00 */ PLAYER_CSTYPE_NONE,
    /* 0x01 */ PLAYER_CSTYPE_ANIM_1,
    /* 0x02 */ PLAYER_CSTYPE_ANIM_2,
    /* 0x03 */ PLAYER_CSTYPE_ANIM_3,
    /* 0x04 */ PLAYER_CSTYPE_ANIM_4,
    /* 0x05 */ PLAYER_CSTYPE_ANIM_5,
    /* 0x06 */ PLAYER_CSTYPE_ANIM_6,
    /* 0x07 */ PLAYER_CSTYPE_ANIM_7,
    /* 0x08 */ PLAYER_CSTYPE_ANIM_8,
    /* 0x09 */ PLAYER_CSTYPE_ANIM_9,
    /* 0x0A */ PLAYER_CSTYPE_ANIM_10,
    /* 0x0B */ PLAYER_CSTYPE_ANIM_11,
    /* 0x0C */ PLAYER_CSTYPE_ANIM_12,
    /* 0x0D */ PLAYER_CSTYPE_ANIM_13,
    /* 0x0E */ PLAYER_CSTYPE_ANIM_14,
    /* 0x0F */ PLAYER_CSTYPE_ANIM_15,
    /* 0x10 */ PLAYER_CSTYPE_ANIM_16,
    /* 0x11 */ PLAYER_CSTYPE_ANIM_17,
    /* 0x12 */ PLAYER_CSTYPE_ANIM_18,
    /* 0x13 */ PLAYER_CSTYPE_ANIM_MAX
} PlayerCsType;

static PlayerCsAnim sPlayerCsModeAnimFuncs[PLAYER_CSTYPE_ANIM_MAX] = {
    NULL,                                                    // PLAYER_CSTYPE_NONE
    Player_CsAnim_SetHorizontalSpeedToZero,                  // PLAYER_CSTYPE_ANIM_1
    Player_CsAnim_PlayOnceWithMorphReset,                    // PLAYER_CSTYPE_ANIM_2
    Player_CsAnim_PlayOnceAdjustedWithLongMorphReset,        // PLAYER_CSTYPE_ANIM_3
    Player_CsAnim_PlayLoopAdjustedWithLongMorphReset,        // PLAYER_CSTYPE_ANIM_4
    Player_CsAnim_ReplacePlayOnceAdjustedUnkFlags,           // PLAYER_CSTYPE_ANIM_5
    Player_CsAnim_ReplacePlayOnce,                           // PLAYER_CSTYPE_ANIM_6
    Player_CsAnim_ReplacePlayLoopAdjustedUnkFlags,           // PLAYER_CSTYPE_ANIM_7
    Player_CsAnim_ReplacePlayLoop,                           // PLAYER_CSTYPE_ANIM_8
    Player_CsAnim_PlayOnce,                                  // PLAYER_CSTYPE_ANIM_9
    Player_CsAnim_PlayLoop,                                  // PLAYER_CSTYPE_ANIM_10
    Player_CsAnim_Update,                                    // PLAYER_CSTYPE_ANIM_11
    Player_CsAnim_PlayLoopAdjustedWithLongMorphOnceFinished, // PLAYER_CSTYPE_ANIM_12
    Player_CsAnim_PlayLoopAdjustedUnkFlagsOnceFinished,      // PLAYER_CSTYPE_ANIM_13
    Player_CsAnim_PlayOnceForOneFrameReset,                  // PLAYER_CSTYPE_ANIM_14
    Player_CsAnim_PlayOnceAdjusted,                          // PLAYER_CSTYPE_ANIM_15
    Player_CsAnim_PlayLoopAdjusted,                          // PLAYER_CSTYPE_ANIM_16
    Player_CsAnim_PlayLoopAdjustedOnceFinished,              // PLAYER_CSTYPE_ANIM_17
    Player_CsAnim_PlayAnimSfx,                               // PLAYER_CSTYPE_ANIM_18
};

static AnimSfxEntry D_80854AF0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 34, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 45, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 51, NA_SE_PL_CALM_HIT, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 64, NA_SE_PL_CALM_HIT, STOP),
};

static AnimSfxEntry D_80854B00[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 3, NA_SE_VO_LI_SURPRISE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 15, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 24, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 30, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_VOICE, 31, NA_SE_VO_LI_FALL_L, STOP),
};

static AnimSfxEntry D_80854B14[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 10, NA_SE_NONE, STOP),
};

typedef struct {
    /* 0x00 */ s8 type;
    /* 0x04 */ union {
        void* ptr;
        void (*func)(PlayState*, Player*, CsCmdActorCue*);
    };
} PlayerCsModeEntry; // size = 0x08

static PlayerCsModeEntry sPlayerCsModeInitFuncs[PLAYER_CSMODE_MAX] = {
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_NONE
    { PLAYER_CSTYPE_ACTION, Player_CsAction_4 },                            // PLAYER_CSMODE_1
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_demo_goma_furimuki },         // PLAYER_CSMODE_2
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_3
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_4
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_bikkuri },               // PLAYER_CSMODE_5
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_6
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_7
    { PLAYER_CSTYPE_ACTION, Player_CsAction_4 },                            // PLAYER_CSMODE_8
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_demo_furimuki },              // PLAYER_CSMODE_9
    { PLAYER_CSTYPE_ACTION, Player_CsAction_7 },                            // PLAYER_CSMODE_10
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_warp },                  // PLAYER_CSMODE_11
    { PLAYER_CSTYPE_ACTION, Player_CsAction_9 },                            // PLAYER_CSMODE_12
    { PLAYER_CSTYPE_ANIM_7, &gPlayerAnim_clink_demo_get1 },                 // PLAYER_CSMODE_13
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_get2 },                 // PLAYER_CSMODE_14
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_get3 },                 // PLAYER_CSMODE_15
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_standup },              // PLAYER_CSMODE_16
    { PLAYER_CSTYPE_ANIM_7, &gPlayerAnim_clink_demo_standup_wait },         // PLAYER_CSMODE_17
    { PLAYER_CSTYPE_ACTION, Player_CsAction_13 },                           // PLAYER_CSMODE_18
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_demo_baru_op1 },              // PLAYER_CSMODE_19
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_demo_baru_op3 },              // PLAYER_CSMODE_20
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_21
    { PLAYER_CSTYPE_ACTION, Player_CsAction_15 },                           // PLAYER_CSMODE_22
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_jibunmiru },             // PLAYER_CSMODE_23
    { PLAYER_CSTYPE_ANIM_9, &gPlayerAnim_link_normal_back_downA },          // PLAYER_CSMODE_24
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_normal_back_down_wake },      // PLAYER_CSMODE_25
    { PLAYER_CSTYPE_ACTION, Player_CsAction_18 },                           // PLAYER_CSMODE_26
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_normal_okarina_end },         // PLAYER_CSMODE_27
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_get_itemA },             // PLAYER_CSMODE_28
    { PLAYER_CSTYPE_ACTION, Player_CsAction_4 },                            // PLAYER_CSMODE_29
    { PLAYER_CSTYPE_ANIM_2, &gPlayerAnim_link_normal_normal2fighter_free }, // PLAYER_CSMODE_30
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_31
    { PLAYER_CSTYPE_NONE, NULL },                                           // PLAYER_CSMODE_32
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_atozusari },            // PLAYER_CSMODE_33
    { PLAYER_CSTYPE_ACTION, Player_CsAction_SwimIdle },                     // PLAYER_CSMODE_34
    { PLAYER_CSTYPE_ACTION, Player_CsAction_22 },                           // PLAYER_CSMODE_35
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_bashi },                // PLAYER_CSMODE_36
    { PLAYER_CSTYPE_ANIM_16, &gPlayerAnim_link_normal_hang_up_down },       // PLAYER_CSMODE_37
    { PLAYER_CSTYPE_ACTION, Player_CsAction_25 },                           // PLAYER_CSMODE_38
    { PLAYER_CSTYPE_ACTION, Player_CsAction_23 },                           // PLAYER_CSMODE_39
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_clink_op3_okiagari },              // PLAYER_CSMODE_40
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_clink_op3_tatiagari },             // PLAYER_CSMODE_41
    { PLAYER_CSTYPE_ACTION, Player_CsAction_28 },                           // PLAYER_CSMODE_42
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_miokuri },              // PLAYER_CSMODE_43
    { PLAYER_CSTYPE_ACTION, Player_CsAction_31 },                           // PLAYER_CSMODE_44
    { PLAYER_CSTYPE_ACTION, Player_CsAction_33 },                           // PLAYER_CSMODE_45
    { PLAYER_CSTYPE_ACTION, Player_CsAction_DrawPlayer },                   // PLAYER_CSMODE_46
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_nozoki },               // PLAYER_CSMODE_47
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_koutai },               // PLAYER_CSMODE_48
    { PLAYER_CSTYPE_ACTION, Player_CsAction_4 },                            // PLAYER_CSMODE_49
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_clink_demo_koutai_kennuki },       // PLAYER_CSMODE_50
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_link_demo_kakeyori },              // PLAYER_CSMODE_51
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_link_demo_kakeyori_mimawasi },     // PLAYER_CSMODE_52
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_link_demo_kakeyori_miokuri },      // PLAYER_CSMODE_53
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_furimuki2 },             // PLAYER_CSMODE_54
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_kaoage },                // PLAYER_CSMODE_55
    { PLAYER_CSTYPE_ANIM_4, &gPlayerAnim_link_demo_kaoage_wait },           // PLAYER_CSMODE_56
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_clink_demo_mimawasi },             // PLAYER_CSMODE_57
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_nozokikomi },            // PLAYER_CSMODE_58
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_kolink_odoroki_demo },             // PLAYER_CSMODE_59
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_link_shagamu_demo },               // PLAYER_CSMODE_60
    { PLAYER_CSTYPE_ANIM_14, &gPlayerAnim_link_okiru_demo },                // PLAYER_CSMODE_61
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_okiru_demo },                 // PLAYER_CSMODE_62
    { PLAYER_CSTYPE_ANIM_5, &gPlayerAnim_link_fighter_power_kiru_start },   // PLAYER_CSMODE_63
    { PLAYER_CSTYPE_ANIM_16, &gPlayerAnim_demo_link_nwait },                // PLAYER_CSMODE_64
    { PLAYER_CSTYPE_ANIM_15, &gPlayerAnim_demo_link_tewatashi },            // PLAYER_CSMODE_65
    { PLAYER_CSTYPE_ANIM_15, &gPlayerAnim_demo_link_orosuu },               // PLAYER_CSMODE_66
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_d_link_orooro },                   // PLAYER_CSMODE_67
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_d_link_imanodare },                // PLAYER_CSMODE_68
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_hatto_demo },                 // PLAYER_CSMODE_69
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_o_get_mae },                       // PLAYER_CSMODE_70
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_o_get_ato },                       // PLAYER_CSMODE_71
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_om_get_mae },                      // PLAYER_CSMODE_72
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_nw_modoru },                       // PLAYER_CSMODE_73
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_gurad },                 // PLAYER_CSMODE_74
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_look_hand },             // PLAYER_CSMODE_75
    { PLAYER_CSTYPE_ANIM_4, &gPlayerAnim_link_demo_sita_wait },             // PLAYER_CSMODE_76
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_ue },                    // PLAYER_CSMODE_77
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_Link_muku },                       // PLAYER_CSMODE_78
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_Link_miageru },                    // PLAYER_CSMODE_79
    { PLAYER_CSTYPE_ANIM_6, &gPlayerAnim_Link_ha },                         // PLAYER_CSMODE_80
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_1kyoro },                        // PLAYER_CSMODE_81
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_2kyoro },                        // PLAYER_CSMODE_82
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_sagaru },                        // PLAYER_CSMODE_83
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_bouzen },                        // PLAYER_CSMODE_84
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_kamaeru },                       // PLAYER_CSMODE_85
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_hajikareru },                    // PLAYER_CSMODE_86
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_ken_miru },                      // PLAYER_CSMODE_87
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_L_mukinaoru },                     // PLAYER_CSMODE_88
    { PLAYER_CSTYPE_ACTION, Player_CsAction_41 },                           // PLAYER_CSMODE_89
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_wait_itemD1_20f },            // PLAYER_CSMODE_90
    { PLAYER_CSTYPE_ACTION, Player_CsAction_44 },                           // PLAYER_CSMODE_91
    { PLAYER_CSTYPE_ACTION, Player_CsAction_46 },                           // PLAYER_CSMODE_92
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_normal_wait_typeB_20f },      // PLAYER_CSMODE_93
    { PLAYER_CSTYPE_ACTION, Player_CsAction_48 },                           // PLAYER_CSMODE_94
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_kousan },                // PLAYER_CSMODE_95
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_return_to_past },        // PLAYER_CSMODE_96
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_last_hit_motion1 },           // PLAYER_CSMODE_97
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_last_hit_motion2 },           // PLAYER_CSMODE_98
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_zeldamiru },             // PLAYER_CSMODE_99
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_kenmiru1 },              // PLAYER_CSMODE_100
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_kenmiru2 },              // PLAYER_CSMODE_101
    { PLAYER_CSTYPE_ANIM_3, &gPlayerAnim_link_demo_kenmiru2_modori },       // PLAYER_CSMODE_102
};

static PlayerCsModeEntry sPlayerCsModeUpdateFuncs[PLAYER_CSMODE_MAX] = {
    { PLAYER_CSTYPE_NONE, NULL },                                            // PLAYER_CSMODE_NONE
    { PLAYER_CSTYPE_ACTION, Player_CsAction_Idle },                          // PLAYER_CSMODE_1
    { PLAYER_CSTYPE_ACTION, Player_CsAction_3 },                             // PLAYER_CSMODE_2
    { PLAYER_CSTYPE_ACTION, Player_CsAction_11 },                            // PLAYER_CSMODE_3
    { PLAYER_CSTYPE_ACTION, Player_CsAction_12 },                            // PLAYER_CSMODE_4
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_5
    { PLAYER_CSTYPE_ACTION, Player_CsAction_55 },                            // PLAYER_CSMODE_6
    { PLAYER_CSTYPE_ACTION, Player_CsAction_53 },                            // PLAYER_CSMODE_7
    { PLAYER_CSTYPE_ACTION, Player_CsAction_Wait },                          // PLAYER_CSMODE_8
    { PLAYER_CSTYPE_ACTION, Player_CsAction_6 },                             // PLAYER_CSMODE_9
    { PLAYER_CSTYPE_ACTION, Player_CsAction_8 },                             // PLAYER_CSMODE_10
    { PLAYER_CSTYPE_ACTION, Player_CsAction_30 },                            // PLAYER_CSMODE_11
    { PLAYER_CSTYPE_ACTION, Player_CsAction_10 },                            // PLAYER_CSMODE_12
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_13
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_14
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_15
    { PLAYER_CSTYPE_ANIM_18, D_80854AF0 },                                   // PLAYER_CSMODE_16
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_17
    { PLAYER_CSTYPE_ACTION, Player_CsAction_14 },                            // PLAYER_CSMODE_18
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_baru_op2 },              // PLAYER_CSMODE_19
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_20
    { PLAYER_CSTYPE_NONE, NULL },                                            // PLAYER_CSMODE_21
    { PLAYER_CSTYPE_ACTION, Player_CsAction_16 },                            // PLAYER_CSMODE_22
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_23
    { PLAYER_CSTYPE_ACTION, Player_CsAction_17 },                            // PLAYER_CSMODE_24
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_25
    { PLAYER_CSTYPE_ANIM_17, &gPlayerAnim_link_normal_okarina_swing },       // PLAYER_CSMODE_26
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_27
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_28
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_29
    { PLAYER_CSTYPE_ACTION, Player_CsAction_19 },                            // PLAYER_CSMODE_30
    { PLAYER_CSTYPE_ACTION, Player_CsAction_20 },                            // PLAYER_CSMODE_31
    { PLAYER_CSTYPE_ACTION, Player_CsAction_21 },                            // PLAYER_CSMODE_32
    { PLAYER_CSTYPE_ANIM_18, D_80854B00 },                                   // PLAYER_CSMODE_33
    { PLAYER_CSTYPE_ACTION, Player_CsAction_SwimSurfaceFromDive },           // PLAYER_CSMODE_34
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_35
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_36
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_37
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_38
    { PLAYER_CSTYPE_ACTION, Player_CsAction_24 },                            // PLAYER_CSMODE_39
    { PLAYER_CSTYPE_ACTION, Player_CsAction_26 },                            // PLAYER_CSMODE_40
    { PLAYER_CSTYPE_ACTION, Player_CsAction_27 },                            // PLAYER_CSMODE_41
    { PLAYER_CSTYPE_ACTION, Player_CsAction_29 },                            // PLAYER_CSMODE_42
    { PLAYER_CSTYPE_ANIM_13, &gPlayerAnim_clink_demo_miokuri_wait },         // PLAYER_CSMODE_43
    { PLAYER_CSTYPE_ACTION, Player_CsAction_32 },                            // PLAYER_CSMODE_44
    { PLAYER_CSTYPE_NONE, NULL },                                            // PLAYER_CSMODE_45
    { PLAYER_CSTYPE_NONE, NULL },                                            // PLAYER_CSMODE_46
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_47
    { PLAYER_CSTYPE_ACTION, Player_CsAction_40 },                            // PLAYER_CSMODE_48
    { PLAYER_CSTYPE_ACTION, Player_CsAction_Wait },                          // PLAYER_CSMODE_49
    { PLAYER_CSTYPE_ACTION, Player_CsAction_37 },                            // PLAYER_CSMODE_50
    { PLAYER_CSTYPE_ANIM_13, &gPlayerAnim_link_demo_kakeyori_wait },         // PLAYER_CSMODE_51
    { PLAYER_CSTYPE_ACTION, Player_CsAction_39 },                            // PLAYER_CSMODE_52
    { PLAYER_CSTYPE_ANIM_13, &gPlayerAnim_link_demo_kakeyori_miokuri_wait }, // PLAYER_CSMODE_53
    { PLAYER_CSTYPE_ACTION, Player_CsAction_38 },                            // PLAYER_CSMODE_54
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_55
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_56
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_clink_demo_mimawasi_wait },        // PLAYER_CSMODE_57
    { PLAYER_CSTYPE_ACTION, Player_CsAction_35 },                            // PLAYER_CSMODE_58
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_59
    { PLAYER_CSTYPE_ANIM_18, D_80854B14 },                                   // PLAYER_CSMODE_60
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_61
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_62
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_63
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_64
    { PLAYER_CSTYPE_ACTION, Player_CsAction_36 },                            // PLAYER_CSMODE_65
    { PLAYER_CSTYPE_ANIM_17, &gPlayerAnim_demo_link_nwait },                 // PLAYER_CSMODE_66
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_d_link_orowait },                  // PLAYER_CSMODE_67
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_demo_link_nwait },                 // PLAYER_CSMODE_68
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_69
    { PLAYER_CSTYPE_ACTION, Player_CsAction_50 },                            // PLAYER_CSMODE_70
    { PLAYER_CSTYPE_ANIM_17, &gPlayerAnim_sude_nwait },                      // PLAYER_CSMODE_71
    { PLAYER_CSTYPE_ACTION, Player_CsAction_50 },                            // PLAYER_CSMODE_72
    { PLAYER_CSTYPE_ANIM_17, &gPlayerAnim_sude_nwait },                      // PLAYER_CSMODE_73
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_gurad_wait },            // PLAYER_CSMODE_74
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_look_hand_wait },        // PLAYER_CSMODE_75
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_76
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_ue_wait },               // PLAYER_CSMODE_77
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_Link_m_wait },                     // PLAYER_CSMODE_78
    { PLAYER_CSTYPE_ANIM_13, &gPlayerAnim_Link_ue_wait },                    // PLAYER_CSMODE_79
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_Link_otituku_w },                  // PLAYER_CSMODE_80
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_L_kw },                            // PLAYER_CSMODE_81
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_82
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_83
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_84
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_85
    { PLAYER_CSTYPE_ACTION, Player_CsAction_49 },                            // PLAYER_CSMODE_86
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_87
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_L_kennasi_w },                     // PLAYER_CSMODE_88
    { PLAYER_CSTYPE_ACTION, Player_CsAction_42 },                            // PLAYER_CSMODE_89
    { PLAYER_CSTYPE_ACTION, Player_CsAction_43 },                            // PLAYER_CSMODE_90
    { PLAYER_CSTYPE_ACTION, Player_CsAction_45 },                            // PLAYER_CSMODE_91
    { PLAYER_CSTYPE_ACTION, Player_CsAction_47 },                            // PLAYER_CSMODE_92
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_93
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_94
    { PLAYER_CSTYPE_ANIM_11, NULL },                                         // PLAYER_CSMODE_95
    { PLAYER_CSTYPE_ACTION, Player_CsAction_51 },                            // PLAYER_CSMODE_96
    { PLAYER_CSTYPE_ACTION, Player_CsAction_52 },                            // PLAYER_CSMODE_97
    { PLAYER_CSTYPE_ACTION, Player_CsAction_52 },                            // PLAYER_CSMODE_98
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_zeldamiru_wait },        // PLAYER_CSMODE_99
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_kenmiru1_wait },         // PLAYER_CSMODE_100
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_link_demo_kenmiru2_wait },         // PLAYER_CSMODE_101
    { PLAYER_CSTYPE_ANIM_12, &gPlayerAnim_demo_link_nwait },                 // PLAYER_CSMODE_102
};

void Player_CsAnimHelper_PlayOnceWithMorphReset(PlayState* play, Player* this, PlayerAnimationHeader* anim) {
    Player_Anim_ResetModelYaw(this);
    Player_Anim_PlayOnceWithMorph(play, this, anim);
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAnimHelper_PlayOnceAdjustedWithLongMorphReset(PlayState* play, Player* this,
                                                            PlayerAnimationHeader* anim) {
    Player_Anim_ResetModelYaw(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Animation_GetLastFrame(anim),
                           ANIMMODE_ONCE, -8.0f);
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAnimHelper_PlayLoopAdjustedWithLongMorphReset(PlayState* play, Player* this,
                                                            PlayerAnimationHeader* anim) {
    Player_Anim_ResetModelYaw(this);
    PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAnim_SetHorizontalSpeedToZero(PlayState* play, Player* this, void* anim) {
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAnim_PlayOnceWithMorphReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayOnceWithMorphReset(play, this, anim);
}

void Player_CsAnim_PlayOnceForOneFrameReset(PlayState* play, Player* this, void* anim) {
    Player_Anim_ResetModelYaw(this);
    Player_Anim_PlayOnceForOneFrame(play, this, anim);
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAnim_PlayOnceAdjustedWithLongMorphReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayOnceAdjustedWithLongMorphReset(play, this, anim);
}

void Player_CsAnim_PlayLoopAdjustedWithLongMorphReset(PlayState* play, Player* this, void* anim) {
    Player_CsAnimHelper_PlayLoopAdjustedWithLongMorphReset(play, this, anim);
}

void Player_CsAnim_ReplacePlayOnceAdjustedUnkFlags(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayOnceAdjustedUnkFlags(play, this, anim);
}

void Player_CsAnim_ReplacePlayOnce(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayOnce(play, this, anim, 0x9C);
}

void Player_CsAnim_ReplacePlayLoopAdjustedUnkFlags(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayLoopAdjustedUnkFlags(play, this, anim);
}

void Player_CsAnim_ReplacePlayLoop(PlayState* play, Player* this, void* anim) {
    Player_AnimReplace_PlayLoop(play, this, anim, 0x9C);
}

void Player_CsAnim_PlayOnce(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayOnce(play, this, anim);
}

void Player_CsAnim_PlayLoop(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayLoop(play, this, anim);
}

void Player_CsAnim_PlayOnceAdjusted(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayOnceAdjusted(play, this, anim);
}

void Player_CsAnim_PlayLoopAdjusted(PlayState* play, Player* this, void* anim) {
    Player_Anim_PlayLoopAdjusted(play, this, anim);
}

void Player_CsAnim_Update(PlayState* play, Player* this, void* anim) {
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAnim_PlayLoopAdjustedWithLongMorphOnceFinished(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_CsAnimHelper_PlayLoopAdjustedWithLongMorphReset(play, this, anim);
        this->actionVar16 = 1;
    }
}

void Player_CsAnim_PlayLoopAdjustedOnceFinished(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_ResetMove(this);
        Player_Anim_PlayLoopAdjusted(play, this, anim);
    }
}

void Player_CsAnim_PlayLoopAdjustedUnkFlagsOnceFinished(PlayState* play, Player* this, void* anim) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoopAdjustedUnkFlags(play, this, anim);
        this->actionVar16 = 1;
    }
}

void Player_CsAnim_PlayAnimSfx(PlayState* play, Player* this, void* arg2) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_AnimSfx_Play(this, arg2);
}

void func_80851314(Player* this) {
    if ((this->unk_448 == NULL) || (this->unk_448->update == NULL)) {
        this->unk_448 = NULL;
    }

    this->lockOnActor = this->unk_448;

    if (this->lockOnActor != NULL) {
        this->actor.shape.rot.y = Player_LookAtTargetActor(this, 0);
    }
}

void Player_CsAction_SwimIdle(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 |= PLAYER_STATE1_SWIMMING;
    this->stateFlags2 |= PLAYER_STATE2_DIVING;
    this->stateFlags1 &= ~(PLAYER_STATE1_JUMPING | PLAYER_STATE1_FREEFALLING);

    Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim);
}

void Player_CsAction_SwimSurfaceFromDive(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.gravity = 0.0f;

    if (this->actionVar8 == 0) {
        if (Player_TrySwimDivingOrGetItem(play, this, NULL)) {
            this->actionVar8 = 1;
        } else {
            Player_UpdateSwimAnim(play, this, NULL, fabsf(this->actor.velocity.y));
            Math_ScaledStepToS(&this->shapePitchOffset, -10000, 800);
            Player_UpdateSwimVelocityYaw(this, &this->actor.velocity.y, 4.0f, this->yaw);
        }
        return;
    }

    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        if (this->actionVar8 == 1) {
            Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
        } else {
            Player_Anim_PlayLoop(play, this, &gPlayerAnim_link_swimer_swim_wait);
        }
    }

    Player_ApplyBuoyancy(this);
    Player_UpdateSwimVelocityYaw(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void Player_CsAction_Idle(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851314(this);

    if (Player_IsFreeSwimming(this)) {
        Player_CsAction_SwimSurfaceFromDive(play, this, NULL);
        return;
    }

    PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_IsShootingHookshot(this) || (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
        Player_TryUpperAction(this, play);
        return;
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
        Player_SwapAction_TryGetItem(this, play);
    }
}

void Player_CsAction_3(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAction_4(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimationHeader* anim;

    if (Player_IsFreeSwimming(this)) {
        Player_CsAction_SwimIdle(play, this, NULL);
        return;
    }

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType);

    if ((this->cueId == PLAYER_CUEID_6) || (this->cueId == PLAYER_CUEID_46)) {
        Player_Anim_PlayOnce(play, this, anim);
    } else {
        Player_Anim_ResetModelYaw(this);
        PlayerAnimation_Change(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f,
                               Animation_GetLastFrame(anim), ANIMMODE_LOOP, -4.0f);
    }

    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAction_Wait(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Player_TryPlayingShootingGallery(play, this)) {
        return;
    }

    if ((this->csMode == PLAYER_CSMODE_49) && (play->csCtx.state == CS_STATE_IDLE)) {
        func_8002DF54(play, NULL, PLAYER_CSMODE_7);
        return;
    }

    if (Player_IsFreeSwimming(this) != 0) {
        Player_CsAction_SwimSurfaceFromDive(play, this, NULL);
        return;
    }

    PlayerAnimation_Update(play, &this->skelAnime);

    if (Player_IsShootingHookshot(this) || (this->stateFlags1 & PLAYER_STATE1_HOLDING_ACTOR)) {
        Player_TryUpperAction(this, play);
    }
}

static AnimSfxEntry D_80855188[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 42, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 48, NA_SE_NONE, STOP),
};

void Player_CsAction_6(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_AnimSfx_Play(this, D_80855188);
}

void Player_CsAction_7(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 &= ~PLAYER_STATE1_AWAITING_THROWN_BOOMERANG;

    this->yaw = this->actor.shape.rot.y = this->actor.world.rot.y =
        Math_Vec3f_Yaw(&this->actor.world.pos, &this->miniCsPosTarget);

    if (this->speedXZ <= 0.0f) {
        this->speedXZ = 0.1f;
    } else if (this->speedXZ > 2.5f) {
        this->speedXZ = 2.5f;
    }
}

void Player_CsAction_8(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 sp1C = 2.5f;

    Player_CutsceneMoveToPos(play, this, &sp1C, 10);

    if (play->sceneId == SCENE_JABU_JABU_BOSS) {
        if (this->actionVar16 == 0) {
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                return;
            }
        } else {
            if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            }
        }
    }

    this->actionVar16++;
    if (this->actionVar16 > 20) {
        this->csMode = PLAYER_CSMODE_11;
    }
}

void Player_CsAction_9(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Setup2_IdleLockOnEnemy(this, play);
}

void Player_CsAction_10(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851314(this);

    if (this->actionVar16 != 0) {
        if (PlayerAnimation_Update(play, &this->skelAnime)) {
            Player_Anim_PlayLoop(play, this, Player_GetWaitRightAnim(this));
            this->actionVar16 = 0;
        }

        Player_ResetLeftRightBlendWeight(this);
    } else {
        func_808401B0(play, this);
    }
}

void Player_CsAction_11(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_CutsceneMove(play, this, cue, 0.0f, 0, 0);
}

void Player_CsAction_12(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_CutsceneMove(play, this, cue, 0.0f, 0, 1);
}

// unused
static PlayerAnimationHeader* D_80855190[] = {
    &gPlayerAnim_link_demo_back_to_past,
    &gPlayerAnim_clink_demo_goto_future,
};

static Vec3f D_80855198 = { -1.0f, 70.0f, 20.0f };

void Player_CsAction_13(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Math_Vec3f_Copy(&this->actor.world.pos, &D_80855198);
    this->actor.shape.rot.y = -0x8000;
    Player_Anim_PlayOnceAdjusted(play, this, this->ageProperties->unk_9C);
    Player_AnimReplace_Setup(play, this, 0x28F);
}

typedef struct {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ s16 unk_02;
} struct_808551A4; // size = 0x04

static struct_808551A4 D_808551A4[] = {
    { NA_SE_IT_SWORD_PUTAWAY_STN, 0 },
    { NA_SE_IT_SWORD_STICK_STN, NA_SE_VO_LI_SWORD_N },
};

static AnimSfxEntry D_808551AC[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 29, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 39, NA_SE_NONE, STOP),
};

void Player_CsAction_14(PlayState* play, Player* this, CsCmdActorCue* cue) {
    struct_808551A4* sp2C;
    Gfx** dLists;

    PlayerAnimation_Update(play, &this->skelAnime);

    if ((LINK_IS_ADULT && PlayerAnimation_OnFrame(&this->skelAnime, 70.0f)) ||
        (!LINK_IS_ADULT && PlayerAnimation_OnFrame(&this->skelAnime, 87.0f))) {
        sp2C = &D_808551A4[gSaveContext.linkAge];
        this->interactRangeActor->parent = &this->actor;

        if (!LINK_IS_ADULT) {
            dLists = gPlayerLeftHandBgsDLs;
        } else {
            dLists = gPlayerLeftHandClosedDLs;
        }
        this->leftHandDLists = dLists + gSaveContext.linkAge;

        Player_PlaySfx(this, sp2C->unk_00);
        if (!LINK_IS_ADULT) {
            Player_AnimSfx_PlayVoice(this, sp2C->unk_02);
        }
    } else if (LINK_IS_ADULT) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 66.0f)) {
            Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_SWORD_L);
        }
    } else {
        Player_AnimSfx_Play(this, D_808551AC);
    }
}

void Player_CsAction_15(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_demo_warp, -PLAYER_ANIM_ADJUSTED_SPEED, 12.0f,
                           12.0f, ANIMMODE_ONCE, 0.0f);
}

static AnimSfxEntry D_808551B4[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_LAND, 30, NA_SE_NONE, STOP),
};

void Player_CsAction_16(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);

    this->actionVar16++;

    if (this->actionVar16 >= 180) {
        if (this->actionVar16 == 180) {
            PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal,
                                   PLAYER_ANIM_ADJUSTED_SPEED, 10.0f,
                                   Animation_GetLastFrame(&gPlayerAnim_link_okarina_warp_goal), ANIMMODE_ONCE, -8.0f);
        }
        Player_AnimSfx_Play(this, D_808551B4);
    }
}

void Player_CsAction_17(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime) && (this->actionVar16 == 0) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_back_downB);
        this->actionVar16 = 1;
    }

    if (this->actionVar16 != 0) {
        Player_StepHorizontalSpeedToZero(this);
    }
}

void Player_CsAction_18(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_CsAnimHelper_PlayOnceAdjustedWithLongMorphReset(play, this, &gPlayerAnim_link_normal_okarina_start);
    Player_SetItemActionToOcarina(this);
    Player_SetModels(this, Player_ModelGroupFromIA(this, this->itemAction));
}

static AnimSfxEntry D_808551B8[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 12, NA_SE_IT_SWORD_PICKOUT, STOP),
};

void Player_CsAction_19(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);

    if (PlayerAnimation_OnFrame(&this->skelAnime, 6.0f)) {
        Player_PullMasterSwordFromPedistal(play, this, 0);
    } else {
        Player_AnimSfx_Play(this, D_808551B8);
    }
}

void Player_CsAction_20(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 0, 1);
}

void Player_CsAction_21(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Math_StepToS(&this->actor.shape.face, 2, 1);
}

void Player_CsAction_22(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplace_PlayOnceAdjusted(play, this, &gPlayerAnim_link_swimer_swim_get, 0x98);
}

void Player_CsAction_23(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplace_PlayOnce(play, this, &gPlayerAnim_clink_op3_negaeri, 0x9C);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_GROAN);
}

void Player_CsAction_24(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoop(play, this, &gPlayerAnim_clink_op3_wait2, 0x9C);
    }
}

void func_80851F14(PlayState* play, Player* this, PlayerAnimationHeader* anim, AnimSfxEntry* arg3) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoopAdjusted(play, this, anim);
        this->actionVar16 = 1;
    } else if (this->actionVar16 == 0) {
        Player_AnimSfx_Play(this, arg3);
    }
}

void Player_CsAction_25(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.shape.shadowDraw = NULL;
    Player_CsAnim_ReplacePlayLoop(play, this, &gPlayerAnim_clink_op3_wait1);
}

static AnimSfxEntry D_808551BC[] = {
    ANIMSFX(ANIMSFX_TYPE_VOICE, 35, NA_SE_VO_LI_RELAX, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 236, NA_SE_PL_SLIPDOWN, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 256, NA_SE_PL_SLIPDOWN, STOP),
};

void Player_CsAction_26(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoop(play, this, &gPlayerAnim_clink_op3_wait3, 0x9C);
        this->actionVar16 = 1;
    } else if (this->actionVar16 == 0) {
        Player_AnimSfx_Play(this, D_808551BC);
        if (PlayerAnimation_OnFrame(&this->skelAnime, 240.0f)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawFeet;
        }
    }
}

static AnimSfxEntry D_808551C8[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 67, NA_SE_PL_LAND_LADDER, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_WALK_WOOD, 84, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_WALK_WOOD, 90, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_WALK_WOOD, 96, NA_SE_NONE, STOP),
};

void Player_CsAction_27(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_AnimSfx_Play(this, D_808551C8);
}

void Player_CsAction_28(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplace_PlayOnceAdjusted(play, this, &gPlayerAnim_clink_demo_futtobi, 0x9D);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
}

void func_808520BC(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 startX = cue->startPos.x;
    f32 startY = cue->startPos.y;
    f32 startZ = cue->startPos.z;

    f32 distX = cue->endPos.x - startX;
    f32 distY = cue->endPos.y - startY;
    f32 distZ = cue->endPos.z - startZ;

    f32 sp4 = (f32)(play->csCtx.curFrame - cue->startFrame) / (f32)(cue->endFrame - cue->startFrame);

    this->actor.world.pos.x = distX * sp4 + startX;
    this->actor.world.pos.y = distY * sp4 + startY;
    this->actor.world.pos.z = distZ * sp4 + startZ;
}

static AnimSfxEntry D_808551D8[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 20, NA_SE_PL_BOUND, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR, 30, NA_SE_PL_BOUND, STOP),
};

void Player_CsAction_29(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_808520BC(play, this, cue);
    PlayerAnimation_Update(play, &this->skelAnime);
    Player_AnimSfx_Play(this, D_808551D8);
}

void Player_CsAction_30(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (cue != NULL) {
        func_808520BC(play, this, cue);
    }
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAction_31(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Anim_PlayOnceWithMorph(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType));
    Player_SetHorizontalSpeedToZero(this);
}

void Player_CsAction_32(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);
}

void Player_CsAction_33(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_AnimReplace_Setup(play, this, 0x98);
}

void Player_CsAction_DrawPlayer(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.draw = Player_Draw;
}

void Player_CsAction_37(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_AnimReplace_PlayLoopAdjustedUnkFlags(play, this, &gPlayerAnim_clink_demo_koutai_wait);
        this->actionVar16 = 1;
    } else if (this->actionVar16 == 0) {
        if (PlayerAnimation_OnFrame(&this->skelAnime, 10.0f)) {
            Player_PullMasterSwordFromPedistal(play, this, 1);
        }
    }
}

static AnimSfxEntry D_808551E0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 10, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 24, NA_SE_NONE, STOP),
};

void Player_CsAction_38(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_furimuki2_wait, D_808551E0);
}

static AnimSfxEntry D_808551E8[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 15, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_LOW, 35, NA_SE_NONE, STOP),
};

void Player_CsAction_35(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80851F14(play, this, &gPlayerAnim_link_demo_nozokikomi_wait, D_808551E8);
}

void Player_CsAction_36(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_Anim_PlayLoopAdjusted(play, this, &gPlayerAnim_demo_link_twait);
        this->actionVar16 = 1;
    }

    if ((this->actionVar16 != 0) && (play->csCtx.curFrame >= 900)) {
        this->rightHandType = PLAYER_MODELTYPE_LH_OPEN;
    } else {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
    }
}

void func_80852414(PlayState* play, Player* this, PlayerAnimationHeader* anim, AnimSfxEntry* arg3) {
    Player_CsAnim_PlayLoopAdjustedUnkFlagsOnceFinished(play, this, anim);
    if (this->actionVar16 == 0) {
        Player_AnimSfx_Play(this, arg3);
    }
}

static AnimSfxEntry D_808551F0[] = {
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 15, NA_SE_NONE, CONTINUE),
    ANIMSFX(ANIMSFX_TYPE_FLOOR_WALK_HIGH, 33, NA_SE_NONE, STOP),
};

void Player_CsAction_40(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80852414(play, this, &gPlayerAnim_clink_demo_koutai_wait, D_808551F0);
}

static AnimSfxEntry D_808551F8[] = {
    ANIMSFX(ANIMSFX_TYPE_GENERAL, 78, NA_SE_PL_KNOCK, STOP),
};

void Player_CsAction_39(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80852414(play, this, &gPlayerAnim_link_demo_kakeyori_wait, D_808551F8);
}

void Player_CsAction_41(PlayState* play, Player* this, CsCmdActorCue* cue) {
    func_80837704(play, this);
}

void Player_CsAction_42(PlayState* play, Player* this, CsCmdActorCue* cue) {
    sControlInput->press.button |= BTN_B;

    Player_Action_ChargeSpinAttack(this, play);
}

void Player_CsAction_43(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_Action_ChargeSpinAttack(this, play);
}

void Player_CsAction_44(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void Player_CsAction_45(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void Player_CsAction_46(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags3 |= PLAYER_STATE3_MIDAIR;
    this->speedXZ = 2.0f;
    this->actor.velocity.y = -1.0f;

    Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_back_downA);
    Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_FALL_L);
}

static void (*D_808551FC[])(Player* this, PlayState* play) = {
    Player_Action_KnockbackFly,
    Player_Action_KnockbackDown,
    Player_Action_KnockbackGetUp,
};

void Player_CsAction_47(PlayState* play, Player* this, CsCmdActorCue* cue) {
    D_808551FC[this->actionVar16](this, play);
}

void Player_CsAction_48(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Player_PullMasterSwordFromPedistal(play, this, 0);
    Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_demo_return_to_past);
}

void Player_CsAction_49(PlayState* play, Player* this, CsCmdActorCue* cue) {
    PlayerAnimation_Update(play, &this->skelAnime);

    if (PlayerAnimation_OnFrame(&this->skelAnime, 10.0f)) {
        this->heldItemAction = this->itemAction = PLAYER_IA_NONE;
        this->heldItemId = ITEM_NONE;
        this->modelGroup = this->nextModelGroup = Player_ModelGroupFromIA(this, PLAYER_IA_NONE);
        this->leftHandDLists = gPlayerLeftHandOpenDLs;
        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        Inventory_DeleteEquipment(play, EQUIP_TYPE_SWORD);
    }
}

static PlayerAnimationHeader* D_80855208[] = {
    &gPlayerAnim_L_okarina_get,
    &gPlayerAnim_om_get,
};

static Vec3s D_80855210[2][2] = {
    { { -200, 700, 100 }, { 800, 600, 800 } },
    { { -200, 500, 0 }, { 600, 400, 600 } },
};

void Player_CsAction_50(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 0, 128, 128, 0 };
    s32 linkAge = gSaveContext.linkAge;
    Vec3f sparklePos;
    Vec3f sp34;
    Vec3s* ptr;

    Player_CsAnim_PlayLoopAdjustedUnkFlagsOnceFinished(play, this, D_80855208[linkAge]);

    if (this->rightHandType != PLAYER_MODELTYPE_RH_FF) {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
        return;
    }

    ptr = D_80855210[gSaveContext.linkAge];

    sp34.x = ptr[0].x + Rand_CenteredFloat(ptr[1].x);
    sp34.y = ptr[0].y + Rand_CenteredFloat(ptr[1].y);
    sp34.z = ptr[0].z + Rand_CenteredFloat(ptr[1].z);

    SkinMatrix_Vec3fMtxFMultXYZ(&this->shieldMf, &sp34, &sparklePos);

    EffectSsKiraKira_SpawnDispersed(play, &sparklePos, &zeroVec, &zeroVec, &primColor, &envColor, 600, -10);
}

void Player_CsAction_51(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        Player_CsAction_53(play, this, cue);
    } else if (this->actionVar16 == 0) {
        Item_Give(play, ITEM_SWORD_MASTER);
        Player_PullMasterSwordFromPedistal(play, this, 0);
    } else {
        func_8084E988(this);
    }
}

void Player_CsAction_52(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (PlayerAnimation_Update(play, &this->skelAnime)) {
        func_8084285C(this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }

    if (this->heldItemAction != PLAYER_IA_SWORD_MASTER) {
        Player_PullMasterSwordFromPedistal(play, this, 1);
    }
}

void Player_CsAction_53(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Player_IsFreeSwimming(this)) {
        Player_SetupSwimIdle(play, this);
        Player_ResetSubCam(play, this);
    } else {
        func_8083C148(this, play);
        if (!Player_SwapAction_TryTalking(this, play)) {
            Player_SwapAction_TryGetItem(this, play);
        }
    }

    this->csMode = PLAYER_CSMODE_NONE;
    this->attentionMode = PLAYER_ATTENTIONMODE_NONE;
}

void Player_CsAction_SetStartPosAndYaw(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;

    if ((play->sceneId == SCENE_KOKIRI_FOREST) && !LINK_IS_ADULT) {
        this->actor.world.pos.y -= 1.0f;
    }

    this->actor.world.pos.z = cue->startPos.z;
    this->yaw = this->actor.shape.rot.y = cue->rot.y;
}

void Player_CsAction_SetStartPosAndYawIfFar(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 xDiff = cue->startPos.x - (s32)this->actor.world.pos.x;
    f32 yDiff = cue->startPos.y - (s32)this->actor.world.pos.y;
    f32 zDiff = cue->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    s16 yawDiff = (s16)cue->rot.y - this->actor.shape.rot.y;

    if ((this->speedXZ == 0.0f) && ((dist > 50.0f) || (ABS(yawDiff) > DEG_TO_BINANG(90.0f)))) {
        Player_CsAction_SetStartPosAndYaw(play, this, cue);
    }

    this->skelAnime.moveFlags = 0;
    Player_Anim_ResetModelYaw(this);
}

void func_80852B4C(PlayState* play, Player* this, CsCmdActorCue* cue, PlayerCsModeEntry* arg3) {
    if (arg3->type > 0) {
        sPlayerCsModeAnimFuncs[arg3->type](play, this, arg3->ptr);
    } else if (arg3->type < 0) {
        arg3->func(play, this, cue);
    }

    if ((D_80858AA0 & 4) && !(this->skelAnime.moveFlags & 4)) {
        this->skelAnime.morphTable[0].y /= this->ageProperties->unk_08;
        D_80858AA0 = 0;
    }
}

void func_80852C0C(PlayState* play, Player* this, s32 csMode) {
    if ((csMode != PLAYER_CSMODE_1) && (csMode != PLAYER_CSMODE_8) && (csMode != PLAYER_CSMODE_49) &&
        (csMode != PLAYER_CSMODE_7)) {
        Player_DetatchHeldActor(play, this);
    }
}

void Player_CsAction_55(PlayState* play, Player* this, CsCmdActorCue* cueUnused) {
    CsCmdActorCue* cue = play->csCtx.playerCue;
    s32 pad;
    s32 csMode;

    if (play->csCtx.state == CS_STATE_STOP) {
        func_8002DF54(play, NULL, PLAYER_CSMODE_7);
        this->cueId = PLAYER_CUEID_NONE;
        Player_SetHorizontalSpeedToZero(this);
        return;
    }

    if (cue == NULL) {
        this->actor.flags &= ~ACTOR_FLAG_IN_UNCULL_ZONE;
        return;
    }

    if (this->cueId != cue->id) {
        csMode = D_808547C4[cue->id];

        if (csMode >= PLAYER_CSMODE_NONE) {
            if ((csMode == PLAYER_CSMODE_3) || (csMode == PLAYER_CSMODE_4)) {
                Player_CsAction_SetStartPosAndYawIfFar(play, this, cue);
            } else {
                Player_CsAction_SetStartPosAndYaw(play, this, cue);
            }
        }

        D_80858AA0 = this->skelAnime.moveFlags;

        Player_Anim_ResetMove(this);
        osSyncPrintf("TOOL MODE=%d\n", csMode);
        func_80852C0C(play, this, ABS(csMode));
        func_80852B4C(play, this, cue, &sPlayerCsModeInitFuncs[ABS(csMode)]);

        this->actionVar16 = 0;
        this->actionVar8 = 0;
        this->cueId = cue->id;
    }

    csMode = D_808547C4[this->cueId];
    func_80852B4C(play, this, cue, &sPlayerCsModeUpdateFuncs[ABS(csMode)]);
}

void Player_Action_StartCutscene(Player* this, PlayState* play) {
    if (this->csMode != this->prevCsMode) {
        D_80858AA0 = this->skelAnime.moveFlags;

        Player_Anim_ResetMove(this);
        this->prevCsMode = this->csMode;
        osSyncPrintf("DEMO MODE=%d\n", this->csMode);
        func_80852C0C(play, this, this->csMode);
        func_80852B4C(play, this, NULL, &sPlayerCsModeInitFuncs[this->csMode]);
    }

    func_80852B4C(play, this, NULL, &sPlayerCsModeUpdateFuncs[this->csMode]);
}

s32 Player_IsDroppingFish(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (Player_Action_DropItemFromBottle == this->actionFunc) && (this->itemAction == PLAYER_IA_BOTTLE_FISH);
}

s32 Player_StartFishing(PlayState* play) {
    Player* this = GET_PLAYER(play);

    Player_DetachHeldActorAndResetAttributes(play, this);
    Player_UseItem(play, this, ITEM_FISHING_POLE);
    return 1;
}

s32 Player_TryGrabbingPlayer(PlayState* play, Player* this) {
    if (!Player_InBlockingCsMode(play, this) && (this->invincibilityTimer >= 0) && !Player_IsShootingHookshot(this) &&
        !(this->stateFlags3 & PLAYER_STATE3_FLYING_ALONG_HOOKSHOT_PATH)) {
        Player_DetachHeldActorAndResetAttributes(play, this);
        Player_SetAction(play, this, Player_Action_Grabbed, 0);
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_re_dead_attack);
        this->stateFlags2 |= PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        Player_ClearAttentionModeAndStopMoving(this);
        Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HELD);
        return true;
    }

    return false;
}

// Sets up player cutscene
s32 Player_SetupCutsceneWithCsMode(PlayState* play, Actor* actor, s32 csMode) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this)) {
        Player_DetachHeldActorAndResetAttributes(play, this);
        Player_SetAction(play, this, Player_Action_StartCutscene, 0);
        this->csMode = csMode;
        this->unk_448 = actor;
        Player_ClearAttentionModeAndStopMoving(this);
        return true;
    }

    return false;
}

void Player_SetupIdleWithMorph(Player* this, PlayState* play) {
    Player_SetAction(play, this, Player_Action_Idle, 1);
    Player_Anim_PlayOnceWithMorph(play, this, Player_GetIdleAnim(this));
    this->yaw = this->actor.shape.rot.y;
}

s32 Player_InflictDamage(PlayState* play, s32 damage) {
    Player* this = GET_PLAYER(play);

    if (!Player_InBlockingCsMode(play, this) && !Player_InflictDamageImpl(play, this, damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_RESTRAINED_BY_ENEMY;
        return true;
    }

    return false;
}

// Start talking with the given actor
void Player_SetupTalk(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);
    s32 pad;

    if ((this->talkActor != NULL) || (actor == this->naviActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_CHECK_WITH_NAVI)) {
        actor->flags |= ACTOR_FLAG_TALK_REQUESTED;
    }

    this->talkActor = actor;
    this->exchangeItemId = EXCH_ITEM_NONE;

    if (actor->textId == 0xFFFF) {
        func_8002DF54(play, actor, PLAYER_CSMODE_1);
        actor->flags |= ACTOR_FLAG_TALK_REQUESTED;
        Player_UnequipItem(play, this);
    } else {
        if (this->actor.flags & ACTOR_FLAG_TALK_REQUESTED) {
            this->actor.textId = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_TALK_REQUESTED;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_RIDING_HORSE) {
            s32 sp24 = this->actionVar16;

            Player_UnequipItem(play, this);
            Player_CsIntoAction_SetupTalk(play, this);

            this->actionVar16 = sp24;
        } else {
            if (Player_IsFreeSwimming(this)) {
                Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupTalk);
                Player_Anim_PlayLoopWithLongMorph(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else if ((actor->category != ACTORCAT_NPC) || (this->heldItemAction == PLAYER_IA_FISHING_POLE)) {
                Player_CsIntoAction_SetupTalk(play, this);

                if (!Player_IsEnemyLockOn(this)) {
                    if ((actor != this->naviActor) && (actor->xzDistToPlayer < 40.0f)) {
                        Player_Anim_PlayOnceAdjusted(play, this, &gPlayerAnim_link_normal_backspace);
                    } else {
                        Player_Anim_PlayLoop(play, this, Player_GetIdleAnim(this));
                    }
                }
            } else {
                Player_SetupCsIntoAction(play, this, Player_CsIntoAction_SetupTalk);
                Player_Anim_PlayOnceAdjusted(play, this,
                                             (actor->xzDistToPlayer < 40.0f) ? &gPlayerAnim_link_normal_backspace
                                                                             : &gPlayerAnim_link_normal_talk_free);
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_backspace) {
                Player_AnimReplace_Setup(play, this, 0x19);
            }

            Player_ClearAttentionModeAndStopMoving(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_IN_CUTSCENE;
    }

    if ((this->naviActor == this->talkActor) && ((this->talkActor->textId & 0xFF00) != 0x200)) {
        this->naviActor->flags |= ACTOR_FLAG_TALK_REQUESTED;
        Player_TurnAroundCamera(play, 0xB);
    }
}
