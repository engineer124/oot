#include "z_kaleido_scope.h"
#include "assets/textures/parameter_static/parameter_static.h"
#include "assets/textures/icon_item_static/icon_item_static.h"

void KaleidoScope_DrawQuestStatus(PlayState* play, GraphicsContext* gfxCtx) {
    static s16 D_8082A070[][4] = {
        { 255, 0, 0, 255 },
        { 255, 70, 0, 150 },
        { 255, 70, 0, 150 },
        { 255, 0, 0, 255 },
    };
    static s16 D_8082A090[][3] = {
        { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 },    { 0, 0, 0 },   { 0, 0, 0 },   { 0, 0, 0 },
        { 0, 60, 0 }, { 90, 0, 0 }, { 0, 40, 110 }, { 80, 40, 0 }, { 70, 0, 90 }, { 90, 90, 0 },
    };
    static s16 D_8082A0D8[] = { 255, 255, 255, 255, 255, 255 };
    static s16 D_8082A0E4[] = { 255, 255, 255, 255, 255, 255 };
    static s16 D_8082A0F0[] = { 150, 150, 150, 150, 150, 150 };
    static s16 D_8082A0FC = 20;
    static s16 D_8082A100 = 0;
    static s16 D_8082A104 = 0;
    static s16 D_8082A108 = 0;
    static s16 D_8082A10C = 0;
    static s16 D_8082A110 = 0;
    static s16 D_8082A114 = 20;
    static s16 D_8082A118 = 0;
    static s16 D_8082A11C = 0;
    static s16 D_8082A120 = 0;
    static u8 D_8082A124[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    static void* D_8082A130[] = {
        gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
    };
    static u16 D_8082A144[] = {
        0xFFCC, 0xFFCC, 0xFFCC, 0xFFCC, 0xFFCC,
    };
    static s16 D_8082A150[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    static s16 D_8082A164[] = {
        150, 255, 100, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
    };
    static s16 D_8082A17C[] = {
        255, 80, 150, 160, 100, 240, 255, 255, 255, 255, 255, 255, 255,
    };
    static s16 D_8082A194[] = {
        100, 40, 255, 0, 255, 100, 255, 255, 255, 255, 255, 255, 230,
    };
    enum {
        /* -3 */ CURSOR_TO_LEFT = -3, // Cursor on the "scroll to left page" position
        /* -2 */ CURSOR_TO_RIGHT,     // Cursor on the "scroll to right page" position
        /* -1 */ CURSOR_NONE          // No position in that direction, cursor stays where it is
    };
    static s8 D_8082A1AC[][4] = {
        /* QUEST_MEDALLION_FOREST */
        { QUEST_MEDALLION_LIGHT, QUEST_MEDALLION_FIRE, QUEST_MEDALLION_LIGHT, CURSOR_TO_RIGHT },
        /* QUEST_MEDALLION_FIRE */
        { QUEST_MEDALLION_FOREST, QUEST_MEDALLION_WATER, QUEST_MEDALLION_WATER, CURSOR_TO_RIGHT },
        /* QUEST_MEDALLION_WATER */ { CURSOR_NONE, QUEST_GORON_RUBY, QUEST_MEDALLION_SPIRIT, QUEST_MEDALLION_FIRE },
        /* QUEST_MEDALLION_SPIRIT */
        { QUEST_MEDALLION_SHADOW, QUEST_MEDALLION_WATER, QUEST_SONG_STORMS, QUEST_MEDALLION_WATER },
        /* QUEST_MEDALLION_SHADOW */
        { QUEST_MEDALLION_LIGHT, QUEST_MEDALLION_SPIRIT, QUEST_HEART_PIECE, QUEST_MEDALLION_LIGHT },
        /* QUEST_MEDALLION_LIGHT */ { CURSOR_NONE, CURSOR_NONE, QUEST_MEDALLION_SHADOW, QUEST_MEDALLION_FOREST },
        /* QUEST_SONG_MINUET */ { QUEST_SONG_LULLABY, CURSOR_NONE, CURSOR_TO_LEFT, QUEST_SONG_BOLERO },
        /* QUEST_SONG_BOLERO */ { QUEST_SONG_EPONA, CURSOR_NONE, QUEST_SONG_MINUET, QUEST_SONG_SERENADE },
        /* QUEST_SONG_SERENADE */ { QUEST_SONG_SARIA, CURSOR_NONE, QUEST_SONG_BOLERO, QUEST_SONG_REQUIEM },
        /* QUEST_SONG_REQUIEM */ { QUEST_SONG_SUN, CURSOR_NONE, QUEST_SONG_SERENADE, QUEST_SONG_NOCTURNE },
        /* QUEST_SONG_NOCTURNE */ { QUEST_SONG_TIME, CURSOR_NONE, QUEST_SONG_REQUIEM, QUEST_SONG_PRELUDE },
        /* QUEST_SONG_PRELUDE */ { QUEST_SONG_STORMS, CURSOR_NONE, QUEST_SONG_NOCTURNE, QUEST_SONG_SNOW },
        /* QUEST_SONG_LULLABY */ { QUEST_SKULL_TOKEN, QUEST_SONG_MINUET, CURSOR_TO_LEFT, QUEST_SONG_EPONA },
        /* QUEST_SONG_EPONA */ { QUEST_SKULL_TOKEN, QUEST_SONG_BOLERO, QUEST_SONG_LULLABY, QUEST_SONG_SARIA },
        /* QUEST_SONG_SARIA */ { QUEST_SKULL_TOKEN, QUEST_SONG_SERENADE, QUEST_SONG_EPONA, QUEST_SONG_SUN },
        /* QUEST_SONG_SUN */ { QUEST_HEART_PIECE, QUEST_SONG_REQUIEM, QUEST_SONG_SARIA, QUEST_SONG_TIME },
        /* QUEST_SONG_TIME */ { QUEST_HEART_PIECE, QUEST_SONG_NOCTURNE, QUEST_SONG_SUN, QUEST_SONG_STORMS },
        /* QUEST_SONG_STORMS */ { QUEST_HEART_PIECE, QUEST_SONG_PRELUDE, QUEST_SONG_TIME, QUEST_SONG_SNOW },
        /* QUEST_SONG_SNOW */ { QUEST_HEART_PIECE, QUEST_KOKIRI_EMERALD, QUEST_SONG_PRELUDE, QUEST_MEDALLION_SPIRIT },
        /* QUEST_KOKIRI_EMERALD */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_SONG_SNOW, QUEST_GORON_RUBY },
        /* QUEST_GORON_RUBY */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_KOKIRI_EMERALD, QUEST_ZORA_SAPPHIRE },
        /* QUEST_ZORA_SAPPHIRE */ { QUEST_MEDALLION_WATER, CURSOR_NONE, QUEST_GORON_RUBY, CURSOR_TO_RIGHT },
        /* QUEST_STONE_OF_AGONY */ { CURSOR_NONE, QUEST_SKULL_TOKEN, CURSOR_TO_LEFT, QUEST_GERUDOS_CARD },
        /* QUEST_GERUDOS_CARD */ { CURSOR_NONE, QUEST_SKULL_TOKEN, QUEST_STONE_OF_AGONY, QUEST_HEART_PIECE },
        /* QUEST_SKULL_TOKEN */ { QUEST_STONE_OF_AGONY, QUEST_SONG_LULLABY, CURSOR_TO_LEFT, QUEST_HEART_PIECE },
        /* QUEST_HEART_PIECE */ { CURSOR_NONE, QUEST_SONG_TIME, QUEST_GERUDOS_CARD, QUEST_MEDALLION_SHADOW },
    };
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    s16 sp226;
    s16 sp224;
    s16 sp222;
    s16 sp220;
    s16 phi_s0;
    s16 phi_s3;
    s16 sp21A;
    s16 sp218;
    s16 sp216;
    s16 pad1;
    s16 phi_v1;
    s16 pad2;
    s16 cursorItem;
    s16 sp208[3];

    OPEN_DISPS(gfxCtx, "../z_kaleido_collect.c", 248);

    if ((!pauseCtx->unk_1E4 || (pauseCtx->unk_1E4 == 5) || (pauseCtx->unk_1E4 == 8)) &&
        (pauseCtx->pageIndex == PAUSE_QUEST)) {
        pauseCtx->cursorColorSet = 0;

        if (pauseCtx->cursorSpecialPos == 0) {
            pauseCtx->nameColorSet = 0;

            if ((pauseCtx->state != 6) || ((pauseCtx->stickAdjX == 0) && (pauseCtx->stickAdjY == 0))) {
                sp216 = pauseCtx->cursorSlot[PAUSE_QUEST];
            } else {
                phi_s3 = pauseCtx->cursorPoint[PAUSE_QUEST];

                if (pauseCtx->stickAdjX < -30) {
                    phi_s0 = D_8082A1AC[phi_s3][2];
                    if (phi_s0 == -3) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                        pauseCtx->unk_1E4 = 0;
                    } else {
                        while (phi_s0 >= 0) {
                            if ((s16)KaleidoScope_UpdateQuestStatusPoint(pauseCtx, phi_s0) != 0) {
                                break;
                            }
                            phi_s0 = D_8082A1AC[phi_s0][2];
                        }
                    }
                } else if (pauseCtx->stickAdjX > 30) {
                    phi_s0 = D_8082A1AC[phi_s3][3];
                    if (phi_s0 == -2) {
                        KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                        pauseCtx->unk_1E4 = 0;
                    } else {
                        while (phi_s0 >= 0) {
                            if ((s16)KaleidoScope_UpdateQuestStatusPoint(pauseCtx, phi_s0) != 0) {
                                break;
                            }
                            phi_s0 = D_8082A1AC[phi_s0][3];
                        }
                    }
                }

                if (pauseCtx->stickAdjY < -30) {
                    phi_s0 = D_8082A1AC[phi_s3][1];
                    while (phi_s0 >= 0) {
                        if ((s16)KaleidoScope_UpdateQuestStatusPoint(pauseCtx, phi_s0) != 0) {
                            break;
                        }
                        phi_s0 = D_8082A1AC[phi_s0][1];
                    }
                } else if (pauseCtx->stickAdjY > 30) {
                    phi_s0 = D_8082A1AC[phi_s3][0];
                    while (phi_s0 >= 0) {
                        if ((s16)KaleidoScope_UpdateQuestStatusPoint(pauseCtx, phi_s0) != 0) {
                            break;
                        }
                        phi_s0 = D_8082A1AC[phi_s0][0];
                    }
                }

                if (phi_s3 != pauseCtx->cursorPoint[PAUSE_QUEST]) {
                    pauseCtx->unk_1E4 = 0;
                    Audio_PlaySfxGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }

                if (pauseCtx->cursorPoint[PAUSE_QUEST] != QUEST_HEART_PIECE) {
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_MEDALLION_LIGHT) {
                            cursorItem = ITEM_MEDALLION_FOREST + pauseCtx->cursorPoint[PAUSE_QUEST];
                            osSyncPrintf("000 ccc=%d\n", cursorItem);
                        } else if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_SONG_SNOW) {
                            cursorItem = ITEM_SCALE_GOLDEN + pauseCtx->cursorPoint[PAUSE_QUEST];
                            osSyncPrintf("111 ccc=%d\n", cursorItem);
                        } else {
                            cursorItem = ITEM_SONG_MINUET + pauseCtx->cursorPoint[PAUSE_QUEST];
                            osSyncPrintf("222 ccc=%d (%d, %d, %d)\n", cursorItem, pauseCtx->cursorPoint[PAUSE_QUEST],
                                         0x12, 0x6C);
                        }
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                        osSyncPrintf("999 ccc=%d (%d,  %d)\n", PAUSE_ITEM_NONE, pauseCtx->cursorPoint[PAUSE_QUEST],
                                     0x18);
                    }
                } else {
                    if ((gSaveContext.inventory.questItems & 0xF0000000) != 0) {
                        cursorItem = ITEM_HEART_CONTAINER;
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                    osSyncPrintf("888 ccc=%d (%d,  %d,  %x)\n", cursorItem, pauseCtx->cursorPoint[PAUSE_QUEST],
                                 ITEM_HEART_CONTAINER, gSaveContext.inventory.questItems & 0xF0000000);
                }

                sp216 = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = sp216;
            }

            KaleidoScope_SetCursorVtx(pauseCtx, sp216 * 4, pauseCtx->questVtx);

            if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0) && (pauseCtx->cursorSpecialPos == 0)) {
                if ((sp216 >= QUEST_SONG_MINUET) && (sp216 < QUEST_KOKIRI_EMERALD)) {
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        sp216 = pauseCtx->cursorSlot[PAUSE_QUEST];
                        pauseCtx->ocarinaSongIdx = gOcarinaSongItemMap[sp216 - QUEST_SONG_MINUET];
                        D_8082A120 = 10;

                        for (phi_s3 = 0; phi_s3 < 8; phi_s3++) {
                            D_8082A124[phi_s3] = 0xFF;
                            D_8082A150[phi_s3] = 0;
                        }

                        D_8082A11C = 0;
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                        AudioOcarina_Start((1 << pauseCtx->ocarinaSongIdx) + OCA_FLAG_8000);
                        pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                        pauseCtx->ocarinaStaff->pos = 0;
                        pauseCtx->ocarinaStaff->state = 0xFF;
                        VREG(21) = -62;
                        VREG(22) = -56;
                        VREG(23) = -49;
                        VREG(24) = -46;
                        VREG(25) = -41;
                        pauseCtx->unk_1E4 = 8;
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                    }
                }
            } else if (pauseCtx->unk_1E4 == 5) {
                if ((pauseCtx->stickAdjX != 0) || (pauseCtx->stickAdjY != 0)) {
                    pauseCtx->unk_1E4 = 0;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
            } else if (pauseCtx->unk_1E4 == 8) {
                if (CHECK_BTN_ALL(input->press.button, BTN_A) && (sp216 >= QUEST_SONG_MINUET) &&
                    (sp216 < QUEST_KOKIRI_EMERALD)) {
                    pauseCtx->unk_1E4 = 9;
                    D_8082A120 = 10;
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                pauseCtx->cursorPoint[PAUSE_QUEST] = QUEST_STONE_OF_AGONY;
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                sp216 = pauseCtx->cursorPoint[PAUSE_QUEST];
                KaleidoScope_SetCursorVtx(pauseCtx, sp216 * 4, pauseCtx->questVtx);
                Audio_PlaySfxGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                    cursorItem = ITEM_SONG_MINUET + pauseCtx->cursorPoint[PAUSE_QUEST];
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
                sp216 = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = sp216;
            }
        } else {
            if (pauseCtx->stickAdjX < -30) {
                pauseCtx->cursorPoint[PAUSE_QUEST] = 0;
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                sp216 = pauseCtx->cursorPoint[PAUSE_QUEST];
                KaleidoScope_SetCursorVtx(pauseCtx, sp216 * 4, pauseCtx->questVtx);
                Audio_PlaySfxGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                    if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_MEDALLION_LIGHT) {
                        cursorItem = ITEM_MEDALLION_FOREST + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_SONG_PRELUDE) {
                        cursorItem = ITEM_BOMB_BAG_30 + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else {
                        cursorItem = ITEM_MEDALLION_SPIRIT + pauseCtx->cursorPoint[PAUSE_QUEST];
                    }
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
                sp216 = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = sp216;
            }
        }

    } else {
        if (pauseCtx->unk_1E4 == 9) {
            pauseCtx->cursorColorSet = 8;

            if (--D_8082A120 == 0) {
                for (phi_s3 = 0; phi_s3 < 8; phi_s3++) {
                    D_8082A124[phi_s3] = 0xFF;
                    D_8082A150[phi_s3] = 0;
                }

                D_8082A11C = 0;
                VREG(21) = -62;
                VREG(22) = -56;
                VREG(23) = -49;
                VREG(24) = -46;
                VREG(25) = -41;
                sp216 = pauseCtx->cursorSlot[PAUSE_QUEST];
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                pauseCtx->ocarinaSongIdx = gOcarinaSongItemMap[sp216 - QUEST_SONG_MINUET];
                AudioOcarina_SetPlaybackSong(pauseCtx->ocarinaSongIdx + 1, 1);
                pauseCtx->unk_1E4 = 2;
                pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                pauseCtx->ocarinaStaff->pos = 0;
                sp216 = pauseCtx->cursorSlot[PAUSE_QUEST];
                KaleidoScope_SetCursorVtx(pauseCtx, sp216 * 4, pauseCtx->questVtx);
            }
        } else {
            sp216 = pauseCtx->cursorSlot[PAUSE_QUEST];
            KaleidoScope_SetCursorVtx(pauseCtx, sp216 * 4, pauseCtx->questVtx);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    D_8082A0FC--;

    for (sp218 = 0, sp21A = 0; sp218 < 6; sp218++, sp21A += 4) {
        if ((D_8082A100 != 1) && (D_8082A100 != 3)) {
            phi_v1 = (D_8082A100 != 0) ? sp218 + 6 : sp218;

            if (D_8082A0FC != 0) {
                sp226 = ABS(D_8082A0D8[sp218] - D_8082A090[phi_v1][0]) / D_8082A0FC;
                sp224 = ABS(D_8082A0E4[sp218] - D_8082A090[phi_v1][1]) / D_8082A0FC;
                sp222 = ABS(D_8082A0F0[sp218] - D_8082A090[phi_v1][2]) / D_8082A0FC;
                if (D_8082A0D8[sp218] >= D_8082A090[phi_v1][0]) {
                    D_8082A0D8[sp218] -= sp226;
                } else {
                    D_8082A0D8[sp218] += sp226;
                }
                if (D_8082A0E4[sp218] >= D_8082A090[phi_v1][1]) {
                    D_8082A0E4[sp218] -= sp224;
                } else {
                    D_8082A0E4[sp218] += sp224;
                }
                if (D_8082A0F0[sp218] >= D_8082A090[phi_v1][2]) {
                    D_8082A0F0[sp218] -= sp222;
                } else {
                    D_8082A0F0[sp218] += sp222;
                }
            } else {
                D_8082A0D8[sp218] = D_8082A090[phi_v1][0];
                D_8082A0E4[sp218] = D_8082A090[phi_v1][1];
                D_8082A0F0[sp218] = D_8082A090[phi_v1][2];
            }
        }

        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST + sp218)) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            gDPSetEnvColor(POLY_OPA_DISP++, D_8082A0D8[sp218], D_8082A0E4[sp218], D_8082A0F0[sp218], 0);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);

            KaleidoScope_DrawQuadTextureRGBA32(gfxCtx, gItemIcons[ITEM_MEDALLION_FOREST + sp218], 24, 24, 0);
        }
    }

    if (D_8082A0FC == 0) {
        D_8082A0FC = ZREG(61 + D_8082A100);
        if (++D_8082A100 >= 4) {
            D_8082A100 = 0;
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gSongNoteTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    for (sp218 = 0; sp218 < QUEST_KOKIRI_EMERALD - QUEST_SONG_MINUET; sp218++, sp21A += 4) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_MINUET + sp218)) {
            if ((QUEST_SONG_MINUET + sp218) == sp216) {
                pauseCtx->questVtx[sp21A + 0].v.ob[0] = pauseCtx->questVtx[sp21A + 2].v.ob[0] =
                    pauseCtx->questVtx[sp21A + 0].v.ob[0] - 2;

                pauseCtx->questVtx[sp21A + 1].v.ob[0] = pauseCtx->questVtx[sp21A + 3].v.ob[0] =
                    pauseCtx->questVtx[sp21A + 1].v.ob[0] + 4;

                pauseCtx->questVtx[sp21A + 0].v.ob[1] = pauseCtx->questVtx[sp21A + 1].v.ob[1] =
                    pauseCtx->questVtx[sp21A + 0].v.ob[1] + 2;

                pauseCtx->questVtx[sp21A + 2].v.ob[1] = pauseCtx->questVtx[sp21A + 3].v.ob[1] =
                    pauseCtx->questVtx[sp21A + 2].v.ob[1] - 4;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, D_8082A164[sp218], D_8082A17C[sp218], D_8082A194[sp218],
                            pauseCtx->alpha);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (sp218 = 0; sp218 < 3; sp218++, sp21A += 4) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD + sp218)) {
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);
            KaleidoScope_DrawQuadTextureRGBA32(gfxCtx, gItemIcons[ITEM_KOKIRI_EMERALD + sp218], 24, 24, 0);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    for (sp218 = 0; sp218 < 3; sp218++, sp21A += 4) {
        if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY + sp218)) {
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            KaleidoScope_DrawQuadTextureRGBA32(gfxCtx, gItemIcons[ITEM_STONE_OF_AGONY + sp218], 24, 24, 0);
        }
    }

    sp226 = ABS(D_8082A104 - D_8082A070[D_8082A118][0]) / D_8082A114;
    sp224 = ABS(D_8082A108 - D_8082A070[D_8082A118][1]) / D_8082A114;
    sp222 = ABS(D_8082A10C - D_8082A070[D_8082A118][2]) / D_8082A114;
    sp220 = ABS(D_8082A110 - D_8082A070[D_8082A118][3]) / D_8082A114;
    if (D_8082A104 >= D_8082A070[D_8082A118][0]) {
        D_8082A104 -= sp226;
    } else {
        D_8082A104 += sp226;
    }
    if (D_8082A108 >= D_8082A070[D_8082A118][1]) {
        D_8082A108 -= sp224;
    } else {
        D_8082A108 += sp224;
    }
    if (D_8082A10C >= D_8082A070[D_8082A118][2]) {
        D_8082A10C -= sp222;
    } else {
        D_8082A10C += sp222;
    }
    if (D_8082A110 >= D_8082A070[D_8082A118][3]) {
        D_8082A110 -= sp220;
    } else {
        D_8082A110 += sp220;
    }

    if (--D_8082A114 == 0) {
        D_8082A104 = D_8082A070[D_8082A118][0];
        D_8082A108 = D_8082A070[D_8082A118][1];
        D_8082A10C = D_8082A070[D_8082A118][2];
        D_8082A110 = D_8082A070[D_8082A118][3];
        D_8082A114 = ZREG(24 + D_8082A118);
        if (++D_8082A118 >= 4) {
            D_8082A118 = 0;
        }
    }

    if ((gSaveContext.inventory.questItems >> QUEST_HEART_PIECE_COUNT) != 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if ((pauseCtx->state == 4) || (pauseCtx->state == 0x12)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, D_8082A070[0][0], D_8082A070[0][1], D_8082A070[0][2],
                            pauseCtx->alpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, D_8082A104, D_8082A108, D_8082A10C, D_8082A110);
        }

        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);

        POLY_OPA_DISP = KaleidoScope_QuadTextureIA8(
            POLY_OPA_DISP,
            gItemIcons[0x79 +
                       (((gSaveContext.inventory.questItems & 0xF0000000) & 0xF0000000) >> QUEST_HEART_PIECE_COUNT)],
            48, 48, 0);
    }

    if (pauseCtx->state == 6) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        sp21A += 4;
        if ((pauseCtx->cursorSpecialPos == 0) && (sp216 >= 6) && (sp216 < QUEST_KOKIRI_EMERALD)) {
            if ((pauseCtx->unk_1E4 < 3) || (pauseCtx->unk_1E4 == 5) || (pauseCtx->unk_1E4 == 8)) {
                if (pauseCtx->cursorItem[pauseCtx->pageIndex] != PAUSE_ITEM_NONE) {
                    pauseCtx->cursorColorSet = 8;
                    if ((pauseCtx->unk_1E4 >= 2) && (pauseCtx->unk_1E4 < 7)) {
                        pauseCtx->cursorColorSet = 0;
                    }
                }
            }
        }

        if (pauseCtx->unk_1E4 == 2) {
            pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();

            if (pauseCtx->ocarinaStaff->pos != 0) {
                if (D_8082A11C + 1 == pauseCtx->ocarinaStaff->pos) {
                    D_8082A11C++;
                    D_8082A124[pauseCtx->ocarinaStaff->pos - 1] = pauseCtx->ocarinaStaff->buttonIndex;
                }

                for (sp218 = 0, phi_s3 = 0; sp218 < 8; sp218++, phi_s3 += 4, sp21A += 4) {
                    if (D_8082A124[sp218] == 0xFF) {
                        break;
                    }

                    if (D_8082A150[sp218] != 255) {
                        D_8082A150[sp218] += VREG(50);
                        if (D_8082A150[sp218] >= 255) {
                            D_8082A150[sp218] = 255;
                        }
                    }

                    pauseCtx->questVtx[sp21A + 0].v.ob[1] = pauseCtx->questVtx[sp21A + 1].v.ob[1] =
                        VREG(21 + D_8082A124[sp218]);

                    pauseCtx->questVtx[sp21A + 2].v.ob[1] = pauseCtx->questVtx[sp21A + 3].v.ob[1] =
                        pauseCtx->questVtx[sp21A + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (D_8082A124[sp218] == 0) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 255, 150, D_8082A150[sp218]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, D_8082A150[sp218]);
                    }

                    gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);
                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);

                    gDPLoadTextureBlock(POLY_OPA_DISP++, D_8082A130[D_8082A124[sp218]], G_IM_FMT_IA, G_IM_SIZ_8b, 16,
                                        16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
                }
            }
        } else if (((pauseCtx->unk_1E4 >= 4) && (pauseCtx->unk_1E4 <= 6)) || (pauseCtx->unk_1E4 == 8)) {
            sp224 = pauseCtx->ocarinaSongIdx;
            sp226 = gOcarinaSongButtons[sp224].numButtons;

            for (sp218 = sp21A, phi_s3 = 0; phi_s3 < sp226; phi_s3++, sp21A += 4) {
                pauseCtx->questVtx[sp21A + 0].v.ob[1] = pauseCtx->questVtx[sp21A + 1].v.ob[1] =
                    VREG(21 + gOcarinaSongButtons[sp224].buttonsIndex[phi_s3]);

                pauseCtx->questVtx[sp21A + 2].v.ob[1] = pauseCtx->questVtx[sp21A + 3].v.ob[1] =
                    pauseCtx->questVtx[sp21A + 0].v.ob[1] - 12;

                gDPPipeSync(POLY_OPA_DISP++);

                if (pauseCtx->unk_1E4 == 8) {
                    if (gOcarinaSongButtons[sp224].buttonsIndex[phi_s3] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 255, 150, 200);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, 200);
                    }
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 150, 150, 150);
                }

                gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);

                gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);

                gDPLoadTextureBlock(POLY_OPA_DISP++, D_8082A130[gOcarinaSongButtons[sp224].buttonsIndex[phi_s3]],
                                    G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }

            if (pauseCtx->unk_1E4 != 8) {
                pauseCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                if (pauseCtx->ocarinaStaff->pos != 0) {
                    if (D_8082A11C == (pauseCtx->ocarinaStaff->pos - 1)) {
                        if ((pauseCtx->ocarinaStaff->buttonIndex >= OCARINA_BTN_A) &&
                            (pauseCtx->ocarinaStaff->buttonIndex <= OCARINA_BTN_C_UP)) {
                            D_8082A124[pauseCtx->ocarinaStaff->pos - 1] = pauseCtx->ocarinaStaff->buttonIndex;
                            D_8082A124[pauseCtx->ocarinaStaff->pos] = 0xFF;
                            D_8082A11C++;
                        }
                    }
                }

                sp21A = sp218 + 32;
                phi_s3 = 0;
                for (; phi_s3 < 8; phi_s3++, sp21A += 4) {
                    if (D_8082A124[phi_s3] == 0xFF) {
                        continue;
                    }

                    if (D_8082A150[phi_s3] != 255) {
                        D_8082A150[phi_s3] += VREG(50);
                        if (D_8082A150[phi_s3] >= 255) {
                            D_8082A150[phi_s3] = 255;
                        }
                    }
                    pauseCtx->questVtx[sp21A + 0].v.ob[1] = pauseCtx->questVtx[sp21A + 1].v.ob[1] =
                        VREG(21 + D_8082A124[phi_s3]);

                    pauseCtx->questVtx[sp21A + 2].v.ob[1] = pauseCtx->questVtx[sp21A + 3].v.ob[1] =
                        pauseCtx->questVtx[sp21A + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (D_8082A124[phi_s3] == 0) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 255, 150, D_8082A150[phi_s3]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, D_8082A150[phi_s3]);
                    }

                    gDPSetEnvColor(POLY_OPA_DISP++, 10, 10, 10, 0);

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[sp21A], 4, 0);

                    gDPLoadTextureBlock(POLY_OPA_DISP++, D_8082A130[D_8082A124[phi_s3]], G_IM_FMT_IA, G_IM_SIZ_8b, 16,
                                        16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
                }

                if (pauseCtx->unk_1E4 == 4) {
                    for (phi_s3 = 0; phi_s3 < 8; phi_s3++) {
                        D_8082A124[phi_s3] = 0xFF;
                        D_8082A150[phi_s3] = 0;
                    }

                    D_8082A11C = 0;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                    AudioOcarina_Start((1 << pauseCtx->ocarinaSongIdx) + OCA_FLAG_8000);
                    pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                    pauseCtx->ocarinaStaff->pos = 0;
                    pauseCtx->ocarinaStaff->state = 0xFE;
                    pauseCtx->unk_1E4 = 5;
                }
            }
        }
    }

    if (CHECK_QUEST_ITEM(QUEST_SKULL_TOKEN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        sp208[0] = sp208[1] = 0;
        sp208[2] = gSaveContext.inventory.gsTokens;

        while (sp208[2] >= 100) {
            sp208[0]++;
            sp208[2] -= 100;
        }

        while (sp208[2] >= 10) {
            sp208[1]++;
            sp208[2] -= 10;
        }

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[164], 24, 0);

        for (phi_s3 = 0, sp218 = 0, sp21A = 0; phi_s3 < 2; phi_s3++) {
            if (phi_s3 == 0) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);
            } else if (gSaveContext.inventory.gsTokens == 100) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 50, 50, pauseCtx->alpha);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            }

            phi_s0 = 0;
            for (sp21A = 0; sp21A < 3; sp21A++, sp218 += 4) {
                if ((sp21A >= 2) || (sp208[sp21A] != 0) || (phi_s0 != 0)) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, ((u8*)gCounterDigit0Tex + (8 * 16 * sp208[sp21A])), G_IM_FMT_I,
                                        G_IM_SIZ_8b, 8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, sp218, sp218 + 2, sp218 + 3, sp218 + 1, 0);

                    phi_s0 = 1;
                }
            }
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_kaleido_collect.c", 863);
}

s32 KaleidoScope_UpdateQuestStatusPoint(PauseContext* pauseCtx, s32 point) {
    pauseCtx->cursorPoint[PAUSE_QUEST] = point;

    return 1;
}
