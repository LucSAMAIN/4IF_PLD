#include "CodeGenVisitor.h"
#include "IR.h"
#include "type.h"
#include "symbole.h"
#include <vector>
#include <string>

// Création d'un CFG global pour stocker les instructions IR
CFG* cfg = nullptr;
BasicBlock* currentBB = nullptr;

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

    // Affichage de l'IR généré (pour debug)
    std::cout << "// Programme en représentation intermédiaire (IR)\n";
    std::cout << "// Bloc principal: " << currentBB->label << "\n";
    for (auto instr : currentBB->instrs) {
        std::cout << "// ";
        instr->gen_asm(std::cout);
        std::cout << "\n";
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDecl_stmt(ifccParser::Decl_stmtContext *ctx)
{
    // Ajout de la variable à la table des symboles
    cfg->add_to_symbol_table(ctx->ID()->getText(), Type::INT);
    
    if (ctx->expr())
    {
        // Évaluation de l'expression
        visit(ctx->expr());
        
        // Ajout d'une instruction IR pour stocker le résultat dans la variable
        std::vector<std::string> params = {ctx->ID()->getText(), "%eax"};
        currentBB->add_IRInstr(IRInstr::copy, Type::INT, params);
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // Évaluation de l'expression
    visit(ctx->expr());
    
    // Ajout d'une instruction IR pour l'assignation
    std::vector<std::string> params = {ctx->ID()->getText(), "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression de retour
    visit(ctx->expr());
    
    // Ajout d'une instruction IR pour le retour
    std::vector<std::string> params = {"%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    // Chargement d'une constante
    std::vector<std::string> params = {"%eax", ctx->CONST()->getText()};
    currentBB->add_IRInstr(IRInstr::ldconst, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // Chargement d'une variable
    std::vector<std::string> params = {"%eax", ctx->ID()->getText()};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression primaire
    visit(ctx->primary());
    
    // Opération NOT logique
    // Nous n'avons pas d'opération NOT directe dans l'IR, donc nous pourrions
    // implémenter cela avec d'autres opérations
    
    // Nous pouvons implémenter NOT comme (1 - x) pour les valeurs booléennes
    std::vector<std::string> params = {"%eax", "1", "%eax"};
    currentBB->add_IRInstr(IRInstr::sub, Type::INT, params);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression primaire
    visit(ctx->primary());
    
    // Négation (0 - x)
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "0"};
    currentBB->add_IRInstr(IRInstr::ldconst, Type::INT, params1);
    
    std::vector<std::string> params2 = {"%eax", temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::sub, Type::INT, params2);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Opération selon l'opérateur
    if (ctx->mOp()->STAR()) {
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::mul, Type::INT, params2);
    }
    else if (ctx->mOp()->SLASH()) {
        // Division - nous supposons que l'IR a une opération de division
        // Nous pourrions ajouter une opération div à l'IR
        // Pour l'instant, nous utilisons une opération générique
        std::vector<std::string> params2 = {"%eax", temp, "%eax"};
        // Utilisation de l'opération call avec des paramètres appropriés
        std::vector<std::string> callParams = {"div", "%eax", temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::call, Type::INT, callParams);
    }
    else if (ctx->mOp()->MOD()) {
        // Modulo - nous supposons que l'IR a une opération de modulo
        // Nous pourrions ajouter une opération mod à l'IR
        // Pour l'instant, nous utilisons une opération générique
        std::vector<std::string> params2 = {"%eax", temp, "%eax"};
        // Utilisation de l'opération call avec des paramètres appropriés
        std::vector<std::string> callParams = {"mod", "%eax", temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::call, Type::INT, callParams);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Opération selon l'opérateur
    if (ctx->aOp()->PLUS()) {
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::add, Type::INT, params2);
    }
    else if (ctx->aOp()->MINUS()) {
        std::vector<std::string> params2 = {"%eax", temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::sub, Type::INT, params2);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Comparaison selon l'opérateur
    if (ctx->compOp()->LT()) {
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params2);
    }
    else if (ctx->compOp()->LE()) {
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::cmp_le, Type::INT, params2);
    }
    else if (ctx->compOp()->GT()) {
        // Pour GT, nous pouvons inverser les opérandes et utiliser LT
        std::vector<std::string> params2 = {"%eax", temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::cmp_lt, Type::INT, params2);
    }
    else if (ctx->compOp()->GE()) {
        // Pour GE, nous pouvons inverser les opérandes et utiliser LE
        std::vector<std::string> params2 = {"%eax", temp, "%eax"};
        currentBB->add_IRInstr(IRInstr::cmp_le, Type::INT, params2);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Comparaison selon l'opérateur
    if (ctx->eqOp()->EQ()) {
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params2);
    }
    else if (ctx->eqOp()->NEQ()) {
        // Pour NEQ, nous pouvons utiliser EQ puis inverser le résultat
        std::vector<std::string> params2 = {"%eax", "%eax", temp};
        currentBB->add_IRInstr(IRInstr::cmp_eq, Type::INT, params2);
        
        // Inversion du résultat (1 - résultat)
        std::string one = cfg->create_new_tempvar(Type::INT);
        std::vector<std::string> params3 = {one, "1"};
        currentBB->add_IRInstr(IRInstr::ldconst, Type::INT, params3);
        
        std::vector<std::string> params4 = {"%eax", one, "%eax"};
        currentBB->add_IRInstr(IRInstr::sub, Type::INT, params4);
    }
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Opération AND logique
    // Nous pourrions ajouter une opération and à l'IR
    // Pour l'instant, nous utilisons une opération générique
    std::vector<std::string> callParams = {"and", "%eax", temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::call, Type::INT, callParams);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Opération XOR logique
    // Nous pourrions ajouter une opération xor à l'IR
    // Pour l'instant, nous utilisons une opération générique
    std::vector<std::string> callParams = {"xor", "%eax", temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::call, Type::INT, callParams);
    
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    // Évaluation de l'opérande gauche
    visit(ctx->left);
    
    // Sauvegarde du résultat dans une variable temporaire
    std::string temp = cfg->create_new_tempvar(Type::INT);
    std::vector<std::string> params1 = {temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::copy, Type::INT, params1);
    
    // Évaluation de l'opérande droite
    visit(ctx->right);
    
    // Opération OR logique
    // Nous pourrions ajouter une opération or à l'IR
    // Pour l'instant, nous utilisons une opération générique
    std::vector<std::string> callParams = {"or", "%eax", temp, "%eax"};
    currentBB->add_IRInstr(IRInstr::call, Type::INT, callParams);
    
    return 0;
}
