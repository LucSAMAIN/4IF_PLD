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
        if (ctx->decl_element(i)->decl_var()->expr()) // si déclaration + assignement direct
        {
            std::string nomVar = scope + "_" + ctx->decl_element(i)->decl_var()->ID()->getText();

            Type type_expr = visit(ctx->decl_element(i)->decl_var()->expr());
            if (type_expr == Type::VOID) {
                std::cerr << "error: type mismatch in assignment of variable " << nomVar << ", expected " << typeToString(stv.varTable[nomVar].type) << " found " << typeToString(type_expr) << "\n";
                type_error++;
            }
            if (type_expr != stv.varTable[nomVar].type) {
                std::cerr << "warning: type mismatch in declaration of variable " << nomVar << ", expected " << typeToString(stv.varTable[nomVar].type) << " found " << typeToString(type_expr) << "\n";
            }
        }
    }
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    Type type_expr = visit(ctx->expr());
    std::istringstream ss(scope);
    std::string func_name;
    std::getline(ss, func_name, '_');

    if (type_expr == Type::VOID) {
        std::cerr << "error: type mismatch in return statement of function " << func_name << ", expected " << typeToString(stv.funcTable[func_name].type) << " found " << typeToString(type_expr) << "\n";
        type_error++;
    }
    if (type_expr != stv.funcTable[func_name].type) {
        std::cerr << "warning: type mismatch in return statement of function " << func_name << ", expected " << typeToString(stv.funcTable[func_name].type) << " found " << typeToString(type_expr) << "\n";
    }
    
    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    Type type_expr = visit(ctx->expr());
    if (type_expr != Type::INT32_T) {
        std::cerr << "warning: type mismatch in if statement, expected " << typeToString(Type::INT32_T) << " found " << typeToString(type_expr) << "\n";
    }

    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx) {
    Type type_expr = visit(ctx->expr());
    if (type_expr != Type::INT32_T) {
        std::cerr << "warning: type mismatch in if statement, expected " << typeToString(Type::INT32_T) << " found " << typeToString(type_expr) << "\n";
    }

    return 0;
}

antlrcpp::Any TypeCheckVisitor::visitIntExpr(ifccParser::IntExprContext *ctx)
{
    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitCharExpr(ifccParser::CharExprContext *ctx)
{
    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitDoubleExpr(ifccParser::DoubleExprContext *ctx)
{
    return Type::FLOAT64_T;
}

antlrcpp::Any TypeCheckVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.varTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();

    return stv.varTable[nomVar].type;
}

antlrcpp::Any TypeCheckVisitor::visitLIdUse(ifccParser::LIdUseContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.varTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();

    return stv.varTable[nomVar].type;
}

antlrcpp::Any TypeCheckVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    Type type_var = visit(ctx->lValue());
    Type type_expr = visit(ctx->value);
    if (type_expr == Type::VOID) {
        std::cerr << "error: type mismatch in assignment of variable expected " << typeToString(type_var) << " found " << typeToString(type_expr) << "\n";
        type_error++;
    }
    // if (type_expr != type_var) {
    //     std::cerr << "error: type mismatch in assignment of variable " << nomVar << ", expected " << typeToString(type_var) << " found " << typeToString(type_expr) << "\n";
    //     type_error++;
    // }
    
    return type_expr;
}
antlrcpp::Any TypeCheckVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    Type type_var = visit(ctx->lValue());
    Type type_expr = visit(ctx->value);
    if (type_expr == Type::VOID) {
        std::cerr << "error: type mismatch in assignment of variable expected " << typeToString(type_var) << " found " << typeToString(type_expr) << "\n";
        type_error++;
    }
    // if (type_expr != type_var) {
    //     std::cerr << "error: type mismatch in assignment of variable " << nomVar << ", expected " << typeToString(type_var) << " found " << typeToString(type_expr) << "\n";
    //     type_error++;
    // }
    
    return type_expr;
}

antlrcpp::Any TypeCheckVisitor::visitUnaryExpr(ifccParser::UnaryExprContext *ctx) {
    Type type_expr = visit(ctx->expr());
    if (type_expr == Type::VOID) {
        std::cerr << "error: type mismatch in unary minus expr found " << typeToString(type_expr) << "\n";
        type_error++;
    }
    
    return type_expr;
}

antlrcpp::Any TypeCheckVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left == Type::VOID || type_right == Type::VOID) {
        std::cerr << "error: type mismatch in multiplication/division expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }
    if (type_left != type_right) {
        std::cerr << "warning: type mismatch in comparison expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
    }
    if (ctx->mOp()->MOD() != nullptr) {
        if (type_left != Type::INT32_T || type_right != Type::INT32_T) {
            std::cerr << "error: type mismatch in modulo expression, expected int, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
            type_error++;
        }
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left == Type::VOID || type_right == Type::VOID) {
        std::cerr << "error: type mismatch in addition/sub expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }
    if (type_left != type_right) {
        std::cerr << "warning: type mismatch in add/sub expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left == Type::VOID || type_right == Type::VOID) {
        std::cerr << "error: type mismatch in comp expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }
    if (type_left != type_right) {
        std::cerr << "warning: type mismatch in comparison expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left == Type::VOID || type_right == Type::VOID) {
        std::cerr << "error: type mismatch in eq expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }
    if (type_left != type_right) {
        std::cerr << "warning: type mismatch in comparison expression, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT32_T || type_right != Type::INT32_T) {
        std::cerr << "error: type mismatch in bitwise and expression, expected int, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT32_T || type_right != Type::INT32_T) {
        std::cerr << "error: type mismatch in bitwise or expression, expected int, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    Type type_left = visit(ctx->left);
    Type type_right = visit(ctx->right);
    if (type_left != Type::INT32_T || type_right != Type::INT32_T) {
        std::cerr << "error: type mismatch in bitwise xor expression, expected int, found " << typeToString(type_left) << " and " << typeToString(type_right) << "\n";
        type_error++;
    }

    return Type::INT32_T;
}

antlrcpp::Any TypeCheckVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    return visit(ctx->expr());
}

antlrcpp::Any TypeCheckVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    std::string func_name = ctx->ID()->getText();

    for (int i = 0; i < std::min<int>(ctx->expr().size(), stv.funcTable[func_name].args.size()); i++) {
        Type type_expr = visit(ctx->expr(i));
        if (type_expr == Type::VOID) {
            std::cerr << "error: type mismatch in argument of function call of " << func_name << ", expected " << typeToString(stv.funcTable[func_name].args[i]->type) << " found " << typeToString(type_expr) << "\n";
            type_error++;
        }
    }

    return stv.funcTable[func_name].type;
}