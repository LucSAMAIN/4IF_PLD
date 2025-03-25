#include "TypeCheckVisitor.h"

#include <iostream>
#include <string>


TypeCheckVisitor::TypeCheckVisitor(SymbolTableGenVisitor& symbolTableGenVisitor) 
    : stv(symbolTableGenVisitor), scope(), type_error(0) {}

TypeCheckVisitor::~TypeCheckVisitor() {
}

antlrcpp::Any TypeCheckVisitor::visitFuncDecl(ifccParser::FuncDeclContext *ctx) {
    scope = ctx->funcName->getText();
    
    // Visite de la fonction
    visit(ctx->block());
    
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    // Visiter tous les statements dans le bloc
    for (int i = 0; i < ctx->stmt().size(); i++) {
        // Cas pour un block
        if (ctx->stmt(i)->block_stmt() != nullptr) {
            scope += "_" + std::to_string(i);
            visit(ctx->stmt(i)); // Appel récursif on va visiter ce block
            while (scope.back() != '_') {
                scope.pop_back();
            }
            scope.pop_back();
        }
        // Cas si juste un statement
        else {
            visit(ctx->stmt(i));
        }
    }
    return 0;
}


antlrcpp::Any TypeCheckVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{ 
    // On a rien besoin de faire si c'est uniquement une déclaration...
    // Car c'est déjà géré par la symbol table.
    for (int i = 0; i < ctx->decl_element().size(); i++) {
        if (ctx->decl_element(i)->expr()) // si déclaration + assignement direct
        {
            std::string nomVar = scope + "_" + ctx->decl_element(i)->ID()->getText();

            Type type_expr = visit(ctx->decl_element(i)->expr());
            if (type_expr != stv.symbolTable[nomVar].type) {
                std::cerr << "error: type mismatch in declaration of variable " << nomVar << ", expected " << typeToString[(int)stv.symbolTable[nomVar].type] << " found " << typeToString[(int)type_expr] << "\n";
                type_error++;
            }
        }
    }
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    Type type_expr = visit(ctx->expr());
    if (type_expr != stv.symbolTable[nomVar].type) {
        std::cerr << "error: type mismatch in assignment of variable " << nomVar << ", expected " << typeToString[(int)stv.symbolTable[nomVar].type] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }
    
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    Type type_expr = visit(ctx->expr());
    std::istringstream ss(scope);
    std::string func_name;
    std::getline(ss, func_name, '_');

    if (type_expr != stv.symbolTable[func_name].type) {
        std::cerr << "error: type mismatch in return statement of function " << func_name << ", expected " << typeToString[(int)stv.symbolTable[func_name].type] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }
    
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    Type type_expr = visit(ctx->expr());
    if (type_expr != Type::INT) {
        std::cerr << "error: type mismatch in if statement, expected int, found " << (int)type_expr << ", expected " << typeToString[(int)Type::INT] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }

    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx) {
    Type type_expr = visit(ctx->expr());
    if (type_expr != Type::INT) {
        std::cerr << "error: type mismatch in if statement, expected int, found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }

    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitIntExpr(ifccParser::IntExprContext *ctx)
{
    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitCharExpr(ifccParser::CharExprContext *ctx)
{
    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();

    return stv.symbolTable[nomVar].type;
}

antlrcpp::Any TypeCheckVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    Type type_expr = visit(ctx->expr());
    if (type_expr != stv.symbolTable[nomVar].type) {
        std::cerr << "error: type mismatch in assignment of variable " << nomVar << ", expected " << typeToString[(int)stv.symbolTable[nomVar].type] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }
    
    return type_expr;
}


antlrcpp::Any TypeCheckVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    Type type_expr = visit(ctx->primary());
    if (type_expr != Type::INT) {
        std::cerr << "error: type mismatch in not expression, expected int, found " << (int)type_expr << ", expected " << typeToString[(int)Type::INT] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }
    
    return type_expr;
}

antlrcpp::Any TypeCheckVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    Type type_expr = visit(ctx->primary());
    if (type_expr != Type::INT) {
        std::cerr << "error: type mismatch in unary minus expression, expected int, found " << (int)type_expr << ", expected " << typeToString[(int)Type::INT] << " found " << typeToString[(int)type_expr] << "\n";
        type_error++;
    }
    
    return type_expr;
}

antlrcpp::Any TypeCheckVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT || type_right != Type::INT) {
        std::cerr << "error: type mismatch in multiplication/division expression, expected int, found " << typeToString[(int)type_left] << " and " << typeToString[(int)type_right] << "\n";
        type_error++;
    }

    return Type::INT;
}

antlrcpp::Any TypeCheckVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    return visit(ctx->expr());
}

antlrcpp::Any TypeCheckVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    std::string func_name = ctx->ID()->getText();

    for (int i = 0; i < std::min<int>(ctx->expr().size(), stv.symbolTable[func_name].index_arg); i++) {
        Type type_expr = visit(ctx->expr(i));
        if (type_expr != stv.symbolTable[func_name+"_"+std::to_string(i)].type) {
            std::cerr << "error: type mismatch in argument of function call of " << func_name << ", expected " << typeToString[(int)stv.symbolTable[func_name+"_"+std::to_string(i)].type] << " found " << typeToString[(int)type_expr] << "\n";
            type_error++;
        }
    }

    return stv.symbolTable[func_name].type;
}