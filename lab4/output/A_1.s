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
subu $sp, $sp, 112
sw $ra, 108($sp)
sw $fp, 104($sp)
addi $fp, $sp, 112
li $t0, 3
li $t1, 8
li $t2, 24
mul $t3, $t0, $t0
sw, $t1, -44($fp)
div $t2, $t1
mflo $t1
sw, $t1, -56($fp)
add $t1, $t3, $t1
li $t9, 4
mul $t4, $t1, $t9
move $a0, $t4
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t8, 3
mul $t5, $t8, $t0
sw, $t5, -64($fp)
add $t5, $t4, $t5
sw, $t2, -48($fp)
sub $t2, $t5, $t2
li $t9, 55
mul $t6, $t2, $t9
div $t6, $t0
mflo $t7
move $a0, $t7
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t8, 10
mul $s0, $t8, $t0
mul $s1, $s0, $t4
sw, $s1, -84($fp)
add $s1, $t0, $s1
li $t9, 4
mul $s2, $t7, $t9
sw, $s2, -92($fp)
add $s2, $s1, $s2
sw, $t0, -40($fp)
div $t4, $t0
mflo $t0
li $t9, 6
sub $s3, $t0, $t9
sw, $s3, -104($fp)
add $s3, $s2, $s3
move $a0, $s3
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $v0, 0
lw $ra, 108($sp)
lw $fp, 104($sp)
addi $sp, $sp, 112
jr $ra
