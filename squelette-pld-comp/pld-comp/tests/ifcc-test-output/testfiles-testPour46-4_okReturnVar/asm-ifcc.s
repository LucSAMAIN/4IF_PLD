
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
    # Accès à la variable a (à implémenter)
    movl $0, %eax
#epilogue 
    popq %rbp
    ret
