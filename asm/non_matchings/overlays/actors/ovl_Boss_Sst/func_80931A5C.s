.late_rodata
glabel D_809379F0
 .word 0x3D567750
glabel D_809379F4
 .word 0x3FA147AE

.text
glabel func_80931A5C
/* 0548C 80931A5C 27BDFFD0 */  addiu   $sp, $sp, 0xFFD0           ## $sp = FFFFFFD0
/* 05490 80931A60 AFBF0024 */  sw      $ra, 0x0024($sp)           
/* 05494 80931A64 AFB00020 */  sw      $s0, 0x0020($sp)           
/* 05498 80931A68 AFA50034 */  sw      $a1, 0x0034($sp)           
/* 0549C 80931A6C 8CAF1C44 */  lw      $t7, 0x1C44($a1)           ## 00001C44
/* 054A0 80931A70 00808025 */  or      $s0, $a0, $zero            ## $s0 = 00000000
/* 054A4 80931A74 AFAF002C */  sw      $t7, 0x002C($sp)           
/* 054A8 80931A78 84820198 */  lh      $v0, 0x0198($a0)           ## 00000198
/* 054AC 80931A7C 10400003 */  beq     $v0, $zero, .L80931A8C     
/* 054B0 80931A80 2458FFFF */  addiu   $t8, $v0, 0xFFFF           ## $t8 = FFFFFFFF
/* 054B4 80931A84 A4980198 */  sh      $t8, 0x0198($a0)           ## 00000198
/* 054B8 80931A88 84820198 */  lh      $v0, 0x0198($a0)           ## 00000198
.L80931A8C:
/* 054BC 80931A8C 44822000 */  mtc1    $v0, $f4                   ## $f4 = 0.00
/* 054C0 80931A90 3C018093 */  lui     $at, %hi(D_809379F0)       ## $at = 80930000
/* 054C4 80931A94 C42879F0 */  lwc1    $f8, %lo(D_809379F0)($at)  
/* 054C8 80931A98 468021A0 */  cvt.s.w $f6, $f4                   
/* 054CC 80931A9C 46083302 */  mul.s   $f12, $f6, $f8             
/* 054D0 80931AA0 0C0400A4 */  jal     sinf
              
/* 054D4 80931AA4 00000000 */  nop
/* 054D8 80931AA8 82020194 */  lb      $v0, 0x0194($s0)           ## 00000194
/* 054DC 80931AAC 3C013F80 */  lui     $at, 0x3F80                ## $at = 3F800000
/* 054E0 80931AB0 44815000 */  mtc1    $at, $f10                  ## $f10 = 1.00
/* 054E4 80931AB4 0002CB40 */  sll     $t9, $v0, 13               
/* 054E8 80931AB8 44999000 */  mtc1    $t9, $f18                  ## $f18 = 0.00
/* 054EC 80931ABC 860801A4 */  lh      $t0, 0x01A4($s0)           ## 000001A4
/* 054F0 80931AC0 46005401 */  sub.s   $f16, $f10, $f0            
/* 054F4 80931AC4 860E0198 */  lh      $t6, 0x0198($s0)           ## 00000198
/* 054F8 80931AC8 44884000 */  mtc1    $t0, $f8                   ## $f8 = 0.00
/* 054FC 80931ACC 46809120 */  cvt.s.w $f4, $f18                  
/* 05500 80931AD0 00026380 */  sll     $t4, $v0, 14               
/* 05504 80931AD4 29C10005 */  slti    $at, $t6, 0x0005           
/* 05508 80931AD8 24050000 */  addiu   $a1, $zero, 0x0000         ## $a1 = 00000000
/* 0550C 80931ADC 3C063F00 */  lui     $a2, 0x3F00                ## $a2 = 3F000000
/* 05510 80931AE0 468042A0 */  cvt.s.w $f10, $f8                  
/* 05514 80931AE4 46048182 */  mul.s   $f6, $f16, $f4             
/* 05518 80931AE8 3C0741C8 */  lui     $a3, 0x41C8                ## $a3 = 41C80000
/* 0551C 80931AEC 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 05520 80931AF0 460A3480 */  add.s   $f18, $f6, $f10            
/* 05524 80931AF4 4600940D */  trunc.w.s $f16, $f18                 
/* 05528 80931AF8 440A8000 */  mfc1    $t2, $f16                  
/* 0552C 80931AFC 00000000 */  nop
/* 05530 80931B00 A60A0032 */  sh      $t2, 0x0032($s0)           ## 00000032
/* 05534 80931B04 860B0032 */  lh      $t3, 0x0032($s0)           ## 00000032
/* 05538 80931B08 016C6823 */  subu    $t5, $t3, $t4              
/* 0553C 80931B0C 10200038 */  beq     $at, $zero, .L80931BF0     
/* 05540 80931B10 A60D00B6 */  sh      $t5, 0x00B6($s0)           ## 000000B6
/* 05544 80931B14 3C0140A0 */  lui     $at, 0x40A0                ## $at = 40A00000
/* 05548 80931B18 44812000 */  mtc1    $at, $f4                   ## $f4 = 5.00
/* 0554C 80931B1C 26040068 */  addiu   $a0, $s0, 0x0068           ## $a0 = 00000068
/* 05550 80931B20 0C01E0C4 */  jal     Math_SmoothStepToF
              
/* 05554 80931B24 E7A40010 */  swc1    $f4, 0x0010($sp)           
/* 05558 80931B28 0C02927F */  jal     SkelAnime_Update
              
/* 0555C 80931B2C 2604014C */  addiu   $a0, $s0, 0x014C           ## $a0 = 0000014C
/* 05560 80931B30 50400041 */  beql    $v0, $zero, .L80931C38     
/* 05564 80931B34 920203E4 */  lbu     $v0, 0x03E4($s0)           ## 000003E4
/* 05568 80931B38 920F03E4 */  lbu     $t7, 0x03E4($s0)           ## 000003E4
/* 0556C 80931B3C 44804000 */  mtc1    $zero, $f8                 ## $f8 = 0.00
/* 05570 80931B40 31F8FFFC */  andi    $t8, $t7, 0xFFFC           ## $t8 = 00000000
/* 05574 80931B44 A21803E4 */  sb      $t8, 0x03E4($s0)           ## 000003E4
/* 05578 80931B48 E6080068 */  swc1    $f8, 0x0068($s0)           ## 00000068
/* 0557C 80931B4C 8FB9002C */  lw      $t9, 0x002C($sp)           
/* 05580 80931B50 8F280680 */  lw      $t0, 0x0680($t9)           ## 00000680
/* 05584 80931B54 31090080 */  andi    $t1, $t0, 0x0080           ## $t1 = 00000000
/* 05588 80931B58 11200012 */  beq     $t1, $zero, .L80931BA4     
/* 0558C 80931B5C 00000000 */  nop
/* 05590 80931B60 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 05594 80931B64 00000000 */  nop
/* 05598 80931B68 3C013F00 */  lui     $at, 0x3F00                ## $at = 3F000000
/* 0559C 80931B6C 44813000 */  mtc1    $at, $f6                   ## $f6 = 0.50
/* 055A0 80931B70 00000000 */  nop
/* 055A4 80931B74 4606003C */  c.lt.s  $f0, $f6                   
/* 055A8 80931B78 00000000 */  nop
/* 055AC 80931B7C 45000005 */  bc1f    .L80931B94                 
/* 055B0 80931B80 00000000 */  nop
/* 055B4 80931B84 0C24C741 */  jal     func_80931D04              
/* 055B8 80931B88 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 055BC 80931B8C 1000002A */  beq     $zero, $zero, .L80931C38   
/* 055C0 80931B90 920203E4 */  lbu     $v0, 0x03E4($s0)           ## 000003E4
.L80931B94:
/* 055C4 80931B94 0C24C7BE */  jal     func_80931EF8              
/* 055C8 80931B98 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 055CC 80931B9C 10000026 */  beq     $zero, $zero, .L80931C38   
/* 055D0 80931BA0 920203E4 */  lbu     $v0, 0x03E4($s0)           ## 000003E4
.L80931BA4:
/* 055D4 80931BA4 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 055D8 80931BA8 00000000 */  nop
/* 055DC 80931BAC 3C013F00 */  lui     $at, 0x3F00                ## $at = 3F000000
/* 055E0 80931BB0 44815000 */  mtc1    $at, $f10                  ## $f10 = 0.50
/* 055E4 80931BB4 8FA40034 */  lw      $a0, 0x0034($sp)           
/* 055E8 80931BB8 26050024 */  addiu   $a1, $s0, 0x0024           ## $a1 = 00000024
/* 055EC 80931BBC 460A003C */  c.lt.s  $f0, $f10                  
/* 055F0 80931BC0 2402000F */  addiu   $v0, $zero, 0x000F         ## $v0 = 0000000F
/* 055F4 80931BC4 45000003 */  bc1f    .L80931BD4                 
/* 055F8 80931BC8 00000000 */  nop
/* 055FC 80931BCC 10000001 */  beq     $zero, $zero, .L80931BD4   
/* 05600 80931BD0 24020008 */  addiu   $v0, $zero, 0x0008         ## $v0 = 00000008
.L80931BD4:
/* 05604 80931BD4 00023400 */  sll     $a2, $v0, 16               
/* 05608 80931BD8 0C007D52 */  jal     Item_DropCollectible
              
/* 0560C 80931BDC 00063403 */  sra     $a2, $a2, 16               
/* 05610 80931BE0 0C24C11D */  jal     func_80930474              
/* 05614 80931BE4 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 05618 80931BE8 10000013 */  beq     $zero, $zero, .L80931C38   
/* 0561C 80931BEC 920203E4 */  lbu     $v0, 0x03E4($s0)           ## 000003E4
.L80931BF0:
/* 05620 80931BF0 3C01428C */  lui     $at, 0x428C                ## $at = 428C0000
/* 05624 80931BF4 44811000 */  mtc1    $at, $f2                   ## $f2 = 70.00
/* 05628 80931BF8 3C018093 */  lui     $at, %hi(D_809379F4)       ## $at = 80930000
/* 0562C 80931BFC C43079F4 */  lwc1    $f16, %lo(D_809379F4)($at) 
/* 05630 80931C00 C6120068 */  lwc1    $f18, 0x0068($s0)          ## 00000068
/* 05634 80931C04 46109102 */  mul.s   $f4, $f18, $f16            
/* 05638 80931C08 E6040068 */  swc1    $f4, 0x0068($s0)           ## 00000068
/* 0563C 80931C0C C6000068 */  lwc1    $f0, 0x0068($s0)           ## 00000068
/* 05640 80931C10 4600103C */  c.lt.s  $f2, $f0                   
/* 05644 80931C14 00000000 */  nop
/* 05648 80931C18 45020004 */  bc1fl   .L80931C2C                 
/* 0564C 80931C1C E6000068 */  swc1    $f0, 0x0068($s0)           ## 00000068
/* 05650 80931C20 10000002 */  beq     $zero, $zero, .L80931C2C   
/* 05654 80931C24 E6020068 */  swc1    $f2, 0x0068($s0)           ## 00000068
/* 05658 80931C28 E6000068 */  swc1    $f0, 0x0068($s0)           ## 00000068
.L80931C2C:
/* 0565C 80931C2C 0C00BE5D */  jal     func_8002F974              
/* 05660 80931C30 24053167 */  addiu   $a1, $zero, 0x3167         ## $a1 = 00003167
/* 05664 80931C34 920203E4 */  lbu     $v0, 0x03E4($s0)           ## 000003E4
.L80931C38:
/* 05668 80931C38 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 0566C 80931C3C 24053965 */  addiu   $a1, $zero, 0x3965         ## $a1 = 00003965
/* 05670 80931C40 304A0002 */  andi    $t2, $v0, 0x0002           ## $t2 = 00000000
/* 05674 80931C44 1140000E */  beq     $t2, $zero, .L80931C80     
/* 05678 80931C48 304BFFFC */  andi    $t3, $v0, 0xFFFC           ## $t3 = 00000000
/* 0567C 80931C4C 0C00BE0A */  jal     Audio_PlayActorSound2
              
/* 05680 80931C50 A20B03E4 */  sb      $t3, 0x03E4($s0)           ## 000003E4
/* 05684 80931C54 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 05688 80931C58 0C24CE37 */  jal     func_809338DC              
/* 0568C 80931C5C 8FA50034 */  lw      $a1, 0x0034($sp)           
/* 05690 80931C60 86020198 */  lh      $v0, 0x0198($s0)           ## 00000198
/* 05694 80931C64 240C0005 */  addiu   $t4, $zero, 0x0005         ## $t4 = 00000005
/* 05698 80931C68 28410006 */  slti    $at, $v0, 0x0006           
/* 0569C 80931C6C 54200004 */  bnel    $at, $zero, .L80931C80     
/* 056A0 80931C70 A6020198 */  sh      $v0, 0x0198($s0)           ## 00000198
/* 056A4 80931C74 10000002 */  beq     $zero, $zero, .L80931C80   
/* 056A8 80931C78 A60C0198 */  sh      $t4, 0x0198($s0)           ## 00000198
/* 056AC 80931C7C A6020198 */  sh      $v0, 0x0198($s0)           ## 00000198
.L80931C80:
/* 056B0 80931C80 0C01DE1C */  jal     Math_SinS
              ## sins?
/* 056B4 80931C84 86040032 */  lh      $a0, 0x0032($s0)           ## 00000032
/* 056B8 80931C88 C6060068 */  lwc1    $f6, 0x0068($s0)           ## 00000068
/* 056BC 80931C8C C6080024 */  lwc1    $f8, 0x0024($s0)           ## 00000024
/* 056C0 80931C90 86040032 */  lh      $a0, 0x0032($s0)           ## 00000032
/* 056C4 80931C94 46003282 */  mul.s   $f10, $f6, $f0             
/* 056C8 80931C98 460A4480 */  add.s   $f18, $f8, $f10            
/* 056CC 80931C9C 0C01DE0D */  jal     Math_CosS
              ## coss?
/* 056D0 80931CA0 E6120024 */  swc1    $f18, 0x0024($s0)          ## 00000024
/* 056D4 80931CA4 C6040068 */  lwc1    $f4, 0x0068($s0)           ## 00000068
/* 056D8 80931CA8 C610002C */  lwc1    $f16, 0x002C($s0)          ## 0000002C
/* 056DC 80931CAC 8FA2002C */  lw      $v0, 0x002C($sp)           
/* 056E0 80931CB0 46002182 */  mul.s   $f6, $f4, $f0              
/* 056E4 80931CB4 46068200 */  add.s   $f8, $f16, $f6             
/* 056E8 80931CB8 E608002C */  swc1    $f8, 0x002C($s0)           ## 0000002C
/* 056EC 80931CBC 8C4D0680 */  lw      $t5, 0x0680($v0)           ## 00000680
/* 056F0 80931CC0 31AE0080 */  andi    $t6, $t5, 0x0080           ## $t6 = 00000000
/* 056F4 80931CC4 51C0000B */  beql    $t6, $zero, .L80931CF4     
/* 056F8 80931CC8 8FBF0024 */  lw      $ra, 0x0024($sp)           
/* 056FC 80931CCC A4400850 */  sh      $zero, 0x0850($v0)         ## 00000850
/* 05700 80931CD0 8E180024 */  lw      $t8, 0x0024($s0)           ## 00000024
/* 05704 80931CD4 AC580024 */  sw      $t8, 0x0024($v0)           ## 00000024
/* 05708 80931CD8 8E0F0028 */  lw      $t7, 0x0028($s0)           ## 00000028
/* 0570C 80931CDC AC4F0028 */  sw      $t7, 0x0028($v0)           ## 00000028
/* 05710 80931CE0 8E18002C */  lw      $t8, 0x002C($s0)           ## 0000002C
/* 05714 80931CE4 AC58002C */  sw      $t8, 0x002C($v0)           ## 0000002C
/* 05718 80931CE8 861900B6 */  lh      $t9, 0x00B6($s0)           ## 000000B6
/* 0571C 80931CEC A45900B6 */  sh      $t9, 0x00B6($v0)           ## 000000B6
/* 05720 80931CF0 8FBF0024 */  lw      $ra, 0x0024($sp)           
.L80931CF4:
/* 05724 80931CF4 8FB00020 */  lw      $s0, 0x0020($sp)           
/* 05728 80931CF8 27BD0030 */  addiu   $sp, $sp, 0x0030           ## $sp = 00000000
/* 0572C 80931CFC 03E00008 */  jr      $ra                        
/* 05730 80931D00 00000000 */  nop
