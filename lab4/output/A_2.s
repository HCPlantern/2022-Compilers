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
subu $sp, $sp, 176
sw $ra, 172($sp)
sw $fp, 168($sp)
addi $fp, $sp, 176
lw $t0, -44($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0, $v0
sw, $t0, -44($fp)
move $t0, $t0
lw $t1, -52($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t1, $v0
sw, $t1, -52($fp)
move $t1, $t1
lw $t2, -60($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t2, $v0
sw, $t2, -60($fp)
move $t2, $t2
sw, $t1, -56($fp)
add $t1, $t0, $t1
sw, $t0, -48($fp)
sw, $t1, -68($fp)
sw, $t2, -64($fp)
lw $t0, -68($fp)
lw $t1, -64($fp)
bgt $t0, $t1, label3
sw, $t0, -68($fp)
sw, $t1, -64($fp)
j label20

label3:
lw $t0, -56($fp)
lw $t1, -64($fp)
sw, $t1, -64($fp)
add $t1, $t0, $t1
sw, $t0, -56($fp)
sw, $t1, -72($fp)
lw $t0, -72($fp)
lw $t1, -48($fp)
bgt $t0, $t1, label4
sw, $t0, -72($fp)
sw, $t1, -48($fp)
j label20

label4:
lw $t0, -48($fp)
lw $t1, -64($fp)
sw, $t1, -64($fp)
add $t1, $t0, $t1
sw, $t0, -48($fp)
sw, $t1, -76($fp)
lw $t0, -76($fp)
lw $t1, -56($fp)
bgt $t0, $t1, label5
sw, $t0, -76($fp)
sw, $t1, -56($fp)
j label20

label5:
lw $t0, -48($fp)
lw $t1, -56($fp)
add $t2, $t0, $t1
lw $t3, -64($fp)
sw, $t3, -64($fp)
add $t3, $t2, $t3
sw, $t0, -48($fp)
sw, $t1, -56($fp)
sw, $t2, -80($fp)
sw, $t3, -84($fp)
lw $t0, -48($fp)
lw $t1, -56($fp)
beq $t0, $t1, label7
sw, $t0, -48($fp)
sw, $t1, -56($fp)
j label9

label7:
lw $t0, -48($fp)
lw $t1, -64($fp)
beq $t0, $t1, label8
sw, $t0, -48($fp)
sw, $t1, -64($fp)
j label9

label8:
li $t0, 1
sw, $t0, -88($fp)
j label18

label9:
lw $t0, -48($fp)
lw $t1, -56($fp)
beq $t0, $t1, label12
j label10

label10:
sw, $t0, -48($fp)
sw, $t1, -56($fp)
lw $t0, -48($fp)
lw $t1, -64($fp)
beq $t0, $t1, label12
j label11

label11:
sw, $t0, -48($fp)
sw, $t1, -64($fp)
lw $t0, -56($fp)
lw $t1, -64($fp)
beq $t0, $t1, label12
sw, $t0, -56($fp)
sw, $t1, -64($fp)
j label13

label12:
li $t0, 2
sw, $t0, -88($fp)
j label18

label13:
lw $t0, -48($fp)
sw, $t0, -48($fp)
mul $t0, $t0, $t0
lw $t1, -56($fp)
sw, $t1, -56($fp)
mul $t1, $t1, $t1
sw, $t1, -96($fp)
add $t1, $t0, $t1
lw $t2, -64($fp)
sw, $t2, -64($fp)
mul $t2, $t2, $t2
sw, $t0, -92($fp)
sw, $t1, -100($fp)
sw, $t2, -104($fp)
lw $t0, -100($fp)
lw $t1, -104($fp)
beq $t0, $t1, label16
j label14

label14:
lw $t2, -48($fp)
sw, $t2, -48($fp)
mul $t2, $t2, $t2
lw $t3, -64($fp)
sw, $t3, -64($fp)
mul $t3, $t3, $t3
sw, $t3, -112($fp)
add $t3, $t2, $t3
lw $t4, -56($fp)
sw, $t4, -56($fp)
mul $t4, $t4, $t4
sw, $t0, -100($fp)
sw, $t1, -104($fp)
sw, $t2, -108($fp)
sw, $t3, -116($fp)
sw, $t4, -120($fp)
lw $t0, -116($fp)
lw $t1, -120($fp)
beq $t0, $t1, label16
j label15

label15:
lw $t2, -56($fp)
sw, $t2, -56($fp)
mul $t2, $t2, $t2
lw $t3, -64($fp)
sw, $t3, -64($fp)
mul $t3, $t3, $t3
sw, $t3, -128($fp)
add $t3, $t2, $t3
lw $t4, -48($fp)
sw, $t4, -48($fp)
mul $t4, $t4, $t4
sw, $t0, -116($fp)
sw, $t1, -120($fp)
sw, $t2, -124($fp)
sw, $t3, -132($fp)
sw, $t4, -136($fp)
lw $t0, -132($fp)
lw $t1, -136($fp)
beq $t0, $t1, label16
sw, $t0, -132($fp)
sw, $t1, -136($fp)
j label17

label16:
li $t0, 3
sw, $t0, -88($fp)
j label18

label17:
li $t0, 0
sw, $t0, -88($fp)

label18:
lw $t0, -88($fp)
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
lw $t1, -84($fp)
move $a0, $t1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
sw, $t0, -88($fp)
sw, $t1, -84($fp)
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
lw $ra, 172($sp)
lw $fp, 168($sp)
addi $sp, $sp, 176
jr $ra
