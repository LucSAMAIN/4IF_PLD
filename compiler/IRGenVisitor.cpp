#include "IRGenVisitor.h"

#include <iostream>
#include <string>
#include <vector>

typedef struct ExprReturn {
    bool isConst;
    Type type;
    union {
        int32_t ivalue;
        double dvalue;
    };

    ExprReturn() : isConst(false), type(Type::INT64_T), ivalue(0) {}
    ExprReturn(bool isConst, Type type, int32_t val) : isConst(isConst), type(type), ivalue(val) {}
    ExprReturn(bool isConst, Type type, double val) : isConst(isConst), type(type), dvalue(val) {}
    ExprReturn(const ExprReturn& other) : isConst(other.isConst), type(other.type) {
        if (isConst) {
            if (type == Type::FLOAT64_T) {
                dvalue = other.dvalue;
            } else {
                ivalue = other.ivalue;
            }
        }
    }
    ExprReturn& operator=(const ExprReturn& other) {
        if (this != &other) {
            isConst = other.isConst;
            type = other.type;
            if (isConst) {
                if (type == Type::FLOAT64_T) {
                    dvalue = other.dvalue;
                } else {
                    ivalue = other.ivalue;
                }
            }
        }
        return *this;
    }
} ExprReturn;

IRGenVisitor::IRGenVisitor(SymbolTableGenVisitor& p_stv) 
    : stv(p_stv), cfgs(), currentBB(nullptr), scope() {}

IRGenVisitor::~IRGenVisitor() {
    // Le CFG doit être libéré par le main
    // Car on le réutilise
}

antlrcpp::Any IRGenVisitor::visitFuncDecl(ifccParser::FuncDeclContext *ctx) {
    // Création d'un nouveau CFG pour chaque fonction
    scope = ctx->funcName->getText();
    CFG* cfg = new CFG(stv, scope);
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
        IRInstr *instruction_jump = new Jump(cfgs.back()->current_bb, cfgs.back()->current_bb->exit_true->label);
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
            std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);
            // std::cout << "# address " << address << "\n";

            // Évaluation de l'expression qu'on place dans le registre universel !reg
            ExprReturn* res(visit(ctx->decl_element(i)->expr()));
            if (res->isConst) {
                IRInstr *instruction_const = nullptr;
                if (res->type == Type::INT32_T) {
                    instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
                } else if (res->type == Type::FLOAT64_T) {
                    instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
                }
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            delete res;

            if (stv.varTable[nomVar].type == Type::INT32_T) {
                IRInstr *instruction = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
                cfgs.back()->current_bb->add_IRInstr(instruction);
            }
            else if (stv.varTable[nomVar].type == Type::FLOAT64_T) {
                IRInstr *instruction = new DWmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
                cfgs.back()->current_bb->add_IRInstr(instruction);
            }
        }
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.varTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    ExprReturn* res(visit(ctx->expr()));

    std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);

    if (res->isConst) {
        if (res->type == Type::INT32_T) {
            IRInstr* instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    delete res;
    
    if (stv.varTable[nomVar].type == Type::INT32_T) {
        IRInstr *instruction = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    else if (stv.varTable[nomVar].type == Type::FLOAT64_T) {
        IRInstr *instruction = new DWmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    ExprReturn* res(visit(ctx->expr()));
    if (res->isConst) {
        if (res->type == Type::INT32_T) {
            IRInstr* instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    delete res;
    
    cfgs.back()->current_bb->exit_true = cfgs.back()->end_block; // default exit
    cfgs.back()->current_bb->exit_false = nullptr;
    
    return 0;
}

antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    ExprReturn* res(visit(ctx->expr()));
    if (res->isConst) {
        if (res->type == Type::INT32_T) {
            IRInstr* instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    delete res;

    if (ctx->block().size() == 1) { // pas de else
        BasicBlock* bb_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_if_true");
        BasicBlock* bb_endif = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_endif");
        bb_endif->exit_true = cfgs.back()->current_bb->exit_true;
        cfgs.back()->current_bb->exit_true = bb_true;
        cfgs.back()->current_bb->exit_false = bb_endif;
        bb_true->exit_true = bb_endif;
        cfgs.back()->add_bb(bb_true);
        cfgs.back()->add_bb(bb_endif);

        IRInstr *instruction_jump = new JumpFalse(cfgs.back()->current_bb, bb_endif->label, bb_true->label, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
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

        IRInstr *instruction_jump = new JumpFalse(cfgs.back()->current_bb, bb_false->label, bb_true->label, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
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

    ExprReturn* res(visit(ctx->expr()));
    if (res->isConst) {
        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
    }
    delete res;

    BasicBlock* bb_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_while_true");
    BasicBlock* bb_endwhile = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_endwhile");
    bb_endwhile->exit_true = bb_test_while->exit_true;
    bb_test_while->exit_true = bb_true;
    bb_test_while->exit_false = bb_endwhile;
    bb_true->exit_true = bb_test_while;
    cfgs.back()->add_bb(bb_true);
    cfgs.back()->add_bb(bb_endwhile);

    IRInstr *instruction_jump = new JumpFalse(cfgs.back()->current_bb, bb_endwhile->label, bb_true->label, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction_jump);

    cfgs.back()->current_bb = bb_true;
    visit(ctx->block());
    cfgs.back()->current_bb = bb_endwhile;

    return 0;
}

antlrcpp::Any IRGenVisitor::visitDoubleExpr(ifccParser::DoubleExprContext *ctx)
{
    return new ExprReturn(true, Type::FLOAT64_T, std::stod(ctx->CONSTDOUBLE()->getText()));
}

antlrcpp::Any IRGenVisitor::visitIntExpr(ifccParser::IntExprContext *ctx)
{
    return new ExprReturn(true, Type::INT32_T, std::stoi(ctx->CONSTINT()->getText()));
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
    return new ExprReturn(true, Type::INT32_T, (int)interpretedChar);
}

antlrcpp::Any IRGenVisitor::visitIdUse(ifccParser::IdUseContext *ctx)
{
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.varTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }
    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();

    std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset); 

    IRInstr *instruction = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address);
    cfgs.back()->current_bb->add_IRInstr(instruction);

    return new ExprReturn(false, stv.varTable[nomVar].type, 0);
}

antlrcpp::Any IRGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    // On récupère le nom et l'adresse stack de la variable en question
    std::string tried_scope = scope;
    while (tried_scope != "" && stv.varTable.find(tried_scope + '_' + ctx->ID()->getText()) == stv.varTable.end()) {
        while (tried_scope.size() != 0 && tried_scope.back() != '_') {
            tried_scope.pop_back();
        }
        if (tried_scope.size() != 0) {
            tried_scope.pop_back();
        }
    }

    std::string nomVar = tried_scope + "_" + ctx->ID()->getText();
    ExprReturn* res(visit(ctx->expr()));

    std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);
    
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    if (res->isConst) {
        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);
    }
    IRInstr *instruction = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction);
    
    return res;
}


antlrcpp::Any IRGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    ExprReturn* res(visit(ctx->primary()));
    if (res->isConst) {
        res->ivalue = !res->ivalue;
        return res;
    }

    IRInstr *instruction_not = new Not(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction_not);
    
    res->isConst = false;
    res->type = Type::INT32_T;
    res->ivalue = 0;
    return res;
}

antlrcpp::Any IRGenVisitor::visitUnaryMinusExpr(ifccParser::UnaryMinusExprContext *ctx) {
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    ExprReturn* res(visit(ctx->primary()));
    if (res->isConst) {
        res->ivalue = -res->ivalue;
        return res;
    }

    IRInstr *instruction_unaryminus = new UnaryMinus(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction_unaryminus);
    
    res->isConst = false;
    res->type = Type::INT32_T;
    res->ivalue = 0;
    return res;
}

antlrcpp::Any IRGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    // Évaluation de l'expression gauche qu'on place dans le registre universel !reg
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            if (ctx->mOp()->STAR()) {
                res_left->ivalue *= res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->mOp()->SLASH()) {
                res_left->ivalue /= res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->mOp()->MOD()) {
                res_left->ivalue %= res_right->ivalue;
                delete res_right;
                return res_left;
            }
        }
        // Appliquer l'opération selon l'opérateur
        if (ctx->mOp()->STAR()) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            IRInstr *instruction_mul = new Mul(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_mul);
        }
        else if (ctx->mOp()->SLASH()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            IRInstr *instruction_div = new Div(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_div);
        }
        else if (ctx->mOp()->MOD()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            IRInstr *instruction_mod = new Mod(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_mod);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res_right;
        
        // Appliquer l'opération selon l'opérateur
        if (ctx->mOp()->STAR()) {
            IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            IRInstr *instruction_mul = new Mul(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_mul);
        }
        else if (ctx->mOp()->SLASH()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            IRInstr *instruction_div = new Div(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_div);
        }
        else if (ctx->mOp()->MOD()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            IRInstr *instruction_mod = new Mod(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_mod);
        }
    }
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    // Évaluation de l'expression gauche qu'on place dans le registre universel !reg
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            if (ctx->aOp()->PLUS()) {
                res_left->ivalue += res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->aOp()->MINUS()) {
                res_left->ivalue -= res_right->ivalue;
                delete res_right;
                return res_left;
            }
        }
        // Appliquer l'opération selon l'opérateur
        if (ctx->aOp()->PLUS()) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            IRInstr *instruction_add = new Add(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_add);
        }
        else if (ctx->aOp()->MINUS()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);

            IRInstr *instruction_sub = new Sub(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_sub);
        }
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        // Évaluation de l'expression right qu'on place dans le registre universel !reg
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res_right;

        // Appliquer l'opération selon l'opérateur
        if (ctx->aOp()->PLUS()) {
            IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            IRInstr *instruction_add = new Add(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_add);
        }
        else if (ctx->aOp()->MINUS()) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

            IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
            cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

            IRInstr *instruction_sub = new Sub(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_sub);
        }
    }    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            if (ctx->compOp()->LT()) {
                res_left->ivalue = res_left->ivalue < res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->compOp()->LE()) {
                res_left->ivalue = res_left->ivalue <= res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->compOp()->GE()) {
                res_left->ivalue = res_left->ivalue >= res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->compOp()->GT()) {
                res_left->ivalue = res_left->ivalue > res_right->ivalue;
                delete res_right;
                return res_left;
            }
        }
        // Appliquer l'opération selon l'opérateur
        IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->compOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }  
        else {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        delete res_right;

        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);

        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->compOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitEqExpr(ifccParser::EqExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            if (ctx->eqOp()->EQ()) {
                res_left->ivalue = res_left->ivalue == res_right->ivalue;
                delete res_right;
                return res_left;
            }
            else if (ctx->eqOp()->NEQ()) {
                res_left->ivalue = res_left->ivalue != res_right->ivalue;
                delete res_right;
                return res_left;
            }
        }
        // Appliquer l'opération selon l'opérateur
        IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);

        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->eqOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } 
        else {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        delete res_right;

        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->eqOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            res_left->ivalue &= res_right->ivalue;
            delete res_right;
            return res_left;
        }
        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        IRInstr *instruction_and = new And(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res_right;

        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        IRInstr *instruction_and = new And(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            res_left->ivalue ^= res_right->ivalue;
            delete res_right;
            return res_left;
        }
        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        IRInstr *instruction_and = new Xor(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res_right;

        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        IRInstr *instruction_and = new Xor(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            res_left->ivalue |= res_right->ivalue;
            delete res_right;
            return res_left;
        }
        IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
        cfgs.back()->current_bb->add_IRInstr(instruction_const);

        IRInstr *instruction_and = new Or(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }

    else {
        std::string temp_left = cfgs.back()->create_new_tempvar(Type::INT32_T);
        std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
        // Copier le résultat de l'expression dans la variable temporaire
        IRInstr *instruction_left = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_left);

        ExprReturn* res_right(visit(ctx->right));
        if (res_right->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res_right;

        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
        
        IRInstr *instruction_and = new Or(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_LEFT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_and);
    }
    
    res_left->isConst = false;
    res_left->type = Type::INT32_T;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitParExpr(ifccParser::ParExprContext *ctx) {
    ExprReturn* res(visit(ctx->expr()));
    return res;
}

antlrcpp::Any IRGenVisitor::visitFuncCall(ifccParser::FuncCallContext *ctx) {
    std::string nomFonction = ctx->ID()->getText();

    std::vector<std::string> args_temp_addr;
    
    for (int i = 0; i < ctx->expr().size(); i++) {
        std::string temp(cfgs.back()->create_new_tempvar(stv.funcTable[nomFonction].args[i]->type));
        args_temp_addr.push_back("RBP" + std::to_string(stv.varTable[temp].offset));
        ExprReturn* res(visit(ctx->expr(i)));
        if (res->isConst) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        delete res;

        IRInstr *instruction_temp = new Wmem(cfgs.back()->current_bb, args_temp_addr.back(), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_temp);
        // plus de 6 args ? faire une  et pop ?
    }

    // On appelle la fonction
    IRInstr *instruction_call = new Call(cfgs.back()->current_bb, nomFonction, args_temp_addr);
    cfgs.back()->current_bb->add_IRInstr(instruction_call);

    return new ExprReturn(false, stv.funcTable[nomFonction].type, 0);
}