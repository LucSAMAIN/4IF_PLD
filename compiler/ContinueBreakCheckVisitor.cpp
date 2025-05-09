#include <sstream>

#include "ContinueBreakCheckVisitor.h"

ContinueBreakCheckVisitor::ContinueBreakCheckVisitor(antlr4::ANTLRInputStream& input) : ErrorVisitor(input), while_count(0) {}
ContinueBreakCheckVisitor::~ContinueBreakCheckVisitor() {}

antlrcpp::Any ContinueBreakCheckVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    // Visiter tous les statements dans le bloc
    for (int i = 0; i < ctx->stmt().size(); i++) {
        visit(ctx->stmt(i));
        // si on a un return on sort du block
        if (ctx->stmt(i)->while_stmt() != nullptr) {
            while_count--;
        }
    }

    return 0;
}

antlrcpp::Any ContinueBreakCheckVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx) {
    while_count++;
    
    return 0;
}

antlrcpp::Any ContinueBreakCheckVisitor::visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) {
    if (while_count == 0) {
        reportError("error: continue statement outside of while loop", ctx);
        error++;
    }
    
    return 0;
}

antlrcpp::Any ContinueBreakCheckVisitor::visitBreak_stmt(ifccParser::Break_stmtContext *ctx) {
    if (while_count == 0) {
        reportError("error: break statement outside of while loop", ctx);
        error++;
    }
    
    return 0;
}
