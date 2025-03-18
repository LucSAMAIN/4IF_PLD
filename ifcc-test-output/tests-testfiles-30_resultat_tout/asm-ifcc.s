
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
    movl $2, %eax
    movl $3, %eax
    movl -4(%rbp), %ebx
    addl %ebx, %eax
    movl $4, %eax
    movl $1, %eax
    movl %eax, -8(%rbp)
    movl $5, %eax
    movl %eax, %ecx
    movl -8(%rbp), %eax
    subl %ecx, %eax
    movl $6, %eax
    movl $7, %eax
    movl $8, %eax
    movl $9, %eax
    movl $9, %eax
    movl %eax, -12(%rbp)
    movl $0, %eax
    movl -12(%rbp), %ebx
    addl %ebx, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
