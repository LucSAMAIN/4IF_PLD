
==================== Symbol Table ====================
Variable	Index	Declared	Used
a		4	Yes		Yes
b		8	Yes		Yes
c		12	Yes		Yes
======================================================

.globl main
 main: 
#prologue 
    pushq %rbp
    movq %rsp, %rbp
    movq $0, -4(%rbp)
    movq $0, -8(%rbp)
    movq $0, -12(%rbp)
    movl $10, %rax
  movl -4(%rbp), %rax
    movl $20, %rax
  movl -8(%rbp), %rax
    movl $30, %rax
  movl -12(%rbp), %rax
    movl $42, %eax
#epilogue 
    popq %rbp
    ret
