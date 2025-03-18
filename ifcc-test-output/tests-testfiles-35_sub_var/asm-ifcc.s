# main_a : type 1 offset: -4 (used) (declared)
# main_b : type 1 offset: -8 (used) (declared)
# main_c : type 1 offset: -12 (used) (declared)
# main : -12

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
main0:
    movl $10, %eax
    movl %eax, -4(%rbp)
    movl $3, %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
    movl %eax, -16(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -20(%rbp)
    movl -16(%rbp), %eax
    movl -20(%rbp), %ecx
    subl %ecx, %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
