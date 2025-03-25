# fibo : type 1 offset: 0 index_arg: 1 (used) (declared)
# fibo_n : type 1 offset: 0 index_arg: 0 (used) (declared)
# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

fibo:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    jmp fibo_0
fibo_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
fibo_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    movl $0, %ecx
    movl -4(%rbp), %eax
    cmp %ecx, %eax
    setle %al
    movzbl %al, %eax
    cmpl $0, %eax
    je fibo_3_if_false
    jmp fibo_3_if_true
fibo_3_if_true:
    movl $0, %eax
    jmp fibo_epilogue
fibo_3_if_false:
    movl %edi, %eax
    movl %eax, -8(%rbp)
    movl $1, %ecx
    movl -8(%rbp), %eax
    cmp %ecx, %eax
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax
    je fibo_6_endif
    jmp fibo_6_if_true
fibo_3_endif:
    movl %edi, %eax
    movl %eax, -16(%rbp)
    movl $1, %eax
    movl %eax, %ecx
    movl -16(%rbp), %eax
    subl %ecx, %eax
    movl %eax, -12(%rbp)
    push %rdi
    movl -12(%rbp), %edi
    call fibo
    pop %rdi
    movl %eax, -20(%rbp)
    movl %edi, %eax
    movl %eax, -28(%rbp)
    movl $2, %eax
    movl %eax, %ecx
    movl -28(%rbp), %eax
    subl %ecx, %eax
    movl %eax, -24(%rbp)
    push %rdi
    movl -24(%rbp), %edi
    call fibo
    pop %rdi
    movl -20(%rbp), %ebx
    addl %ebx, %eax
    jmp fibo_epilogue
fibo_6_if_true:
    movl $1, %eax
    jmp fibo_epilogue
fibo_6_endif:
    jmp fibo_3_endif

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
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %edi
    call fibo
    jmp main_epilogue
