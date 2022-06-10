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
subu $sp, $sp, 68
sw $ra, 64($sp)
sw $fp, 60($sp)
addi $fp, $sp, 68
add $t0, $fp, -40
li $t8, 0
sw $t8, 0($t0)
add $t1, $fp, -40
lw $t2, 0($t1)
sw, $t0, -48($fp)
sw, $t1, -52($fp)
sw, $t2, -56($fp)
lw $t0, -56($fp)
lw $t1, -40($fp)
bne $t0, $t1, label1
sw, $t0, -56($fp)
sw, $t1, -40($fp)
j label2

label1:
add $t0, $fp, -40
lw $t1, 0($t0)
move $a0, $t1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
sw, $t0, -60($fp)
sw, $t1, -64($fp)
j label3

label2:
li $a0, 10
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4

label3:
li $v0, 0
lw $ra, 64($sp)
lw $fp, 60($sp)
addi $sp, $sp, 68
jr $ra
