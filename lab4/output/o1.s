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
subu $sp, $sp, 64
sw $ra, 60($sp)
sw $fp, 56($sp)
addi $fp, $sp, 64
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
sw, $t3, -52($fp)
move $t3, $t3
sw, $t0, -40($fp)
sw, $t1, -44($fp)
sw, $t2, -48($fp)
sw, $t3, -56($fp)

label0:
lw $t0, -48($fp)
lw $t1, -56($fp)
blt $t0, $t1, label1
sw, $t0, -48($fp)
sw, $t1, -56($fp)
j label5

label1:
lw $t0, -40($fp)
lw $t1, -44($fp)
add $t2, $t0, $t1
move $a0, $t1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0, $t1
move $t1, $t2
sw, $t1, -44($fp)
lw $t1, -48($fp)
li $t9, 1
add $t1, $t1, $t9
sw, $t0, -40($fp)
sw, $t1, -48($fp)
sw, $t2, -60($fp)
j label0

label5:
lw $t0, -40($fp)
move $v0, $t0
lw $ra, 60($sp)
lw $fp, 56($sp)
addi $sp, $sp, 64
jr $ra
