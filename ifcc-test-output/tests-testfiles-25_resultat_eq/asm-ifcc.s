
# Génération du code assembleur x86 à partir de l'IR :
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $0, %rsp
main0:
    movl $1, %eax
    movl $1, %eax
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
