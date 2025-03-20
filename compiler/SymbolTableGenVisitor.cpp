#include <sstream>

#include "SymbolTableGenVisitor.h"

Type fromStringToType(std::string s)
{   
    if (s == "char")
        return Type::CHAR;
    else if (s == "void")
        return Type::VOID;
    else
        return Type::INT;
}

SymbolTableGenVisitor::SymbolTableGenVisitor() : symbolTable(), offsetTable(), scope() {
    symbolTable["putchar"] = {Type::VOID, 0, 1, true, true};
    symbolTable["getchar"] = {Type::VOID, 0, 1, true, true};
}

antlrcpp::Any SymbolTableGenVisitor::visitFuncDecl(ifccParser::FuncDeclContext *ctx) {
    scope = ctx->funcName->getText();
    symbolTable[ctx->funcName->getText()] = {fromStringToType(ctx->funcType()->getText()), 0, (int)ctx->type().size(), true, false};
    for (int i = 1; i < ctx->ID().size(); i++) { // le nom de la fonction est quand même dans la liste 
        // même si on lui a donné un nom différent
        if (ctx->type(i-1)->getText() == "int") { // le type de la fonction a un non terminal différent donc pas 
            // dans la liste des types donc on décale de 1
            symbolTable[scope + "_" + ctx->ID(i)->getText()] = {Type::INT, 0, i-1, true, false};
        }
    }
    visit(ctx->block());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) {
    for (int i = 0; i < ctx->decl_element().size(); i++) {
        if (symbolTable.find(scope + '_' + ctx->decl_element(i)->ID()->getText()) != symbolTable.end()) {
            std::cerr << "error: variable name already declared " << ctx->decl_element(i)->ID()->getText() << "\n";
            return 0;
        }
        std::string funcName;
        std::istringstream ss_scope(scope);
        std::getline(ss_scope, funcName, '_');
        // std::cout << "# funcName " << funcName << " scope " << scope << "\n";
        if (ctx->type()->getText() == "int") {
            offsetTable[funcName] -= 4;
            symbolTable[scope + '_' + ctx->decl_element(i)->ID()->getText()] = {Type::INT, offsetTable[funcName], -1, true, false};
        }
        
        if (ctx->decl_element(i)->expr() != nullptr) {
            visit(ctx->decl_element(i)->expr());
        }
    }
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        return 0;
    }
    visit(ctx->expr());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx) {
    /*
        On a (x = a), on veut donc update la symbol table de a pour dire qu'elle est utilisé
        dans notre programme.
        On doit donc trouver où elle se trouve...
        Autrement dit:
        int main()
        {
            int x = 1;

            {
                int a = x;
            }
            return 0;
        }
        Au moment ou on fait a = x on veut indiquer que x est bien utilisé, mais on ne sait pas ou la trouver... 
    */
    std::string tried_scope = scope;
    // On parcourt notre scope pour savoir dans quel contexte se trouve l'IDUse!
    while (tried_scope != "" && symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == symbolTable.end()) {
        // on update le scope:
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        return 0;
    }
    symbolTable[tried_scope + '_' + ctx->ID()->getText()].used = true;
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitBlock(ifccParser::BlockContext *ctx) {
    for (int i = 0; i < ctx->stmt().size(); i++) {
        if (ctx->stmt(i)->block_stmt() != nullptr) {
            scope += "_" + std::to_string(i);
            visit(ctx->stmt(i));
            while (scope.back() != '_') { // on sait pas a l'avance si i a plusieurs digits ou non (i = 15, 17698 ou 3)
                scope.pop_back();
            }
            scope.pop_back(); // remove '_'
        }
        else {
            visit(ctx->stmt(i));
        }
    }
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        return 0;
    }
    symbolTable[tried_scope + '_' + ctx->ID()->getText()].used = true;
    visit(ctx->expr());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    if (symbolTable.find(ctx->ID()->getText()) == symbolTable.end()) {
        std::cerr << "error: function not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        return 0;
    }
    if (symbolTable[ctx->ID()->getText()].index_arg != ctx->expr().size()) {
        std::cerr << "error: function " << ctx->ID()->getText() << " expects " << symbolTable[ctx->ID()->getText()].index_arg << " arguments, got " << ctx->expr().size() << "\n";
        return 0;
    }
    symbolTable[ctx->ID()->getText()].used = true;
    for (int i = 0; i < ctx->expr().size(); i++) {
        visit(ctx->expr(i));
    }
    return 0;
}