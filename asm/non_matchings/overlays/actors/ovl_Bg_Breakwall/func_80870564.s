glabel func_80870564
/* 00984 80870564 3C0E8016 */  lui     $t6, %hi(gGameInfo)
/* 00988 80870568 8DCEFA90 */  lw      $t6, %lo(gGameInfo)($t6)
/* 0098C 8087056C 27BDFFE8 */  addiu   $sp, $sp, 0xFFE8           ## $sp = FFFFFFE8
/* 00990 80870570 AFBF0014 */  sw      $ra, 0x0014($sp)           
/* 00994 80870574 AFA5001C */  sw      $a1, 0x001C($sp)           
/* 00998 80870578 85CF0A74 */  lh      $t7, 0x0A74($t6)           ## 80160A74
/* 0099C 8087057C 00803825 */  or      $a3, $a0, $zero            ## $a3 = 00000000
/* 009A0 80870580 C4E8000C */  lwc1    $f8, 0x000C($a3)           ## 0000000C
/* 009A4 80870584 448F2000 */  mtc1    $t7, $f4                   ## $f4 = 0.00
/* 009A8 80870588 24840028 */  addiu   $a0, $a0, 0x0028           ## $a0 = 00000028
/* 009AC 8087058C 3C063F80 */  lui     $a2, 0x3F80                ## $a2 = 3F800000
/* 009B0 80870590 468021A0 */  cvt.s.w $f6, $f4                   
/* 009B4 80870594 46083280 */  add.s   $f10, $f6, $f8             
/* 009B8 80870598 44055000 */  mfc1    $a1, $f10                  
/* 009BC 8087059C 0C01DE80 */  jal     Math_StepToF
              
/* 009C0 808705A0 00000000 */  nop
/* 009C4 808705A4 8FBF0014 */  lw      $ra, 0x0014($sp)           
/* 009C8 808705A8 27BD0018 */  addiu   $sp, $sp, 0x0018           ## $sp = 00000000
/* 009CC 808705AC 03E00008 */  jr      $ra                        
/* 009D0 808705B0 00000000 */  nop
