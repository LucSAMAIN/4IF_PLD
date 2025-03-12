.text
.globl main
main: 
    pushq %rbp # save %rbp on the stack
    movq %rsp, %rbp # define %rbp for the current function
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl $9, %eax
    movl %eax, -8(%rbp)
    movl -4(%rbp), %eax
    movl %eax, %ebx
    movl $2, %eax
    movl %eax, %ebx
    movl $3, %eax
    imul %ebx, %eax
    addl %ebx, %eax
    movl %eax, %ebx
    movl $4, %eax
    movl %eax, %ebx
    movl $5, %eax
    movl %eax, %ecx
    movl %ebx, %eax
    cqo
    idivl %ecx
    movl %eax, %ebx
    movl $6, %eax
    movl %eax, %ecx
    movl %ebx, %eax
    cqo
    idivl %ecx
    movl %edx, %eax
    subl %eax, %ebx
    movl %ebx, %eax
    movl %eax, %ebx
    movl $7, %eax
    movl %eax, %ebx
    movl $8, %eax
    neg %eax
    andl %ebx, %eax
    xorl %ebx, %eax
    movl %eax, %ebx
    movl -8(%rbp), %eax
    movl %eax, %ebx
    xor %eax, %eax
    not %eax
    and $1, %eax
    andl %ebx, %eax
    orl %ebx, %eax
    popq %rbp # restore %rbp from the stack
    ret
