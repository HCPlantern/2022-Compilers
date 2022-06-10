.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra
write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra

fact:
subu $sp, $sp, 56
sw $ra, 52($sp)
sw $fp, 48($sp)
addi $fp, $sp, 56
sw $a0, -44($fp)
lw $t0, -44($fp)
li $t9, 1
beq $t0, $t9, label0
sw, $t0, -44($fp)
j label1

label0:
lw $t0, -44($fp)
move $v0, $t0
lw $ra, 52($sp)
lw $fp, 48($sp)
addi $sp, $sp, 56
jr $ra

label1:
li $t9, 1
sub $t1, $t0, $t9
sw, $t0, -44($fp)
sw, $t1, -48($fp)
lw $t0, -48($fp)
move $a0, $t0
jal fact
lw $t0, -44($fp)
mul $t1, $t0, $v0
move $v0, $t1
lw $ra, 52($sp)
lw $fp, 48($sp)
addi $sp, $sp, 56
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
li $t9, 1
bgt $t0, $t9, label3
sw, $t0, -48($fp)
j label4

label3:
lw $t0, -48($fp)
move $a0, $t0
jal fact
j label5

label4:
li $v0, 1

label5:
move $a0, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $v0, 0
lw $ra, 48($sp)
lw $fp, 44($sp)
addi $sp, $sp, 52
jr $ra
