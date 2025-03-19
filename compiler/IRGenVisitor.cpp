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
    for (int i = 0; i < ctx->decl_element().size(); i++) {
        if (ctx->decl_element(i)->expr()) // si déclaration + assignement direct
        {
            std::string nomVar = scope + "_" + ctx->decl_element(i)->ID()->getText();
            // std::cout << "# nomVar " << i << " : " << nomVar << "\n";
            std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset);
            // std::cout << "# address " << address << "\n";

            // Évaluation de l'expression qu'on place dans le registre universel !reg
            visit(ctx->decl_element(i)->expr());
            Operation *wmem = new Wmem(cfg->current_bb, address, "!reg"); // block, dst, src
            IRInstr *instruction = new IRInstr(cfg->current_bb, wmem);
            cfg->current_bb->add_IRInstr(instruction);
        }
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();
    // std::cout << "# nomVar " << nomVar << "\n";
    std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset); 
    // std::cout << "# address " << address << "\n";

    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->expr());
    Operation *wmem = new Wmem(cfg->current_bb, address, "!reg"); // block, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, wmem);
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
    Operation *op_const = new LdConst(cfg->current_bb, "!reg", std::stoi(ctx->CONST()->getText()));  // block, dst, src
    IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
    cfg->current_bb->add_IRInstr(instruction_const);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();
    std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset); 

    Operation *rmem = new Rmem(cfg->current_bb, "!reg", address);
    IRInstr *instruction = new IRInstr(cfg->current_bb, rmem);
    cfg->current_bb->add_IRInstr(instruction);

    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    // On récupère le nom et l'adresse stack de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();
    // std::cout << "# nomVar " << nomVar << "\n";
    std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset); 
    // std::cout << "# address " << address << "\n";

    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->expr());
    Operation *wmem = new Wmem(cfg->current_bb, address, "!reg"); // block, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, wmem);
    cfg->current_bb->add_IRInstr(instruction);

    // la valeur est retournée dans !reg
    
    return 0;
}


antlrcpp::Any IRGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->primary()); 

    Operation *operation_not = new Not(cfg->current_bb, "!reg");
    IRInstr *instruction_not = new IRInstr(cfg->current_bb, operation_not);
    cfg->current_bb->add_IRInstr(instruction_not);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    visit(ctx->primary()); 

    Operation *operation_unaryminus = new UnaryMinus(cfg->current_bb, "!reg");
    IRInstr *instruction_unaryminus = new IRInstr(cfg->current_bb, operation_unaryminus);
    cfg->current_bb->add_IRInstr(instruction_unaryminus);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression left qu'on place dans le registre universel !reg
    visit(ctx->right);
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->mOp()->STAR()) {
        Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);

        Operation *operation_mul = new Mul(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_mul = new IRInstr(cfg->current_bb, operation_mul);
        cfg->current_bb->add_IRInstr(instruction_mul);
    }
    else if (ctx->mOp()->SLASH()) {
        Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
        cfg->current_bb->add_IRInstr(instruction_copy_right);

        Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);

        Operation *operation_div = new Div(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_div = new IRInstr(cfg->current_bb, operation_div);
        cfg->current_bb->add_IRInstr(instruction_div);
    }
    else if (ctx->mOp()->MOD()) {
        Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
        cfg->current_bb->add_IRInstr(instruction_copy_right);

        Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);

        Operation *operation_mod = new Mod(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_mod = new IRInstr(cfg->current_bb, operation_mod);
        cfg->current_bb->add_IRInstr(instruction_mod);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {

    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression left qu'on place dans le registre universel !reg
    visit(ctx->right);   
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->aOp()->PLUS()) {
        Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);

        Operation *operation_add = new Add(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_add = new IRInstr(cfg->current_bb, operation_add);
        cfg->current_bb->add_IRInstr(instruction_add);
    }
    else if (ctx->aOp()->MINUS()) {
        Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
        cfg->current_bb->add_IRInstr(instruction_copy_right);

        Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);

        Operation *operation_sub = new Sub(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_sub = new IRInstr(cfg->current_bb, operation_sub);
        cfg->current_bb->add_IRInstr(instruction_sub);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression right qu'on place dans le registre universel !reg
    visit(ctx->right);   

    Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
    IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
    cfg->current_bb->add_IRInstr(instruction_copy_right);

    Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
    IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
    cfg->current_bb->add_IRInstr(instruction_read_left);
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->compOp()->LT()) {
        Operation *operation_lt = new CmpLt(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_lt = new IRInstr(cfg->current_bb, operation_lt);
        cfg->current_bb->add_IRInstr(instruction_lt);
    }
    else if (ctx->compOp()->LE()) {
        Operation *operation_le = new CmpLe(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_le = new IRInstr(cfg->current_bb, operation_le);
        cfg->current_bb->add_IRInstr(instruction_le);
    }
    else if (ctx->compOp()->GE()) {
        Operation *operation_ge = new CmpGe(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_ge = new IRInstr(cfg->current_bb, operation_ge);
        cfg->current_bb->add_IRInstr(instruction_ge);
    }
    else if (ctx->compOp()->GT()) {
        Operation *operation_gt = new CmpGt(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_gt = new IRInstr(cfg->current_bb, operation_gt);
        cfg->current_bb->add_IRInstr(instruction_gt);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression right qu'on place dans le registre universel !reg
    visit(ctx->right);   

    Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
    IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
    cfg->current_bb->add_IRInstr(instruction_copy_right);

    Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
    IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
    cfg->current_bb->add_IRInstr(instruction_read_left);
    
    // Appliquer l'opération selon l'opérateur
    if (ctx->eqOp()->EQ()) {
        Operation *operation_eq = new CmpEq(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_eq = new IRInstr(cfg->current_bb, operation_eq);
        cfg->current_bb->add_IRInstr(instruction_eq);
    }
    else if (ctx->eqOp()->NEQ()) {
        Operation *operation_neq = new CmpNeq(cfg->current_bb, "!reg", "!regRight");
        IRInstr *instruction_neq = new IRInstr(cfg->current_bb, operation_neq);
        cfg->current_bb->add_IRInstr(instruction_neq);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression left qu'on place dans le registre universel !reg
    visit(ctx->right);   

    Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
    IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
    cfg->current_bb->add_IRInstr(instruction_read_left);

    Operation *operation_and = new And(cfg->current_bb, "!reg", "!regLeft");
    IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
    cfg->current_bb->add_IRInstr(instruction_and);

    return 0;
}

antlrcpp::Any IRGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression left qu'on place dans le registre universel !reg
    visit(ctx->right);   

    Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
    IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
    cfg->current_bb->add_IRInstr(instruction_read_left);

    Operation *operation_xor = new Xor(cfg->current_bb, "!reg", "!regLeft");
    IRInstr *instruction_xor = new IRInstr(cfg->current_bb, operation_xor);
    cfg->current_bb->add_IRInstr(instruction_xor);

    return 0;
}

antlrcpp::Any IRGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    // Évaluation de l'expression droite qu'on place dans le registre universel !reg
    visit(ctx->left);
    std::string temp_left = cfg->create_new_tempvar(Type::INT);
    std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset); 
    
    // Copier le résultat de l'expression dans la variable temporaire
    Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
    IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
    cfg->current_bb->add_IRInstr(instruction_left);


    // Évaluation de l'expression left qu'on place dans le registre universel !reg
    visit(ctx->right);   

    Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
    IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
    cfg->current_bb->add_IRInstr(instruction_read_left);

    Operation *operation_or = new Or(cfg->current_bb, "!reg", "!regLeft");
    IRInstr *instruction_or = new IRInstr(cfg->current_bb, operation_or);
    cfg->current_bb->add_IRInstr(instruction_or);

    return 0;
}
