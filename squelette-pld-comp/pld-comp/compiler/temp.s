.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $10, %eax 
    movl %eax, -4(%rbp)
    movl $2, %eax 
    movl %eax, -8(%rbp)
    movl -4(%rbp), %eax 
    movl %eax, %edx 
    movl -8(%rbp), %eax 
    subl %edx, %eax
#epilogue 
    popq %rbp
    ret
