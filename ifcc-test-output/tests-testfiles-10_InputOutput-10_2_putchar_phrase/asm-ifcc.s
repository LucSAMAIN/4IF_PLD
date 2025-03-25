# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    movl $72, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %edi
    call putchar
    movl $101, %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %edi
    call putchar
    movl $108, %eax
    movl %eax, -12(%rbp)
    movl -12(%rbp), %edi
    call putchar
    movl $108, %eax
    movl %eax, -16(%rbp)
    movl -16(%rbp), %edi
    call putchar
    movl $111, %eax
    movl %eax, -20(%rbp)
    movl -20(%rbp), %edi
    call putchar
    movl $32, %eax
    movl %eax, -24(%rbp)
    movl -24(%rbp), %edi
    call putchar
    movl $87, %eax
    movl %eax, -28(%rbp)
    movl -28(%rbp), %edi
    call putchar
    movl $111, %eax
    movl %eax, -32(%rbp)
    movl -32(%rbp), %edi
    call putchar
    movl $114, %eax
    movl %eax, -36(%rbp)
    movl -36(%rbp), %edi
    call putchar
    movl $108, %eax
    movl %eax, -40(%rbp)
    movl -40(%rbp), %edi
    call putchar
    movl $100, %eax
    movl %eax, -44(%rbp)
    movl -44(%rbp), %edi
    call putchar
    movl $33, %eax
    movl %eax, -48(%rbp)
    movl -48(%rbp), %edi
    call putchar
    movl $10, %eax
    movl %eax, -52(%rbp)
    movl -52(%rbp), %edi
    call putchar
    movl $0, %eax
    jmp main_epilogue
