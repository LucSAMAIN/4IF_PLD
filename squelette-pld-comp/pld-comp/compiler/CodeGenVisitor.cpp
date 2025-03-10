#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;

    // prologue
    std::cout << "#prologue \n";
    std::cout << "    pushq %rbp\n";
    std::cout << "    movq %rsp, %rbp\n";

    this->visit( ctx->return_stmt() );
    
    // epilogue
    std::cout << "#epilogue \n";
    std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "    movl $"<<retval<<", %eax\n" ;


    return 0;
}
