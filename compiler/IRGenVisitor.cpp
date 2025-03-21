#include "IRGenVisitor.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility> // pour std::pair


IRGenVisitor::IRGenVisitor(SymbolTableGenVisitor& symbolTableGenVisitor) 
    : symbolTableGenVisitor(symbolTableGenVisitor), cfgs(), currentBB(nullptr), scope() {}

IRGenVisitor::~IRGenVisitor() {
    // Le CFG doit être libéré par le main
    // Car on le réutilise
}

antlrcpp::Any IRGenVisitor::visitFuncDecl(ifccParser::FuncDeclContext *ctx) {
    // Création d'un nouveau CFG pour chaque fonction
    scope = ctx->funcName->getText();
    CFG* cfg = new CFG(symbolTableGenVisitor, scope);
    cfgs.push_back(cfg);
    
    // Visite de la fonction
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
            // si on a un return on sort du block
            if (ctx->stmt(i)->return_stmt() != nullptr) {
                break;
            }
        }
    }
    if (cfgs.back()->current_bb->exit_true) {
        Operation *op_jump = new Jump(cfgs.back()->current_bb, cfgs.back()->current_bb->exit_true->label);
        IRInstr *instruction_jump = new IRInstr(cfgs.back()->current_bb, op_jump);
        cfgs.back()->current_bb->add_IRInstr(instruction_jump);
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
            std::string address = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].offset);
            // std::cout << "# address " << address << "\n";

            // Évaluation de l'expression qu'on place dans le registre universel !reg
            std::pair<bool, int> res(visit(ctx->decl_element(i)->expr()));
            if (res.first) {
                Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
                IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            Operation *wmem = new Wmem(cfgs.back()->current_bb, address, "!reg"); // block, dst, src
            IRInstr *instruction = new IRInstr(cfgs.back()->current_bb, wmem);
            cfgs.back()->current_bb->add_IRInstr(instruction);
        }
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && cfgs.back()->stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == cfgs.back()->stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    std::pair<bool, int> res(visit(ctx->expr()));

    // on regarde si c'est un argument de la fonction
    if (cfgs.back()->stv.symbolTable[nomVar].index_arg >= 0) {
        // Évaluation de l'expression qu'on place dans le registre universel !reg
        if (res.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }

        Operation *copy_arg = new Copy(cfgs.back()->current_bb, "!arg" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].index_arg), "!reg");
        IRInstr *instruction_copy_arg = new IRInstr(cfgs.back()->current_bb, copy_arg);
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_arg);
    }
    else {
        std::string address = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].offset);

        // Évaluation de l'expression qu'on place dans le registre universel !reg
        if (res.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        Operation *wmem = new Wmem(cfgs.back()->current_bb, address, "!reg"); // block, dst, src
        IRInstr *instruction = new IRInstr(cfgs.back()->current_bb, wmem);
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
    }
    
    cfgs.back()->current_bb->exit_true = cfgs.back()->end_block; // default exit
    cfgs.back()->current_bb->exit_false = nullptr;
    
    return 0; // pour savoir qu'on a un return
}

antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
    }

    if (ctx->block().size() == 1) { // pas de else
        BasicBlock* bb_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_if_true");
        BasicBlock* bb_endif = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_endif");
        bb_endif->exit_true = cfgs.back()->current_bb->exit_true;
        cfgs.back()->current_bb->exit_true = bb_true;
        cfgs.back()->current_bb->exit_false = bb_endif;
        bb_true->exit_true = bb_endif;
        cfgs.back()->add_bb(bb_true);
        cfgs.back()->add_bb(bb_endif);

        Operation *op_jump = new JumpFalse(cfgs.back()->current_bb, bb_endif->label, bb_true->label, "!reg");
        IRInstr *instruction_jump = new IRInstr(cfgs.back()->current_bb, op_jump);
        cfgs.back()->current_bb->add_IRInstr(instruction_jump);

        cfgs.back()->current_bb = bb_true;
        visit(ctx->block(0));
        cfgs.back()->current_bb = bb_endif;
    }
    else {
        BasicBlock* bb_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_if_true");
        BasicBlock* bb_false = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_if_false");
        BasicBlock* bb_endif = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_endif");
        bb_endif->exit_true = cfgs.back()->current_bb->exit_true;
        cfgs.back()->current_bb->exit_true = bb_true;
        cfgs.back()->current_bb->exit_false = bb_false;
        bb_true->exit_true = bb_endif;
        bb_false->exit_true = bb_endif;
        cfgs.back()->add_bb(bb_true);
        cfgs.back()->add_bb(bb_false);
        cfgs.back()->add_bb(bb_endif);

        Operation *op_jump = new JumpFalse(cfgs.back()->current_bb, bb_false->label, bb_true->label, "!reg");
        IRInstr *instruction_jump = new IRInstr(cfgs.back()->current_bb, op_jump);
        cfgs.back()->current_bb->add_IRInstr(instruction_jump);

        cfgs.back()->current_bb = bb_true;
        visit(ctx->block(0));
        cfgs.back()->current_bb = bb_false;
        visit(ctx->block(1));
        cfgs.back()->current_bb = bb_endif;
    }

    return 0;
}

antlrcpp::Any IRGenVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx) {
    BasicBlock* bb_test_while = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_test_while");
    bb_test_while->exit_true = cfgs.back()->current_bb->exit_true; // pour pas le perdre
    cfgs.back()->current_bb->exit_true = bb_test_while;
    cfgs.back()->add_bb(bb_test_while);
    cfgs.back()->current_bb = bb_test_while;

    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
    }

    BasicBlock* bb_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_while_true");
    BasicBlock* bb_endwhile = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_endwhile");
    bb_endwhile->exit_true = bb_test_while->exit_true;
    bb_test_while->exit_true = bb_true;
    bb_test_while->exit_false = bb_endwhile;
    bb_true->exit_true = bb_test_while;
    cfgs.back()->add_bb(bb_true);
    cfgs.back()->add_bb(bb_endwhile);

    Operation *op_jump = new JumpFalse(cfgs.back()->current_bb, bb_endwhile->label, bb_true->label, "!reg");
    IRInstr *instruction_jump = new IRInstr(cfgs.back()->current_bb, op_jump);
    cfgs.back()->current_bb->add_IRInstr(instruction_jump);

    cfgs.back()->current_bb = bb_true;
    visit(ctx->block());
    cfgs.back()->current_bb = bb_endwhile;

    return 0;
}

antlrcpp::Any IRGenVisitor::visitIntExpr(ifccParser::IntExprContext *ctx)
{
    return std::pair<bool, int>(true, std::stoi(ctx->CONSTINT()->getText()));
}

antlrcpp::Any IRGenVisitor::visitCharExpr(ifccParser::CharExprContext *ctx)
{
    char interpretedChar;
    if (ctx->CONSTCHAR()->getText().size() == 3) {
        interpretedChar = ctx->CONSTCHAR()->getText()[1];
    }
    else {
        if (ctx->CONSTCHAR()->getText()[1] == '\\') {
            switch (ctx->CONSTCHAR()->getText()[2]) {
                case 'n':
                    interpretedChar = '\n';
                    break;
                case 't':
                    interpretedChar = '\t';
                    break;
                case 'r':
                    interpretedChar = '\r';
                    break;
                case '0':
                    interpretedChar = '\0';
                    break;
                case '\\':
                    interpretedChar = '\\';
                    break;
                case '\'':
                    interpretedChar = '\'';
                    break;
                case '\"':
                    interpretedChar = '\"';
                    break;
                default:
                    interpretedChar = ctx->CONSTCHAR()->getText()[2];
                    break;
            }
        }
    }
    return std::pair<bool, int>(true, interpretedChar);
}

antlrcpp::Any IRGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && cfgs.back()->stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == cfgs.back()->stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();

    // on regarde si c'est un argument de la fonction
    if (cfgs.back()->stv.symbolTable[nomVar].index_arg >= 0) {
        Operation *copy_arg = new Copy(cfgs.back()->current_bb, "!reg", "!arg" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].index_arg));
        IRInstr *instruction_copy_arg = new IRInstr(cfgs.back()->current_bb, copy_arg);
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_arg);
    }
    else {
        std::string address = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].offset); 

        Operation *rmem = new Rmem(cfgs.back()->current_bb, "!reg", address);
        IRInstr *instruction = new IRInstr(cfgs.back()->current_bb, rmem);
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }

    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && cfgs.back()->stv.symbolTable.find(tried_scope + '_' + ctx->ID()->getText()) == cfgs.back()->stv.symbolTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }

    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    std::pair<bool, int> res(visit(ctx->expr()));

    // on regarde si c'est un argument de la fonction
    if (cfgs.back()->stv.symbolTable[nomVar].index_arg >= 0) {
        // Évaluation de l'expression qu'on place dans le registre universel !reg
        if (res.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }

        Operation *copy_arg = new Copy(cfgs.back()->current_bb, "!arg" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].index_arg), "!reg");
        IRInstr *instruction_copy_arg = new IRInstr(cfgs.back()->current_bb, copy_arg);
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_arg);
    }
    else {
        std::string address = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[nomVar].offset);
        
        // Évaluation de l'expression qu'on place dans le registre universel !reg
        if (res.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        Operation *wmem = new Wmem(cfgs.back()->current_bb, address, "!reg"); // block, dst, src
        IRInstr *instruction = new IRInstr(cfgs.back()->current_bb, wmem);
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    
    return res;
}


antlrcpp::Any IRGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->primary()));
    if (res.first) {
        return std::pair<bool, int>(true, !res.second);
    }

    Operation *operation_not = new Not(cfgs.back()->current_bb, "!reg");
    IRInstr *instruction_not = new IRInstr(cfgs.back()->current_bb, operation_not);
    cfgs.back()->current_bb->add_IRInstr(instruction_not);
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->primary()));
    if (res.first) {
        return std::pair<bool, int>(true, -res.second);
    }

    Operation *operation_unaryminus = new UnaryMinus(cfgs.back()->current_bb, "!reg");
    IRInstr *instruction_unaryminus = new IRInstr(cfgs.back()->current_bb, operation_unaryminus);
    cfgs.back()->current_bb->add_IRInstr(instruction_unaryminus);
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    // Évaluation de l'expression gauche qu'on place dans le registre universel !reg
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            if (ctx->mOp()->STAR()) {
                return std::pair<bool, int>(true, res_left.second * res_right.second);
            }
            else if (ctx->mOp()->SLASH()) {
                return std::pair<bool, int>(true, res_left.second / res_right.second);
            }
            else if (ctx->mOp()->MOD()) {
                return std::pair<bool, int>(true, res_left.second % res_right.second);
            }
        }
        // Appliquer l'opération selon l'opérateur
        if (ctx->mOp()->STAR()) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regLeft", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            Operation *operation_mul = new Mul(cfgs.back()->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_mul = new IRInstr(cfgs.back()->current_bb, operation_mul);
            cfgs.back()->current_bb->add_IRInstr(instruction_mul);
        }
        else if (ctx->mOp()->SLASH()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            Operation *operation_div = new Div(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_div = new IRInstr(cfgs.back()->current_bb, operation_div);
            cfgs.back()->current_bb->add_IRInstr(instruction_div);
        }
        else if (ctx->mOp()->MOD()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            Operation *operation_mod = new Mod(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_mod = new IRInstr(cfgs.back()->current_bb, operation_mod);
            cfgs.back()->current_bb->add_IRInstr(instruction_mod);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        
        // Appliquer l'opération selon l'opérateur
        if (ctx->mOp()->STAR()) {
            Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!regLeft", address_left);
            IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            Operation *operation_mul = new Mul(cfgs.back()->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_mul = new IRInstr(cfgs.back()->current_bb, operation_mul);
            cfgs.back()->current_bb->add_IRInstr(instruction_mul);
        }
        else if (ctx->mOp()->SLASH()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!reg", address_left);
            IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            Operation *operation_div = new Div(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_div = new IRInstr(cfgs.back()->current_bb, operation_div);
            cfgs.back()->current_bb->add_IRInstr(instruction_div);
        }
        else if (ctx->mOp()->MOD()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!reg", address_left);
            IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            Operation *operation_mod = new Mod(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_mod = new IRInstr(cfgs.back()->current_bb, operation_mod);
            cfgs.back()->current_bb->add_IRInstr(instruction_mod);
        }
    }    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    // Évaluation de l'expression gauche qu'on place dans le registre universel !reg
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            if (ctx->aOp()->PLUS()) {
                return std::pair<bool, int>(true, res_left.second + res_right.second);
            }
            else if (ctx->aOp()->MINUS()) {
                return std::pair<bool, int>(true, res_left.second - res_right.second);
            }
        }
        // Appliquer l'opération selon l'opérateur
        if (ctx->aOp()->PLUS()) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regLeft", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            Operation *operation_add = new Add(cfgs.back()->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_add = new IRInstr(cfgs.back()->current_bb, operation_add);
            cfgs.back()->current_bb->add_IRInstr(instruction_add);
        }
        else if (ctx->aOp()->MINUS()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            Operation *operation_sub = new Sub(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_sub = new IRInstr(cfgs.back()->current_bb, operation_sub);
            cfgs.back()->current_bb->add_IRInstr(instruction_sub);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }  

        // Appliquer l'opération selon l'opérateur
        if (ctx->aOp()->PLUS()) {
            Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!regLeft", address_left);
            IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            Operation *operation_add = new Add(cfgs.back()->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_add = new IRInstr(cfgs.back()->current_bb, operation_add);
            cfgs.back()->current_bb->add_IRInstr(instruction_add);
        }
        else if (ctx->aOp()->MINUS()) {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!reg", address_left);
            IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            Operation *operation_sub = new Sub(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_sub = new IRInstr(cfgs.back()->current_bb, operation_sub);
            cfgs.back()->current_bb->add_IRInstr(instruction_sub);
        }
    }    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            if (ctx->compOp()->LT()) {
                return std::pair<bool, int>(true, res_left.second < res_right.second);
            }
            else if (ctx->compOp()->LE()) {
                return std::pair<bool, int>(true, res_left.second <= res_right.second);
            }
            else if (ctx->compOp()->GE()) {
                return std::pair<bool, int>(true, res_left.second >= res_right.second);
            }
            else if (ctx->compOp()->GT()) {
                return std::pair<bool, int>(true, res_left.second > res_right.second);
            }
        }
        // Appliquer l'opération selon l'opérateur
        Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
        if (ctx->compOp()->LT()) {
            Operation *operation_lt = new CmpLt(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_lt = new IRInstr(cfgs.back()->current_bb, operation_lt);
            cfgs.back()->current_bb->add_IRInstr(instruction_lt);
        }
        else if (ctx->compOp()->LE()) {
            Operation *operation_le = new CmpLe(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_le = new IRInstr(cfgs.back()->current_bb, operation_le);
            cfgs.back()->current_bb->add_IRInstr(instruction_le);
        }
        else if (ctx->compOp()->GE()) {
            Operation *operation_ge = new CmpGe(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_ge = new IRInstr(cfgs.back()->current_bb, operation_ge);
            cfgs.back()->current_bb->add_IRInstr(instruction_ge);
        }
        else if (ctx->compOp()->GT()) {
            Operation *operation_gt = new CmpGt(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_gt = new IRInstr(cfgs.back()->current_bb, operation_gt);
            cfgs.back()->current_bb->add_IRInstr(instruction_gt);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regRight", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }  
        else {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }

        Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        if (ctx->compOp()->LT()) {
            Operation *operation_lt = new CmpLt(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_lt = new IRInstr(cfgs.back()->current_bb, operation_lt);
            cfgs.back()->current_bb->add_IRInstr(instruction_lt);
        }
        else if (ctx->compOp()->LE()) {
            Operation *operation_le = new CmpLe(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_le = new IRInstr(cfgs.back()->current_bb, operation_le);
            cfgs.back()->current_bb->add_IRInstr(instruction_le);
        }
        else if (ctx->compOp()->GE()) {
            Operation *operation_ge = new CmpGe(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_ge = new IRInstr(cfgs.back()->current_bb, operation_ge);
            cfgs.back()->current_bb->add_IRInstr(instruction_ge);
        }
        else if (ctx->compOp()->GT()) {
            Operation *operation_gt = new CmpGt(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_gt = new IRInstr(cfgs.back()->current_bb, operation_gt);
            cfgs.back()->current_bb->add_IRInstr(instruction_gt);
        }
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            if (ctx->eqOp()->EQ()) {
                return std::pair<bool, int>(true, res_left.second == res_right.second);
            }
            else if (ctx->eqOp()->NEQ()) {
                return std::pair<bool, int>(true, res_left.second != res_right.second);
            }
        }
        // Appliquer l'opération selon l'opérateur
        Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
        if (ctx->eqOp()->EQ()) {
            Operation *operation_eq = new CmpEq(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_eq = new IRInstr(cfgs.back()->current_bb, operation_eq);
            cfgs.back()->current_bb->add_IRInstr(instruction_eq);
        }
        else if (ctx->eqOp()->NEQ()) {
            Operation *operation_neq = new CmpNeq(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_neq = new IRInstr(cfgs.back()->current_bb, operation_neq);
            cfgs.back()->current_bb->add_IRInstr(instruction_neq);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regRight", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } 
        else {
            Operation *copy_right = new Copy(cfgs.back()->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfgs.back()->current_bb, copy_right);
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }

        Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        if (ctx->eqOp()->EQ()) {
            Operation *operation_eq = new CmpEq(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_eq = new IRInstr(cfgs.back()->current_bb, operation_eq);
            cfgs.back()->current_bb->add_IRInstr(instruction_eq);
        }
        else if (ctx->eqOp()->NEQ()) {
            Operation *operation_neq = new CmpNeq(cfgs.back()->current_bb, "!reg", "!regRight");
            IRInstr *instruction_neq = new IRInstr(cfgs.back()->current_bb, operation_neq);
            cfgs.back()->current_bb->add_IRInstr(instruction_neq);
        }
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            return std::pair<bool, int>(true, res_left.second & res_right.second);
        }
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new And(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } 
        Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new And(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            return std::pair<bool, int>(true, res_left.second ^ res_right.second);
        }
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new Xor(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }   

        Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new Xor(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    std::pair<bool, int> res_left(visit(ctx->left));
    if (res_left.first) {
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            return std::pair<bool, int>(true, res_left.second | res_right.second);
        }
        Operation *op_const = new LdConst(cfgs.back()->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new Or(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfgs.back()->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfgs.back()->current_bb, wmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }  

        Operation *rmem_left = new Rmem(cfgs.back()->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfgs.back()->current_bb, rmem_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new Or(cfgs.back()->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfgs.back()->current_bb, operation_and);
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    std::pair<bool, int> res(visit(ctx->expr()));
    return res;
}

antlrcpp::Any IRGenVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    std::string nomFonction = ctx->ID()->getText();

    std::vector<std::string> args_temp_addr;
    
    for (int i = 0; i < ctx->expr().size(); i++) {
        std::string temp(cfgs.back()->create_new_tempvar(Type::INT));
        args_temp_addr.push_back("RBP" + std::to_string(cfgs.back()->stv.symbolTable[temp].offset));
        std::pair<bool, int> res(visit(ctx->expr(i)));
        if (res.first) {
            Operation *op_const = new LdConst(cfgs.back()->current_bb, "!reg", res.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfgs.back()->current_bb, op_const);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        Operation *wmem_temp = new Wmem(cfgs.back()->current_bb, args_temp_addr.back(), "!reg");
        IRInstr *instruction_temp = new IRInstr(cfgs.back()->current_bb, wmem_temp);
        cfgs.back()->current_bb->add_IRInstr(instruction_temp);
        // plus de 6 args ? faire une operation push et pop ?
    }

    // on sauvegarde nos registres
    std::string callingFunc(cfgs.back()->current_bb->cfg->functionName);
    for (int i = 0; i < std::min<int>(6, cfgs.back()->stv.symbolTable[callingFunc].index_arg); i++) {
        Operation *push = new Push(cfgs.back()->current_bb, "!arg"+std::to_string(i)+"64");
        IRInstr *instruction_push = new IRInstr(cfgs.back()->current_bb, push);
        cfgs.back()->current_bb->add_IRInstr(instruction_push);
    }

    // on remet les arguments à la bonne place
    for (int i = 0; i < args_temp_addr.size(); i++) {
        Operation *rmem_temp = new Rmem(cfgs.back()->current_bb, "!arg"+std::to_string(i)+"32", args_temp_addr[i]);
        IRInstr *instruction_temp = new IRInstr(cfgs.back()->current_bb, rmem_temp);
        cfgs.back()->current_bb->add_IRInstr(instruction_temp);
    }

    // On appelle la fonction
    Operation *call = new Call(cfgs.back()->current_bb, nomFonction);  // block, dst, src
    IRInstr *instruction_call = new IRInstr(cfgs.back()->current_bb, call);
    cfgs.back()->current_bb->add_IRInstr(instruction_call);

    // on reprend nos registres
    for (int i = std::min<int>(6, cfgs.back()->stv.symbolTable[callingFunc].index_arg)-1; i >= 0; i--) {
        Operation *pop = new Pop(cfgs.back()->current_bb, "!arg"+std::to_string(i)+"64");
        IRInstr *instruction_pop = new IRInstr(cfgs.back()->current_bb, pop);
        cfgs.back()->current_bb->add_IRInstr(instruction_pop);
    }

    return std::pair<bool, int>(false, 0);
}