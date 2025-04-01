#ifndef SYMBOL_TABLE_GEN_VISITOR_H
#define SYMBOL_TABLE_GEN_VISITOR_H

#include <string>
#include <map>
#include <vector>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

enum class Type {
    VOID,
    INT64_T,
    INT32_T,
    INT16_T,
    INT8_T,
    FLOAT64_T,
    PTR_INT32_T,
    PTR_FLOAT64_T,
};

std::string typeToString(Type t);
Type stringToType(std::string s);
int typeSize(Type t);

typedef struct VarInfos
{
    Type type;
    std::string name;
    int offset;
    bool declared;
    bool used;
} VarInfos;

typedef struct FuncInfos
{
    Type type;
    int offset;
    std::vector<VarInfos*> args;
    bool used;
} FuncInfos;

class SymbolTableGenVisitor : public ifccBaseVisitor
{
public:
    SymbolTableGenVisitor();
    virtual ~SymbolTableGenVisitor() {}

    int getErrorCount() { return error_count; }

    virtual antlrcpp::Any visitFuncDecl(ifccParser::FuncDeclContext *ctx) override;
    virtual antlrcpp::Any visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitLIdUse(ifccParser::LIdUseContext *ctx) override;
    virtual antlrcpp::Any visitFuncCall(ifccParser::FuncCallContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitAssignExpr(ifccParser::AssignExprContext *ctx) override;
    virtual antlrcpp::Any visitSuffixDecrement(ifccParser::SuffixDecrementContext *ctx) override;
    virtual antlrcpp::Any visitSuffixIncrement(ifccParser::SuffixIncrementContext *ctx) override;
    virtual antlrcpp::Any visitPrefixDecrement(ifccParser::PrefixDecrementContext *ctx) override;
    virtual antlrcpp::Any visitPrefixIncrement(ifccParser::PrefixIncrementContext *ctx) override;
    std::map<std::string, VarInfos> varTable;
    std::map<std::string, FuncInfos> funcTable;
    std::string scope;
    int error_count;
};

#endif // SYMBOL_TABLE_GEN_VISITOR_H