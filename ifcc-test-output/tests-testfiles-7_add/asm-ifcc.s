# main_a : type 1 offset: -4 (used) (declared)
# main : -4

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
main0:
    movl $1, %eax
    movl %eax, -8(%rbp)
    movl $1, %eax
    movl %eax, -12(%rbp)
    movl -8(%rbp), %ebx
    movl -12(%rbp), %eax
    addl %ebx, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
