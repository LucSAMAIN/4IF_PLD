.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $0, -4(%rbp)
    movq $42, %rax
    movq %rax, -4(%rbp)
    movq $42, %rax
#epilogue 
    popq %rbp
    ret
