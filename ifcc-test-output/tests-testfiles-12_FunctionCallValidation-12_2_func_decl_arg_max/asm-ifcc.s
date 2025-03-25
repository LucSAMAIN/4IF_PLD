# expr : type 1 offset: 0 index_arg: 6 (used) (declared)
# expr_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# expr_b : type 1 offset: 0 index_arg: 1 (used) (declared)
# expr_c : type 1 offset: 0 index_arg: 2 (used) (declared)
# expr_d : type 1 offset: 0 index_arg: 3 (used) (declared)
# expr_e : type 1 offset: 0 index_arg: 4 (used) (declared)
# expr_f : type 1 offset: 0 index_arg: 5 (used) (declared)
# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

expr:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    jmp expr_0
expr_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
expr_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl %esi, %eax
    movl -4(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -8(%rbp)
    movl %edx, %eax
    movl %eax, -12(%rbp)
    movl %ecx, %eax
    movl -12(%rbp), %ebx
    addl %ebx, %eax
    movl -8(%rbp), %ebx
    imull %ebx, %eax
    movl %eax, -16(%rbp)
    movl %r8d, %eax
    movl %eax, -20(%rbp)
    movl %r9d, %eax
    movl -20(%rbp), %ebx
    addl %ebx, %eax
    movl -16(%rbp), %ebx
    imull %ebx, %eax
    jmp expr_epilogue

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
    movl $3, %eax
    movl %eax, -4(%rbp)
    movl $7, %eax
    movl %eax, -8(%rbp)
    movl $5, %eax
    movl %eax, -12(%rbp)
    movl $10, %eax
    movl %eax, -16(%rbp)
    movl $123, %eax
    movl %eax, -20(%rbp)
    movl $-6, %eax
    movl %eax, -24(%rbp)
    movl -4(%rbp), %edi
    movl -8(%rbp), %esi
    movl -12(%rbp), %edx
    movl -16(%rbp), %ecx
    movl -20(%rbp), %r8d
    movl -24(%rbp), %r9d
    call expr
    jmp main_epilogue
