#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "IRInstr.h"

class IRGenVisitor : public ifccBaseVisitor
{
public:
    IRGenVisitor(SymbolTableGenVisitor& symbolTableGenVisitor); 
    virtual ~IRGenVisitor();
    
    std::vector<CFG*>& getCFGs() { return cfgs; }
    
    virtual antlrcpp::Any visitFuncDecl(ifccParser::FuncDeclContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIf_stmt(ifccParser::If_stmtContext *ctx) override;
    virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext *ctx) override;
    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) override;
    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIntExpr(ifccParser::IntExprContext *ctx) override;
    virtual antlrcpp::Any visitCharExpr(ifccParser::CharExprContext *ctx) override;
    virtual antlrcpp::Any visitDoubleExpr(ifccParser::DoubleExprContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitLIdUse(ifccParser::LIdUseContext *ctx) override;
    virtual antlrcpp::Any visitAssignExpr(ifccParser::AssignExprContext *ctx) override;
    virtual antlrcpp::Any visitSuffixDecrement(ifccParser::SuffixDecrementContext *ctx) override;
    virtual antlrcpp::Any visitSuffixIncrement(ifccParser::SuffixIncrementContext *ctx) override;
    virtual antlrcpp::Any visitPrefixDecrement(ifccParser::PrefixDecrementContext *ctx) override;
    virtual antlrcpp::Any visitPrefixIncrement(ifccParser::PrefixIncrementContext *ctx) override;
    virtual antlrcpp::Any visitUnaryExpr(ifccParser::UnaryExprContext *ctx) override;
    virtual antlrcpp::Any visitMulDivExpr(ifccParser::MulDivExprContext *ctx) override;
    virtual antlrcpp::Any visitAddSubExpr(ifccParser::AddSubExprContext *ctx) override;
    virtual antlrcpp::Any visitCompExpr(ifccParser::CompExprContext *ctx) override;
    virtual antlrcpp::Any visitEqExpr(ifccParser::EqExprContext *ctx) override;
    virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
    virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
    virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
    virtual antlrcpp::Any visitLogAndExpr(ifccParser::LogAndExprContext *ctx) override;
    virtual antlrcpp::Any visitLogOrExpr(ifccParser::LogOrExprContext *ctx) override;
    virtual antlrcpp::Any visitParExpr(ifccParser::ParExprContext *ctx) override;
    virtual antlrcpp::Any visitFuncCall(ifccParser::FuncCallContext *ctx) override;
    
private:
    SymbolTableGenVisitor stv;
    std::vector<CFG*> cfgs;              // Le Control Flow Graph
    BasicBlock* currentBB; // Le bloc de base courant
    std::string scope;
};
