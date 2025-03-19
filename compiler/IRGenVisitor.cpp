#include "IRGenVisitor.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility> // pour std::pair


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
            std::pair<bool, int> res(visit(ctx->decl_element(i)->expr()));
            if (res.first) {
                Operation *op_const = new LdConst(cfg->current_bb, "!reg", res.second);  // block, dst, src
                IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
                cfg->current_bb->add_IRInstr(instruction_const);
            }
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
    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfg->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);
    }
    Operation *wmem = new Wmem(cfg->current_bb, address, "!reg"); // block, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, wmem);
    cfg->current_bb->add_IRInstr(instruction);
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfg->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);
    }
    
    cfg->current_bb->exit_true = cfg->end_block; // default exit
    cfg->current_bb->exit_false = nullptr;
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitConst(ifccParser::ConstContext *ctx)
{
    // Operation *op_const = new LdConst(cfg->current_bb, "!reg", std::stoi(ctx->CONST()->getText()));  // block, dst, src
    // IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
    // cfg->current_bb->add_IRInstr(instruction_const);
    
    return std::pair<bool, int>(true, std::stoi(ctx->CONST()->getText()));
}

antlrcpp::Any IRGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();
    std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset); 

    Operation *rmem = new Rmem(cfg->current_bb, "!reg", address);
    IRInstr *instruction = new IRInstr(cfg->current_bb, rmem);
    cfg->current_bb->add_IRInstr(instruction);

    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    // On récupère le nom et l'adresse stack de la variable en question
    std::string nomVar = scope + "_" + ctx->ID()->getText();
    std::string address = "RBP" + std::to_string(cfg->stv.symbolTable[nomVar].offset); 

    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->expr()));
    if (res.first) {
        Operation *op_const = new LdConst(cfg->current_bb, "!reg", res.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);
    }
    Operation *wmem = new Wmem(cfg->current_bb, address, "!reg"); // block, dst, src
    IRInstr *instruction = new IRInstr(cfg->current_bb, wmem);
    cfg->current_bb->add_IRInstr(instruction);

    // la valeur est retournée dans !reg
    
    return res;
}


antlrcpp::Any IRGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->primary()));
    if (res.first) {
        return std::pair<bool, int>(true, !res.second);
    }

    Operation *operation_not = new Not(cfg->current_bb, "!reg");
    IRInstr *instruction_not = new IRInstr(cfg->current_bb, operation_not);
    cfg->current_bb->add_IRInstr(instruction_not);
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    std::pair<bool, int> res(visit(ctx->primary()));
    if (res.first) {
        return std::pair<bool, int>(true, -res.second);
    }

    Operation *operation_unaryminus = new UnaryMinus(cfg->current_bb, "!reg");
    IRInstr *instruction_unaryminus = new IRInstr(cfg->current_bb, operation_unaryminus);
    cfg->current_bb->add_IRInstr(instruction_unaryminus);
    
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
            Operation *op_const = new LdConst(cfg->current_bb, "!regLeft", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);

            Operation *operation_mul = new Mul(cfg->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_mul = new IRInstr(cfg->current_bb, operation_mul);
            cfg->current_bb->add_IRInstr(instruction_mul);
        }
        else if (ctx->mOp()->SLASH()) {
            Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
            cfg->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);

            Operation *operation_div = new Div(cfg->current_bb, "!reg", "!regRight");
            IRInstr *instruction_div = new IRInstr(cfg->current_bb, operation_div);
            cfg->current_bb->add_IRInstr(instruction_div);
        }
        else if (ctx->mOp()->MOD()) {
            Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
            cfg->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);

            Operation *operation_mod = new Mod(cfg->current_bb, "!reg", "!regRight");
            IRInstr *instruction_mod = new IRInstr(cfg->current_bb, operation_mod);
            cfg->current_bb->add_IRInstr(instruction_mod);
        }
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        }
        
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
            Operation *op_const = new LdConst(cfg->current_bb, "!regLeft", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);

            Operation *operation_add = new Add(cfg->current_bb, "!reg", "!regLeft");
            IRInstr *instruction_add = new IRInstr(cfg->current_bb, operation_add);
            cfg->current_bb->add_IRInstr(instruction_add);
        }
        else if (ctx->aOp()->MINUS()) {
            Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
            cfg->current_bb->add_IRInstr(instruction_copy_right);

            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_left.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);

            Operation *operation_sub = new Sub(cfg->current_bb, "!reg", "!regRight");
            IRInstr *instruction_sub = new IRInstr(cfg->current_bb, operation_sub);
            cfg->current_bb->add_IRInstr(instruction_sub);
        }
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        }  

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
        Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
        cfg->current_bb->add_IRInstr(instruction_copy_right);

        Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);
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
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!regRight", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        }  
        else {
            Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
            cfg->current_bb->add_IRInstr(instruction_copy_right);
        }

        Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);
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
        Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
        IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
        cfg->current_bb->add_IRInstr(instruction_copy_right);

        Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);
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
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!regRight", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        } 
        else {
            Operation *copy_right = new Copy(cfg->current_bb, "!regRight", "!reg");
            IRInstr *instruction_copy_right = new IRInstr(cfg->current_bb, copy_right);
            cfg->current_bb->add_IRInstr(instruction_copy_right);
        }

        Operation *rmem_left = new Rmem(cfg->current_bb, "!reg", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);
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
        Operation *op_const = new LdConst(cfg->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new And(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        } 
        Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new And(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
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
        Operation *op_const = new LdConst(cfg->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new Xor(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        }   

        Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new Xor(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
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
        Operation *op_const = new LdConst(cfg->current_bb, "!regLeft", res_left.second);  // block, dst, src
        IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
        cfg->current_bb->add_IRInstr(instruction_const);

        Operation *operation_and = new Or(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfg->create_new_tempvar(Type::INT);
        std::string address_left = "RBP" + std::to_string(cfg->stv.symbolTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        Operation *wmem_left = new Wmem(cfg->current_bb, address_left, "!reg");
        IRInstr *instruction_left = new IRInstr(cfg->current_bb, wmem_left);
        cfg->current_bb->add_IRInstr(instruction_left);

        std::pair<bool, int> res_right(visit(ctx->right));
        if (res_right.first) {
            Operation *op_const = new LdConst(cfg->current_bb, "!reg", res_right.second);  // block, dst, src
            IRInstr *instruction_const = new IRInstr(cfg->current_bb, op_const);
            cfg->current_bb->add_IRInstr(instruction_const);
        }  

        Operation *rmem_left = new Rmem(cfg->current_bb, "!regLeft", address_left);
        IRInstr *instruction_read_left = new IRInstr(cfg->current_bb, rmem_left);
        cfg->current_bb->add_IRInstr(instruction_read_left);
        
        Operation *operation_and = new Or(cfg->current_bb, "!reg", "!regLeft");
        IRInstr *instruction_and = new IRInstr(cfg->current_bb, operation_and);
        cfg->current_bb->add_IRInstr(instruction_and);
    }
    
    return std::pair<bool, int>(false, 0);
}

antlrcpp::Any IRGenVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    std::pair<bool, int> res(visit(ctx->expr()));
    return res;
}
