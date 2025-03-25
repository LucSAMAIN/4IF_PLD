# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# main_a : type 1 offset: -4 index_arg: -1 (declared)
# main_b : type 1 offset: -8 index_arg: -1 (declared)
# main_c : type 1 offset: -12 index_arg: -1 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : -12
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
    jmp main_epilogue
