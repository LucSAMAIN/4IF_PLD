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
    movl $7, %ecx
    movl -8(%rbp), %eax
    cmp %ecx, %eax
    setl %al
    movzbl %al, %eax
    cmpl $0, %eax
    je main_3_if_false
    jmp main_3_if_true
main_3_if_true:
    movl -4(%rbp), %eax
    movl %eax, -12(%rbp)
    movl $5, %ecx
    movl -12(%rbp), %eax
    cmp %ecx, %eax
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax
    je main_6_if_false
    jmp main_6_if_true
main_3_if_false:
    movl $3, %eax
    movl %eax, -4(%rbp)
    jmp main_3_endif
main_3_endif:
    movl -4(%rbp), %eax
    jmp main_epilogue
main_6_if_true:
    movl $1, %eax
    movl %eax, -4(%rbp)
    jmp main_6_endif
main_6_if_false:
    movl $2, %eax
    movl %eax, -4(%rbp)
    jmp main_6_endif
main_6_endif:
    jmp main_3_endif
