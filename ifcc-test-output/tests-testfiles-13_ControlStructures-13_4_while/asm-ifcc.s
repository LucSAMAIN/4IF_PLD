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
    subq $32, %rsp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    movl $0, %eax
    movl %eax, -4(%rbp)
main_3_test_while:
    movl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    movl $5, %ecx
    movl -8(%rbp), %eax
    cmp %ecx, %eax
    setl %al
    movzbl %al, %eax
    cmpl $0, %eax
    je main_4_endwhile
    jmp main_4_while_true
main_4_while_true:
    movl -4(%rbp), %eax
    movl %eax, -16(%rbp)
    movl $48, %eax
    movl -16(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %edi
    call putchar
    movl $10, %eax
    movl %eax, -20(%rbp)
    movl -20(%rbp), %edi
    call putchar
    movl -4(%rbp), %eax
    movl %eax, -24(%rbp)
    movl $1, %eax
    movl -24(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -4(%rbp)
    jmp main_3_test_while
main_4_endwhile:
    movl $0, %eax
    jmp main_epilogue
