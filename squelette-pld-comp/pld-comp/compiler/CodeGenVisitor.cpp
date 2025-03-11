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
    if (ctx->declaration() != nullptr)
    {
        visit(ctx->declaration());
    }
    // case we have an assignement
    else if (ctx->assignment() != nullptr)
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
    if (ctx->expression() != nullptr)
    {
        // Évaluer l'expression (la valeur sera dans %rax)
        visit(ctx->expression());

        // Stocker la valeur de %rax à l'emplacement mémoire de la variable
        std::cout << "    movq %rax, -" << offset << "(%rbp)\n";
    }
    // Sinon, c'est une simple déclaration (int a;)
    else
    {
        // Initialiser à 0
        std::cout << "    movq $0, -" << offset << "(%rbp)\n";
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssignment(ifccParser::AssignmentContext *ctx)
{
    // mettre dans %rax l'expression en question:
    visit(ctx->expression());

    std::string varName = ctx->IDENTIFIER()->getText();
    int offset = symbolTable.at(varName).index;
    std::cout << "    movq %rax, -" << offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpression(ifccParser::ExpressionContext *ctx)
{
    // case constant
    if (ctx->CONST() != nullptr)
    {
        int val = std::stoi(ctx->CONST()->getText());
        // on met dans rax la valeur temp
        std::cout << "    movq $" << val << ", %rax\n";
    }
    // case identifier
    else if (ctx->IDENTIFIER() != nullptr)
    {
        std::string varName = ctx->IDENTIFIER()->getText();
        int offset = symbolTable.at(varName).index;
        // on met dans rax la valeur temp
        std::cout << "    movq -" << offset << "(%rbp), %rax\n";
    }

    return 0;
}


antlrcpp::Any CodeGenVisitor::visitBinary_operation(ifccParser::Binary_operationContext *ctx)
{
    

    return 0;
}