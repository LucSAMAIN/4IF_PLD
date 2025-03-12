#pragma once

#include <string>
#include <map>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

typedef struct VarInfos
{
    std::string type;
    int offset;
    bool declared;
    bool used;
    std::string scope;
} VarInfos;

class SymbolTableGenVisitor : public ifccBaseVisitor
{
public:
    SymbolTableGenVisitor() : symbolTable(), scope("main"), offset(0) {}
    virtual ~SymbolTableGenVisitor() {}

    std::map<std::string, VarInfos>& getSymbolTable() { return symbolTable; }

    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
private:
    std::map<std::string, VarInfos> symbolTable;
    std::string scope;
    int offset;
};
