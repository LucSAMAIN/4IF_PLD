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
    FLOAT64_T
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
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual antlrcpp::Any visitIdUse(ifccParser::IdUseContext *ctx) override;
    virtual antlrcpp::Any visitFuncCall(ifccParser::FuncCallContext *ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitAssignExpr(ifccParser::AssignExprContext *ctx) override;
    std::map<std::string, VarInfos> varTable;
    std::map<std::string, FuncInfos> funcTable;
    std::string scope;
    int error_count;

    //fonction pour print la symbol table
    void printSymbolTable();
};

#endif // SYMBOL_TABLE_GEN_VISITOR_H