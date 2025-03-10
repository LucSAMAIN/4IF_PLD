
==================== Symbol Table ====================
Variable	Index	Declared	Used
a		4	Yes		Yes
======================================================

.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movl $3, %rax
    movq %rax, -4(%rbp)
    movl $42, %eax
#epilogue 
    popq %rbp
    ret
