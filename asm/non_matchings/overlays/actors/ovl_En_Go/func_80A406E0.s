.late_rodata

glabel D_80A422BC
    .float -0.1

.text
glabel func_80A406E0
/* 02170 80A406E0 27BDFFD8 */  addiu   $sp, $sp, 0xFFD8           ## $sp = FFFFFFD8
/* 02174 80A406E4 AFBF0024 */  sw      $ra, 0x0024($sp)           
/* 02178 80A406E8 AFB00020 */  sw      $s0, 0x0020($sp)           
/* 0217C 80A406EC AFA5002C */  sw      $a1, 0x002C($sp)           
/* 02180 80A406F0 8482001C */  lh      $v0, 0x001C($a0)           ## 0000001C
/* 02184 80A406F4 24010090 */  addiu   $at, $zero, 0x0090         ## $at = 00000090
/* 02188 80A406F8 00808025 */  or      $s0, $a0, $zero            ## $s0 = 00000000
/* 0218C 80A406FC 304200F0 */  andi    $v0, $v0, 0x00F0           ## $v0 = 00000000
/* 02190 80A40700 14410037 */  bne     $v0, $at, .L80A407E0       
/* 02194 80A40704 00000000 */  nop
/* 02198 80A40708 848E01E0 */  lh      $t6, 0x01E0($a0)           ## 000001E0
/* 0219C 80A4070C 24010002 */  addiu   $at, $zero, 0x0002         ## $at = 00000002
/* 021A0 80A40710 15C10033 */  bne     $t6, $at, .L80A407E0       
/* 021A4 80A40714 00000000 */  nop
/* 021A8 80A40718 3C028016 */  lui     $v0, %hi(gSaveContext)
/* 021AC 80A4071C 2442E660 */  addiu   $v0, %lo(gSaveContext)
/* 021B0 80A40720 904F003E */  lbu     $t7, 0x003E($v0)           ## 8015E69E
/* 021B4 80A40724 3C188012 */  lui     $t8, %hi(gItemSlots+0x2d)
/* 021B8 80A40728 11E00003 */  beq     $t7, $zero, .L80A40738     
/* 021BC 80A4072C 00000000 */  nop
/* 021C0 80A40730 10000064 */  beq     $zero, $zero, .L80A408C4   
/* 021C4 80A40734 A48001E0 */  sh      $zero, 0x01E0($a0)         ## 000001E0
.L80A40738:
/* 021C8 80A40738 93187491 */  lbu     $t8, %lo(gItemSlots+0x2d)($t8)
/* 021CC 80A4073C 24010036 */  addiu   $at, $zero, 0x0036         ## $at = 00000036
/* 021D0 80A40740 3C0580A4 */  lui     $a1, %hi(func_80A40B74)    ## $a1 = 80A40000
/* 021D4 80A40744 0058C821 */  addu    $t9, $v0, $t8              
/* 021D8 80A40748 93280074 */  lbu     $t0, 0x0074($t9)           ## 00000074
/* 021DC 80A4074C 24A50B74 */  addiu   $a1, $a1, %lo(func_80A40B74) ## $a1 = 80A40B74
/* 021E0 80A40750 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 021E4 80A40754 15010019 */  bne     $t0, $at, .L80A407BC       
/* 021E8 80A40758 00000000 */  nop
/* 021EC 80A4075C 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 021F0 80A40760 0C28FB78 */  jal     func_80A3EDE0              
/* 021F4 80A40764 24050002 */  addiu   $a1, $zero, 0x0002         ## $a1 = 00000002
/* 021F8 80A40768 24090064 */  addiu   $t1, $zero, 0x0064         ## $t1 = 00000064
/* 021FC 80A4076C 3C0580A4 */  lui     $a1, %hi(func_80A40D5C)    ## $a1 = 80A40000
/* 02200 80A40770 A609021E */  sh      $t1, 0x021E($s0)           ## 0000021E
/* 02204 80A40774 A60001E0 */  sh      $zero, 0x01E0($s0)         ## 000001E0
/* 02208 80A40778 24A50D5C */  addiu   $a1, $a1, %lo(func_80A40D5C) ## $a1 = 80A40D5C
/* 0220C 80A4077C 0C28F95C */  jal     func_80A3E570              
/* 02210 80A40780 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 02214 80A40784 8FA4002C */  lw      $a0, 0x002C($sp)           
/* 02218 80A40788 3C010001 */  lui     $at, 0x0001                ## $at = 00010000
/* 0221C 80A4078C 240A0037 */  addiu   $t2, $zero, 0x0037         ## $t2 = 00000037
/* 02220 80A40790 00240821 */  addu    $at, $at, $a0              
/* 02224 80A40794 A02A03DC */  sb      $t2, 0x03DC($at)           ## 000103DC
/* 02228 80A40798 3C018016 */  lui     $at, %hi(gSaveContext+0x13d2)
/* 0222C 80A4079C A420FA32 */  sh      $zero, %lo(gSaveContext+0x13d2)($at)
/* 02230 80A407A0 AFA00010 */  sw      $zero, 0x0010($sp)         
/* 02234 80A407A4 2405105E */  addiu   $a1, $zero, 0x105E         ## $a1 = 0000105E
/* 02238 80A407A8 2406FF9D */  addiu   $a2, $zero, 0xFF9D         ## $a2 = FFFFFF9D
/* 0223C 80A407AC 0C02003E */  jal     func_800800F8              
/* 02240 80A407B0 02003825 */  or      $a3, $s0, $zero            ## $a3 = 00000000
/* 02244 80A407B4 10000044 */  beq     $zero, $zero, .L80A408C8   
/* 02248 80A407B8 8FBF0024 */  lw      $ra, 0x0024($sp)           
.L80A407BC:
/* 0224C 80A407BC 0C28F95C */  jal     func_80A3E570              
/* 02250 80A407C0 A60001E0 */  sh      $zero, 0x01E0($s0)         ## 000001E0
/* 02254 80A407C4 0C041B33 */  jal     func_80106CCC              
/* 02258 80A407C8 8FA4002C */  lw      $a0, 0x002C($sp)           
/* 0225C 80A407CC 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 02260 80A407D0 0C2902DD */  jal     func_80A40B74              
/* 02264 80A407D4 8FA5002C */  lw      $a1, 0x002C($sp)           
/* 02268 80A407D8 1000003B */  beq     $zero, $zero, .L80A408C8   
/* 0226C 80A407DC 8FBF0024 */  lw      $ra, 0x0024($sp)           
.L80A407E0:
/* 02270 80A407E0 54400014 */  bnel    $v0, $zero, .L80A40834     
/* 02274 80A407E4 86020212 */  lh      $v0, 0x0212($s0)           ## 00000212
/* 02278 80A407E8 860B01E0 */  lh      $t3, 0x01E0($s0)           ## 000001E0
/* 0227C 80A407EC 24010002 */  addiu   $at, $zero, 0x0002         ## $at = 00000002
/* 02280 80A407F0 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 02284 80A407F4 1561000E */  bne     $t3, $at, .L80A40830       
/* 02288 80A407F8 3C0580A4 */  lui     $a1, %hi(func_80A40B74)    ## $a1 = 80A40000
/* 0228C 80A407FC 0C28F95C */  jal     func_80A3E570              
/* 02290 80A40800 24A50B74 */  addiu   $a1, $a1, %lo(func_80A40B74) ## $a1 = 80A40B74
/* 02294 80A40804 8FAD002C */  lw      $t5, 0x002C($sp)           
/* 02298 80A40808 3C010001 */  lui     $at, 0x0001                ## $at = 00010000
/* 0229C 80A4080C 240C0004 */  addiu   $t4, $zero, 0x0004         ## $t4 = 00000004
/* 022A0 80A40810 002D0821 */  addu    $at, $at, $t5              
/* 022A4 80A40814 A02C04BF */  sb      $t4, 0x04BF($at)           ## 000104BF
/* 022A8 80A40818 8FAF002C */  lw      $t7, 0x002C($sp)           
/* 022AC 80A4081C 3C010001 */  lui     $at, 0x0001                ## $at = 00010000
/* 022B0 80A40820 240E0036 */  addiu   $t6, $zero, 0x0036         ## $t6 = 00000036
/* 022B4 80A40824 002F0821 */  addu    $at, $at, $t7              
/* 022B8 80A40828 10000026 */  beq     $zero, $zero, .L80A408C4   
/* 022BC 80A4082C A02E03DC */  sb      $t6, 0x03DC($at)           ## 000103DC
.L80A40830:
/* 022C0 80A40830 86020212 */  lh      $v0, 0x0212($s0)           ## 00000212
.L80A40834:
/* 022C4 80A40834 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 022C8 80A40838 14400003 */  bne     $v0, $zero, .L80A40848     
/* 022CC 80A4083C 2458FFFF */  addiu   $t8, $v0, 0xFFFF           ## $t8 = FFFFFFFF
/* 022D0 80A40840 10000003 */  beq     $zero, $zero, .L80A40850   
/* 022D4 80A40844 00001825 */  or      $v1, $zero, $zero          ## $v1 = 00000000
.L80A40848:
/* 022D8 80A40848 A6180212 */  sh      $t8, 0x0212($s0)           ## 00000212
/* 022DC 80A4084C 86030212 */  lh      $v1, 0x0212($s0)           ## 00000212
.L80A40850:
/* 022E0 80A40850 5460001D */  bnel    $v1, $zero, .L80A408C8     
/* 022E4 80A40854 8FBF0024 */  lw      $ra, 0x0024($sp)           
/* 022E8 80A40858 0C28FC57 */  jal     func_80A3F15C              
/* 022EC 80A4085C 8FA5002C */  lw      $a1, 0x002C($sp)           
/* 022F0 80A40860 54400019 */  bnel    $v0, $zero, .L80A408C8     
/* 022F4 80A40864 8FBF0024 */  lw      $ra, 0x0024($sp)           
/* 022F8 80A40868 0C28FC98 */  jal     func_80A3F260              
/* 022FC 80A4086C 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 02300 80A40870 8619001C */  lh      $t9, 0x001C($s0)           ## 0000001C
/* 02304 80A40874 3C0180A4 */  lui     $at, %hi(D_80A422BC)       ## $at = 80A40000
/* 02308 80A40878 C42422BC */  lwc1    $f4, %lo(D_80A422BC)($at)  
/* 0230C 80A4087C 24010090 */  addiu   $at, $zero, 0x0090         ## $at = 00000090
/* 02310 80A40880 332800F0 */  andi    $t0, $t9, 0x00F0           ## $t0 = 00000000
/* 02314 80A40884 15010005 */  bne     $t0, $at, .L80A4089C       
/* 02318 80A40888 E6040168 */  swc1    $f4, 0x0168($s0)           ## 00000168
/* 0231C 80A4088C 3C013F00 */  lui     $at, 0x3F00                ## $at = 3F000000
/* 02320 80A40890 44810000 */  mtc1    $at, $f0                   ## $f0 = 0.50
/* 02324 80A40894 10000005 */  beq     $zero, $zero, .L80A408AC   
/* 02328 80A40898 C6060168 */  lwc1    $f6, 0x0168($s0)           ## 00000168
.L80A4089C:
/* 0232C 80A4089C 3C013F80 */  lui     $at, 0x3F80                ## $at = 3F800000
/* 02330 80A408A0 44810000 */  mtc1    $at, $f0                   ## $f0 = 1.00
/* 02334 80A408A4 00000000 */  nop
/* 02338 80A408A8 C6060168 */  lwc1    $f6, 0x0168($s0)           ## 00000168
.L80A408AC:
/* 0233C 80A408AC 3C0580A4 */  lui     $a1, %hi(func_80A408D8)    ## $a1 = 80A40000
/* 02340 80A408B0 24A508D8 */  addiu   $a1, $a1, %lo(func_80A408D8) ## $a1 = 80A408D8
/* 02344 80A408B4 46003202 */  mul.s   $f8, $f6, $f0              
/* 02348 80A408B8 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 0234C 80A408BC 0C28F95C */  jal     func_80A3E570              
/* 02350 80A408C0 E6080168 */  swc1    $f8, 0x0168($s0)           ## 00000168
.L80A408C4:
/* 02354 80A408C4 8FBF0024 */  lw      $ra, 0x0024($sp)           
.L80A408C8:
/* 02358 80A408C8 8FB00020 */  lw      $s0, 0x0020($sp)           
/* 0235C 80A408CC 27BD0028 */  addiu   $sp, $sp, 0x0028           ## $sp = 00000000
/* 02360 80A408D0 03E00008 */  jr      $ra                        
/* 02364 80A408D4 00000000 */  nop
