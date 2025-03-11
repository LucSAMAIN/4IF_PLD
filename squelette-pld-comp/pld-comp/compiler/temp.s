.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $1, %rax
    movq %rax, -4(%rbp)
    movq $2, %rax
    movq %rax, -8(%rbp)
#not implemented yet
    movq %rax, -12(%rbp)
    movq -12(%rbp), %rax
#epilogue 
    popq %rbp
    ret
.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $1, %rax
    movq %rax, -4(%rbp)
#not implemented yet
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rax
#epilogue 
    popq %rbp
    ret
