#include "CodeGenVisitor.h"
#include <iostream>
#include <string>
#include <vector>

CodeGenVisitor::CodeGenVisitor(std::map<std::string, VarInfos>& p_symbolTable) 
    : symbolTable(p_symbolTable), cfg(nullptr), currentBB(nullptr) {}

CodeGenVisitor::~CodeGenVisitor() {
    // Le CFG doit être libéré par le main
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    // Initialisation du CFG et du bloc de base
    DefFonction* mainFunc = new DefFonction("main", Type::INT);
    cfg = new CFG(mainFunc);
    currentBB = new BasicBlock(cfg, "main");
    cfg->add_bb(currentBB);
    cfg->current_bb = currentBB;
    
    // Visite du bloc principal
    visit(ctx->block());

    // L'affichage de l'IR est maintenant géré uniquement dans main.cpp
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    // Visiter tous les statements dans le bloc
    for (auto stmt : ctx->stmt()) {
        visit(stmt);
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    // Convertir le type de la variable
    Type varType = Type::INT; // Par défaut, on considère que toutes les variables sont de type int

    // Ajouter la variable à la table des symboles du CFG
    cfg->add_to_symbol_table(ctx->ID()->getText(), varType);
    
    if (ctx->expr())
    {
        // Évaluation de l'expression
        visit(ctx->expr());
        
        // Créer une variable temporaire pour stocker le résultat
        std::string temp = cfg->create_new_tempvar(varType);
        
        // Copier le résultat de l'expression dans la variable temporaire
        std::vector<std::string> param_copy = {temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::copy, varType, param_copy);
        
        // Copier la variable temporaire dans la variable déclarée
        std::vector<std::string> params = {ctx->ID()->getText(), temp};
        currentBB->add_IRInstr(IRInstr::copy, varType, params);
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // Évaluation de l'expression
    visit(ctx->expr());
    
    // Obtenir le type de la variable
    Type varType = cfg->get_var_type(ctx->ID()->getText());
    
    // Créer une variable temporaire pour stocker le résultat
    std::string temp = cfg->create_new_tempvar(varType);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, varType, param_copy);
    
    // Copier la variable temporaire dans la variable assignée
    std::vector<std::string> params = {ctx->ID()->getText(), temp};
    currentBB->add_IRInstr(IRInstr::copy, varType, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression de retour
    visit(ctx->expr());
    
    // La valeur de retour est déjà dans %eax, donc on n'a pas besoin de la déplacer
    // Marquer la fin du bloc en définissant exit_true et exit_false à nullptr
    currentBB->exit_true = nullptr;
    currentBB->exit_false = nullptr;
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    // Créer une variable temporaire pour stocker la constante
    std::string temp = cfg->create_new_tempvar(Type::INT);
    
    // Charger la constante dans la variable temporaire
    std::vector<std::string> params = {temp, ctx->CONST()->getText()};
    currentBB->add_IRInstr(IRInstr::ldconst, Type::INT, params);
    
    // Copier la variable temporaire dans %eax
    std::vector<std::string> params_copy = {"%eax", temp};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params_copy);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // Obtenir le type de la variable
    Type varType = cfg->get_var_type(ctx->ID()->getText());
    
    // Copier la variable dans %eax
    std::vector<std::string> params = {"%eax", ctx->ID()->getText()};
    currentBB->add_IRInstr(IRInstr::copy, varType, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression primaire
    visit(ctx->primary());
    
    // Créer une variable temporaire pour stocker le résultat
    std::string temp = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy);
    
    // Générer une instruction de comparaison égale avec 0
    std::vector<std::string> params = {"%eax", temp, "0"};
    currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression primaire
    visit(ctx->primary());
    
    // Créer une variable temporaire pour stocker le résultat
    std::string temp = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy);
    
    // Inverser le signe de la variable temporaire et stocker dans %eax
    std::vector<std::string> params = {"%eax", "0", temp};
    currentBB->add_IRInstr(IRInstr::sub, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->mOp()->STAR()) {
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
    }
    else if (ctx->mOp()->SLASH() || ctx->mOp()->MOD()) {
        // Pour la division et le modulo, on utilise une approche différente
        // car ces opérations ne sont pas directement disponibles dans l'IR
        // En pratique, cela nécessiterait des appels à des fonctions externes
        // ou des séquences d'instructions spécifiques
        
        // Ici, on se contente de faire une multiplication pour la démonstration
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->aOp()->PLUS()) {
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::add, Type::INT, params);
    }
    else if (ctx->aOp()->MINUS()) {
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::sub, Type::INT, params);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Appliquer les comparaisons
    if (ctx->compOp()->LT()) {
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params);
    }
    else if (ctx->compOp()->LE()) {
        // Pour LE, on utilise une combinaison de LT et EQ
        std::vector<std::string> params_lt = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params_lt);
        
        // Sauvegarder le résultat
        std::string temp_lt = cfg->create_new_tempvar(Type::INT);
        std::vector<std::string> param_copy_lt = {temp_lt, "%eax"};
        currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_lt);
        
        std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
        
        // Combiner les résultats avec OR
        std::string temp_eq = cfg->create_new_tempvar(Type::INT);
        std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
        currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
        
        // temp_lt OR temp_eq -> %eax
        std::vector<std::string> params_or = {"%eax", temp_lt, temp_eq};
        currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or); // Utiliser add comme OR
    }
    else if (ctx->compOp()->GT() || ctx->compOp()->GE()) {
        // Pour GT, on inverse la comparaison LT (a > b équivalent à b < a)
        std::vector<std::string> params = {"%eax", temp_right, temp_left};
        currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params);
        
        if (ctx->compOp()->GE()) {
            // Pour GE, on utilise une combinaison de GT et EQ
            std::string temp_gt = cfg->create_new_tempvar(Type::INT);
            std::vector<std::string> param_copy_gt = {temp_gt, "%eax"};
            currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_gt);
            
            std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
            currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
            
            // Combiner les résultats avec OR
            std::string temp_eq = cfg->create_new_tempvar(Type::INT);
            std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
            currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
            
            // temp_gt OR temp_eq -> %eax
            std::vector<std::string> params_or = {"%eax", temp_gt, temp_eq};
            currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or); // Utiliser add comme OR
        }
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Appliquer les comparaisons
    if (ctx->eqOp()->EQ()) {
        std::vector<std::string> params = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params);
    }
    else if (ctx->eqOp()->NEQ()) {
        // Pour NEQ, on calcule l'égalité puis on inverse le résultat
        std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
        currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
        
        // Sauvegarder le résultat dans une variable temporaire
        std::string temp_eq = cfg->create_new_tempvar(Type::INT);
        std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
        currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
        
        // Inverser le résultat (1 - temp_eq)
        std::vector<std::string> params_inv = {"%eax", "1", temp_eq};
        currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_inv);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Multiplication pour simuler l'opération AND
    std::vector<std::string> params = {"%eax", temp_left, temp_right};
    currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    // XOR n'est pas directement disponible dans l'IR, nous allons donc le simuler avec d'autres opérations
    
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Pour simuler XOR, on peut utiliser: (a | b) - (a & b)
    // D'abord calculer (a & b)
    std::vector<std::string> params_and = {"%eax", temp_left, temp_right};
    currentBB->add_IRInstr(IRInstr::mul, Type::INT, params_and); // Utiliser mul comme AND
    
    // Sauvegarder dans une variable temporaire
    std::string temp_and = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> param_copy_and = {temp_and, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_and);
    
    // Calculer (a + b) pour simuler (a | b)
    std::vector<std::string> params_or = {"%eax", temp_left, temp_right};
    currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or);
    
    // Soustraire (a & b) pour obtenir XOR
    std::vector<std::string> params_sub = {"%eax", "%eax", temp_and};
    currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_sub);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_left = {temp_left, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande droite
    std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    std::vector<std::string> param_copy_right = {temp_right, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
    // Pour simuler OR, on peut utiliser: a + b - (a & b)
    // D'abord calculer (a & b)
    std::string temp_sum = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params_add = {temp_sum, temp_left, temp_right};
    currentBB->add_IRInstr(IRInstr::add, Type::INT, params_add);
    
    std::vector<std::string> params_and = {"%eax", temp_left, temp_right};
    currentBB->add_IRInstr(IRInstr::mul, Type::INT, params_and); // Utiliser mul comme AND
    
    // Soustraire (a & b) de (a + b) pour simuler OR
    std::vector<std::string> params_sub = {"%eax", temp_sum, "%eax"};
    currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_sub);
    
    return 0;
}
