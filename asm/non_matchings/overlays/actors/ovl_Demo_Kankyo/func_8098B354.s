.rdata
glabel D_8098CA14
    .asciz "../z_demo_kankyo.c"
    .balign 4

glabel D_8098CA28
    .asciz "../z_demo_kankyo.c"
    .balign 4

glabel D_8098CA3C
    .asciz "../z_demo_kankyo.c"
    .balign 4

.late_rodata
glabel D_8098CB54
    .float 0.2

glabel D_8098CB58
    .float 0.02

glabel D_8098CB5C
 .word 0x3C8EFA35

.text
glabel func_8098B354
/* 024D4 8098B354 27BDFF10 */  addiu   $sp, $sp, 0xFF10           ## $sp = FFFFFF10
/* 024D8 8098B358 AFB60068 */  sw      $s6, 0x0068($sp)           
/* 024DC 8098B35C 00A0B025 */  or      $s6, $a1, $zero            ## $s6 = 00000000
/* 024E0 8098B360 AFBF0074 */  sw      $ra, 0x0074($sp)           
/* 024E4 8098B364 AFBE0070 */  sw      $s8, 0x0070($sp)           
/* 024E8 8098B368 AFB7006C */  sw      $s7, 0x006C($sp)           
/* 024EC 8098B36C AFB50064 */  sw      $s5, 0x0064($sp)           
/* 024F0 8098B370 AFB40060 */  sw      $s4, 0x0060($sp)           
/* 024F4 8098B374 AFB3005C */  sw      $s3, 0x005C($sp)           
/* 024F8 8098B378 AFB20058 */  sw      $s2, 0x0058($sp)           
/* 024FC 8098B37C AFB10054 */  sw      $s1, 0x0054($sp)           
/* 02500 8098B380 AFB00050 */  sw      $s0, 0x0050($sp)           
/* 02504 8098B384 F7BE0048 */  sdc1    $f30, 0x0048($sp)          
/* 02508 8098B388 F7BC0040 */  sdc1    $f28, 0x0040($sp)          
/* 0250C 8098B38C F7BA0038 */  sdc1    $f26, 0x0038($sp)          
/* 02510 8098B390 F7B80030 */  sdc1    $f24, 0x0030($sp)          
/* 02514 8098B394 F7B60028 */  sdc1    $f22, 0x0028($sp)          
/* 02518 8098B398 F7B40020 */  sdc1    $f20, 0x0020($sp)          
/* 0251C 8098B39C 8CA50000 */  lw      $a1, 0x0000($a1)           ## 00000000
/* 02520 8098B3A0 00808825 */  or      $s1, $a0, $zero            ## $s1 = 00000000
/* 02524 8098B3A4 3C068099 */  lui     $a2, %hi(D_8098CA14)       ## $a2 = 80990000
/* 02528 8098B3A8 24C6CA14 */  addiu   $a2, $a2, %lo(D_8098CA14)  ## $a2 = 8098CA14
/* 0252C 8098B3AC 27A400A0 */  addiu   $a0, $sp, 0x00A0           ## $a0 = FFFFFFB0
/* 02530 8098B3B0 24070982 */  addiu   $a3, $zero, 0x0982         ## $a3 = 00000982
/* 02534 8098B3B4 0C031AB1 */  jal     Graph_OpenDisps              
/* 02538 8098B3B8 00A09825 */  or      $s3, $a1, $zero            ## $s3 = 00000000
/* 0253C 8098B3BC 9223014D */  lbu     $v1, 0x014D($s1)           ## 0000014D
/* 02540 8098B3C0 3C178099 */  lui     $s7, %hi(D_8098C4F0)       ## $s7 = 80990000
/* 02544 8098B3C4 26F7C4F0 */  addiu   $s7, $s7, %lo(D_8098C4F0)  ## $s7 = 8098C4F0
/* 02548 8098B3C8 28610014 */  slti    $at, $v1, 0x0014           
/* 0254C 8098B3CC 10200004 */  beq     $at, $zero, .L8098B3E0     
/* 02550 8098B3D0 00601025 */  or      $v0, $v1, $zero            ## $v0 = 00000000
/* 02554 8098B3D4 246E0001 */  addiu   $t6, $v1, 0x0001           ## $t6 = 00000001
/* 02558 8098B3D8 A22E014D */  sb      $t6, 0x014D($s1)           ## 0000014D
/* 0255C 8098B3DC 31C200FF */  andi    $v0, $t6, 0x00FF           ## $v0 = 00000001
.L8098B3E0:
/* 02560 8098B3E0 2454FFFF */  addiu   $s4, $v0, 0xFFFF           ## $s4 = 00000000
/* 02564 8098B3E4 0014A400 */  sll     $s4, $s4, 16               
/* 02568 8098B3E8 0014A403 */  sra     $s4, $s4, 16               
/* 0256C 8098B3EC 0680012F */  bltz    $s4, .L8098B8AC            
/* 02570 8098B3F0 3C018099 */  lui     $at, %hi(D_8098CB54)       ## $at = 80990000
/* 02574 8098B3F4 C43ECB54 */  lwc1    $f30, %lo(D_8098CB54)($at) 
/* 02578 8098B3F8 3C013F80 */  lui     $at, 0x3F80                ## $at = 3F800000
/* 0257C 8098B3FC 4481D000 */  mtc1    $at, $f26                  ## $f26 = 1.00
/* 02580 8098B400 3C014180 */  lui     $at, 0x4180                ## $at = 41800000
/* 02584 8098B404 4481C000 */  mtc1    $at, $f24                  ## $f24 = 16.00
/* 02588 8098B408 3C013F00 */  lui     $at, 0x3F00                ## $at = 3F000000
/* 0258C 8098B40C 4481B000 */  mtc1    $at, $f22                  ## $f22 = 0.50
/* 02590 8098B410 4480E000 */  mtc1    $zero, $f28                ## $f28 = 0.00
/* 02594 8098B414 241E0001 */  addiu   $s8, $zero, 0x0001         ## $s8 = 00000001
/* 02598 8098B418 9223014D */  lbu     $v1, 0x014D($s1)           ## 0000014D
.L8098B41C:
/* 0259C 8098B41C 44942000 */  mtc1    $s4, $f4                   ## $f4 = 0.00
/* 025A0 8098B420 3C014F80 */  lui     $at, 0x4F80                ## $at = 4F800000
/* 025A4 8098B424 44834000 */  mtc1    $v1, $f8                   ## $f8 = 0.00
/* 025A8 8098B428 468021A0 */  cvt.s.w $f6, $f4                   
/* 025AC 8098B42C 04610004 */  bgez    $v1, .L8098B440            
/* 025B0 8098B430 468042A0 */  cvt.s.w $f10, $f8                  
/* 025B4 8098B434 44818000 */  mtc1    $at, $f16                  ## $f16 = 4294967296.00
/* 025B8 8098B438 00000000 */  nop
/* 025BC 8098B43C 46105280 */  add.s   $f10, $f10, $f16           
.L8098B440:
/* 025C0 8098B440 460A3483 */  div.s   $f18, $f6, $f10            
/* 025C4 8098B444 00147880 */  sll     $t7, $s4,  2               
/* 025C8 8098B448 01F47821 */  addu    $t7, $t7, $s4              
/* 025CC 8098B44C 000F78C0 */  sll     $t7, $t7,  3               
/* 025D0 8098B450 022F8021 */  addu    $s0, $s1, $t7              
/* 025D4 8098B454 92020172 */  lbu     $v0, 0x0172($s0)           ## 00000172
/* 025D8 8098B458 10400007 */  beq     $v0, $zero, .L8098B478     
/* 025DC 8098B45C 4612D501 */  sub.s   $f20, $f26, $f18           
/* 025E0 8098B460 105E0035 */  beq     $v0, $s8, .L8098B538       
/* 025E4 8098B464 24010002 */  addiu   $at, $zero, 0x0002         ## $at = 00000002
/* 025E8 8098B468 1041004D */  beq     $v0, $at, .L8098B5A0       
/* 025EC 8098B46C 268E0001 */  addiu   $t6, $s4, 0x0001           ## $t6 = 00000001
/* 025F0 8098B470 10000052 */  beq     $zero, $zero, .L8098B5BC   
/* 025F4 8098B474 00000000 */  nop
.L8098B478:
/* 025F8 8098B478 44802000 */  mtc1    $zero, $f4                 ## $f4 = 0.00
/* 025FC 8098B47C A6000170 */  sh      $zero, 0x0170($s0)         ## 00000170
/* 02600 8098B480 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02604 8098B484 E604016C */  swc1    $f4, 0x016C($s0)           ## 0000016C
/* 02608 8098B488 46160201 */  sub.s   $f8, $f0, $f22             
/* 0260C 8098B48C 46184402 */  mul.s   $f16, $f8, $f24            
/* 02610 8098B490 00000000 */  nop
/* 02614 8098B494 46148182 */  mul.s   $f6, $f16, $f20            
/* 02618 8098B498 4600328D */  trunc.w.s $f10, $f6                  
/* 0261C 8098B49C 44195000 */  mfc1    $t9, $f10                  
/* 02620 8098B4A0 00000000 */  nop
/* 02624 8098B4A4 00194400 */  sll     $t0, $t9, 16               
/* 02628 8098B4A8 00084C03 */  sra     $t1, $t0, 16               
/* 0262C 8098B4AC 44899000 */  mtc1    $t1, $f18                  ## $f18 = 0.00
/* 02630 8098B4B0 00000000 */  nop
/* 02634 8098B4B4 46809120 */  cvt.s.w $f4, $f18                  
/* 02638 8098B4B8 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 0263C 8098B4BC E6040150 */  swc1    $f4, 0x0150($s0)           ## 00000150
/* 02640 8098B4C0 46160201 */  sub.s   $f8, $f0, $f22             
/* 02644 8098B4C4 46184402 */  mul.s   $f16, $f8, $f24            
/* 02648 8098B4C8 00000000 */  nop
/* 0264C 8098B4CC 46148182 */  mul.s   $f6, $f16, $f20            
/* 02650 8098B4D0 4600328D */  trunc.w.s $f10, $f6                  
/* 02654 8098B4D4 440B5000 */  mfc1    $t3, $f10                  
/* 02658 8098B4D8 00000000 */  nop
/* 0265C 8098B4DC 000B6400 */  sll     $t4, $t3, 16               
/* 02660 8098B4E0 000C6C03 */  sra     $t5, $t4, 16               
/* 02664 8098B4E4 448D9000 */  mtc1    $t5, $f18                  ## $f18 = 0.00
/* 02668 8098B4E8 00000000 */  nop
/* 0266C 8098B4EC 46809120 */  cvt.s.w $f4, $f18                  
/* 02670 8098B4F0 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02674 8098B4F4 E6040154 */  swc1    $f4, 0x0154($s0)           ## 00000154
/* 02678 8098B4F8 46160201 */  sub.s   $f8, $f0, $f22             
/* 0267C 8098B4FC 92080172 */  lbu     $t0, 0x0172($s0)           ## 00000172
/* 02680 8098B500 A2000173 */  sb      $zero, 0x0173($s0)         ## 00000173
/* 02684 8098B504 46184402 */  mul.s   $f16, $f8, $f24            
/* 02688 8098B508 25090001 */  addiu   $t1, $t0, 0x0001           ## $t1 = 00000001
/* 0268C 8098B50C A2090172 */  sb      $t1, 0x0172($s0)           ## 00000172
/* 02690 8098B510 46148182 */  mul.s   $f6, $f16, $f20            
/* 02694 8098B514 4600328D */  trunc.w.s $f10, $f6                  
/* 02698 8098B518 440F5000 */  mfc1    $t7, $f10                  
/* 0269C 8098B51C 00000000 */  nop
/* 026A0 8098B520 000FC400 */  sll     $t8, $t7, 16               
/* 026A4 8098B524 0018CC03 */  sra     $t9, $t8, 16               
/* 026A8 8098B528 44999000 */  mtc1    $t9, $f18                  ## $f18 = 0.00
/* 026AC 8098B52C 00000000 */  nop
/* 026B0 8098B530 46809120 */  cvt.s.w $f4, $f18                  
/* 026B4 8098B534 E6040158 */  swc1    $f4, 0x0158($s0)           ## 00000158
.L8098B538:
/* 026B8 8098B538 3C058099 */  lui     $a1, %hi(D_8098CFA4)       ## $a1 = 80990000
/* 026BC 8098B53C 3C068099 */  lui     $a2, %hi(D_8098CFA8)       ## $a2 = 80990000
/* 026C0 8098B540 3C078099 */  lui     $a3, %hi(D_8098C504)       ## $a3 = 80990000
/* 026C4 8098B544 260A0170 */  addiu   $t2, $s0, 0x0170           ## $t2 = 00000170
/* 026C8 8098B548 260B016C */  addiu   $t3, $s0, 0x016C           ## $t3 = 0000016C
/* 026CC 8098B54C AFAB0014 */  sw      $t3, 0x0014($sp)           
/* 026D0 8098B550 AFAA0010 */  sw      $t2, 0x0010($sp)           
/* 026D4 8098B554 24E7C504 */  addiu   $a3, $a3, %lo(D_8098C504)  ## $a3 = 8098C504
/* 026D8 8098B558 24C6CFA8 */  addiu   $a2, $a2, %lo(D_8098CFA8)  ## $a2 = 8098CFA8
/* 026DC 8098B55C 24A5CFA4 */  addiu   $a1, $a1, %lo(D_8098CFA4)  ## $a1 = 8098CFA4
/* 026E0 8098B560 0C02ECAD */  jal     func_800BB2B4              
/* 026E4 8098B564 27A400D4 */  addiu   $a0, $sp, 0x00D4           ## $a0 = FFFFFFE4
/* 026E8 8098B568 10400004 */  beq     $v0, $zero, .L8098B57C     
/* 026EC 8098B56C 27A400B4 */  addiu   $a0, $sp, 0x00B4           ## $a0 = FFFFFFC4
/* 026F0 8098B570 920C0172 */  lbu     $t4, 0x0172($s0)           ## 00000172
/* 026F4 8098B574 258D0001 */  addiu   $t5, $t4, 0x0001           ## $t5 = 00000001
/* 026F8 8098B578 A20D0172 */  sb      $t5, 0x0172($s0)           ## 00000172
.L8098B57C:
/* 026FC 8098B57C 0C00BBC5 */  jal     func_8002EF14              
/* 02700 8098B580 02202825 */  or      $a1, $s1, $zero            ## $a1 = 00000000
/* 02704 8098B584 3C068099 */  lui     $a2, %hi(D_8098CFB8)       ## $a2 = 80990000
/* 02708 8098B588 24C6CFB8 */  addiu   $a2, $a2, %lo(D_8098CFB8)  ## $a2 = 8098CFB8
/* 0270C 8098B58C 27A400B4 */  addiu   $a0, $sp, 0x00B4           ## $a0 = FFFFFFC4
/* 02710 8098B590 0C262ADA */  jal     func_8098AB68              
/* 02714 8098B594 27A500D4 */  addiu   $a1, $sp, 0x00D4           ## $a1 = FFFFFFE4
/* 02718 8098B598 10000008 */  beq     $zero, $zero, .L8098B5BC   
/* 0271C 8098B59C 00000000 */  nop
.L8098B5A0:
/* 02720 8098B5A0 15C30006 */  bne     $t6, $v1, .L8098B5BC       
/* 02724 8098B5A4 00000000 */  nop
/* 02728 8098B5A8 92CF1D6C */  lbu     $t7, 0x1D6C($s6)           ## 00001D6C
/* 0272C 8098B5AC 15E00003 */  bne     $t7, $zero, .L8098B5BC     
/* 02730 8098B5B0 00000000 */  nop
/* 02734 8098B5B4 0C00B55C */  jal     Actor_Kill
              
/* 02738 8098B5B8 02202025 */  or      $a0, $s1, $zero            ## $a0 = 00000000
.L8098B5BC:
/* 0273C 8098B5BC 3C018099 */  lui     $at, %hi(D_8098CFB8)       ## $at = 80990000
/* 02740 8098B5C0 C428CFB8 */  lwc1    $f8, %lo(D_8098CFB8)($at)  
/* 02744 8098B5C4 92020173 */  lbu     $v0, 0x0173($s0)           ## 00000173
/* 02748 8098B5C8 E608015C */  swc1    $f8, 0x015C($s0)           ## 0000015C
/* 0274C 8098B5CC C430CFBC */  lwc1    $f16, %lo(D_8098CFBC)($at) 
/* 02750 8098B5D0 3C018099 */  lui     $at, %hi(D_8098CFC0)       ## $at = 80990000
/* 02754 8098B5D4 E6100160 */  swc1    $f16, 0x0160($s0)          ## 00000160
/* 02758 8098B5D8 C426CFC0 */  lwc1    $f6, %lo(D_8098CFC0)($at)  
/* 0275C 8098B5DC 10400007 */  beq     $v0, $zero, .L8098B5FC     
/* 02760 8098B5E0 E6060164 */  swc1    $f6, 0x0164($s0)           ## 00000164
/* 02764 8098B5E4 105E000B */  beq     $v0, $s8, .L8098B614       
/* 02768 8098B5E8 24010002 */  addiu   $at, $zero, 0x0002         ## $at = 00000002
/* 0276C 8098B5EC 10410018 */  beq     $v0, $at, .L8098B650       
/* 02770 8098B5F0 26040168 */  addiu   $a0, $s0, 0x0168           ## $a0 = 00000168
/* 02774 8098B5F4 10000050 */  beq     $zero, $zero, .L8098B738   
/* 02778 8098B5F8 920C0172 */  lbu     $t4, 0x0172($s0)           ## 00000172
.L8098B5FC:
/* 0277C 8098B5FC 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02780 8098B600 00000000 */  nop
/* 02784 8098B604 92180173 */  lbu     $t8, 0x0173($s0)           ## 00000173
/* 02788 8098B608 E6000168 */  swc1    $f0, 0x0168($s0)           ## 00000168
/* 0278C 8098B60C 27190001 */  addiu   $t9, $t8, 0x0001           ## $t9 = 00000001
/* 02790 8098B610 A2190173 */  sb      $t9, 0x0173($s0)           ## 00000173
.L8098B614:
/* 02794 8098B614 4405D000 */  mfc1    $a1, $f26                  
/* 02798 8098B618 4406B000 */  mfc1    $a2, $f22                  
/* 0279C 8098B61C 3C073ECC */  lui     $a3, 0x3ECC                ## $a3 = 3ECC0000
/* 027A0 8098B620 34E7CCCD */  ori     $a3, $a3, 0xCCCD           ## $a3 = 3ECCCCCD
/* 027A4 8098B624 26040168 */  addiu   $a0, $s0, 0x0168           ## $a0 = 00000168
/* 027A8 8098B628 0C01E0C4 */  jal     Math_SmoothStepToF
              
/* 027AC 8098B62C E7BE0010 */  swc1    $f30, 0x0010($sp)          
/* 027B0 8098B630 C60A0168 */  lwc1    $f10, 0x0168($s0)          ## 00000168
/* 027B4 8098B634 24080002 */  addiu   $t0, $zero, 0x0002         ## $t0 = 00000002
/* 027B8 8098B638 460AD03E */  c.le.s  $f26, $f10                 
/* 027BC 8098B63C 00000000 */  nop
/* 027C0 8098B640 4502003D */  bc1fl   .L8098B738                 
/* 027C4 8098B644 920C0172 */  lbu     $t4, 0x0172($s0)           ## 00000172
/* 027C8 8098B648 1000003A */  beq     $zero, $zero, .L8098B734   
/* 027CC 8098B64C A2080173 */  sb      $t0, 0x0173($s0)           ## 00000173
.L8098B650:
/* 027D0 8098B650 4405E000 */  mfc1    $a1, $f28                  
/* 027D4 8098B654 4406B000 */  mfc1    $a2, $f22                  
/* 027D8 8098B658 3C073E99 */  lui     $a3, 0x3E99                ## $a3 = 3E990000
/* 027DC 8098B65C 34E7999A */  ori     $a3, $a3, 0x999A           ## $a3 = 3E99999A
/* 027E0 8098B660 0C01E0C4 */  jal     Math_SmoothStepToF
              
/* 027E4 8098B664 E7BE0010 */  swc1    $f30, 0x0010($sp)          
/* 027E8 8098B668 C6120168 */  lwc1    $f18, 0x0168($s0)          ## 00000168
/* 027EC 8098B66C 461C903E */  c.le.s  $f18, $f28                 
/* 027F0 8098B670 00000000 */  nop
/* 027F4 8098B674 45020030 */  bc1fl   .L8098B738                 
/* 027F8 8098B678 920C0172 */  lbu     $t4, 0x0172($s0)           ## 00000172
/* 027FC 8098B67C 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02800 8098B680 00000000 */  nop
/* 02804 8098B684 46160101 */  sub.s   $f4, $f0, $f22             
/* 02808 8098B688 46182202 */  mul.s   $f8, $f4, $f24             
/* 0280C 8098B68C 00000000 */  nop
/* 02810 8098B690 46144402 */  mul.s   $f16, $f8, $f20            
/* 02814 8098B694 4600818D */  trunc.w.s $f6, $f16                  
/* 02818 8098B698 440A3000 */  mfc1    $t2, $f6                   
/* 0281C 8098B69C 00000000 */  nop
/* 02820 8098B6A0 000A5C00 */  sll     $t3, $t2, 16               
/* 02824 8098B6A4 000B6403 */  sra     $t4, $t3, 16               
/* 02828 8098B6A8 448C5000 */  mtc1    $t4, $f10                  ## $f10 = 0.00
/* 0282C 8098B6AC 00000000 */  nop
/* 02830 8098B6B0 468054A0 */  cvt.s.w $f18, $f10                 
/* 02834 8098B6B4 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02838 8098B6B8 E6120150 */  swc1    $f18, 0x0150($s0)          ## 00000150
/* 0283C 8098B6BC 46160101 */  sub.s   $f4, $f0, $f22             
/* 02840 8098B6C0 46182202 */  mul.s   $f8, $f4, $f24             
/* 02844 8098B6C4 00000000 */  nop
/* 02848 8098B6C8 46144402 */  mul.s   $f16, $f8, $f20            
/* 0284C 8098B6CC 4600818D */  trunc.w.s $f6, $f16                  
/* 02850 8098B6D0 440E3000 */  mfc1    $t6, $f6                   
/* 02854 8098B6D4 00000000 */  nop
/* 02858 8098B6D8 000E7C00 */  sll     $t7, $t6, 16               
/* 0285C 8098B6DC 000FC403 */  sra     $t8, $t7, 16               
/* 02860 8098B6E0 44985000 */  mtc1    $t8, $f10                  ## $f10 = 0.00
/* 02864 8098B6E4 00000000 */  nop
/* 02868 8098B6E8 468054A0 */  cvt.s.w $f18, $f10                 
/* 0286C 8098B6EC 0C03F66B */  jal     Rand_ZeroOne
              ## Rand.Next() float
/* 02870 8098B6F0 E6120154 */  swc1    $f18, 0x0154($s0)          ## 00000154
/* 02874 8098B6F4 46160101 */  sub.s   $f4, $f0, $f22             
/* 02878 8098B6F8 240B0001 */  addiu   $t3, $zero, 0x0001         ## $t3 = 00000001
/* 0287C 8098B6FC E61C0168 */  swc1    $f28, 0x0168($s0)          ## 00000168
/* 02880 8098B700 A20B0173 */  sb      $t3, 0x0173($s0)           ## 00000173
/* 02884 8098B704 46182202 */  mul.s   $f8, $f4, $f24             
/* 02888 8098B708 00000000 */  nop
/* 0288C 8098B70C 46144402 */  mul.s   $f16, $f8, $f20            
/* 02890 8098B710 4600818D */  trunc.w.s $f6, $f16                  
/* 02894 8098B714 44083000 */  mfc1    $t0, $f6                   
/* 02898 8098B718 00000000 */  nop
/* 0289C 8098B71C 00084C00 */  sll     $t1, $t0, 16               
/* 028A0 8098B720 00095403 */  sra     $t2, $t1, 16               
/* 028A4 8098B724 448A5000 */  mtc1    $t2, $f10                  ## $f10 = 0.00
/* 028A8 8098B728 00000000 */  nop
/* 028AC 8098B72C 468054A0 */  cvt.s.w $f18, $f10                 
/* 028B0 8098B730 E6120158 */  swc1    $f18, 0x0158($s0)          ## 00000158
.L8098B734:
/* 028B4 8098B734 920C0172 */  lbu     $t4, 0x0172($s0)           ## 00000172
.L8098B738:
/* 028B8 8098B738 C6120150 */  lwc1    $f18, 0x0150($s0)          ## 00000150
/* 028BC 8098B73C C610015C */  lwc1    $f16, 0x015C($s0)          ## 0000015C
/* 028C0 8098B740 C60A0154 */  lwc1    $f10, 0x0154($s0)          ## 00000154
/* 028C4 8098B744 C6080160 */  lwc1    $f8, 0x0160($s0)           ## 00000160
/* 028C8 8098B748 46109300 */  add.s   $f12, $f18, $f16           
/* 028CC 8098B74C C6100158 */  lwc1    $f16, 0x0158($s0)          ## 00000158
/* 028D0 8098B750 C6120164 */  lwc1    $f18, 0x0164($s0)          ## 00000164
/* 028D4 8098B754 46085380 */  add.s   $f14, $f10, $f8            
/* 028D8 8098B758 29810002 */  slti    $at, $t4, 0x0002           
/* 028DC 8098B75C 1020004E */  beq     $at, $zero, .L8098B898     
/* 028E0 8098B760 46128000 */  add.s   $f0, $f16, $f18            
/* 028E4 8098B764 3C010001 */  lui     $at, 0x0001                ## $at = 00010000
/* 028E8 8098B768 34211DA0 */  ori     $at, $at, 0x1DA0           ## $at = 00011DA0
/* 028EC 8098B76C 44060000 */  mfc1    $a2, $f0                   
/* 028F0 8098B770 3C150401 */  lui     $s5, %hi(gGameKeepMoteDL0)                ## $s5 = 04010000
/* 028F4 8098B774 26B50130 */  addiu   $s5, $s5, %lo(gGameKeepMoteDL0)           ## $s5 = 04010130
/* 028F8 8098B778 02C19021 */  addu    $s2, $s6, $at              
/* 028FC 8098B77C 0C034261 */  jal     Matrix_Translate              
/* 02900 8098B780 00003825 */  or      $a3, $zero, $zero          ## $a3 = 00000000
/* 02904 8098B784 3C018099 */  lui     $at, %hi(D_8098CB58)       ## $at = 80990000
/* 02908 8098B788 C428CB58 */  lwc1    $f8, %lo(D_8098CB58)($at)  
/* 0290C 8098B78C C6040168 */  lwc1    $f4, 0x0168($s0)           ## 00000168
/* 02910 8098B790 24070001 */  addiu   $a3, $zero, 0x0001         ## $a3 = 00000001
/* 02914 8098B794 46144402 */  mul.s   $f16, $f8, $f20            
/* 02918 8098B798 00000000 */  nop
/* 0291C 8098B79C 46102302 */  mul.s   $f12, $f4, $f16            
/* 02920 8098B7A0 44066000 */  mfc1    $a2, $f12                  
/* 02924 8098B7A4 0C0342A3 */  jal     Matrix_Scale              
/* 02928 8098B7A8 46006386 */  mov.s   $f14, $f12                 
/* 0292C 8098B7AC 8E6202D0 */  lw      $v0, 0x02D0($s3)           ## 000002D0
/* 02930 8098B7B0 3C0EFA00 */  lui     $t6, 0xFA00                ## $t6 = FA000000
/* 02934 8098B7B4 35CE0080 */  ori     $t6, $t6, 0x0080           ## $t6 = FA000080
/* 02938 8098B7B8 244D0008 */  addiu   $t5, $v0, 0x0008           ## $t5 = 00000008
/* 0293C 8098B7BC AE6D02D0 */  sw      $t5, 0x02D0($s3)           ## 000002D0
/* 02940 8098B7C0 240FFFFF */  addiu   $t7, $zero, 0xFFFF         ## $t7 = FFFFFFFF
/* 02944 8098B7C4 AC4F0004 */  sw      $t7, 0x0004($v0)           ## 00000004
/* 02948 8098B7C8 AC4E0000 */  sw      $t6, 0x0000($v0)           ## 00000000
/* 0294C 8098B7CC 8E6202D0 */  lw      $v0, 0x02D0($s3)           ## 000002D0
/* 02950 8098B7D0 3C19FB00 */  lui     $t9, 0xFB00                ## $t9 = FB000000
/* 02954 8098B7D4 24580008 */  addiu   $t8, $v0, 0x0008           ## $t8 = 00000008
/* 02958 8098B7D8 AE7802D0 */  sw      $t8, 0x02D0($s3)           ## 000002D0
/* 0295C 8098B7DC AC590000 */  sw      $t9, 0x0000($v0)           ## 00000000
/* 02960 8098B7E0 92EC0009 */  lbu     $t4, 0x0009($s7)           ## 8098C4F9
/* 02964 8098B7E4 92E9000B */  lbu     $t1, 0x000B($s7)           ## 8098C4FB
/* 02968 8098B7E8 92F8000A */  lbu     $t8, 0x000A($s7)           ## 8098C4FA
/* 0296C 8098B7EC 000C6E00 */  sll     $t5, $t4, 24               
/* 02970 8098B7F0 00095200 */  sll     $t2, $t1,  8               
/* 02974 8098B7F4 014D7025 */  or      $t6, $t2, $t5              ## $t6 = 00000008
/* 02978 8098B7F8 0018CC00 */  sll     $t9, $t8, 16               
/* 0297C 8098B7FC 01D94025 */  or      $t0, $t6, $t9              ## $t0 = FB000008
/* 02980 8098B800 350900FF */  ori     $t1, $t0, 0x00FF           ## $t1 = FB0000FF
/* 02984 8098B804 AC490004 */  sw      $t1, 0x0004($v0)           ## 00000004
/* 02988 8098B808 0C024F61 */  jal     func_80093D84              
/* 0298C 8098B80C 8EC40000 */  lw      $a0, 0x0000($s6)           ## 00000000
/* 02990 8098B810 02402025 */  or      $a0, $s2, $zero            ## $a0 = 00000000
/* 02994 8098B814 0C03424C */  jal     Matrix_Mult              
/* 02998 8098B818 24050001 */  addiu   $a1, $zero, 0x0001         ## $a1 = 00000001
/* 0299C 8098B81C 860B0174 */  lh      $t3, 0x0174($s0)           ## 00000174
/* 029A0 8098B820 3C018099 */  lui     $at, %hi(D_8098CB5C)       ## $at = 80990000
/* 029A4 8098B824 C432CB5C */  lwc1    $f18, %lo(D_8098CB5C)($at) 
/* 029A8 8098B828 448B3000 */  mtc1    $t3, $f6                   ## $f6 = 0.00
/* 029AC 8098B82C 24050001 */  addiu   $a1, $zero, 0x0001         ## $a1 = 00000001
/* 029B0 8098B830 468032A0 */  cvt.s.w $f10, $f6                  
/* 029B4 8098B834 46125302 */  mul.s   $f12, $f10, $f18           
/* 029B8 8098B838 0C0343B5 */  jal     Matrix_RotateZ              
/* 029BC 8098B83C 00000000 */  nop
/* 029C0 8098B840 8E6202D0 */  lw      $v0, 0x02D0($s3)           ## 000002D0
/* 029C4 8098B844 3C0ADA38 */  lui     $t2, 0xDA38                ## $t2 = DA380000
/* 029C8 8098B848 354A0003 */  ori     $t2, $t2, 0x0003           ## $t2 = DA380003
/* 029CC 8098B84C 244C0008 */  addiu   $t4, $v0, 0x0008           ## $t4 = 00000008
/* 029D0 8098B850 AE6C02D0 */  sw      $t4, 0x02D0($s3)           ## 000002D0
/* 029D4 8098B854 3C058099 */  lui     $a1, %hi(D_8098CA28)       ## $a1 = 80990000
/* 029D8 8098B858 AC4A0000 */  sw      $t2, 0x0000($v0)           ## 00000000
/* 029DC 8098B85C 8EC40000 */  lw      $a0, 0x0000($s6)           ## 00000000
/* 029E0 8098B860 24A5CA28 */  addiu   $a1, $a1, %lo(D_8098CA28)  ## $a1 = 8098CA28
/* 029E4 8098B864 24060A0C */  addiu   $a2, $zero, 0x0A0C         ## $a2 = 00000A0C
/* 029E8 8098B868 0C0346A2 */  jal     Matrix_NewMtx              
/* 029EC 8098B86C 00409025 */  or      $s2, $v0, $zero            ## $s2 = 00000000
/* 029F0 8098B870 AE420004 */  sw      $v0, 0x0004($s2)           ## 00000004
/* 029F4 8098B874 8E6202D0 */  lw      $v0, 0x02D0($s3)           ## 000002D0
/* 029F8 8098B878 3C0FDE00 */  lui     $t7, 0xDE00                ## $t7 = DE000000
/* 029FC 8098B87C 244D0008 */  addiu   $t5, $v0, 0x0008           ## $t5 = 00000008
/* 02A00 8098B880 AE6D02D0 */  sw      $t5, 0x02D0($s3)           ## 000002D0
/* 02A04 8098B884 AC550004 */  sw      $s5, 0x0004($v0)           ## 00000004
/* 02A08 8098B888 AC4F0000 */  sw      $t7, 0x0000($v0)           ## 00000000
/* 02A0C 8098B88C 86180174 */  lh      $t8, 0x0174($s0)           ## 00000174
/* 02A10 8098B890 270E0190 */  addiu   $t6, $t8, 0x0190           ## $t6 = 00000190
/* 02A14 8098B894 A60E0174 */  sh      $t6, 0x0174($s0)           ## 00000174
.L8098B898:
/* 02A18 8098B898 2694FFFF */  addiu   $s4, $s4, 0xFFFF           ## $s4 = FFFFFFFF
/* 02A1C 8098B89C 0014A400 */  sll     $s4, $s4, 16               
/* 02A20 8098B8A0 0014A403 */  sra     $s4, $s4, 16               
/* 02A24 8098B8A4 0683FEDD */  bgezl   $s4, .L8098B41C            
/* 02A28 8098B8A8 9223014D */  lbu     $v1, 0x014D($s1)           ## 0000014D
.L8098B8AC:
/* 02A2C 8098B8AC 3C068099 */  lui     $a2, %hi(D_8098CA3C)       ## $a2 = 80990000
/* 02A30 8098B8B0 24C6CA3C */  addiu   $a2, $a2, %lo(D_8098CA3C)  ## $a2 = 8098CA3C
/* 02A34 8098B8B4 27A400A0 */  addiu   $a0, $sp, 0x00A0           ## $a0 = FFFFFFB0
/* 02A38 8098B8B8 8EC50000 */  lw      $a1, 0x0000($s6)           ## 00000000
/* 02A3C 8098B8BC 0C031AD5 */  jal     Graph_CloseDisps              
/* 02A40 8098B8C0 24070A13 */  addiu   $a3, $zero, 0x0A13         ## $a3 = 00000A13
/* 02A44 8098B8C4 8FBF0074 */  lw      $ra, 0x0074($sp)           
/* 02A48 8098B8C8 D7B40020 */  ldc1    $f20, 0x0020($sp)          
/* 02A4C 8098B8CC D7B60028 */  ldc1    $f22, 0x0028($sp)          
/* 02A50 8098B8D0 D7B80030 */  ldc1    $f24, 0x0030($sp)          
/* 02A54 8098B8D4 D7BA0038 */  ldc1    $f26, 0x0038($sp)          
/* 02A58 8098B8D8 D7BC0040 */  ldc1    $f28, 0x0040($sp)          
/* 02A5C 8098B8DC D7BE0048 */  ldc1    $f30, 0x0048($sp)          
/* 02A60 8098B8E0 8FB00050 */  lw      $s0, 0x0050($sp)           
/* 02A64 8098B8E4 8FB10054 */  lw      $s1, 0x0054($sp)           
/* 02A68 8098B8E8 8FB20058 */  lw      $s2, 0x0058($sp)           
/* 02A6C 8098B8EC 8FB3005C */  lw      $s3, 0x005C($sp)           
/* 02A70 8098B8F0 8FB40060 */  lw      $s4, 0x0060($sp)           
/* 02A74 8098B8F4 8FB50064 */  lw      $s5, 0x0064($sp)           
/* 02A78 8098B8F8 8FB60068 */  lw      $s6, 0x0068($sp)           
/* 02A7C 8098B8FC 8FB7006C */  lw      $s7, 0x006C($sp)           
/* 02A80 8098B900 8FBE0070 */  lw      $s8, 0x0070($sp)           
/* 02A84 8098B904 03E00008 */  jr      $ra                        
/* 02A88 8098B908 27BD00F0 */  addiu   $sp, $sp, 0x00F0           ## $sp = 00000000
/* 02A8C 8098B90C 00000000 */  nop
