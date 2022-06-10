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
subu $sp, $sp, 184
sw $ra, 180($sp)
sw $fp, 176($sp)
addi $fp, $sp, 184
li $t0, 0
sw, $t0, -48($fp)

label0:
lw $t0, -48($fp)
li $t9, 5
blt $t0, $t9, label1
sw, $t0, -48($fp)
j label3

label1:
lw $t0, -48($fp)
li $t9, 4
mul $t1, $t0, $t9
li $t8, 0
sw, $t1, -68($fp)
add $t1, $t8, $t1
add $t8, $fp, -48
sw, $t1, -72($fp)
add $t1, $t8, $t1
lw $t2, -80($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t2, $v0
sw $t2, 0($t1)
li $t9, 1
add $t0, $t0, $t9
sw, $t0, -48($fp)
sw, $t1, -76($fp)
sw, $t2, -80($fp)
j label0

label3:
li $t0, 0
sw, $t0, -48($fp)

label4:
lw $t0, -48($fp)
li $t9, 4
blt $t0, $t9, label5
sw, $t0, -48($fp)
j label13

label5:
lw $t0, -48($fp)
li $t9, 1
add $t1, $t0, $t9
sw, $t0, -48($fp)
sw, $t1, -84($fp)

label6:
lw $t0, -84($fp)
li $t9, 5
blt $t0, $t9, label7
sw, $t0, -84($fp)
j label12

label7:
lw $t0, -48($fp)
li $t9, 4
mul $t1, $t0, $t9
li $t8, 0
sw, $t1, -88($fp)
add $t1, $t8, $t1
add $t8, $fp, -48
sw, $t1, -92($fp)
add $t1, $t8, $t1
lw $t2, -84($fp)
li $t9, 4
mul $t3, $t2, $t9
li $t8, 0
sw, $t3, -100($fp)
add $t3, $t8, $t3
add $t8, $fp, -48
sw, $t3, -104($fp)
add $t3, $t8, $t3
lw $t4, 0($t1)
lw $t5, 0($t3)
sw, $t0, -48($fp)
sw, $t1, -96($fp)
sw, $t2, -84($fp)
sw, $t3, -108($fp)
sw, $t4, -112($fp)
sw, $t5, -116($fp)
lw $t0, -112($fp)
lw $t1, -116($fp)
bgt $t0, $t1, label8
sw, $t0, -112($fp)
sw, $t1, -116($fp)
j label11

label8:
lw $t0, -48($fp)
li $t9, 4
mul $t1, $t0, $t9
li $t8, 0
sw, $t1, -120($fp)
add $t1, $t8, $t1
add $t8, $fp, -48
sw, $t1, -124($fp)
add $t1, $t8, $t1
lw $t2, 0($t1)
li $t9, 4
mul $t3, $t0, $t9
li $t8, 0
sw, $t3, -136($fp)
add $t3, $t8, $t3
add $t8, $fp, -48
sw, $t3, -140($fp)
add $t3, $t8, $t3
lw $t4, -84($fp)
li $t9, 4
mul $t5, $t4, $t9
li $t8, 0
sw, $t5, -148($fp)
add $t5, $t8, $t5
add $t8, $fp, -48
sw, $t5, -152($fp)
add $t5, $t8, $t5
sw $t0, 0($t3)
li $t9, 4
mul $t6, $t4, $t9
li $t8, 0
sw, $t6, -160($fp)
add $t6, $t8, $t6
add $t8, $fp, -48
sw, $t6, -164($fp)
add $t6, $t8, $t6
sw $t2, 0($t6)
sw, $t0, -48($fp)
sw, $t1, -128($fp)
sw, $t2, -132($fp)
sw, $t3, -144($fp)
sw, $t4, -84($fp)
sw, $t5, -156($fp)
sw, $t6, -168($fp)

label11:
lw $t0, -84($fp)
li $t9, 1
add $t0, $t0, $t9
sw, $t0, -84($fp)
j label6

label12:
lw $t0, -48($fp)
li $t9, 1
add $t0, $t0, $t9
sw, $t0, -48($fp)
j label4

label13:
li $t0, 0
sw, $t0, -48($fp)

label14:
lw $t0, -48($fp)
li $t9, 5
blt $t0, $t9, label15
sw, $t0, -48($fp)
j label17

label15:
lw $t0, -48($fp)
li $t9, 4
mul $t1, $t0, $t9
li $t8, 0
sw, $t1, -172($fp)
add $t1, $t8, $t1
add $t8, $fp, -48
sw, $t1, -176($fp)
add $t1, $t8, $t1
lw $t2, 0($t1)
move $a0, $t2
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t9, 1
add $t0, $t0, $t9
sw, $t0, -48($fp)
sw, $t1, -180($fp)
sw, $t2, -184($fp)
j label14

label17:
li $v0, 0
lw $ra, 180($sp)
lw $fp, 176($sp)
addi $sp, $sp, 184
jr $ra
