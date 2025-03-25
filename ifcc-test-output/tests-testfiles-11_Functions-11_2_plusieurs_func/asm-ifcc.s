# add : type 1 offset: 0 index_arg: 2 (used) (declared)
# add_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# add_b : type 1 offset: 0 index_arg: 1 (used) (declared)
# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# mul : type 1 offset: 0 index_arg: 2 (used) (declared)
# mul_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# mul_b : type 1 offset: 0 index_arg: 1 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# sub : type 1 offset: 0 index_arg: 2 (used) (declared)
# sub_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# sub_b : type 1 offset: 0 index_arg: 1 (used) (declared)
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

sub:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    jmp sub_0
sub_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
sub_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl %esi, %eax
    movl %eax, %ecx
    movl -4(%rbp), %eax
    subl %ecx, %eax
    jmp sub_epilogue

mul:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    jmp mul_0
mul_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
mul_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl %esi, %eax
    movl -4(%rbp), %ebx
    imull %ebx, %eax
    jmp mul_epilogue

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
    movl $5, %eax
    movl %eax, -8(%rbp)
    movl $6, %eax
    movl %eax, -12(%rbp)
    movl -8(%rbp), %edi
    movl -12(%rbp), %esi
    call add
    movl %eax, -4(%rbp)
    movl $7, %eax
    movl %eax, -20(%rbp)
    movl $3, %eax
    movl %eax, -24(%rbp)
    movl -20(%rbp), %edi
    movl -24(%rbp), %esi
    call sub
    movl %eax, -16(%rbp)
    movl -4(%rbp), %edi
    movl -16(%rbp), %esi
    call mul
    jmp main_epilogue
