#include "SymbolTableVisitor.h"

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx) {
    // Visite tous les statements
    for (auto statement : ctx->statement()) {
        visit(statement);
    }
    
    // Visite le return statement
    visit(ctx->return_stmt());
    
    // Affiche la table des symboles
    printSymbolTable();
    
    return antlrcpp::Any();
}

antlrcpp::Any SymbolTableVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    std::string varName = ctx->IDENTIFIER()->getText();
    
    // Vérifie si la variable a déjà été déclarée
    if (symbolTable.find(varName) != symbolTable.end()) {
        std::string error = "Error: Variable '" + varName + "' is already declared";
        std::cerr << error << std::endl;
        errors.push_back(error);
    } else {
        // Ajoute la variable à la table des symboles
        VariableInfo info;
        info.index = nextIndex;
        info.isDeclared = true;
        info.isUsed = false;  // Elle sera utilisée si elle est initialisée
        symbolTable[varName] = info;
        
        // Incrémente l'index pour la prochaine variable (par 4 octets)
        nextIndex += 4;
        
        // Si c'est une déclaration avec initialisation
        if (ctx->expression() != nullptr) {
            // Visite l'expression pour vérifier qu'elle est valide
            // (par exemple, vérifier que les variables utilisées sont déclarées)
            visit(ctx->expression());
            
            // Marque la variable comme utilisée car elle est initialisée
            symbolTable[varName].isUsed = true;
        }
    }
    
    return antlrcpp::Any();
}

antlrcpp::Any SymbolTableVisitor::visitAssignment(ifccParser::AssignmentContext *ctx) {
    std::string varName = ctx->IDENTIFIER()->getText();
    
    // Vérifie si la variable a été déclarée
    if (symbolTable.find(varName) == symbolTable.end()) {
        std::string error = "Error: Variable '" + varName + "' used in assignment without declaration";
        std::cerr << error << std::endl;
        errors.push_back(error);
    } else {
        // Marque la variable comme utilisée (en écriture)
        symbolTable[varName].isUsed = true;
    }
    
    // Visite l'expression à droite
    visit(ctx->expression());
    
    return antlrcpp::Any();
}

antlrcpp::Any SymbolTableVisitor::visitExpression(ifccParser::ExpressionContext *ctx) {
    // Si l'expression est un identifiant, vérifie si la variable a été déclarée
    if (ctx->IDENTIFIER() != nullptr) {
        std::string varName = ctx->IDENTIFIER()->getText();
        
        if (symbolTable.find(varName) == symbolTable.end()) {
            std::string error = "Error: Variable '" + varName + "' used without declaration";
            std::cerr << error << std::endl;
            errors.push_back(error);
        } else {
            // Marque la variable comme utilisée (en lecture)
            symbolTable[varName].isUsed = true;
        }
    }
    
    return antlrcpp::Any();
}

antlrcpp::Any SymbolTableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    // Visite l'expression du return
    visit(ctx->expression());
    
    return antlrcpp::Any();
}

bool SymbolTableVisitor::checkSymbolTable() {
    bool valid = true;
    
    // Vérifie si toutes les variables déclarées sont utilisées
    for (const auto& entry : symbolTable) {
        if (!entry.second.isUsed) {
            std::string warning = "Warning: Variable '" + entry.first + "' is declared but never used";
            std::cerr << warning << std::endl;
            // Ce n'est qu'un avertissement, donc on ne modifie pas 'valid'
        }
    }
    
    // S'il y a des erreurs, la table n'est pas valide
    if (!errors.empty()) {
        valid = false;
    }
    
    return valid;
}

void SymbolTableVisitor::printSymbolTable() const {
    std::cout << "=== Symbol Table ===\n";
    std::cout << "Variable\tIndex\tDeclared\tUsed\n";
    
    for (const auto& entry : symbolTable) {
        std::cout << entry.first << "\t\t"
                  << entry.second.index << "\t"
                  << (entry.second.isDeclared ? "Yes" : "No") << "\t\t"
                  << (entry.second.isUsed ? "Yes" : "No") << "\n";
    }
    
    std::cout << "===================\n";
}