.late_rodata
glabel D_80A66908
    .float 0.01

.text
glabel func_80A60470
/* 05180 80A60470 27BDFFC8 */  addiu   $sp, $sp, 0xFFC8           ## $sp = FFFFFFC8
/* 05184 80A60474 AFBF002C */  sw      $ra, 0x002C($sp)
/* 05188 80A60478 AFB00028 */  sw      $s0, 0x0028($sp)
/* 0518C 80A6047C AFA5003C */  sw      $a1, 0x003C($sp)
/* 05190 80A60480 C48401C4 */  lwc1    $f4, 0x01C4($a0)           ## 000001C4
/* 05194 80A60484 240E0008 */  addiu   $t6, $zero, 0x0008         ## $t6 = 00000008
/* 05198 80A60488 AC8E0210 */  sw      $t6, 0x0210($a0)           ## 00000210
/* 0519C 80A6048C E7A40034 */  swc1    $f4, 0x0034($sp)
/* 051A0 80A60490 8C8F0158 */  lw      $t7, 0x0158($a0)           ## 00000158
/* 051A4 80A60494 3C1980A6 */  lui     $t9, %hi(D_80A65E58)       ## $t9 = 80A60000
/* 051A8 80A60498 00808025 */  or      $s0, $a0, $zero            ## $s0 = 00000000
/* 051AC 80A6049C 000FC080 */  sll     $t8, $t7,  2
/* 051B0 80A604A0 0338C821 */  addu    $t9, $t9, $t8
/* 051B4 80A604A4 8F395E58 */  lw      $t9, %lo(D_80A65E58)($t9)
/* 051B8 80A604A8 0C028800 */  jal     Animation_GetLastFrame

/* 051BC 80A604AC 8F240020 */  lw      $a0, 0x0020($t9)           ## 80A60020
/* 051C0 80A604B0 8E080158 */  lw      $t0, 0x0158($s0)           ## 00000158
/* 051C4 80A604B4 44823000 */  mtc1    $v0, $f6                   ## $f6 = 0.00
/* 051C8 80A604B8 3C0A80A6 */  lui     $t2, %hi(D_80A65E58)       ## $t2 = 80A60000
/* 051CC 80A604BC 00084880 */  sll     $t1, $t0,  2
/* 051D0 80A604C0 8E0B0210 */  lw      $t3, 0x0210($s0)           ## 00000210
/* 051D4 80A604C4 01495021 */  addu    $t2, $t2, $t1
/* 051D8 80A604C8 8D4A5E58 */  lw      $t2, %lo(D_80A65E58)($t2)
/* 051DC 80A604CC 46803220 */  cvt.s.w $f8, $f6
/* 051E0 80A604D0 000B6080 */  sll     $t4, $t3,  2
/* 051E4 80A604D4 3C01C040 */  lui     $at, 0xC040                ## $at = C0400000
/* 051E8 80A604D8 014C6821 */  addu    $t5, $t2, $t4
/* 051EC 80A604DC 44815000 */  mtc1    $at, $f10                  ## $f10 = -3.00
/* 051F0 80A604E0 8DA50000 */  lw      $a1, 0x0000($t5)           ## 00000000
/* 051F4 80A604E4 240E0002 */  addiu   $t6, $zero, 0x0002         ## $t6 = 00000002
/* 051F8 80A604E8 AFAE0014 */  sw      $t6, 0x0014($sp)
/* 051FC 80A604EC E7A80010 */  swc1    $f8, 0x0010($sp)
/* 05200 80A604F0 260401AC */  addiu   $a0, $s0, 0x01AC           ## $a0 = 000001AC
/* 05204 80A604F4 3C063FC0 */  lui     $a2, 0x3FC0                ## $a2 = 3FC00000
/* 05208 80A604F8 8FA70034 */  lw      $a3, 0x0034($sp)
/* 0520C 80A604FC 0C029468 */  jal     Animation_Change

/* 05210 80A60500 E7AA0018 */  swc1    $f10, 0x0018($sp)
/* 05214 80A60504 C6100028 */  lwc1    $f16, 0x0028($s0)          ## 00000028
/* 05218 80A60508 44809000 */  mtc1    $zero, $f18                ## $f18 = 0.00
/* 0521C 80A6050C 44802000 */  mtc1    $zero, $f4                 ## $f4 = 0.00
/* 05220 80A60510 8E0201CC */  lw      $v0, 0x01CC($s0)           ## 000001CC
/* 05224 80A60514 AE000244 */  sw      $zero, 0x0244($s0)         ## 00000244
/* 05228 80A60518 E6100274 */  swc1    $f16, 0x0274($s0)          ## 00000274
/* 0522C 80A6051C E612006C */  swc1    $f18, 0x006C($s0)          ## 0000006C
/* 05230 80A60520 E6040060 */  swc1    $f4, 0x0060($s0)           ## 00000060
/* 05234 80A60524 844F0002 */  lh      $t7, 0x0002($v0)           ## 00000002
/* 05238 80A60528 3C0180A6 */  lui     $at, %hi(D_80A66908)       ## $at = 80A60000
/* 0523C 80A6052C C42A6908 */  lwc1    $f10, %lo(D_80A66908)($at)
/* 05240 80A60530 448F3000 */  mtc1    $t7, $f6                   ## $f6 = 0.00
/* 05244 80A60534 C608025C */  lwc1    $f8, 0x025C($s0)           ## 0000025C
/* 05248 80A60538 8E1801F0 */  lw      $t8, 0x01F0($s0)           ## 000001F0
/* 0524C 80A6053C 46803020 */  cvt.s.w $f0, $f6
/* 05250 80A60540 3C078013 */  lui     $a3, %hi(D_801333E0)
/* 05254 80A60544 3C088013 */  lui     $t0, %hi(D_801333E8)
/* 05258 80A60548 37190008 */  ori     $t9, $t8, 0x0008           ## $t9 = 00000008
/* 0525C 80A6054C 24E733E0 */  addiu   $a3, %lo(D_801333E0)
/* 05260 80A60550 AE1901F0 */  sw      $t9, 0x01F0($s0)           ## 000001F0
/* 05264 80A60554 460A0402 */  mul.s   $f16, $f0, $f10
/* 05268 80A60558 250833E8 */  addiu   $t0, %lo(D_801333E8)
/* 0526C 80A6055C 24042818 */  addiu   $a0, $zero, 0x2818         ## $a0 = 00002818
/* 05270 80A60560 260500E4 */  addiu   $a1, $s0, 0x00E4           ## $a1 = 000000E4
/* 05274 80A60564 24060004 */  addiu   $a2, $zero, 0x0004         ## $a2 = 00000004
/* 05278 80A60568 46104481 */  sub.s   $f18, $f8, $f16
/* 0527C 80A6056C E612025C */  swc1    $f18, 0x025C($s0)          ## 0000025C
/* 05280 80A60570 AFA80014 */  sw      $t0, 0x0014($sp)
/* 05284 80A60574 0C03DCE3 */  jal     Audio_PlaySoundGeneral

/* 05288 80A60578 AFA70010 */  sw      $a3, 0x0010($sp)
/* 0528C 80A6057C 44806000 */  mtc1    $zero, $f12                ## $f12 = 0.00
/* 05290 80A60580 240500AA */  addiu   $a1, $zero, 0x00AA         ## $a1 = 000000AA
/* 05294 80A60584 2406000A */  addiu   $a2, $zero, 0x000A         ## $a2 = 0000000A
/* 05298 80A60588 0C02A800 */  jal     func_800AA000
/* 0529C 80A6058C 2407000A */  addiu   $a3, $zero, 0x000A         ## $a3 = 0000000A
/* 052A0 80A60590 8FBF002C */  lw      $ra, 0x002C($sp)
/* 052A4 80A60594 8FB00028 */  lw      $s0, 0x0028($sp)
/* 052A8 80A60598 27BD0038 */  addiu   $sp, $sp, 0x0038           ## $sp = 00000000
/* 052AC 80A6059C 03E00008 */  jr      $ra
/* 052B0 80A605A0 00000000 */  nop
