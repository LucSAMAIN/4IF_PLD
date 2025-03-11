#include "SymbolTableGenVisitor.h"

antlrcpp::Any SymbolTableGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) {
    if (symbolTable.find(ctx->ID()->getText()) != symbolTable.end()) {
        std::cerr << "error: variable name already declared " << ctx->ID()->getText() << "\n";
        return 0;
    }
    if (ctx->type()->getText() == "int") {
        offset -= 4;
    }
    symbolTable[ctx->ID()->getText()] = {ctx->type()->getText(), offset, true, false};
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx) {
    if (symbolTable.find(ctx->ID()->getText()) == symbolTable.end()) {
        std::cerr << "error: undeclared variable " << ctx->ID()->getText() << "\n";
        return 0;
    }
    symbolTable[ctx->ID()->getText()].used = true;
    return 0;
}