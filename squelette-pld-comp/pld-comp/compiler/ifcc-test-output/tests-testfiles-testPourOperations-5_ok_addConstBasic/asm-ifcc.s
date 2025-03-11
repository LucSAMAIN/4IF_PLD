.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -4(%rbp)
    movl $1, %eax 
    movl %eax, %edx 
    movl $2, %eax 
    addl %edx, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax 
#epilogue 
    popq %rbp
    ret
