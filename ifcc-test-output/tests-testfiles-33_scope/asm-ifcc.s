# main_2_a : type 1 offset: -8 (declared)
# main_3_a : type 1 offset: -16 (declared)
# main_3_i : type 1 offset: -12 (used) (declared)
# main_4_a : type 1 offset: -20 (declared)
# main_i : type 1 offset: -4 (used) (declared)
# main : -20

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
main0:
    movl $0, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -4(%rbp)
    movl 0(%rbp), %eax
    movl %eax, -8(%rbp)
    movl $1, %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -16(%rbp)
    movl 0(%rbp), %eax
    movl %eax, -20(%rbp)
    movl -4(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
