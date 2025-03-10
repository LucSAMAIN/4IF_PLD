.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $3, %rax
    movq %rax, -4(%rbp)
    movq -4(%rbp), %rax
#epilogue 
    popq %rbp
    ret
