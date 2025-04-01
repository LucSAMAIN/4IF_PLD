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
            if (ctx->stmt(i)->break_stmt() != nullptr) {
                break;
            }
            if (ctx->stmt(i)->continue_stmt() != nullptr) {
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
        if (ctx->decl_element(i)->decl_var()->expr()) // si déclaration + assignement direct
        {
            std::string nomVar = scope + "_" + ctx->decl_element(i)->decl_var()->ID()->getText();
            // std::cout << "# nomVar " << i << " : " << nomVar << "\n";
            std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);
            // std::cout << "# address " << address << "\n";

            // Évaluation de l'expression qu'on place dans le registre universel !reg
            ExprReturn* res(visit(ctx->decl_element(i)->decl_var()->expr()));
            if (res->isConst) {
                IRInstr *instruction_const = nullptr;
                if (res->type == Type::INT32_T) {
                    instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
                } else if (res->type == Type::FLOAT64_T) {
                    instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
                }
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }

            if (stv.varTable[nomVar].type == Type::INT32_T) {
                if (res->type == Type::FLOAT64_T) {
                    IRInstr *instruction = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
                    cfgs.back()->current_bb->add_IRInstr(instruction);
                }
                IRInstr *instruction = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
                cfgs.back()->current_bb->add_IRInstr(instruction);
            }
            else if (stv.varTable[nomVar].type == Type::FLOAT64_T) {
                if (res->type == Type::INT32_T) {
                    IRInstr *instruction = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
                    cfgs.back()->current_bb->add_IRInstr(instruction);
                }
                IRInstr *instruction = new DWmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
                cfgs.back()->current_bb->add_IRInstr(instruction);
            }
            delete res;
        }
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    std::string nomVar = visit(ctx->lValue()).as<std::string>();
    std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);
    ExprReturn* res(visit(ctx->value));


    if (res->isConst) {
        if (res->type == Type::INT32_T) {
            IRInstr* instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        } else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    
    if (stv.varTable[nomVar].type == Type::INT32_T) {
        if (res->type == Type::FLOAT64_T) {
            IRInstr *instruction = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction);
        }
        IRInstr *instruction = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    else if (stv.varTable[nomVar].type == Type::FLOAT64_T) {
        if (res->type == Type::INT32_T) {
            IRInstr *instruction = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction);
        }
        IRInstr *instruction = new DWmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    delete res;
    
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
    if (stv.funcTable[cfgs.back()->functionName].type == Type::INT32_T && res->type == Type::FLOAT64_T) {
        IRInstr *instruction = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    else if (stv.funcTable[cfgs.back()->functionName].type == Type::FLOAT64_T && res->type == Type::INT32_T) {
        IRInstr *instruction = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction);
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
        }
        else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    if (res->type == Type::FLOAT64_T) {
        IRInstr *instruction = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction);
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
        if (res->type == Type::INT32_T) {
            IRInstr* instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res->type == Type::FLOAT64_T) {
            IRInstr* instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    if (res->type == Type::FLOAT64_T) {
        IRInstr *instruction = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction);
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

antlrcpp::Any IRGenVisitor::visitContinue_stmt(ifccParser::Continue_stmtContext *ctx) {
    // on cherche le label du test de la boucle
    BasicBlock* bb_test_while = cfgs.back()->current_bb;
    while (bb_test_while->label.find("test_while") == std::string::npos) {
        if (bb_test_while->exit_false) {
            bb_test_while = bb_test_while->exit_false;
        }
        else {
            bb_test_while = bb_test_while->exit_true;
        }
    }
    cfgs.back()->current_bb->exit_true = bb_test_while; // on le met dans le bon bloc
    cfgs.back()->current_bb->exit_false = nullptr; // on le met à null pour pas qu'il aille ailleurs
    return 0;
}
antlrcpp::Any IRGenVisitor::visitBreak_stmt(ifccParser::Break_stmtContext *ctx) {
    // on cherche le label de fin de boucle
    BasicBlock* bb_test_while = cfgs.back()->current_bb;
    while (bb_test_while->label.find("endwhile") == std::string::npos) {
        if (bb_test_while->exit_false) {
            bb_test_while = bb_test_while->exit_false;
        }
        else {
            bb_test_while = bb_test_while->exit_true;
        }
    }
    cfgs.back()->current_bb->exit_true = bb_test_while; // on le met dans le bon bloc
    cfgs.back()->current_bb->exit_false = nullptr; // on le met à null pour pas qu'il aille ailleurs
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

    if (stv.varTable[nomVar].type == Type::INT32_T) {
        IRInstr *instruction = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address);
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    else if (stv.varTable[nomVar].type == Type::FLOAT64_T) {
        IRInstr *instruction = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address);
        cfgs.back()->current_bb->add_IRInstr(instruction);
    }
    

    return new ExprReturn(false, stv.varTable[nomVar].type, 0);
}

antlrcpp::Any IRGenVisitor::visitLIdUse(ifccParser::LIdUseContext *ctx) {
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

    return nomVar;
}

antlrcpp::Any IRGenVisitor::visitAssignExpr(ifccParser::AssignExprContext *ctx) {
    std::string nomVar = visit(ctx->lValue()).as<std::string>();
    std::string address = "RBP" + std::to_string(stv.varTable[nomVar].offset);
    ExprReturn* res(visit(ctx->value));
    
    // Évaluation de l'expression qu'on place dans le registre universel !reg
    if (res->isConst) {
        if (res->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    if (res->type == Type::INT32_T) {
        IRInstr *instruction_write = new Wmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_write);

        IRInstr *instruction_read = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address);
        cfgs.back()->current_bb->add_IRInstr(instruction_read);
    }
    else if (res->type == Type::FLOAT64_T) {
        IRInstr *instruction_write = new DWmem(cfgs.back()->current_bb, address, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_write);

        IRInstr *instruction_read = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address);
        cfgs.back()->current_bb->add_IRInstr(instruction_read);
    }
    delete res;
    
    return new ExprReturn(false, stv.varTable[nomVar].type, 0);
}

antlrcpp::Any IRGenVisitor::visitUnaryExpr(ifccParser::UnaryExprContext *ctx) {
    if (ctx->unaryOp()->MINUS()) {
        ExprReturn* res(visit(ctx->expr()));
        if (res->isConst) {
            if (res->type == Type::INT32_T) {
                res->ivalue = -res->ivalue;
            }
            else if (res->type == Type::FLOAT64_T) {
                res->dvalue = -res->dvalue;
            }
            return res;
        }

        if (res->type == Type::INT32_T) {
            IRInstr *instruction_unaryminus = new UnaryMinus(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_unaryminus);
        }
        else if (res->type == Type::FLOAT64_T) {
            IRInstr *instruction_unaryminus = new DUnaryMinus(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_unaryminus);
        }

        return res;
    }
    else if (ctx->unaryOp()->NOT()) {
        ExprReturn* res(visit(ctx->expr()));
        if (res->isConst) {
            if (res->type == Type::INT32_T) {
                res->ivalue = !res->ivalue;
            }
            else if (res->type == Type::FLOAT64_T) {
                res->dvalue = !res->dvalue;
            }
            return res;
        }

        if (res->type == Type::INT32_T) {
            IRInstr *instruction_not = new Not(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_not);
        }
        else if (res->type == Type::FLOAT64_T) {
            IRInstr *instruction_not = new DNot(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_not);

            res->type = Type::INT32_T;
        }

        return res;
    }

    return nullptr;
}

antlrcpp::Any IRGenVisitor::visitMulDivExpr(ifccParser::MulDivExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    std::string temp_left = cfgs.back()->create_new_tempvar(res_left->type);
    std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_write_mem = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_write_mem = new DWmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }

    ExprReturn* res_right(visit(ctx->right));
    Type operation_type = res_left->type == Type::FLOAT64_T || res_right->type == Type::FLOAT64_T ? Type::FLOAT64_T : Type::INT32_T;
    if (res_right->isConst) {
        if (res_left->isConst) { // is const expr
            cfgs.back()->current_bb->pop_IRInstr();
            cfgs.back()->current_bb->pop_IRInstr();
            if (operation_type == Type::INT32_T) {
                if (ctx->mOp()->STAR()) {
                    res_left->ivalue = res_left->ivalue * res_right->ivalue;
                }
                else if (ctx->mOp()->SLASH()) {
                    res_left->ivalue = res_left->ivalue / res_right->ivalue;
                }
                else if (ctx->mOp()->MOD()) {
                    res_left->ivalue = res_left->ivalue % res_right->ivalue;
                }
            }
            else if (operation_type == Type::FLOAT64_T) {
                if (res_left->type == Type::INT32_T) {
                    res_left->dvalue = res_left->ivalue;
                }
                if (res_right->type == Type::INT32_T) {
                    res_right->dvalue = res_right->ivalue;
                }

                if (ctx->mOp()->STAR()) {
                    res_left->dvalue = res_left->dvalue * res_right->dvalue;
                }
                else if (ctx->mOp()->SLASH()) {
                    res_left->dvalue = res_left->dvalue / res_right->dvalue;
                }
            }
            delete res_right;
            res_left->type = operation_type;
            return res_left;
        }
        else { // left is not const expr so we just put the const right
            if (res_right->type == Type::INT32_T) {
                IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            else if (res_right->type == Type::FLOAT64_T) {
                IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), res_right->dvalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
        }
    }
    else { // right is not const, we have its value in reg, we copy in reg right
        if (res_right->type == Type::INT32_T) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        else if (res_right->type == Type::FLOAT64_T) {
            IRInstr *instruction_copy_right = new DCopy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
    }
    // we put back left in reg
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_read_left = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }

    // if we have to, we promote the int to double
    if (res_left->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }
    else if (res_right->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }

    // Appliquer l'opération selon l'opérateur
    if (operation_type == Type::INT32_T) {
        IRInstr *instruction_comp = nullptr;
        if (ctx->mOp()->STAR()) {
            instruction_comp = new Mul(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        }
        else if (ctx->mOp()->SLASH()) {
            instruction_comp = new Div(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        }
        else if (ctx->mOp()->MOD()) {
            instruction_comp = new Mod(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        }
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    else if (operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_comp = nullptr;
        if (ctx->mOp()->STAR()) {
            instruction_comp = new DMul(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM));
        }
        else if (ctx->mOp()->SLASH()) {
            instruction_comp = new DDiv(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM));
        }
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }

    res_left->isConst = false;
    res_left->type = operation_type;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    std::string temp_left = cfgs.back()->create_new_tempvar(res_left->type);
    std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_write_mem = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_write_mem = new DWmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }

    ExprReturn* res_right(visit(ctx->right));
    Type operation_type = res_left->type == Type::FLOAT64_T || res_right->type == Type::FLOAT64_T ? Type::FLOAT64_T : Type::INT32_T;
    if (res_right->isConst) {
        if (res_left->isConst) { // is const expr
            cfgs.back()->current_bb->pop_IRInstr();
            cfgs.back()->current_bb->pop_IRInstr();
            if (operation_type == Type::INT32_T) {
                if (ctx->aOp()->PLUS()) {
                    res_left->ivalue = res_left->ivalue + res_right->ivalue;
                }
                else if (ctx->aOp()->MINUS()) {
                    res_left->ivalue = res_left->ivalue - res_right->ivalue;
                }
            }
            else if (operation_type == Type::FLOAT64_T) {
                if (res_left->type == Type::INT32_T) {
                    res_left->dvalue = res_left->ivalue;
                }
                if (res_right->type == Type::INT32_T) {
                    res_right->dvalue = res_right->ivalue;
                }

                if (ctx->aOp()->PLUS()) {
                    res_left->dvalue = res_left->dvalue + res_right->dvalue;
                }
                else if (ctx->aOp()->MINUS()) {
                    res_left->dvalue = res_left->dvalue - res_right->dvalue;
                }
            }
            delete res_right;
            res_left->type = operation_type;
            return res_left;
        }
        else { // left is not const expr so we just put the const right
            if (res_right->type == Type::INT32_T) {
                IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            else if (res_right->type == Type::FLOAT64_T) {
                IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), res_right->dvalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
        }
    }
    else { // right is not const, we have its value in reg, we copy in reg right
        if (res_right->type == Type::INT32_T) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        else if (res_right->type == Type::FLOAT64_T) {
            IRInstr *instruction_copy_right = new DCopy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
    }
    // we put back left in reg
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_read_left = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }

    // if we have to, we promote the int to double
    if (res_left->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }
    else if (res_right->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }

    // Appliquer l'opération selon l'opérateur
    if (operation_type == Type::INT32_T) {
        IRInstr *instruction_comp = nullptr;
        if (ctx->aOp()->PLUS()) {
            instruction_comp = new Add(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        }
        else if (ctx->aOp()->MINUS()) {
            instruction_comp = new Sub(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        }
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    else if (operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_comp = nullptr;
        if (ctx->aOp()->PLUS()) {
            instruction_comp = new DAdd(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM));
        }
        else if (ctx->aOp()->MINUS()) {
            instruction_comp = new DSub(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM));
        }
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }

    res_left->isConst = false;
    res_left->type = operation_type;
    res_left->ivalue = 0;
    return res_left;
}

antlrcpp::Any IRGenVisitor::visitCompExpr(ifccParser::CompExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    std::string temp_left = cfgs.back()->create_new_tempvar(res_left->type);
    std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_write_mem = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_write_mem = new DWmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }

    ExprReturn* res_right(visit(ctx->right));
    Type operation_type = res_left->type == Type::FLOAT64_T || res_right->type == Type::FLOAT64_T ? Type::FLOAT64_T : Type::INT32_T;
    if (res_right->isConst) {
        if (res_left->isConst) { // is const expr
            cfgs.back()->current_bb->pop_IRInstr();
            cfgs.back()->current_bb->pop_IRInstr();
            if (operation_type == Type::INT32_T) {
                if (ctx->compOp()->LT()) {
                    res_left->ivalue = res_left->ivalue < res_right->ivalue;
                }
                else if (ctx->compOp()->LE()) {
                    res_left->ivalue = res_left->ivalue <= res_right->ivalue;
                }
                else if (ctx->compOp()->GE()) {
                    res_left->ivalue = res_left->ivalue >= res_right->ivalue;
                }
                else if (ctx->compOp()->GT()) {
                    res_left->ivalue = res_left->ivalue > res_right->ivalue;
                }
            }
            else if (operation_type == Type::FLOAT64_T) {
                if (res_left->type == Type::INT32_T) {
                    res_left->dvalue = res_left->ivalue;
                }
                if (res_right->type == Type::INT32_T) {
                    res_right->dvalue = res_right->ivalue;
                }

                if (ctx->compOp()->LT()) {
                    res_left->dvalue = res_left->dvalue < res_right->dvalue;
                }
                else if (ctx->compOp()->LE()) {
                    res_left->dvalue = res_left->dvalue <= res_right->dvalue;
                }
                else if (ctx->compOp()->GE()) {
                    res_left->dvalue = res_left->dvalue >= res_right->dvalue;
                }
                else if (ctx->compOp()->GT()) {
                    res_left->dvalue = res_left->dvalue > res_right->dvalue;
                }
            }
            delete res_right;
            res_left->type = Type::INT32_T;
            return res_left;
        }
        else { // left is not const expr so we just put the const right
            if (res_right->type == Type::INT32_T) {
                IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            else if (res_right->type == Type::FLOAT64_T) {
                IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), res_right->dvalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
        }
    }
    else { // right is not const, we have its value in reg, we copy in reg right
        if (res_right->type == Type::INT32_T) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        else if (res_right->type == Type::FLOAT64_T) {
            IRInstr *instruction_copy_right = new DCopy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
    }
    // we put back left in reg
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_read_left = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }

    // if we have to, we promote the int to double
    if (res_left->type == Type::INT32_T && operation_type== Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }
    else if (res_right->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }

    // Appliquer l'opération selon l'opérateur
    if (operation_type == Type::INT32_T) {
        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->compOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    else if (operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_comp = new CompareDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), ctx->compOp()->getText());
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
        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    std::string temp_left = cfgs.back()->create_new_tempvar(res_left->type);
    std::string address_left = "RBP" + std::to_string(stv.varTable[temp_left].offset);
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_write_mem = new Wmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_write_mem = new DWmem(cfgs.back()->current_bb, address_left, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
    }

    ExprReturn* res_right(visit(ctx->right));
    Type operation_type = res_left->type == Type::FLOAT64_T || res_right->type == Type::FLOAT64_T ? Type::FLOAT64_T : Type::INT32_T;
    if (res_right->isConst) {
        if (res_left->isConst) { // is const expr
            cfgs.back()->current_bb->pop_IRInstr();
            cfgs.back()->current_bb->pop_IRInstr();
            if (operation_type == Type::INT32_T) {
                if (ctx->eqOp()->EQ()) {
                    res_left->ivalue = res_left->ivalue == res_right->ivalue;
                }
                else if (ctx->eqOp()->NEQ()) {
                    res_left->ivalue = res_left->ivalue != res_right->ivalue;
                }
            }
            else if (operation_type == Type::FLOAT64_T) {
                if (res_left->type == Type::INT32_T) { // promote
                    res_left->dvalue = res_left->ivalue;
                }
                if (res_right->type == Type::INT32_T) { // promote
                    res_right->dvalue = res_right->ivalue;
                }

                if (ctx->eqOp()->EQ()) {
                    res_left->dvalue = res_left->dvalue == res_right->dvalue;
                }
                else if (ctx->eqOp()->NEQ()) {
                    res_left->dvalue = res_left->dvalue != res_right->dvalue;
                }
            }
            delete res_right;
            res_left->type = Type::INT32_T;
            return res_left;
        }
        else { // left is not const expr so we just put the const right
            if (res_right->type == Type::INT32_T) {
                IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), res_right->ivalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            else if (res_right->type == Type::FLOAT64_T) {
                IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), res_right->dvalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
        }
    }
    else { // right is not const, we have its value in reg, we copy in reg right
        if (res_right->type == Type::INT32_T) {
            IRInstr *instruction_copy_right = new Copy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
        else if (res_right->type == Type::FLOAT64_T) {
            IRInstr *instruction_copy_right = new DCopy(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_copy_right);
        }
    }
    // we put back left in reg
    if (res_left->type == Type::INT32_T) {
        IRInstr *instruction_read_left = new Rmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }
    else if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_read_left = new DRmem(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), address_left);
        cfgs.back()->current_bb->add_IRInstr(instruction_read_left);
    }

    // if we have to, we promote the int to double
    if (res_left->type == Type::INT32_T && operation_type== Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }
    else if (res_right->type == Type::INT32_T && operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR));
        cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
    }

    // Appliquer l'opération selon l'opérateur
    if (operation_type == Type::INT32_T) {
        IRInstr *instruction_comp = new CompareInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_32, RegisterType::GPR), ctx->eqOp()->getText());
        cfgs.back()->current_bb->add_IRInstr(instruction_comp);
    }
    else if (operation_type == Type::FLOAT64_T) {
        IRInstr *instruction_comp = new CompareDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM), ctx->eqOp()->getText());
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

antlrcpp::Any IRGenVisitor::visitLogAndExpr(ifccParser::LogAndExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        if (res_left->type == Type::INT32_T && res_left->ivalue == 0) {
            return res_left;
        }
        else if (res_left->type == Type::FLOAT64_T && res_left->dvalue == 0.0) {
            res_left->ivalue = 0;
            res_left->type = Type::INT32_T;
            return res_left;
        }

        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_double_to_int = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_double_to_int);
    }

    BasicBlock* bb_expr_true = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_expr_true");
    cfgs.back()->add_bb(bb_expr_true);
    BasicBlock* bb_expr_end = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_expr_end");
    cfgs.back()->add_bb(bb_expr_end);
    bb_expr_end->exit_true = cfgs.back()->current_bb->exit_true;
    cfgs.back()->current_bb->exit_true = bb_expr_true;
    cfgs.back()->current_bb->exit_false = bb_expr_end;
    bb_expr_true->exit_true = bb_expr_end;

    IRInstr *instruction_jump = new JumpFalse(cfgs.back()->current_bb, bb_expr_end->label, bb_expr_true->label, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction_jump);

    cfgs.back()->current_bb = bb_expr_true;

    ExprReturn* res_right(visit(ctx->right));
    if (res_right->isConst) {
        delete res_left;
        cfgs.back()->current_bb = bb_expr_end;
        if (res_right->type == Type::INT32_T) {
            return res_right;
        }
        else if (res_right->type == Type::FLOAT64_T) {
            res_right->ivalue = res_right->dvalue;
            res_right->type = Type::INT32_T;
            return res_right;
        }
    }

    if (res_right->type == Type::FLOAT64_T) {
        IRInstr *instruction_double_to_int = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_double_to_int);
    }

    cfgs.back()->current_bb = bb_expr_end;

    res_right->isConst = false;
    res_right->type = Type::INT32_T;
    res_right->ivalue = 0;
    return res_right;
}

antlrcpp::Any IRGenVisitor::visitLogOrExpr(ifccParser::LogOrExprContext *ctx) {
    ExprReturn* res_left(visit(ctx->left));
    if (res_left->isConst) {
        if (res_left->type == Type::INT32_T && res_left->ivalue != 0) {
            return res_left;
        }
        else if (res_left->type == Type::FLOAT64_T && res_left->dvalue != 0.0) {
            res_left->ivalue = res_left->dvalue;
            res_left->type = Type::INT32_T;
            return res_left;
        }

        if (res_left->type == Type::INT32_T) {
            IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res_left->ivalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
        else if (res_left->type == Type::FLOAT64_T) {
            IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res_left->dvalue);
            cfgs.back()->current_bb->add_IRInstr(instruction_const);
        }
    }
    if (res_left->type == Type::FLOAT64_T) {
        IRInstr *instruction_double_to_int = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_double_to_int);
    }

    BasicBlock* bb_expr_false = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_expr_false");
    cfgs.back()->add_bb(bb_expr_false);
    BasicBlock* bb_expr_end = new BasicBlock(cfgs.back(), cfgs.back()->new_BB_name() + "_expr_end");
    cfgs.back()->add_bb(bb_expr_end);
    bb_expr_end->exit_true = cfgs.back()->current_bb->exit_true;
    cfgs.back()->current_bb->exit_true = bb_expr_end;
    cfgs.back()->current_bb->exit_false = bb_expr_false;
    bb_expr_false->exit_true = bb_expr_end;

    IRInstr *instruction_jump = new JumpFalse(cfgs.back()->current_bb, bb_expr_false->label, bb_expr_end->label, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
    cfgs.back()->current_bb->add_IRInstr(instruction_jump);

    cfgs.back()->current_bb = bb_expr_false;

    ExprReturn* res_right(visit(ctx->right));
    if (res_right->isConst) {
        delete res_left;
        cfgs.back()->current_bb = bb_expr_end;
        if (res_right->type == Type::INT32_T) {
            return res_right;
        }
        else if (res_right->type == Type::FLOAT64_T) {
            res_right->ivalue = res_right->dvalue;
            res_right->type = Type::INT32_T;
            return res_right;
        }
    }

    if (res_right->type == Type::FLOAT64_T) {
        IRInstr *instruction_double_to_int = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
        cfgs.back()->current_bb->add_IRInstr(instruction_double_to_int);
    }

    cfgs.back()->current_bb = bb_expr_end;

    res_right->isConst = false;
    res_right->type = Type::INT32_T;
    res_right->ivalue = 0;
    return res_right;
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
            if (res->type == Type::INT32_T) {
                IRInstr *instruction_const = new LdConstInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), res->ivalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
            else if (res->type == Type::FLOAT64_T) {
                IRInstr *instruction_const = new LdConstDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), res->dvalue);
                cfgs.back()->current_bb->add_IRInstr(instruction_const);
            }
        }

        if (stv.funcTable[nomFonction].args[i]->type == Type::INT32_T) {
            if (res->type == Type::FLOAT64_T) {
                IRInstr *instruction_double_to_int = new DoubleToInt(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
                cfgs.back()->current_bb->add_IRInstr(instruction_double_to_int);
            }
            IRInstr *instruction_write_mem = new Wmem(cfgs.back()->current_bb, args_temp_addr.back(), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
            cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
        }
        else if (stv.funcTable[nomFonction].args[i]->type == Type::FLOAT64_T) {
            if (res->type == Type::INT32_T) {
                IRInstr *instruction_int_to_double = new IntToDouble(cfgs.back()->current_bb, VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_32, RegisterType::GPR));
                cfgs.back()->current_bb->add_IRInstr(instruction_int_to_double);
            }
            IRInstr *instruction_write_mem = new DWmem(cfgs.back()->current_bb, args_temp_addr.back(), VirtualRegister(RegisterFunction::REG, RegisterSize::SIZE_64, RegisterType::XMM));
            cfgs.back()->current_bb->add_IRInstr(instruction_write_mem);
        }

        delete res;
        // plus de 6 args ? faire une  et pop ?
    }

    // On appelle la fonction
    IRInstr *instruction_call = new Call(cfgs.back()->current_bb, nomFonction, args_temp_addr);
    cfgs.back()->current_bb->add_IRInstr(instruction_call);

    return new ExprReturn(false, stv.funcTable[nomFonction].type, 0);
}