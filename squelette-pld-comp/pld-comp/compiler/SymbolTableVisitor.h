#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccParser.h"
#include <map>
#include <string>
#include <iostream>
#include <vector>

// Structure pour stocker les informations liés à chaque variable.
struct VariableInfo {
    int index;
    bool isDeclared;
    bool isUsed;
};

// -----------------------------------------
// WARNING: uniquement des entier de 32 bits
// -----------------------------------------
class SymbolTableVisitor : public ifccBaseVisitor {
public:


    SymbolTableVisitor() : nextIndex(4) {} // car la première on la réserve pour la valeur initiale (avant qu'on lance notre programme il y avait qqchose ca évite d'override)...
    
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

    // getters:
    std::map<std::string, VariableInfo>& getSymbolTable(){return this->symbolTable;}
    std::vector<std::string>& getErrors(){return this->errors;}

private:
    std::map<std::string, VariableInfo> symbolTable;
    int nextIndex = 4;  // Toujours un multiple de 4
    std::vector<std::string> errors;  // Pour collecter les erreurs
};