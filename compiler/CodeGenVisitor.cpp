#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    std::cout << ".text\n";
    std::cout << ".globl main\n";
    std::cout << "main: \n";

    std::cout << "    pushq %rbp # save %rbp on the stack\n";
    std::cout << "    movq %rsp, %rbp # define %rbp for the current function\n";
    // std::cout << "    subq $16, %rsp # allocate space for local variables\n";

    visit(ctx->block());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    if (ctx->expr())
    {
        visit(ctx->expr());
        std::cout << "    movl %eax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp)\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    visit(ctx->expr());

    std::cout << "    movl %eax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visit(ctx->expr());

    std::cout << "    movq %rbp, %rsp\n";
    std::cout << "    popq %rbp # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    if (ctx->CONST()->getText() == "0") // xor is faster than mov
    {
        std::cout << "    xor %eax, %eax\n";
    }
    else
    std::cout << "    movl $" << ctx->CONST()->getText() << ", %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    std::cout << "    movl " << symbolTable[ctx->ID()->getText()].offset << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    visit(ctx->primary());
    std::cout << "    not %eax\n";
    std::cout << "    and $1, %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    visit(ctx->primary());
    std::cout << "    neg %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    movl %eax, %ebx\n";
    std::cout << "    popq %rax\n";
    if (ctx->mOp()->STAR())
    {
        std::cout << "    imul %ebx, %eax\n";
    }
    else if (ctx->mOp()->SLASH())
    {
        std::cout << "    cqo\n";
        std::cout << "    idiv %ebx\n";
    }
    else if (ctx->mOp()->MOD())
    {
        std::cout << "    cqo\n";
        std::cout << "    idiv %ebx\n";
        std::cout << "    movl %edx, %eax\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    movl %eax, %ebx\n";
    std::cout << "    popq %rax\n";
    if (ctx->aOp()->PLUS())
    {
        std::cout << "    addl %ebx, %eax\n";
    }
    else if (ctx->aOp()->MINUS())
    {
        std::cout << "    subl %ebx, %eax\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    movl %eax, %ebx\n";
    std::cout << "    popq %rax\n";
    std::cout << "    cmpl %eax, %ebx\n"; // right - left
    if (ctx->compOp()->LT())
    {
        std::cout << "    setl %al\n";
    }
    else if (ctx->compOp()->LE())
    {
        std::cout << "    setle %al\n";
    }
    else if (ctx->compOp()->GT())
    {
        std::cout << "    setg %al\n";
    }
    else if (ctx->compOp()->GE())
    {
        std::cout << "    setge %al\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    movl %eax, %ebx\n";
    std::cout << "    popq %rax\n";
    std::cout << "    cmpl %eax, %ebx\n";
    if (ctx->eqOp()->EQ())
    {
        std::cout << "    sete %al\n";
    }
    else if (ctx->eqOp()->NEQ())
    {
        std::cout << "    setne %al\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    popq %rbx\n";
    std::cout << "    andl %ebx, %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    popq %rbx\n";
    std::cout << "    xorl %ebx, %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    visit(ctx->left);
    std::cout << "    pushq %rax\n";
    visit(ctx->right);
    std::cout << "    popq %rbx\n";
    std::cout << "    orl %ebx, %eax\n";
    return 0;
}
