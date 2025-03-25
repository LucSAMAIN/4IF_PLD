# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
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
    movl $97, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %edi
    call putchar
    movl $0, %eax
    jmp main_epilogue
