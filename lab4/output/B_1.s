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

mod:
subu $sp, $sp, 60
sw $ra, 56($sp)
sw $fp, 52($sp)
addi $fp, $sp, 60
sw $a0, -44($fp)
sw $a1, -48($fp)
lw $t0, -44($fp)
lw $t1, -48($fp)
div $t0, $t1
mflo $t2
sw, $t1, -48($fp)
mul $t1, $t2, $t1
sw, $t1, -56($fp)
sub $t1, $t0, $t1
move $v0, $t1
lw $ra, 56($sp)
lw $fp, 52($sp)
addi $sp, $sp, 60
jr $ra

DigitSum:
subu $sp, $sp, 60
sw $ra, 56($sp)
sw $fp, 52($sp)
addi $fp, $sp, 60
sw $a0, -44($fp)
lw $t0, -44($fp)
li $t9, 0
beq $t0, $t9, label0
sw, $t0, -44($fp)
j label1

label0:
li $v0, 0
lw $ra, 56($sp)
lw $fp, 52($sp)
addi $sp, $sp, 60
jr $ra

label1:
lw $t0, -44($fp)
move $a0, $t0
lw $t1, -44($fp)
move $a1, $t1
jal mod
sw, $t0, -44($fp)
sw, $t1, -44($fp)
lw $t0, -44($fp)
li $t9, 10
div $t0, $t9
mflo $t1
sw, $t0, -44($fp)
sw, $t1, -52($fp)
lw $t0, -52($fp)
move $a0, $t0
jal DigitSum
sw, $t0, -52($fp)
add $t0, $v0, $v0
move $v0, $t0
lw $ra, 56($sp)
lw $fp, 52($sp)
addi $sp, $sp, 60
jr $ra

main:
subu $sp, $sp, 52
sw $ra, 48($sp)
sw $fp, 44($sp)
addi $fp, $sp, 52
lw $t0, -44($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0, $v0
sw, $t0, -44($fp)
move $t0, $t0
sw, $t0, -48($fp)
lw $t0, -48($fp)
li $t9, 0
blt $t0, $t9, label3
sw, $t0, -48($fp)
j label4

label3:
li $a0, -1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
j label5

label4:
lw $t0, -48($fp)
move $a0, $t0
jal DigitSum
sw, $t0, -48($fp)
move $a0, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4

label5:
li $v0, 0
lw $ra, 48($sp)
lw $fp, 44($sp)
addi $sp, $sp, 52
jr $ra
