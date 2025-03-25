# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# main_x : type 1 offset: -4 index_arg: -1 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : -4
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
    movl $5, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl $5, %ecx
    movl -8(%rbp), %eax
    cmp %ecx, %eax
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax
    je main_3_endif
    jmp main_3_if_true
main_3_if_true:
    movl $3, %eax
    jmp main_epilogue
main_3_endif:
    movl -4(%rbp), %eax
    jmp main_epilogue
