
# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
main0:
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl $4, %eax
    movl %eax, -8(%rbp)
    movl -4(%rbp), %ebx
    movl -8(%rbp), %eax
    addl %ebx, %eax
    movl $4, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
