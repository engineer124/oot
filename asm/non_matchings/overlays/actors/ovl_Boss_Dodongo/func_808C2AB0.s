glabel func_808C2AB0
/* 01920 808C2AB0 44800000 */  mtc1    $zero, $f0                 ## $f0 = 0.00
/* 01924 808C2AB4 27BDFFD0 */  addiu   $sp, $sp, 0xFFD0           ## $sp = FFFFFFD0
/* 01928 808C2AB8 AFB00028 */  sw      $s0, 0x0028($sp)
/* 0192C 808C2ABC AFBF002C */  sw      $ra, 0x002C($sp)
/* 01930 808C2AC0 00808025 */  or      $s0, $a0, $zero            ## $s0 = 00000000
/* 01934 808C2AC4 E4800068 */  swc1    $f0, 0x0068($a0)           ## 00000068
/* 01938 808C2AC8 E48001E4 */  swc1    $f0, 0x01E4($a0)           ## 000001E4
/* 0193C 808C2ACC 3C040600 */  lui     $a0, %hi(D_060061D4)                ## $a0 = 06000000
/* 01940 808C2AD0 0C028800 */  jal     Animation_GetLastFrame

/* 01944 808C2AD4 248461D4 */  addiu   $a0, $a0, %lo(D_060061D4)           ## $a0 = 060061D4
/* 01948 808C2AD8 44822000 */  mtc1    $v0, $f4                   ## $f4 = 0.00
/* 0194C 808C2ADC 44800000 */  mtc1    $zero, $f0                 ## $f0 = 0.00
/* 01950 808C2AE0 3C050600 */  lui     $a1, %hi(D_060061D4)                ## $a1 = 06000000
/* 01954 808C2AE4 468021A0 */  cvt.s.w $f6, $f4
/* 01958 808C2AE8 240E0002 */  addiu   $t6, $zero, 0x0002         ## $t6 = 00000002
/* 0195C 808C2AEC 44070000 */  mfc1    $a3, $f0
/* 01960 808C2AF0 AFAE0014 */  sw      $t6, 0x0014($sp)
/* 01964 808C2AF4 24A561D4 */  addiu   $a1, $a1, %lo(D_060061D4)           ## $a1 = 060061D4
/* 01968 808C2AF8 2604014C */  addiu   $a0, $s0, 0x014C           ## $a0 = 0000014C
/* 0196C 808C2AFC E7A60010 */  swc1    $f6, 0x0010($sp)
/* 01970 808C2B00 3C063F80 */  lui     $a2, 0x3F80                ## $a2 = 3F800000
/* 01974 808C2B04 0C029468 */  jal     Animation_Change

/* 01978 808C2B08 E7A00018 */  swc1    $f0, 0x0018($sp)
/* 0197C 808C2B0C 3C0F808C */  lui     $t7, %hi(func_808C30F4)    ## $t7 = 808C0000
/* 01980 808C2B10 25EF30F4 */  addiu   $t7, $t7, %lo(func_808C30F4) ## $t7 = 808C30F4
/* 01984 808C2B14 24180032 */  addiu   $t8, $zero, 0x0032         ## $t8 = 00000032
/* 01988 808C2B18 AE0F0190 */  sw      $t7, 0x0190($s0)           ## 00000190
/* 0198C 808C2B1C A61801DA */  sh      $t8, 0x01DA($s0)           ## 000001DA
/* 01990 808C2B20 A60001AE */  sh      $zero, 0x01AE($s0)         ## 000001AE
/* 01994 808C2B24 8FBF002C */  lw      $ra, 0x002C($sp)
/* 01998 808C2B28 8FB00028 */  lw      $s0, 0x0028($sp)
/* 0199C 808C2B2C 27BD0030 */  addiu   $sp, $sp, 0x0030           ## $sp = 00000000
/* 019A0 808C2B30 03E00008 */  jr      $ra
/* 019A4 808C2B34 00000000 */  nop
