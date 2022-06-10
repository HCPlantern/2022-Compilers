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
subu $sp, $sp, 136
sw $ra, 132($sp)
sw $fp, 128($sp)
addi $fp, $sp, 136
lw $t0, -40($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0, $v0
sw, $t0, -40($fp)
move $t0, $t0
lw $t1, -48($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t1, $v0
sw, $t1, -48($fp)
move $t1, $t1
lw $t2, -56($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t2, $v0
sw, $t2, -56($fp)
move $t2, $t2
sw, $t1, -52($fp)
add $t1, $t0, $t1
sw, $t0, -44($fp)
sw, $t1, -64($fp)
sw, $t2, -60($fp)
lw $t0, -64($fp)
lw $t1, -60($fp)
bgt $t0, $t1, label3
sw, $t0, -64($fp)
sw, $t1, -60($fp)
j label20

label3:
lw $t0, -52($fp)
lw $t1, -60($fp)
sw, $t1, -60($fp)
add $t1, $t0, $t1
sw, $t0, -52($fp)
sw, $t1, -68($fp)
lw $t0, -68($fp)
lw $t1, -44($fp)
bgt $t0, $t1, label4
sw, $t0, -68($fp)
sw, $t1, -44($fp)
j label20

label4:
lw $t0, -44($fp)
lw $t1, -60($fp)
sw, $t1, -60($fp)
add $t1, $t0, $t1
sw, $t0, -44($fp)
sw, $t1, -72($fp)
lw $t0, -72($fp)
lw $t1, -52($fp)
bgt $t0, $t1, label5
sw, $t0, -72($fp)
sw, $t1, -52($fp)
j label20

label5:
lw $t0, -44($fp)
lw $t1, -52($fp)
add $t2, $t0, $t1
lw $t3, -60($fp)
sw, $t3, -60($fp)
add $t3, $t2, $t3
sw, $t0, -44($fp)
sw, $t1, -52($fp)
sw, $t2, -76($fp)
sw, $t3, -80($fp)
lw $t0, -44($fp)
lw $t1, -52($fp)
beq $t0, $t1, label7
sw, $t0, -44($fp)
sw, $t1, -52($fp)
j label9

label7:
lw $t0, -44($fp)
lw $t1, -60($fp)
beq $t0, $t1, label8
sw, $t0, -44($fp)
sw, $t1, -60($fp)
j label9

label8:
li $t0, 1
sw, $t0, -84($fp)
j label18

label9:
lw $t0, -44($fp)
lw $t1, -52($fp)
beq $t0, $t1, label12
j label10

label10:
sw, $t0, -44($fp)
sw, $t1, -52($fp)
lw $t0, -44($fp)
lw $t1, -60($fp)
beq $t0, $t1, label12
j label11

label11:
sw, $t0, -44($fp)
sw, $t1, -60($fp)
lw $t0, -52($fp)
lw $t1, -60($fp)
beq $t0, $t1, label12
sw, $t0, -52($fp)
sw, $t1, -60($fp)
j label13

label12:
li $t0, 2
sw, $t0, -84($fp)
j label18

label13:
lw $t0, -44($fp)
sw, $t0, -44($fp)
mul $t0, $t0, $t0
lw $t1, -52($fp)
sw, $t1, -52($fp)
mul $t1, $t1, $t1
sw, $t1, -92($fp)
add $t1, $t0, $t1
lw $t2, -60($fp)
sw, $t2, -60($fp)
mul $t2, $t2, $t2
sw, $t0, -88($fp)
sw, $t1, -96($fp)
sw, $t2, -100($fp)
lw $t0, -96($fp)
lw $t1, -100($fp)
beq $t0, $t1, label16
j label14

label14:
lw $t2, -44($fp)
sw, $t2, -44($fp)
mul $t2, $t2, $t2
lw $t3, -60($fp)
sw, $t3, -60($fp)
mul $t3, $t3, $t3
sw, $t3, -108($fp)
add $t3, $t2, $t3
lw $t4, -52($fp)
sw, $t4, -52($fp)
mul $t4, $t4, $t4
sw, $t0, -96($fp)
sw, $t1, -100($fp)
sw, $t2, -104($fp)
sw, $t3, -112($fp)
sw, $t4, -116($fp)
lw $t0, -112($fp)
lw $t1, -116($fp)
beq $t0, $t1, label16
j label15

label15:
lw $t2, -52($fp)
sw, $t2, -52($fp)
mul $t2, $t2, $t2
lw $t3, -60($fp)
sw, $t3, -60($fp)
mul $t3, $t3, $t3
sw, $t3, -124($fp)
add $t3, $t2, $t3
lw $t4, -44($fp)
sw, $t4, -44($fp)
mul $t4, $t4, $t4
sw, $t0, -112($fp)
sw, $t1, -116($fp)
sw, $t2, -120($fp)
sw, $t3, -128($fp)
sw, $t4, -132($fp)
lw $t0, -128($fp)
lw $t1, -132($fp)
beq $t0, $t1, label16
sw, $t0, -128($fp)
sw, $t1, -132($fp)
j label17

label16:
li $t0, 3
sw, $t0, -84($fp)
j label18

label17:
li $t0, 0
sw, $t0, -84($fp)

label18:
lw $t0, -84($fp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
lw $t1, -80($fp)
move $a0, $t1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
sw, $t0, -84($fp)
sw, $t1, -80($fp)
j label21

label20:
li $a0, -1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4

label21:
li $v0, 0
lw $ra, 132($sp)
lw $fp, 128($sp)
addi $sp, $sp, 136
jr $ra
