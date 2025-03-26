#ifndef SYMBOL_TABLE_GEN_VISITOR_H
#define SYMBOL_TABLE_GEN_VISITOR_H

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

extern std::string typeToString[];

Type fromStringToType(std::string s);
std::string fromTypeToString(Type t);

typedef struct VarInfos
{
    Type type;
    int offset;
    int index_arg;
    bool declared;
    bool used;
} VarInfos;


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
    std::map<std::string, VarInfos> symbolTable;
    std::map<std::string, int> offsetTable; // pour les fonctions connaitre le début d'offset.
    std::string scope;
    int error_count;

    //fonction pour print la symbol table
    void printSymbolTable();
};

#endif // SYMBOL_TABLE_GEN_VISITOR_H