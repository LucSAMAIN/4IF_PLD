#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    std::cout << ".text\n";
    std::cout<< ".globl main\n";
    std::cout<< "main: \n";

    std::cout << "    pushq %rbp # save %rbp on the stack\n";
    std::cout << "    movq %rsp, %rbp # define %rbp for the current function\n";

    visit(ctx->block());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    if (ctx->expr())
    {
        visit(ctx->expr());
        std::cout << "    movq %rax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp)\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    visit(ctx->expr());

    std::cout << "    movq %rax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    visit(ctx->expr());

    std::cout << "    popq %rbp # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    if (ctx->CONST()->getText() == "0")
    {
        std::cout << "    xor %rax, %rax\n";
    }
    else
    std::cout << "    movq $" << ctx->CONST()->getText() << ", %rax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    std::cout << "    movq " << symbolTable[ctx->ID()->getText()].offset << "(%rbp), %rax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    visit(ctx->primary());
    std::cout << "    not %rax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    visit(ctx->primary());
    std::cout << "    neg %rax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    visit(ctx->left());
    std::cout << "    movq %rax, %rbx\n";
    visit(ctx->right());
    std::cout << "    movq %rbx, %rcx\n";
    if (ctx->mOp()->STAR())
    {
        std::cout << "    imul %rcx, %rax\n";
    }
    else
    {
        std::cout << "    cqo\n";
        std::cout << "    idiv %rcx\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    
}

antlrcpp::Any CodeGenVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    
}
