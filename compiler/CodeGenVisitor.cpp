#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    std::cout << ".text\n";
    std::cout<< ".globl main\n" ;
    std::cout<< "main: \n" ;

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
        std::cout << "    movq %rax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp) # visit decl\n";
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    visit(ctx->expr());

    std::cout << "    movq %rax, " << symbolTable[ctx->ID()->getText()].offset << "(%rbp) # visit assign\n";

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
    // cehck type before printing
    std::cout << "    movq $" << ctx->CONST()->getText() << ", %rax # visit int\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    std::cout << "    movq " << symbolTable[ctx->ID()->getText()].offset << "(%rbp), %rax # visit idUse\n";
    return 0;
}
