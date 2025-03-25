# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    movl $-3, %eax
    jmp main_epilogue
