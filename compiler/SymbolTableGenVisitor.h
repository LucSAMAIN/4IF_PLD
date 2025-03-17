#pragma once

#include <string>
#include <map>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

enum class Type {
    VOID,
    INT,
    CHAR,
    INT64_T,
    INT32_T,
    INT16_T,
    INT8_T
}; 

Type fromStringToType(std::string s)
{   
    if (s == "CHAR")
        return Type::CHAR;
    else
        return Type::INT;
}

typedef struct VarInfos
{
    Type type;
    int offset;
    bool declared;
    bool used;
} VarInfos;

class SymbolTableGenVisitor : public ifccBaseVisitor
{
public:
    SymbolTableGenVisitor() : symbolTable(), offsetTable(), scope("main") {}
    virtual ~SymbolTableGenVisitor() {}

    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;

    std::map<std::string, VarInfos> symbolTable;
    std::map<std::string, int> offsetTable;
    std::string scope;
};
