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
li $t0, 0
li $t1, 1
li $t2, 0
lw $t3, -52($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t3, $v0
move $t4, $t3
sw, $t0, -40($fp)
sw, $t1, -44($fp)
sw, $t2, -48($fp)
sw, $t4, -56($fp)

label0:
j label5

label1:
lw $t1, -40($fp)
lw $t2, -44($fp)
add $t0, $t1, $t2
move $a0, $t2
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t1, $t2
move $t2, $t0
sw, $t0, -60($fp)
li $t9, 1
add $t0, $t0, $t9
j label0
sw, $t0, -48($fp)
sw, $t1, -40($fp)

label5:
lw $t0, -40($fp)
move $v0, $t0
jr $ra
