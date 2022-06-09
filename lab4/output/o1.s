.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
    li      $v0,    4
    la      $a0,    _prompt
    syscall 
    li      $v0,    5
    syscall 
    jr      $ra
write:
    li      $v0,    1
    syscall 
    li      $v0,    4
    la      $a0,    _ret
    syscall 
    move    $v0,    $0
    jr      $ra

main:
    li      $t0,    0
    li      $t1,    1
    li      $t2,    0
    lw      $t3,    -56($fp)
    addi    $sp,    $sp,        -4
    sw      $ra,    0($sp)
    jal     read
    addi    $sp,    $sp,        4
    lw      $ra,    0($sp)
    move    $t3,    $v0
    move    $t4,    $t3
    sw      $t0,    -44($fp)
    sw      $t1,    -48($fp)
    sw      $t2,    -52($fp)
    sw      $t3,    -56($fp)
    sw      $t4,    -60($fp)

label0:
    j       label5

label1:
    lw      $t1,    -44($fp)
    lw      $t2,    -48($fp)
    add     $t0,    $t1,        $t2
    move    $t3,    $t0
    addi    $sp,    $sp,        -4
    sw      $ra,    0($sp)
    jal     write
    lw      $ra,    0($sp)
    addi    $sp,    $sp,        4
    move    $t0,    $t2
    move    $t2,    $t3
    lw      $t4,    -52($fp)
    li      $t1,    1
    add     $t3,    $t4,        $t1
    move    $t5,    $t3
    j       label0
    sw      $t0,    -44($fp)
    sw      $t1,    -44($fp)
    sw      $t2,    -48($fp)
    sw      $t3,    -72($fp)
    sw      $t4,    -52($fp)
    sw      $t5,    -52($fp)

label5:
    lw      $t0,    -44($fp)
    move    $v0,    $t0
    jr      $ra
