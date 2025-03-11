.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $2, %eax 
    movl %eax, -4(%rbp)
    movl $3, %eax 
    movl %eax, -8(%rbp)
    movl $5, %eax 
    movl %eax, -12(%rbp)
    movl -4(%rbp), %eax 
    movl %eax, %edx 
    movl -8(%rbp), %eax 
    movl %eax, -16(%rbp)
unary_operation_prefixe not implemented yet
    movl %eax, %edx 
    movl -20(%rbp), %eax 
    movl %eax, -20(%rbp)
    movl -20(%rbp), %eax 
#epilogue 
    popq %rbp
    ret
