#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableGenVisitor.h"

class CodeGenVisitor : public ifccBaseVisitor
{
public:
    CodeGenVisitor(std::map<std::string, VarInfos>& p_symbolTable) : symbolTable(p_symbolTable) {}
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual antlrcpp::Any visitConst(ifccParser::ConstContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitNotExpr(ifccParser::NotExprContext *ctx) override;
    virtual antlrcpp::Any visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) override;
    virtual antlrcpp::Any visitMulDivExpr(ifccParser::MulDivExprContext *ctx) override;
    virtual antlrcpp::Any visitAddSubExpr(ifccParser::AddSubExprContext *ctx) override;
    virtual antlrcpp::Any visitCompExpr(ifccParser::CompExprContext *ctx) override;
    virtual antlrcpp::Any visitEqExpr(ifccParser::EqExprContext *ctx) override;
    virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
    virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
    virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
private:
    std::map<std::string, VarInfos> symbolTable;
};;
