.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $6, %rax
    movq $6, %rax
#epilogue 
    popq %rbp
    ret
