#ifndef CONTINUEBREAKCHECKVISITOR_H
#define CONTINUEBREAKCHECKVISITOR_H

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class ContinueBreakCheckVisitor : public ifccBaseVisitor
{
public:
    ContinueBreakCheckVisitor(); 
    virtual ~ContinueBreakCheckVisitor();

    int getNumberError() { return error; }
    
    virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext *ctx) override;
    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) override;
    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    
private:
    int error;
    int while_count;
};

#endif