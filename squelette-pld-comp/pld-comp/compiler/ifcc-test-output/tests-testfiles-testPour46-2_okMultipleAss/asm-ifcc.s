.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $0, -4(%rbp)
    movq $0, -8(%rbp)
    movq $0, -12(%rbp)
    movq $10, %rax
    movq %rax, -4(%rbp)
    movq $20, %rax
    movq %rax, -8(%rbp)
    movq $30, %rax
    movq %rax, -12(%rbp)
    movq $42, %rax
#epilogue 
    popq %rbp
    ret
