# Variable Table:
# Name: main_a, Type: int32_t, offset: -4, declared: 1, used: 1
# Name: main_b, Type: int32_t, offset: -8, declared: 1, used: 1
# Name: main_c, Type: int32_t, offset: -12, declared: 1, used: 1
# Name: putchar_0, Type: int32_t, offset: 0, declared: 1, used: 0
# Function Table:
# Name: getchar, Return Type: int32_t, Parameters: 
# Name: main, Return Type: int32_t, Parameters: 
# Name: putchar, Return Type: void, Parameters: int32_t putchar_0, 
.text
.globl main

main:
    pushq %rbp # prologue
    movq %rsp, %rbp
    subq $32, %rsp
main_0:
    movl $17, %eax # ldconstint
    movl %eax, -4(%rbp) # wmem
    movl $42, %eax # ldconstint
    movl %eax, -8(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, -16(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, %ecx # copy
    movl -16(%rbp), %eax # rmem
    imull %ecx, %eax # mul
    movl %eax, -20(%rbp) # wmem
    movl -8(%rbp), %eax # rmem
    movl %eax, -24(%rbp) # wmem
    movl -8(%rbp), %eax # rmem
    movl %eax, %ecx # copy
    movl -24(%rbp), %eax # rmem
    imull %ecx, %eax # mul
    movl %eax, %ecx # copy
    movl -20(%rbp), %eax # rmem
    addl %ecx, %eax # add
    movl %eax, -28(%rbp) # wmem
    movl $1, %ecx # ldconstint
    movl -28(%rbp), %eax # rmem
    addl %ecx, %eax # add
    movl %eax, -12(%rbp) # wmem
    movl -12(%rbp), %eax # rmem
    jmp main_epilogue # jump
main_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret
