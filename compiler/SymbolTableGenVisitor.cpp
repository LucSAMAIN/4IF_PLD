#include "SymbolTableGenVisitor.h"

antlrcpp::Any SymbolTableGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) {
    if (symbolTable.find(scope + '_' + ctx->ID()->getText()) != symbolTable.end()) {
        std::cerr << "error: variable name already declared " << ctx->ID()->getText() << "\n";
        return 0;
    }
    if (ctx->type()->getText() == "int") {
        offset -= 4;
    }
    symbolTable[scope + '_' + ctx->ID()->getText()] = {ctx->type()->getText(), offset, true, false};
    if (ctx->expr() != nullptr) {
        visit(ctx->expr());
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