.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $1, %eax 
    movl %eax, -4(%rbp)
    movl $2, %eax 
    movl %eax, -8(%rbp)
    movl -4(%rbp), %eax 
    movl %eax, %edx 
    movl -8(%rbp), %eax 
    addl %edx, %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax 
#epilogue 
    popq %rbp
    ret
