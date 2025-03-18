#include "IRGenVisitor.h"

#include <iostream>
#include <string>
#include <vector>


IRGenVisitor::IRGenVisitor(SymbolTableGenVisitor& symbolTableGenVisitor) 
    : symbolTableGenVisitor(symbolTableGenVisitor), cfg(nullptr), currentBB(nullptr), scope() {}

IRGenVisitor::~IRGenVisitor() {
    // Le CFG doit être libéré par le main
    // Car on le réutilise
}

antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    // Initialisation du CFG et du bloc de base
    cfg = new CFG(symbolTableGenVisitor);
    scope = "main";
    
    // Visite du bloc principal
    visit(ctx->block());
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitBlock(ifccParser::BlockContext *ctx)
{
    // Visiter tous les statements dans le bloc
    for (int i = 0; i < ctx->stmt().size(); i++) {
        // Cas pour un block
        if (ctx->stmt(i)->block_stmt() != nullptr) {
            scope += "_" + std::to_string(i);
            visit(ctx->stmt(i)); // Appel récursif on va visiter ce block
            while (scope.back() != '_') {
                scope.pop_back();
            }
            scope.pop_back();
        }
        // Cas si juste un statement
        else {
            visit(ctx->stmt(i));
        }
    }
    return 0;
}


antlrcpp::Any IRGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{ 
    // On a rien besoin de faire si c'est uniquement une déclaration...
    // Car c'est déjà géré par la symbol table.

    if (ctx->expr()) // si déclaration + assignement direct
    {
        // On récupère le nom de la variable en question
        std::string nomVar = scope + "_" + ctx->ID()->getText();

        // Évaluation de l'expression qu'on place dans le registre universel !reg
        visit(ctx->expr());

        Operation *op = new Copy(cfg->current_bb, nomVar, "!reg");  // block, dst, src
        IRInstr *instruction = new IRInstr(cfg->current_bb, op);
        cfg->current_bb->add_IRInstr(instruction);
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // On récupère le nom de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();

    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->expr());

    Operation *op = new Copy(cfg->current_bb, nomVar, "!reg");  // block, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, op);
    cfg->current_bb->add_IRInstr(instruction);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->expr());
    
    cfg->current_bb->exit_true = cfg->end_block; // default exit
    cfg->current_bb->exit_false = nullptr;
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    Operation *op_const = new LdConst(cfg->current_bb, "!reg", std::stoi(ctx->CONST()->getText()));  // bb, dst, src
    IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
    cfg->current_bb->add_IRInstr(instruction_const);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    std::string nomVar = scope + "_" + ctx->ID()->getText();

    Operation *op = new Copy(cfg->current_bb, "!reg", nomVar);  // bb, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, op);
    cfg->current_bb->add_IRInstr(instruction);

    return 0;
}

// antlrcpp::Any IRGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
//     // Évaluation de l'expression primaire
//     visit(ctx->primary());
    
//     // Créer une variable temporaire pour stocker le résultat
//     std::string temp = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy = {temp, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy);
    
//     // Générer une instruction de comparaison égale avec 0
//     std::vector<std::string> params = {"%eax", temp, "0"};
//     currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params);
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
//     // Évaluation de l'expression primaire
//     visit(ctx->primary());
    
//     // Créer une variable temporaire pour stocker le résultat
//     std::string temp = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy = {temp, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy);
    
//     // Inverser le signe de la variable temporaire et stocker dans %eax
//     std::vector<std::string> params = {"%eax", "0", temp};
//     currentBB->add_IRInstr(IRInstr::sub, Type::INT, params);
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Appliquer l'opération selon l'opérateur
//     if (ctx->mOp()->STAR()) {
//         std::vector<std::string> params = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
//     }
//     else if (ctx->mOp()->SLASH() || ctx->mOp()->MOD()) {
//         // Pour la division et le modulo, on utilise une approche différente
//         // car ces opérations ne sont pas directement disponibles dans l'IR
//         // En pratique, cela nécessiterait des appels à des fonctions externes
//         // ou des séquences d'instructions spécifiques
        
//         // Ici, on se contente de faire une multiplication pour la démonstration
//         std::vector<std::string> params = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
//     }
    
//     return 0;
// }

antlrcpp::Any IRGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *op = new Copy(cfg->current_bb, temp_left, "!reg");  // bb, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, op);
    cfg->current_bb->add_IRInstr(instruction);
    
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

// antlrcpp::Any IRGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Appliquer les comparaisons
//     if (ctx->compOp()->LT()) {
//         std::vector<std::string> params = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params);
//     }
//     else if (ctx->compOp()->LE()) {
//         // Pour LE, on utilise une combinaison de LT et EQ
//         std::vector<std::string> params_lt = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params_lt);
        
//         // Sauvegarder le résultat
//         std::string temp_lt = cfg->create_new_tempvar(Type::INT);
//         std::vector<std::string> param_copy_lt = {temp_lt, "%eax"};
//         currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_lt);
        
//         std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
        
//         // Combiner les résultats avec OR
//         std::string temp_eq = cfg->create_new_tempvar(Type::INT);
//         std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
//         currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
        
//         // temp_lt OR temp_eq -> %eax
//         std::vector<std::string> params_or = {"%eax", temp_lt, temp_eq};
//         currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or); // Utiliser add comme OR
//     }
//     else if (ctx->compOp()->GT() || ctx->compOp()->GE()) {
//         // Pour GT, on inverse la comparaison LT (a > b équivalent à b < a)
//         std::vector<std::string> params = {"%eax", temp_right, temp_left};
//         currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params);
        
//         if (ctx->compOp()->GE()) {
//             // Pour GE, on utilise une combinaison de GT et EQ
//             std::string temp_gt = cfg->create_new_tempvar(Type::INT);
//             std::vector<std::string> param_copy_gt = {temp_gt, "%eax"};
//             currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_gt);
            
//             std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
//             currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
            
//             // Combiner les résultats avec OR
//             std::string temp_eq = cfg->create_new_tempvar(Type::INT);
//             std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
//             currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
            
//             // temp_gt OR temp_eq -> %eax
//             std::vector<std::string> params_or = {"%eax", temp_gt, temp_eq};
//             currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or); // Utiliser add comme OR
//         }
//     }
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Appliquer les comparaisons
//     if (ctx->eqOp()->EQ()) {
//         std::vector<std::string> params = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params);
//     }
//     else if (ctx->eqOp()->NEQ()) {
//         // Pour NEQ, on calcule l'égalité puis on inverse le résultat
//         std::vector<std::string> params_eq = {"%eax", temp_left, temp_right};
//         currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params_eq);
        
//         // Sauvegarder le résultat dans une variable temporaire
//         std::string temp_eq = cfg->create_new_tempvar(Type::INT);
//         std::vector<std::string> param_copy_eq = {temp_eq, "%eax"};
//         currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_eq);
        
//         // Inverser le résultat (1 - temp_eq)
//         std::vector<std::string> params_inv = {"%eax", "1", temp_eq};
//         currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_inv);
//     }
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Multiplication pour simuler l'opération AND
//     std::vector<std::string> params = {"%eax", temp_left, temp_right};
//     currentBB->add_IRInstr(IRInstr::mul, Type::INT, params);
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
//     // XOR n'est pas directement disponible dans l'IR, nous allons donc le simuler avec d'autres opérations
    
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Pour simuler XOR, on peut utiliser: (a | b) - (a & b)
//     // D'abord calculer (a & b)
//     std::vector<std::string> params_and = {"%eax", temp_left, temp_right};
//     currentBB->add_IRInstr(IRInstr::mul, Type::INT, params_and); // Utiliser mul comme AND
    
//     // Sauvegarder dans une variable temporaire
//     std::string temp_and = cfg->create_new_tempvar(Type::INT);
//     std::vector<std::string> param_copy_and = {temp_and, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_and);
    
//     // Calculer (a + b) pour simuler (a | b)
//     std::vector<std::string> params_or = {"%eax", temp_left, temp_right};
//     currentBB->add_IRInstr(IRInstr::add, Type::INT, params_or);
    
//     // Soustraire (a & b) pour obtenir XOR
//     std::vector<std::string> params_sub = {"%eax", "%eax", temp_and};
//     currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_sub);
    
//     return 0;
// }

// antlrcpp::Any IRGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
//     // Évaluation de l'opérande gauche
//     visit(ctx->left);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande gauche
//     std::string temp_left = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_left = {temp_left, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_left);
    
//     // Évaluation de l'opérande droite
//     visit(ctx->right);
    
//     // Créer une variable temporaire pour stocker le résultat de l'opérande droite
//     std::string temp_right = cfg->create_new_tempvar(Type::INT);
    
//     // Copier le résultat de l'expression dans la variable temporaire
//     std::vector<std::string> param_copy_right = {temp_right, "%eax"};
//     currentBB->add_IRInstr(IRInstr::copy, Type::INT, param_copy_right);
    
//     // Pour simuler OR, on peut utiliser: a + b - (a & b)
//     // D'abord calculer (a & b)
//     std::string temp_sum = cfg->create_new_tempvar(Type::INT);
//     std::vector<std::string> params_add = {temp_sum, temp_left, temp_right};
//     currentBB->add_IRInstr(IRInstr::add, Type::INT, params_add);
    
//     std::vector<std::string> params_and = {"%eax", temp_left, temp_right};
//     currentBB->add_IRInstr(IRInstr::mul, Type::INT, params_and); // Utiliser mul comme AND
    
//     // Soustraire (a & b) de (a + b) pour simuler OR
//     std::vector<std::string> params_sub = {"%eax", temp_sum, "%eax"};
//     currentBB->add_IRInstr(IRInstr::sub, Type::INT, params_sub);
    
//     return 0;
// }
