
# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
main0:
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl $2, %eax
    movl %eax, %ecx
    movl -4(%rbp), %eax
    subl %ecx, %eax
    movl %eax, -8(%rbp)
    movl $3, %eax
    movl -8(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -12(%rbp)
    movl $4, %eax
    movl %eax, %ecx
    movl -12(%rbp), %eax
    subl %ecx, %eax
    movl %eax, -16(%rbp)
    movl $5, %eax
    movl -16(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -20(%rbp)
    movl $6, %eax
    movl %eax, %ecx
    movl -20(%rbp), %eax
    subl %ecx, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
