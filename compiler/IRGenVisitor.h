#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "IR.h"

class IRGenVisitor : public ifccBaseVisitor
{
public:
    IRGenVisitor(SymbolTableGenVisitor& symbolTableGenVisitor); 
    virtual ~IRGenVisitor();
    
    CFG* getCFG() { return cfg; }
    
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual antlrcpp::Any visitConst(ifccParser::ConstContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitAssignExpr(ifccParser::AssignExprContext *ctx) override;
    // virtual antlrcpp::Any visitNotExpr(ifccParser::NotExprContext *ctx) override;
    // virtual antlrcpp::Any visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) override;
    virtual antlrcpp::Any visitMulDivExpr(ifccParser::MulDivExprContext *ctx) override;
    virtual antlrcpp::Any visitAddSubExpr(ifccParser::AddSubExprContext *ctx) override;
    // virtual antlrcpp::Any visitCompExpr(ifccParser::CompExprContext *ctx) override;
    // virtual antlrcpp::Any visitEqExpr(ifccParser::EqExprContext *ctx) override;
    // virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
    // virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
    // virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
    
private:
    SymbolTableGenVisitor symbolTableGenVisitor;
    CFG* cfg;              // Le Control Flow Graph
    BasicBlock* currentBB; // Le bloc de base courant
    std::string scope;
};
