.globl main
main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $1, %eax 
    movl %eax, -4(%rbp)
unary_operation_prefixe not implemented yet
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax 
#epilogue 
    popq %rbp
    ret
