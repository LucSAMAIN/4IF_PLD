#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    std::cout << ".globl main\n";
    std::cout << "main: \n";

    // prologue
    std::cout << "#prologue \n";
    std::cout << "    pushq %rbp\n";
    std::cout << "    movq %rsp, %rbp\n";

    for (auto statement : ctx->statement())
    {
        this->visit(statement);
    }
    this->visit(ctx->return_stmt());

    // epilogue
    std::cout << "#epilogue \n";
    std::cout << "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Stocker dans %rax la valeur temporaire
    visit(ctx->expression());
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitStatement(ifccParser::StatementContext *ctx)
{
    // case we have a declaration:
    if (ctx->declaration())
    {
        visit(ctx->declaration());
    }
    // case we have an assignement
    else if (ctx->assignment())
    {
        visit(ctx->assignment());
    }
    // a stand alone expression
    else if (ctx->expression())
    {
        visit(ctx->expression());
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx)
{
    std::string varName = ctx->IDENTIFIER()->getText();

    // Récupérer l'index de la variable dans la table des symboles
    int offset = symbolTable.at(varName).index;

    // Si la déclaration inclut une initialisation (int a = 5;)
    if (ctx->expression())
    {
        // Évaluer l'expression (la valeur sera dans %rax)
        visit(ctx->expression());

        // Stocker la valeur de %rax à l'emplacement mémoire de la variable
        std::cout << "    movl %eax, -" << offset << "(%rbp)\n";
    }
    // Sinon, c'est une simple déclaration (int a;)
    else
    {
        // Initialiser à 0
        std::cout << "    movl $0, -" << offset << "(%rbp)\n";
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssignment(ifccParser::AssignmentContext *ctx)
{
    // mettre dans %rax l'expression en question:
    visit(ctx->expression());

    std::string varName = ctx->IDENTIFIER()->getText();
    int offset = symbolTable.at(varName).index;
    std::cout << "    movl %eax, -" << offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpression(ifccParser::ExpressionContext *ctx)
// we need a place to store the destination after evaluating our expression
{
    // le raw string de l'expression
    std::string exprText = ctx->getText();

    // case constant
    if (ctx->CONST())
    {
        int val = std::stoi(ctx->CONST()->getText());
        // on met dans rax la valeur temp
        std::cout << "    movl $" << val << ", %eax \n";
    }
    // case identifier
    else if (ctx->IDENTIFIER())
    {
        std::string varName = ctx->IDENTIFIER()->getText();
        int offset = symbolTable.at(varName).index;
        // on met dans rax la valeur temp
        std::cout << "    movl -" << offset << "(%rbp), %eax \n";
    }
    // case binary op:
    else if (ctx->binary_operation())
    {
        // first we get the left expression and put in '%rdx'
        visit(ctx->expression(0));
        std::cout << "    movl %eax, %edx \n";
        // same for the right expression but in '%rax'
        visit(ctx->expression(1));
        
        // we can then do the operation:
        std::string op = ctx->binary_operation()->getText();
        if(op == "+")
        {
            // for an addition lets just use 'addq':
            std::cout << "    addl %edx, %eax\n"; // je met dans rax car je me dis que pour l'assignement c'est la ou on pioche ?
        }
    }
    // case unary_suffixe op:
    else if (ctx->unary_operation_suffixe())
    {
        std::cout << "unary_operation_suffixe not implemented yet\n";
    }
    // case unary_prefixe op:
    else if (ctx->unary_operation_prefixe())
    {
        std::cout << "unary_operation_prefixe not implemented yet\n";
    }
    // case '(' expression ')':
    else if (exprText.size() >= 2 && exprText[0] == '(' && exprText[exprText.size() - 1] == ')')
    {
        visit(ctx->expression(0));
    }

    return 0;
}
