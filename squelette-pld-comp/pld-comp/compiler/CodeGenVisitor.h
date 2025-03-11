#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"

class CodeGenVisitor : public ifccBaseVisitor
{
public:
        CodeGenVisitor(std::map<std::string, VariableInfo> &symbolTable) : symbolTable(symbolTable){};

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

        // 4.6
        virtual antlrcpp::Any visitStatement(ifccParser::StatementContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
        virtual antlrcpp::Any visitAssignment(ifccParser::AssignmentContext *ctx) override;
        virtual antlrcpp::Any visitExpression(ifccParser::ExpressionContext *ctx) override;
        
        // 4.7
        virtual antlrcpp::Any visitBinary_operation(ifccParser::Binary_operationContext *ctx) override;

private:
        std::map<std::string, VariableInfo> symbolTable;
};
