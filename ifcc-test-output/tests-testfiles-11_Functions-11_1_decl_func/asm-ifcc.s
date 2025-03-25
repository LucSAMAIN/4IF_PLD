# func : type 1 offset: 0 index_arg: 0 (used) (declared)
# getchar : type 0 offset: 0 index_arg: 1 (used) (declared)
# main : type 1 offset: 0 index_arg: 0 (declared)
# putchar : type 0 offset: 0 index_arg: 1 (used) (declared)
.text
.globl main

func:
    pushq %rbp
    movq %rsp, %rbp
    jmp func_0
func_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
func_0:
    movl $5, %eax
    jmp func_epilogue

main:
    pushq %rbp
    movq %rsp, %rbp
    jmp main_0
main_epilogue:
    movq %rbp, %rsp
    popq %rbp
    ret
main_0:
    call func
    jmp main_epilogue
