# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# main_b : type 1 offset: -8 index_arg: -1 (used) (declared)
# main_x : type 1 offset: -4 index_arg: -1 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : -8
.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
    jmp main_epilogue
