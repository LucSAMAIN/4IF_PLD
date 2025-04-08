#ifndef CONTINUEBREAKCHECKVISITOR_H
#define CONTINUEBREAKCHECKVISITOR_H

#include "ErrorVisitor.h"

class ContinueBreakCheckVisitor : public ErrorVisitor
{
public:
    ContinueBreakCheckVisitor(antlr4::ANTLRInputStream& input);
    virtual ~ContinueBreakCheckVisitor();
    
    virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext *ctx) override;
    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) override;
    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    
private:
    int while_count;
};

#endif