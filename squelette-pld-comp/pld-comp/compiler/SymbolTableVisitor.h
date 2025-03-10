#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include <map>
#include <string>
#include <iostream>
#include <vector>


// -----------------------------------------
// WARNING: uniquement des entier de 32 bits
// -----------------------------------------
class SymbolTableVisitor : public ifccBaseVisitor {
public:
    // Structure pour stocker les informations d'une variable
    struct VariableInfo {
        int index;
        bool isDeclared;
        bool isUsed;
    };

    SymbolTableVisitor() : nextIndex(0) {}
    
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(ifccParser::AssignmentContext *ctx) override;
    virtual antlrcpp::Any visitExpression(ifccParser::ExpressionContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    
    // Vérifie si la table des symboles est valide
    bool checkSymbolTable();
    
    // Getter pour la table des symboles (utile pour CodeGenVisitor)
    std::map<std::string, VariableInfo> getSymbolTable() const {
        return symbolTable;
    }
    
    // Affiche la table des symboles pour debug
    void printSymbolTable() const;

private:
    std::map<std::string, VariableInfo> symbolTable;
    int nextIndex;  // Toujours un multiple de 4
    std::vector<std::string> errors;  // Pour collecter les erreurs
};