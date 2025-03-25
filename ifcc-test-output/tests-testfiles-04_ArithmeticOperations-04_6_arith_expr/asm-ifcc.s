# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# main_a : type 1 offset: -4 index_arg: -1 (declared)
# main_b : type 1 offset: -8 index_arg: -1 (declared)
# main_c : type 1 offset: -12 index_arg: -1 (used) (declared)
# main_d : type 1 offset: -16 index_arg: -1 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : -16
.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    movl $68, %eax
    movl %eax, -4(%rbp)
    movl $-42, %eax
    movl %eax, -8(%rbp)
    movl $5, %eax
    movl %eax, -12(%rbp)
    movl $7, %eax
    movl %eax, -16(%rbp)
    movl -12(%rbp), %eax
    movl %eax, -20(%rbp)
    movl -16(%rbp), %eax
    movl %eax, %ecx
    movl -20(%rbp), %eax
    cqo
    idivl %ecx
    jmp main_epilogue
