# main_a : type 1 offset: -4 (declared)
# main_b : type 1 offset: -8 (declared)
# main_c : type 1 offset: -12 (used) (declared)
# main_d : type 1 offset: -16 (used) (declared)
# main : -16

# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
main0:
    movl $12, %eax
    movl %eax, -20(%rbp)
    movl $5, %eax
    movl -20(%rbp), %ebx
    addl %ebx, %eax
    movl $4, %eax
    movl %eax, -4(%rbp)
    movl $5, %eax
    movl $2, %eax
    movl $5, %eax
    movl $6, %eax
    movl %eax, -8(%rbp)
    movl $5, %eax
    movl $3, %eax
    movl %eax, -24(%rbp)
    movl $2, %eax
    movl -24(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -12(%rbp)
    movl $5, %eax
    movl $7, %eax
    movl %eax, -28(%rbp)
    movl $1, %eax
    movl %eax, %ecx
    movl -28(%rbp), %eax
    subl %ecx, %eax
    movl %eax, -16(%rbp)
    movl -12(%rbp), %eax
    movl -16(%rbp), %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
