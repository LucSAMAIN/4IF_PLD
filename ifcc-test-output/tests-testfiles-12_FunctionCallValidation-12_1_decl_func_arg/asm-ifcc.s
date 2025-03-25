# add : type 1 offset: 0 index_arg: 2 (used) (declared)
# add_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# add_b : type 1 offset: 0 index_arg: 1 (used) (declared)
# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

add:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    jmp add_0
add_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
add_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl %esi, %eax
    movl -4(%rbp), %ebx
    addl %ebx, %eax
    jmp add_epilogue

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
    movl $3, %eax
    movl %eax, -4(%rbp)
    movl $7, %eax
    movl %eax, -8(%rbp)
    movl -4(%rbp), %edi
    movl -8(%rbp), %esi
    call add
    jmp main_epilogue
