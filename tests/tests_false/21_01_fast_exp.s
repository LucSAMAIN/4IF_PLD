.text
.globl main

print_int:
    pushq %rbp # prologue
    movq %rsp, %rbp
    subq $48, %rsp
    movl %edi, -4(%rbp)
print_int_0:
    movl -4(%rbp), %eax # rmem
    movl %eax, -8(%rbp) # wmem
    movl $0, %ecx # ldconstint
    movl -8(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    setl %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je print_int_1_endif
    jmp print_int_1_if_true
print_int_1_if_true:
    movl $45, %eax # ldconstint
    movl %eax, -12(%rbp) # wmem
    # call putchar
    movl -12(%rbp), %edi
    call putchar
    movl -4(%rbp), %eax # rmem
    neg %eax # unary minus
    movl %eax, -4(%rbp) # wmem
    # jmp print_int_1_endif # jump
print_int_1_endif:
    movl -4(%rbp), %eax # rmem
    movl %eax, -16(%rbp) # wmem
    movl $10, %ecx # ldconstint
    movl -16(%rbp), %eax # rmem
    cqo # div
    idivl %ecx
    movl %eax, -20(%rbp) # wmem
    movl $0, %ecx # ldconstint
    movl -20(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    setne %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je print_int_3_endif
    jmp print_int_3_if_true
print_int_3_if_true:
    movl -4(%rbp), %eax # rmem
    movl %eax, -28(%rbp) # wmem
    movl $10, %ecx # ldconstint
    movl -28(%rbp), %eax # rmem
    cqo # div
    idivl %ecx
    movl %eax, -24(%rbp) # wmem
    # call print_int
    movl -24(%rbp), %edi
    call print_int
    # jmp print_int_3_endif # jump
print_int_3_endif:
    movl -4(%rbp), %eax # rmem
    movl %eax, -36(%rbp) # wmem
    movl $10, %ecx # ldconstint
    movl -36(%rbp), %eax # rmem
    cqo # mod
    idivl %ecx
    movl %edx, %eax
    movl %eax, -40(%rbp) # wmem
    movl $48, %ecx # ldconstint
    movl -40(%rbp), %eax # rmem
    addl %ecx, %eax # add
    movl %eax, -32(%rbp) # wmem
    # call putchar
    movl -32(%rbp), %edi
    call putchar
    jmp print_int_epilogue # jump
print_int_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret

fast_exp:
    pushq %rbp # prologue
    movq %rsp, %rbp
    subq $64, %rsp
    movl %edi, -4(%rbp)
    movl %esi, -8(%rbp)
fast_exp_0:
    movl -8(%rbp), %eax # rmem
    movl %eax, -12(%rbp) # wmem
    movl $0, %ecx # ldconstint
    movl -12(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je fast_exp_1_endif
    jmp fast_exp_1_if_true
fast_exp_1_if_true:
    movl $1, %eax # ldconstint
    jmp fast_exp_epilogue # jump
fast_exp_1_endif:
    movl -8(%rbp), %eax # rmem
    movl %eax, -16(%rbp) # wmem
    movl $1, %ecx # ldconstint
    movl -16(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je fast_exp_3_endif
    jmp fast_exp_3_if_true
fast_exp_3_if_true:
    movl -4(%rbp), %eax # rmem
    jmp fast_exp_epilogue # jump
fast_exp_3_endif:
    movl -8(%rbp), %eax # rmem
    movl %eax, -20(%rbp) # wmem
    movl $2, %ecx # ldconstint
    movl -20(%rbp), %eax # rmem
    cqo # mod
    idivl %ecx
    movl %edx, %eax
    movl %eax, -24(%rbp) # wmem
    movl $0, %ecx # ldconstint
    movl -24(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    sete %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je fast_exp_5_if_false
    jmp fast_exp_5_if_true
fast_exp_5_if_true:
    movl -4(%rbp), %eax # rmem
    movl %eax, -32(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, %ecx # copy
    movl -32(%rbp), %eax # rmem
    imull %ecx, %eax # mul
    movl %eax, -28(%rbp) # wmem
    movl -8(%rbp), %eax # rmem
    movl %eax, -40(%rbp) # wmem
    movl $2, %ecx # ldconstint
    movl -40(%rbp), %eax # rmem
    cqo # div
    idivl %ecx
    movl %eax, -36(%rbp) # wmem
    # call fast_exp
    movl -28(%rbp), %edi
    movl -36(%rbp), %esi
    call fast_exp
    jmp fast_exp_epilogue # jump
fast_exp_5_if_false:
    movl -4(%rbp), %eax # rmem
    movl %eax, -44(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, -52(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, %ecx # copy
    movl -52(%rbp), %eax # rmem
    imull %ecx, %eax # mul
    movl %eax, -48(%rbp) # wmem
    movl -8(%rbp), %eax # rmem
    movl %eax, -60(%rbp) # wmem
    movl $2, %ecx # ldconstint
    movl -60(%rbp), %eax # rmem
    cqo # div
    idivl %ecx
    movl %eax, -56(%rbp) # wmem
    # call fast_exp
    movl -48(%rbp), %edi
    movl -56(%rbp), %esi
    call fast_exp
    movl %eax, %ecx # copy
    movl -44(%rbp), %eax # rmem
    imull %ecx, %eax # mul
    jmp fast_exp_epilogue # jump
fast_exp_5_endif:
    jmp fast_exp_epilogue # jump
fast_exp_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret

print_intro:
    pushq %rbp # prologue
    movq %rsp, %rbp
    subq $96, %rsp
print_intro_0:
    movl $69, %eax # ldconstint
    movl %eax, -4(%rbp) # wmem
    # call putchar
    movl -4(%rbp), %edi
    call putchar
    movl $110, %eax # ldconstint
    movl %eax, -8(%rbp) # wmem
    # call putchar
    movl -8(%rbp), %edi
    call putchar
    movl $116, %eax # ldconstint
    movl %eax, -12(%rbp) # wmem
    # call putchar
    movl -12(%rbp), %edi
    call putchar
    movl $101, %eax # ldconstint
    movl %eax, -16(%rbp) # wmem
    # call putchar
    movl -16(%rbp), %edi
    call putchar
    movl $114, %eax # ldconstint
    movl %eax, -20(%rbp) # wmem
    # call putchar
    movl -20(%rbp), %edi
    call putchar
    movl $32, %eax # ldconstint
    movl %eax, -24(%rbp) # wmem
    # call putchar
    movl -24(%rbp), %edi
    call putchar
    movl $98, %eax # ldconstint
    movl %eax, -28(%rbp) # wmem
    # call putchar
    movl -28(%rbp), %edi
    call putchar
    movl $97, %eax # ldconstint
    movl %eax, -32(%rbp) # wmem
    # call putchar
    movl -32(%rbp), %edi
    call putchar
    movl $115, %eax # ldconstint
    movl %eax, -36(%rbp) # wmem
    # call putchar
    movl -36(%rbp), %edi
    call putchar
    movl $101, %eax # ldconstint
    movl %eax, -40(%rbp) # wmem
    # call putchar
    movl -40(%rbp), %edi
    call putchar
    movl $32, %eax # ldconstint
    movl %eax, -44(%rbp) # wmem
    # call putchar
    movl -44(%rbp), %edi
    call putchar
    movl $97, %eax # ldconstint
    movl %eax, -48(%rbp) # wmem
    # call putchar
    movl -48(%rbp), %edi
    call putchar
    movl $110, %eax # ldconstint
    movl %eax, -52(%rbp) # wmem
    # call putchar
    movl -52(%rbp), %edi
    call putchar
    movl $100, %eax # ldconstint
    movl %eax, -56(%rbp) # wmem
    # call putchar
    movl -56(%rbp), %edi
    call putchar
    movl $32, %eax # ldconstint
    movl %eax, -60(%rbp) # wmem
    # call putchar
    movl -60(%rbp), %edi
    call putchar
    movl $101, %eax # ldconstint
    movl %eax, -64(%rbp) # wmem
    # call putchar
    movl -64(%rbp), %edi
    call putchar
    movl $120, %eax # ldconstint
    movl %eax, -68(%rbp) # wmem
    # call putchar
    movl -68(%rbp), %edi
    call putchar
    movl $112, %eax # ldconstint
    movl %eax, -72(%rbp) # wmem
    # call putchar
    movl -72(%rbp), %edi
    call putchar
    movl $32, %eax # ldconstint
    movl %eax, -76(%rbp) # wmem
    # call putchar
    movl -76(%rbp), %edi
    call putchar
    movl $58, %eax # ldconstint
    movl %eax, -80(%rbp) # wmem
    # call putchar
    movl -80(%rbp), %edi
    call putchar
    movl $10, %eax # ldconstint
    movl %eax, -84(%rbp) # wmem
    # call putchar
    movl -84(%rbp), %edi
    call putchar
    jmp print_intro_epilogue # jump
print_intro_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret

main:
    pushq %rbp # prologue
    movq %rsp, %rbp
    subq $64, %rsp
main_0:
    movl $0, %eax # ldconstint
    movl %eax, -8(%rbp) # wmem
    movl $0, %eax # ldconstint
    movl %eax, -12(%rbp) # wmem
    movl $0, %eax # ldconstint
    movl %eax, -16(%rbp) # wmem
    # call print_intro
    call print_intro
main_1_test_while:
    # call getchar
    call getchar
    movl %eax, -4(%rbp) # wmem
    movl %eax, -20(%rbp) # wmem
    movl $10, %ecx # ldconstint
    movl -20(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    setne %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je main_2_endwhile
    jmp main_2_while_true
main_2_while_true:
    movl $10, %eax # ldconstint
    movl -8(%rbp), %ebx # rmem
    imull %ebx, %eax # mul
    movl %eax, -8(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, -24(%rbp) # wmem
    movl $48, %ecx # ldconstint
    movl -24(%rbp), %eax # rmem
    subl %ecx, %eax # sub
    movl -8(%rbp), %ebx # rmem
    addl %ebx, %eax # add
    movl %eax, -8(%rbp) # wmem
    jmp main_1_test_while # jump
main_2_endwhile:
main_4_test_while:
    # call getchar
    call getchar
    movl %eax, -4(%rbp) # wmem
    movl %eax, -28(%rbp) # wmem
    movl $10, %ecx # ldconstint
    movl -28(%rbp), %eax # rmem
    cmp %ecx, %eax # compare int
    setne %al
    movzbl %al, %eax
    cmpl $0, %eax # jump false
    je main_5_endwhile
    jmp main_5_while_true
main_5_while_true:
    movl $10, %eax # ldconstint
    movl -12(%rbp), %ebx # rmem
    imull %ebx, %eax # mul
    movl %eax, -12(%rbp) # wmem
    movl -4(%rbp), %eax # rmem
    movl %eax, -32(%rbp) # wmem
    movl $48, %ecx # ldconstint
    movl -32(%rbp), %eax # rmem
    subl %ecx, %eax # sub
    movl -12(%rbp), %ebx # rmem
    addl %ebx, %eax # add
    movl %eax, -12(%rbp) # wmem
    jmp main_4_test_while # jump
main_5_endwhile:
    movl -8(%rbp), %eax # rmem
    movl %eax, -36(%rbp) # wmem
    movl -12(%rbp), %eax # rmem
    movl %eax, -40(%rbp) # wmem
    # call fast_exp
    movl -36(%rbp), %edi
    movl -40(%rbp), %esi
    call fast_exp
    movl %eax, -16(%rbp) # wmem
    movl -8(%rbp), %eax # rmem
    movl %eax, -44(%rbp) # wmem
    # call print_int
    movl -44(%rbp), %edi
    call print_int
    movl $94, %eax # ldconstint
    movl %eax, -48(%rbp) # wmem
    # call putchar
    movl -48(%rbp), %edi
    call putchar
    movl -12(%rbp), %eax # rmem
    movl %eax, -52(%rbp) # wmem
    # call print_int
    movl -52(%rbp), %edi
    call print_int
    movl $61, %eax # ldconstint
    movl %eax, -56(%rbp) # wmem
    # call putchar
    movl -56(%rbp), %edi
    call putchar
    movl -16(%rbp), %eax # rmem
    movl %eax, -60(%rbp) # wmem
    # call print_int
    movl -60(%rbp), %edi
    call print_int
    movl $10, %eax # ldconstint
    movl %eax, -64(%rbp) # wmem
    # call putchar
    movl -64(%rbp), %edi
    call putchar
    movl $0, %eax # ldconstint
    jmp main_epilogue # jump
main_epilogue:
    movq %rbp, %rsp # epilogue
    popq %rbp
    ret
