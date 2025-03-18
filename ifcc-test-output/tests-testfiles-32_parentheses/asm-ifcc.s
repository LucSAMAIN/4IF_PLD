
# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
main0:
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl $2, %eax
    movl %eax, -8(%rbp)
    movl $3, %eax
    movl %eax, -12(%rbp)
    movl $4, %eax
    movl %eax, -16(%rbp)
    movl $5, %eax
    movl %eax, -20(%rbp)
    movl $6, %eax
    movl %eax, -24(%rbp)
    movl -20(%rbp), %eax
    movl -24(%rbp), %ecx
    subl %ecx, %eax
    movl %eax, -28(%rbp)
    movl -16(%rbp), %ebx
    movl -28(%rbp), %eax
    addl %ebx, %eax
    movl %eax, -32(%rbp)
    movl -12(%rbp), %eax
    movl -32(%rbp), %ecx
    subl %ecx, %eax
    movl %eax, -36(%rbp)
    movl -8(%rbp), %ebx
    movl -36(%rbp), %eax
    addl %ebx, %eax
    movl %eax, -40(%rbp)
    movl -4(%rbp), %eax
    movl -40(%rbp), %ecx
    subl %ecx, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
