# main_x : type 1 offset: -4 (used) (declared)
# main : -4

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
main0:
    movl $5, %eax
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
