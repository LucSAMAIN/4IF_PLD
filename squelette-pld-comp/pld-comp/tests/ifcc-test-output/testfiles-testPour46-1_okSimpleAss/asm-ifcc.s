
==================== Symbol Table ====================
Variable	Index	Declared	Used
a		4	Yes		Yes
======================================================

.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $0, -4(%rbp)
    movl $42, %rax
  movl -4(%rbp), %rax
    movl $42, %eax
#epilogue 
    popq %rbp
    ret
