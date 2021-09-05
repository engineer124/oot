#include "global.h"

s32 D_8012CED0 = 0;

s32 sShrinkWindowTargetVal = 0;
s32 sShrinkWindowCurrentVal = 0;

void ShrinkWindow_SetTargetVal(s32 value) {
    if (HREG(80) == 0x13 && HREG(81) == 1) {
        osSyncPrintf("shrink_window_setval(%d)\n", value);
    }
    sShrinkWindowTargetVal = value;
}

u32 ShrinkWindow_GetTargetVal(void) {
    return sShrinkWindowTargetVal;
}

void ShrinkWindow_SetCurrentVal(s32 currentVal) {
    if (HREG(80) == 0x13 && HREG(81) == 1) {
        osSyncPrintf("shrink_window_setnowval(%d)\n", currentVal);
    }
    sShrinkWindowCurrentVal = currentVal;
}

u32 ShrinkWindow_GetCurrentVal(void) {
    return sShrinkWindowCurrentVal;
}

void ShrinkWindow_Init(void) {
    if (HREG(80) == 0x13 && HREG(81) == 1) {
        osSyncPrintf("shrink_window_init()\n");
    }
    D_8012CED0 = 0;
    sShrinkWindowTargetVal = 0;
    sShrinkWindowCurrentVal = 0;
}

void ShrinkWindow_Destroy(void) {
    if (HREG(80) == 0x13 && HREG(81) == 1) {
        osSyncPrintf("shrink_window_cleanup()\n");
    }
    sShrinkWindowCurrentVal = 0;
}

void ShrinkWindow_Update(s32 updateRate) {
    s32 off;

    if (updateRate == 3) {
        off = 10;
    } else {
        off = 30 / updateRate;
    }

    if (sShrinkWindowCurrentVal < sShrinkWindowTargetVal) {
        if (D_8012CED0 != 1) {
            D_8012CED0 = 1;
        }

        if (sShrinkWindowCurrentVal + off < sShrinkWindowTargetVal) {
            sShrinkWindowCurrentVal += off;
        } else {
            sShrinkWindowCurrentVal = sShrinkWindowTargetVal;
        }
    } else if (sShrinkWindowTargetVal < sShrinkWindowCurrentVal) {
        if (D_8012CED0 != 2) {
            D_8012CED0 = 2;
        }

        if (sShrinkWindowTargetVal < sShrinkWindowCurrentVal - off) {
            sShrinkWindowCurrentVal -= off;
        } else {
            sShrinkWindowCurrentVal = sShrinkWindowTargetVal;
        }
    } else {
        D_8012CED0 = 0;
    }

    if (HREG(80) == 0x13) {
        if (HREG(94) != 0x13) {
            HREG(94) = 0x13;
            HREG(81) = 0;
            HREG(82) = 0;
            HREG(83) = 0;
            HREG(84) = 0;
            HREG(85) = 0;
            HREG(86) = 0;
            HREG(87) = 0;
            HREG(88) = 0;
            HREG(89) = 0;
        }
        HREG(83) = D_8012CED0;
        HREG(84) = sShrinkWindowCurrentVal;
        HREG(85) = sShrinkWindowTargetVal;
        HREG(86) = off;
    }
}
