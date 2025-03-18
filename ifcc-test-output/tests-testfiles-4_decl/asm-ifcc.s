# main_b : type 1 offset: -8 (used) (declared)
# main_x : type 1 offset: -4 (used) (declared)
# main : -8

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
main0:
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
