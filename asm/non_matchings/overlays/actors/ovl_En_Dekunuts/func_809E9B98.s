glabel func_809E9B98
/* 00638 809E9B98 27BDFFD0 */  addiu   $sp, $sp, 0xFFD0           ## $sp = FFFFFFD0
/* 0063C 809E9B9C AFBF001C */  sw      $ra, 0x001C($sp)           
/* 00640 809E9BA0 AFB00018 */  sw      $s0, 0x0018($sp)           
/* 00644 809E9BA4 AFA50034 */  sw      $a1, 0x0034($sp)           
/* 00648 809E9BA8 3C013F00 */  lui     $at, 0x3F00                ## $at = 3F000000
/* 0064C 809E9BAC 44813000 */  mtc1    $at, $f6                   ## $f6 = 0.50
/* 00650 809E9BB0 C4840168 */  lwc1    $f4, 0x0168($a0)           ## 00000168
/* 00654 809E9BB4 00808025 */  or      $s0, $a0, $zero            ## $s0 = 00000000
/* 00658 809E9BB8 00001825 */  or      $v1, $zero, $zero          ## $v1 = 00000000
/* 0065C 809E9BBC 4606203C */  c.lt.s  $f4, $f6                   
/* 00660 809E9BC0 2604014C */  addiu   $a0, $s0, 0x014C           ## $a0 = 0000014C
/* 00664 809E9BC4 3C054110 */  lui     $a1, 0x4110                ## $a1 = 41100000
/* 00668 809E9BC8 45000002 */  bc1f    .L809E9BD4                 
/* 0066C 809E9BCC 00000000 */  nop
/* 00670 809E9BD0 24030001 */  addiu   $v1, $zero, 0x0001         ## $v1 = 00000001
.L809E9BD4:
/* 00674 809E9BD4 50600006 */  beql    $v1, $zero, .L809E9BF0     
/* 00678 809E9BD8 AFA3002C */  sw      $v1, 0x002C($sp)           
/* 0067C 809E9BDC 86020196 */  lh      $v0, 0x0196($s0)           ## 00000196
/* 00680 809E9BE0 10400002 */  beq     $v0, $zero, .L809E9BEC     
/* 00684 809E9BE4 244EFFFF */  addiu   $t6, $v0, 0xFFFF           ## $t6 = FFFFFFFF
/* 00688 809E9BE8 A60E0196 */  sh      $t6, 0x0196($s0)           ## 00000196
.L809E9BEC:
/* 0068C 809E9BEC AFA3002C */  sw      $v1, 0x002C($sp)           
.L809E9BF0:
/* 00690 809E9BF0 0C0295B2 */  jal     Animation_OnFrame              
/* 00694 809E9BF4 AFA40020 */  sw      $a0, 0x0020($sp)           
/* 00698 809E9BF8 3C014110 */  lui     $at, 0x4110                ## $at = 41100000
/* 0069C 809E9BFC 44816000 */  mtc1    $at, $f12                  ## $f12 = 9.00
/* 006A0 809E9C00 10400005 */  beq     $v0, $zero, .L809E9C18     
/* 006A4 809E9C04 8FA40020 */  lw      $a0, 0x0020($sp)           
/* 006A8 809E9C08 920F02D9 */  lbu     $t7, 0x02D9($s0)           ## 000002D9
/* 006AC 809E9C0C 35F80001 */  ori     $t8, $t7, 0x0001           ## $t8 = 00000001
/* 006B0 809E9C10 1000000C */  beq     $zero, $zero, .L809E9C44   
/* 006B4 809E9C14 A21802D9 */  sb      $t8, 0x02D9($s0)           ## 000002D9
.L809E9C18:
/* 006B8 809E9C18 0C0295B2 */  jal     Animation_OnFrame              
/* 006BC 809E9C1C 3C054100 */  lui     $a1, 0x4100                ## $a1 = 41000000
/* 006C0 809E9C20 3C014110 */  lui     $at, 0x4110                ## $at = 41100000
/* 006C4 809E9C24 44816000 */  mtc1    $at, $f12                  ## $f12 = 9.00
/* 006C8 809E9C28 10400006 */  beq     $v0, $zero, .L809E9C44     
/* 006CC 809E9C2C 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 006D0 809E9C30 0C00BE0A */  jal     Audio_PlayActorSound2
              
/* 006D4 809E9C34 2405387C */  addiu   $a1, $zero, 0x387C         ## $a1 = 0000387C
/* 006D8 809E9C38 3C014110 */  lui     $at, 0x4110                ## $at = 41100000
/* 006DC 809E9C3C 44816000 */  mtc1    $at, $f12                  ## $f12 = 9.00
/* 006E0 809E9C40 00000000 */  nop
.L809E9C44:
/* 006E4 809E9C44 C6000164 */  lwc1    $f0, 0x0164($s0)           ## 00000164
/* 006E8 809E9C48 3C014140 */  lui     $at, 0x4140                ## $at = 41400000
/* 006EC 809E9C4C 460C003C */  c.lt.s  $f0, $f12                  
/* 006F0 809E9C50 00000000 */  nop
/* 006F4 809E9C54 45020004 */  bc1fl   .L809E9C68                 
/* 006F8 809E9C58 44817000 */  mtc1    $at, $f14                  ## $f14 = 12.00
/* 006FC 809E9C5C 1000000B */  beq     $zero, $zero, .L809E9C8C   
/* 00700 809E9C60 46006006 */  mov.s   $f0, $f12                  
/* 00704 809E9C64 44817000 */  mtc1    $at, $f14                  ## $f14 = 12.00
.L809E9C68:
/* 00708 809E9C68 00000000 */  nop
/* 0070C 809E9C6C 4600703C */  c.lt.s  $f14, $f0                  
/* 00710 809E9C70 00000000 */  nop
/* 00714 809E9C74 45020004 */  bc1fl   .L809E9C88                 
/* 00718 809E9C78 46000086 */  mov.s   $f2, $f0                   
/* 0071C 809E9C7C 10000002 */  beq     $zero, $zero, .L809E9C88   
/* 00720 809E9C80 46007086 */  mov.s   $f2, $f14                  
/* 00724 809E9C84 46000086 */  mov.s   $f2, $f0                   
.L809E9C88:
/* 00728 809E9C88 46001006 */  mov.s   $f0, $f2                   
.L809E9C8C:
/* 0072C 809E9C8C 460C0201 */  sub.s   $f8, $f0, $f12             
/* 00730 809E9C90 3C0140A0 */  lui     $at, 0x40A0                ## $at = 40A00000
/* 00734 809E9C94 44818000 */  mtc1    $at, $f16                  ## $f16 = 5.00
/* 00738 809E9C98 3C0142F0 */  lui     $at, 0x42F0                ## $at = 42F00000
/* 0073C 809E9C9C 460C4282 */  mul.s   $f10, $f8, $f12            
/* 00740 809E9CA0 46105480 */  add.s   $f18, $f10, $f16           
/* 00744 809E9CA4 4600910D */  trunc.w.s $f4, $f18                  
/* 00748 809E9CA8 44082000 */  mfc1    $t0, $f4                   
/* 0074C 809E9CAC 00000000 */  nop
/* 00750 809E9CB0 A608030A */  sh      $t0, 0x030A($s0)           ## 0000030A
/* 00754 809E9CB4 8FA9002C */  lw      $t1, 0x002C($sp)           
/* 00758 809E9CB8 1520000C */  bne     $t1, $zero, .L809E9CEC     
/* 0075C 809E9CBC 00000000 */  nop
/* 00760 809E9CC0 C6060090 */  lwc1    $f6, 0x0090($s0)           ## 00000090
/* 00764 809E9CC4 44814000 */  mtc1    $at, $f8                   ## $f8 = 120.00
/* 00768 809E9CC8 00000000 */  nop
/* 0076C 809E9CCC 4608303C */  c.lt.s  $f6, $f8                   
/* 00770 809E9CD0 00000000 */  nop
/* 00774 809E9CD4 45000005 */  bc1f    .L809E9CEC                 
/* 00778 809E9CD8 00000000 */  nop
/* 0077C 809E9CDC 0C27A61A */  jal     func_809E9868              
/* 00780 809E9CE0 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 00784 809E9CE4 10000021 */  beq     $zero, $zero, .L809E9D6C   
/* 00788 809E9CE8 8FAB002C */  lw      $t3, 0x002C($sp)           
.L809E9CEC:
/* 0078C 809E9CEC 0C02927F */  jal     SkelAnime_Update
              
/* 00790 809E9CF0 8FA40020 */  lw      $a0, 0x0020($sp)           
/* 00794 809E9CF4 1040001C */  beq     $v0, $zero, .L809E9D68     
/* 00798 809E9CF8 3C0142F0 */  lui     $at, 0x42F0                ## $at = 42F00000
/* 0079C 809E9CFC C6020090 */  lwc1    $f2, 0x0090($s0)           ## 00000090
/* 007A0 809E9D00 44815000 */  mtc1    $at, $f10                  ## $f10 = 120.00
/* 007A4 809E9D04 00000000 */  nop
/* 007A8 809E9D08 460A103C */  c.lt.s  $f2, $f10                  
/* 007AC 809E9D0C 00000000 */  nop
/* 007B0 809E9D10 45020006 */  bc1fl   .L809E9D2C                 
/* 007B4 809E9D14 860A0196 */  lh      $t2, 0x0196($s0)           ## 00000196
/* 007B8 809E9D18 0C27A61A */  jal     func_809E9868              
/* 007BC 809E9D1C 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 007C0 809E9D20 10000012 */  beq     $zero, $zero, .L809E9D6C   
/* 007C4 809E9D24 8FAB002C */  lw      $t3, 0x002C($sp)           
/* 007C8 809E9D28 860A0196 */  lh      $t2, 0x0196($s0)           ## 00000196
.L809E9D2C:
/* 007CC 809E9D2C 3C0143A0 */  lui     $at, 0x43A0                ## $at = 43A00000
/* 007D0 809E9D30 1540000B */  bne     $t2, $zero, .L809E9D60     
/* 007D4 809E9D34 00000000 */  nop
/* 007D8 809E9D38 44818000 */  mtc1    $at, $f16                  ## $f16 = 320.00
/* 007DC 809E9D3C 00000000 */  nop
/* 007E0 809E9D40 4602803C */  c.lt.s  $f16, $f2                  
/* 007E4 809E9D44 00000000 */  nop
/* 007E8 809E9D48 45000005 */  bc1f    .L809E9D60                 
/* 007EC 809E9D4C 00000000 */  nop
/* 007F0 809E9D50 0C27A5DC */  jal     func_809E9770              
/* 007F4 809E9D54 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
/* 007F8 809E9D58 10000004 */  beq     $zero, $zero, .L809E9D6C   
/* 007FC 809E9D5C 8FAB002C */  lw      $t3, 0x002C($sp)           
.L809E9D60:
/* 00800 809E9D60 0C27A600 */  jal     func_809E9800              
/* 00804 809E9D64 02002025 */  or      $a0, $s0, $zero            ## $a0 = 00000000
.L809E9D68:
/* 00808 809E9D68 8FAB002C */  lw      $t3, 0x002C($sp)           
.L809E9D6C:
/* 0080C 809E9D6C 3C014320 */  lui     $at, 0x4320                ## $at = 43200000
/* 00810 809E9D70 5160001E */  beql    $t3, $zero, .L809E9DEC     
/* 00814 809E9D74 8FBF001C */  lw      $ra, 0x001C($sp)           
/* 00818 809E9D78 C6020090 */  lwc1    $f2, 0x0090($s0)           ## 00000090
/* 0081C 809E9D7C 44819000 */  mtc1    $at, $f18                  ## $f18 = 160.00
/* 00820 809E9D80 00000000 */  nop
/* 00824 809E9D84 4602903C */  c.lt.s  $f18, $f2                  
/* 00828 809E9D88 00000000 */  nop
/* 0082C 809E9D8C 45020017 */  bc1fl   .L809E9DEC                 
/* 00830 809E9D90 8FBF001C */  lw      $ra, 0x001C($sp)           
/* 00834 809E9D94 C6000094 */  lwc1    $f0, 0x0094($s0)           ## 00000094
/* 00838 809E9D98 3C0142F0 */  lui     $at, 0x42F0                ## $at = 42F00000
/* 0083C 809E9D9C 44812000 */  mtc1    $at, $f4                   ## $f4 = 120.00
/* 00840 809E9DA0 46000005 */  abs.s   $f0, $f0                   
/* 00844 809E9DA4 4604003C */  c.lt.s  $f0, $f4                   
/* 00848 809E9DA8 00000000 */  nop
/* 0084C 809E9DAC 4502000F */  bc1fl   .L809E9DEC                 
/* 00850 809E9DB0 8FBF001C */  lw      $ra, 0x001C($sp)           
/* 00854 809E9DB4 860C0196 */  lh      $t4, 0x0196($s0)           ## 00000196
/* 00858 809E9DB8 3C0143F0 */  lui     $at, 0x43F0                ## $at = 43F00000
/* 0085C 809E9DBC 51800007 */  beql    $t4, $zero, .L809E9DDC     
/* 00860 809E9DC0 3C013F80 */  lui     $at, 0x3F80                ## $at = 3F800000
/* 00864 809E9DC4 44813000 */  mtc1    $at, $f6                   ## $f6 = 1.00
/* 00868 809E9DC8 00000000 */  nop
/* 0086C 809E9DCC 4606103C */  c.lt.s  $f2, $f6                   
/* 00870 809E9DD0 00000000 */  nop
/* 00874 809E9DD4 45000004 */  bc1f    .L809E9DE8                 
/* 00878 809E9DD8 3C013F80 */  lui     $at, 0x3F80                ## $at = 3F800000
.L809E9DDC:
/* 0087C 809E9DDC 44814000 */  mtc1    $at, $f8                   ## $f8 = 1.00
/* 00880 809E9DE0 00000000 */  nop
/* 00884 809E9DE4 E6080168 */  swc1    $f8, 0x0168($s0)           ## 00000168
.L809E9DE8:
/* 00888 809E9DE8 8FBF001C */  lw      $ra, 0x001C($sp)           
.L809E9DEC:
/* 0088C 809E9DEC 8FB00018 */  lw      $s0, 0x0018($sp)           
/* 00890 809E9DF0 27BD0030 */  addiu   $sp, $sp, 0x0030           ## $sp = 00000000
/* 00894 809E9DF4 03E00008 */  jr      $ra                        
/* 00898 809E9DF8 00000000 */  nop
