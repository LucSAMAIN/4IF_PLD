
# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $0, %rsp
main0:
    movl $1, %eax
    movl $2, %eax
    movl $3, %eax
    movl $4, %eax
    movl $1, %eax
    movl $5, %eax
    movl $6, %eax
    movl $7, %eax
    movl $8, %eax
    movl $9, %eax
    movl $9, %eax
    movl $0, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
