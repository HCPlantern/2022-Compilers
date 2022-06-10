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

main:
subu $sp, $sp, 64
sw $ra, 60($sp)
sw $fp, 56($sp)
addi $fp, $sp, 64
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
move $a0, $t0
li $a1, 400
jal mod
sw, $t0, -48($fp)
li $t9, 0
beq $v0, $t9, label1
j label2

label1:
li $t0, 1
sw, $t0, -56($fp)
j label6

label2:
lw $t0, -48($fp)
move $a0, $t0
li $a1, 4
jal mod
sw, $t0, -48($fp)
li $t9, 0
beq $v0, $t9, label3
j label5

label3:
lw $t0, -48($fp)
move $a0, $t0
li $a1, 100
jal mod
sw, $t0, -48($fp)
li $t9, 0
bne $v0, $t9, label4
j label5

label4:
li $t0, 1
sw, $t0, -56($fp)
j label6

label5:
li $t0, 0
sw, $t0, -56($fp)

label6:
lw $t0, -56($fp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $v0, 0
lw $ra, 60($sp)
lw $fp, 56($sp)
addi $sp, $sp, 64
jr $ra
