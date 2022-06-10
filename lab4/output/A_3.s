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
subu $sp, $sp, 104
sw $ra, 100($sp)
sw $fp, 96($sp)
addi $fp, $sp, 104
li $t0, 1
li $t1, 1
lw $t2, -52($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t2, $v0
sw, $t2, -52($fp)
move $t2, $t2
sw, $t0, -44($fp)
sw, $t1, -48($fp)
sw, $t2, -56($fp)
lw $t0, -56($fp)
li $t9, 1
blt $t0, $t9, label3
sw, $t0, -56($fp)
j label4

label3:
li $a0, 0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
j label15

label4:
lw $t0, -56($fp)
li $t9, 1
beq $t0, $t9, label6
j label5

label5:
sw, $t0, -56($fp)
lw $t0, -56($fp)
li $t9, 2
beq $t0, $t9, label6
sw, $t0, -56($fp)
j label7

label6:
li $a0, 1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
j label15

label7:
lw $t0, -56($fp)
li $t9, 2
bgt $t0, $t9, label8
sw, $t0, -56($fp)
j label15

label8:
li $t0, 3
sw, $t0, -60($fp)

label9:
lw $t0, -60($fp)
lw $t1, -56($fp)
ble $t0, $t1, label10
sw, $t0, -60($fp)
sw, $t1, -56($fp)
j label14

label10:
lw $t0, -44($fp)
lw $t1, -48($fp)
add $t2, $t0, $t1
move $t0, $t1
move $t1, $t2
sw, $t1, -48($fp)
lw $t1, -60($fp)
li $t9, 1
add $t1, $t1, $t9
sw, $t0, -44($fp)
sw, $t1, -60($fp)
sw, $t2, -64($fp)
j label9

label14:
lw $t0, -64($fp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
sw, $t0, -64($fp)

label15:
li $v0, 0
lw $ra, 100($sp)
lw $fp, 96($sp)
addi $sp, $sp, 104
jr $ra
