# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# print_int : type 0 offset: 0 index_arg: 1 (used) (declared)
# print_int_x : type 1 offset: 0 index_arg: 0 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

print_int:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    jmp print_int_0
print_int_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
print_int_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl $0, %ecx
    movl -4(%rbp), %eax
    cmp %ecx, %eax
    setl %al
    movzbl %al, %eax
    cmpl $0, %eax
    je print_int_3_endif
    jmp print_int_3_if_true
print_int_3_if_true:
    movl $45, %eax
    movl %eax, -8(%rbp)
    push %rdi
    movl -8(%rbp), %edi
    call putchar
    pop %rdi
    movl %edi, %eax
    neg %eax
    movl %eax, %edi
    jmp print_int_3_endif
print_int_3_endif:
    movl %edi, %eax
    movl %eax, -12(%rbp)
    movl $10, %eax
    movl %eax, %ecx
    movl -12(%rbp), %eax
    cqo
    idivl %ecx
    movl %eax, -16(%rbp)
    movl $0, %ecx
    movl -16(%rbp), %eax
    cmp %ecx, %eax
    setne %al
    movzbl %al, %eax
    cmpl $0, %eax
    je print_int_5_endif
    jmp print_int_5_if_true
print_int_5_if_true:
    movl %edi, %eax
    movl %eax, -24(%rbp)
    movl $10, %eax
    movl %eax, %ecx
    movl -24(%rbp), %eax
    cqo
    idivl %ecx
    movl %eax, -20(%rbp)
    push %rdi
    movl -20(%rbp), %edi
    call print_int
    pop %rdi
    jmp print_int_5_endif
print_int_5_endif:
    movl %edi, %eax
    movl %eax, -32(%rbp)
    movl $10, %eax
    movl %eax, %ecx
    movl -32(%rbp), %eax
    cqo
    idivl %ecx
    movl %edx, %eax
    movl %eax, -36(%rbp)
    movl $48, %eax
    movl -36(%rbp), %ebx
    addl %ebx, %eax
    movl %eax, -28(%rbp)
    push %rdi
    movl -28(%rbp), %edi
    call putchar
    pop %rdi
    jmp print_int_epilogue

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
    movl $-273, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %edi
    call print_int
    movl $10, %eax
    movl %eax, -8(%rbp)
    movl -8(%rbp), %edi
    call putchar
    movl $0, %eax
    jmp main_epilogue
