# Variable Table:
# Name: main_a, Type: int32_t, offset: -4, declared: 1, used: 1
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
    subq $16, %rsp
main_0:
    movl $5, %eax # ldconstint
    movl %eax, -4(%rbp) # wmem
    # call getchar
    call getchar
    movl %eax, -4(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    jmp main_epilogue # jump
main_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret
