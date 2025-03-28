#include "IRInstr.h"
#include "IR.h"


// implémentation de Prologue
Prologue::Prologue(BasicBlock* p_bb)
    : IRInstr(p_bb)
{
}

std::string Prologue::get_operation_name() const {
    return "prologue";
}

void Prologue::gen_x86(std::ostream& o) {
    o << "    pushq %rbp" << "\n";
    o << "    movq %rsp, %rbp" << "\n";
    
    // Allouer de l'espace pour les variables locales
    int frameSize = ((-bb->cfg->stv.funcTable[bb->cfg->functionName].offset + 15) & ~15);  // Alignement sur 16 octets uniquement
    if (frameSize > 0) {
        o << "    subq $" << frameSize << ", %rsp" << "\n";
    }

    // on sauvegarde les registres
    for (int i = 0; i < std::min<int>(6, bb->cfg->stv.funcTable[bb->cfg->functionName].args.size()); i++) {
        o << "    movl " << bb->cfg->IR_reg_to_x86("!arg" + std::to_string(i) + "32") << ", " << bb->cfg->IR_addr_to_x86("RBP" + std::to_string(bb->cfg->stv.varTable[bb->cfg->stv.funcTable[bb->cfg->functionName].args[i]->name].offset)) << "\n";
    }

    // o << "    jmp " << bb->cfg->functionName << "_0\n";
    // on peut l'enlever car arrive tout le temps juste après
}

// implémentation de Epilogue
Epilogue::Epilogue(BasicBlock* p_bb)
    : IRInstr(p_bb)
{
}

std::string Epilogue::get_operation_name() const {
    return "epilogue";
}

void Epilogue::gen_x86(std::ostream& o) {
    o << "    movq %rbp, %rsp\n";
    o << "    popq %rbp\n";
    o << "    ret\n";
}

// Implémentation de LdConst
LdConst::LdConst(BasicBlock* p_bb, const std::string& dest_reg, int val) 
    : IRInstr(p_bb), dest(dest_reg), value(val) {}


std::string LdConst::get_operation_name() const {
    return "ldconst";
}

void LdConst::gen_x86(std::ostream& o) {
    o << "    movl $" << value << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Copy
Copy::Copy(BasicBlock* p_bb, const std::string& dest_reg, const std::string& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {}


std::string Copy::get_operation_name() const {
    return "copy";
}

void Copy::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Add
Add::Add(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Add::get_operation_name() const {
    return "add";
}

void Add::gen_x86(std::ostream& o) {
    o << "    addl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Sub
Sub::Sub(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Sub::get_operation_name() const {
    return "sub";
}

// sub %ebx, %eax ==> %eax = %eax - %ebx, donc on doit inverser op2 et op1
//  b-a s'écrit subl a, b
void Sub::gen_x86(std::ostream& o) {
    o << "    subl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// UnaryMinus
UnaryMinus::UnaryMinus(BasicBlock* p_bb, const std::string& dest_reg) 
    : IRInstr(p_bb), dest(dest_reg) {}


std::string UnaryMinus::get_operation_name() const {
    return "unaryMinus";
}

void UnaryMinus::gen_x86(std::ostream& o) {
    o << "    neg " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}


Not::Not(BasicBlock* p_bb, const std::string& dest_reg) 
    : IRInstr(p_bb), dest(dest_reg) {}


std::string Not::get_operation_name() const {
    return "not";
}

void Not::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    sete " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de Mul
Mul::Mul(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Mul::get_operation_name() const {
    return "mul";
}

void Mul::gen_x86(std::ostream& o) {
    o << "    imull " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Div
Div::Div(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Div::get_operation_name() const {
    return "div";
}

void Div::gen_x86(std::ostream& o) {
    o << "    cqo\n";
    o << "    idivl " << bb->cfg->IR_reg_to_x86(op2) << "\n";
    if (bb->cfg->IR_reg_to_x86(dest) != "%eax") {
        o << "    movl " << "%eax" << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    }
}

// Implémentation de Mod
Mod::Mod(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Mod::get_operation_name() const {
    return "mod";
}

void Mod::gen_x86(std::ostream& o) {
    o << "    cqo\n";
    o << "    idivl " << bb->cfg->IR_reg_to_x86(op2) << "\n";
    if (bb->cfg->IR_reg_to_x86(dest) != "%edx") {
        o << "    movl " << "%edx, " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    }
}

// Implémentation de Rmem
Rmem::Rmem(BasicBlock* p_bb, const std::string& dest_reg, const std::string& address) 
    : IRInstr(p_bb), dest(dest_reg), addr(address) {}


std::string Rmem::get_operation_name() const {
    return "rmem";
}

void Rmem::gen_x86(std::ostream& o) {
    // o << " # Rmem addr " << addr << "\n";
    o << "    movl " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Wmem
Wmem::Wmem(BasicBlock* p_bb, const std::string& address, const std::string& src_reg) 
    : IRInstr(p_bb), addr(address), src(src_reg) {}


std::string Wmem::get_operation_name() const {
    return "wmem";
}

void Wmem::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << "\n";
}

// Implémentation de Call
Call::Call(BasicBlock* p_bb, const std::string& function, const std::vector<std::string>& arguments) : IRInstr(p_bb), func_name(function), args(arguments) {}

std::string Call::get_operation_name() const {
    return "call";
}

void Call::gen_x86(std::ostream& o) {
    // on sauvegarde les registres de la fonction appelante
    // on pourrait améliorer en sachant combien de registres la fonction utilise
    // pour ne pas en sauvegarder des inutiles
    for (int i = 0; i < 6; i++) {
        o << "    push " << bb->cfg->IR_reg_to_x86("!arg"+std::to_string(i)+"64") << "\n";
    }

    // on place les variables dans les registres
    for (int i = 0; i < args.size(); i++) {
        o << "    movl " << bb->cfg->IR_addr_to_x86(args[i]) << ", " << bb->cfg->IR_reg_to_x86("!arg"+std::to_string(i)+"32") << "\n";
    }

    o << "    call " << func_name << "\n";

    // on remet les registres comme ils étaient
    for (int i = 5; i >= 0; i--) {
        o << "    pop " << bb->cfg->IR_reg_to_x86("!arg"+std::to_string(i)+"64") << "\n";
    }
}


// Implémentation de CmpEq
CmpEq::CmpEq(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpEq::get_operation_name() const {
    return "cmp_eq";
}

void CmpEq::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    sete " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de CmpLt
CmpNeq::CmpNeq(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpNeq::get_operation_name() const {
    return "cmp_neq";
}

void CmpNeq::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setne " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de CmpLe
CmpLe::CmpLe(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2)
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpLe::get_operation_name() const {
    return "cmp_le";
}

void CmpLe::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setle " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpLt::CmpLt(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpLt::get_operation_name() const {
    return "cmp_lt";
}

void CmpLt::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setl " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpGe::CmpGe(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpGe::get_operation_name() const {
    return "cmp_ge";
}

void CmpGe::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setge " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpGt::CmpGt(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}

std::string CmpGt::get_operation_name() const {
    return "cmp_gt";
}

void CmpGt::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setg " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

And::And(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string And::get_operation_name() const {
    return "and";
}

void And::gen_x86(std::ostream& o) {
    o << "    andl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

Or::Or(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Or::get_operation_name() const {
    return "or";
}

void Or::gen_x86(std::ostream& o) {
    o << "    orl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

Xor::Xor(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Xor::get_operation_name() const {
    return "xor";
}

void Xor::gen_x86(std::ostream& o) {
    o << "    xorl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

Jump::Jump(BasicBlock* p_bb, const std::string& p_dest_label) 
    : IRInstr(p_bb), dest_label(p_dest_label) {}


std::string Jump::get_operation_name() const {
    return "jump";
}

void Jump::gen_x86(std::ostream& o) {
    o << "    jmp " << dest_label << "\n";
}

JumpFalse::JumpFalse(BasicBlock* p_bb, const std::string& p_dest_false, const std::string& p_dest_true, const std::string& p_op) 
    : IRInstr(p_bb), dest_false(p_dest_false), dest_true(p_dest_true), op(p_op) {}


std::string JumpFalse::get_operation_name() const {
    return "jumpfalse";
}

void JumpFalse::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(op) << "\n";
    o << "    je " << dest_false << "\n";
    o << "    jmp " << dest_true << "\n";
}

Push::Push(BasicBlock* p_bb, const std::string& p_op) 
    : IRInstr(p_bb), op(p_op) {}


std::string Push::get_operation_name() const {
    return "push";
}

void Push::gen_x86(std::ostream& o) {
    o << "    push " << bb->cfg->IR_reg_to_x86(op) << "\n";
}

Pop::Pop(BasicBlock* p_bb, const std::string& p_dest) 
    : IRInstr(p_bb), dest(p_dest) {}


std::string Pop::get_operation_name() const {
    return "pop";
}

void Pop::gen_x86(std::ostream& o) {
    o << "    pop " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}