# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# print : type 0 offset: 0 index_arg: 1 (used) (declared)
# print_a : type 1 offset: 0 index_arg: 0 (used) (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

print:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    jmp print_0
print_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
print_0:
    movl %edi, %eax
    movl %eax, -4(%rbp)
    push %rdi
    movl -4(%rbp), %edi
    call putchar
    pop %rdi
    jmp print_epilogue

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
    movl $45, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %edi
    call print
    movl $0, %eax
    jmp main_epilogue
