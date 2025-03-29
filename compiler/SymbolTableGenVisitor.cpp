#include <sstream>

#include "SymbolTableGenVisitor.h"

std::string typeToString(Type t)
{
    std::string typeToString[] = {
        "void",
        "int64_t",
        "int32_t",
        "int16_t",
        "int8_t",
        "float64_t"
    };
    return typeToString[(int)t];
}

Type stringToType(std::string s)
{   
    if (s == "int")
        return Type::INT32_T;
    else if (s == "void")
        return Type::VOID;
    else if (s == "char")
        return Type::INT8_T;
    else if (s == "double")
        return Type::FLOAT64_T;
    return Type::INT32_T;
}

int typeSize(Type t)
{
    int typeSize[] = {
        0,
        8,
        4,
        2,
        1,
        8
    };
    return typeSize[(int)t];
}

SymbolTableGenVisitor::SymbolTableGenVisitor() : varTable(), funcTable(), scope(), error_count(0) {
    funcTable["putchar"] = {.type = Type::VOID, .offset = 0, .args = {}, .used = false};
    varTable["putchar_0"] = {.type = Type::INT32_T, .name = "putchar_0", .offset = 0, .declared = true, .used = false};
    funcTable["putchar"].args.push_back(&varTable["putchar_0"]);

    funcTable["getchar"] = {.type = Type::VOID, .offset = 0, .args = {}, .used = false};
    varTable["getchar_0"] = {.type = Type::INT32_T, .name = "getchar_0", .offset = 0, .declared = true, .used = false};
    funcTable["getchar"].args.push_back(&varTable["getchar_0"]);
}

antlrcpp::Any SymbolTableGenVisitor::visitFuncDecl(ifccParser::FuncDeclContext *ctx) {
    scope = ctx->funcName->getText();
    funcTable[ctx->funcName->getText()] = {.type = stringToType(ctx->funcType()->getText()),
                                            .offset = 0, 
                                            .args = {}, 
                                            .used = false};

    for (int i = 1; i < ctx->ID().size(); i++) { // le nom de la fonction est quand même dans la liste  des ID
        // même si on lui a donné un nom différent dans la grammaire, on commence à 1
        std::string varName = scope + "_" + ctx->ID(i)->getText();
        funcTable[ctx->funcName->getText()].offset -= typeSize(stringToType(ctx->type(i-1)->getText()));
        varTable[varName] = {.type = stringToType(ctx->type(i-1)->getText()),
                            .name = varName, 
                            .offset = funcTable[ctx->funcName->getText()].offset, 
                            .declared = true, 
                            .used = false};
        funcTable[ctx->funcName->getText()].args.push_back(&varTable[varName]);
    }
    visit(ctx->block());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx) {
    for (int i = 0; i < ctx->decl_element().size(); i++) {
        if (varTable.find(scope + '_' + ctx->decl_element(i)->ID()->getText()) != varTable.end()) {
            std::cerr << "error: variable name already declared " << ctx->decl_element(i)->ID()->getText() << "\n";
            error_count++;
        }
        std::string funcName;
        std::istringstream ss_scope(scope);
        std::getline(ss_scope, funcName, '_');
        // std::cout << "# funcName " << funcName << " scope " << scope << "\n";

        std::string varName = scope + "_" + ctx->decl_element(i)->ID()->getText();

        funcTable[funcName].offset -= typeSize(stringToType(ctx->type()->getText()));
        varTable[varName] = {.type = stringToType(ctx->type()->getText()),
            .name = varName, 
            .offset = funcTable[funcName].offset, 
            .declared = true, 
            .used = false};
        
        if (ctx->decl_element(i)->expr() != nullptr) {
            visit(ctx->decl_element(i)->expr());
        }
    }
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && varTable.find(tried_scope + '_' + ctx->ID()->getText()) == varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        error_count++;
    }
    visit(ctx->expr());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx) {
    /*
        On a (x = a), on veut donc update la symbol table de a pour dire qu'elle est utilisé
        dans notre programme.
        On doit donc trouver où elle se trouve...
        Autrement dit:
        int main()
        {
            int x = 1;

            {
                int a = x;
            }
            return 0;
        }
        Au moment ou on fait a = x on veut indiquer que x est bien utilisé, mais on ne sait pas ou la trouver... 
    */
    std::string tried_scope = scope;
    // On parcourt notre scope pour savoir dans quel contexte se trouve l'IDUse!
    while (tried_scope != "" && varTable.find(tried_scope + '_' + ctx->ID()->getText()) == varTable.end()) {
        // on update le scope:
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        error_count++;
    }
    varTable[tried_scope + '_' + ctx->ID()->getText()].used = true;
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitBlock(ifccParser::BlockContext *ctx) {
    for (int i = 0; i < ctx->stmt().size(); i++) {
        if (ctx->stmt(i)->block_stmt() != nullptr) {
            scope += "_" + std::to_string(i);
            visit(ctx->stmt(i));
            while (scope.back() != '_') { // on sait pas a l'avance si i a plusieurs digits ou non (i = 15, 17698 ou 3)
                scope.pop_back();
            }
            scope.pop_back(); // remove '_'
        }
        else {
            visit(ctx->stmt(i));
        }
    }
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    std::string tried_scope = scope;
    while (tried_scope != "" && varTable.find(tried_scope + '_' + ctx->ID()->getText()) == varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    if (tried_scope.size() == 0) {
        std::cerr << "error: variable not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        error_count++;
    }
    varTable[tried_scope + '_' + ctx->ID()->getText()].used = true;
    visit(ctx->expr());
    return 0;
}

antlrcpp::Any SymbolTableGenVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    if (funcTable.find(ctx->ID()->getText()) == funcTable.end()) {
        std::cerr << "error: function not declared " << ctx->ID()->getText() << " in scope " << scope << "\n";
        error_count++;
    }
    if (funcTable[ctx->ID()->getText()].args.size() != ctx->expr().size()) {
        std::cerr << "error: function " << ctx->ID()->getText() << " expects " << funcTable[ctx->ID()->getText()].args.size() << " arguments, got " << ctx->expr().size() << "\n";
        error_count++;
    }
    funcTable[ctx->ID()->getText()].used = true;
    for (int i = 0; i < ctx->expr().size(); i++) {
        visit(ctx->expr(i));
    }
    return 0;
}