#include "global.h"

void Setup_InitImpl(SetupState* this) {
    osSyncPrintf("ゼルダ共通データ初期化\n"); // "Zelda common data initalization"
    SaveContext_Init();

    /*
    // vanilla
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, ConsoleLogo_Init, ConsoleLogoState);
    */

    this->state.running = false;
    gSaveContext.gameMode = GAMEMODE_NORMAL;
    gSaveContext.linkAge = LINK_AGE_ADULT;
    Sram_InitDebugSave();
    gSaveContext.dayTime = CLOCK_TIME(12, 0);
    gSaveContext.entranceIndex = ENTR_WATER_TEMPLE_BOSS_0;
    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = ENTR_LOAD_OPENING;
    gSaveContext.zTargetSetting = 1; // Hold
    SET_EVENTCHKINF(EVENTCHKINF_74); // Morpha Intro CS Watched
    SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
}

void Setup_Destroy(GameState* thisx) {
}

void Setup_Init(GameState* thisx) {
    SetupState* this = (SetupState*)thisx;

    this->state.destroy = Setup_Destroy;
    Setup_InitImpl(this);
}
