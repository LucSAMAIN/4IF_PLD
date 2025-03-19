#include "operation.h"
#include "IR.h" // Pour accéder à BasicBlock et CFG

// Implémentation de la classe Operation
Operation::Operation() {}

Operation::~Operation() {}

// implémentation de Prologue
Prologue::Prologue(BasicBlock* p_bb)
    : Operation(), bb(p_bb)
{
}

std::string Prologue::get_operation_name() const {
    return "prologue";
}

void Prologue::gen_x86(std::ostream& o) {
    o << "    pushq %rbp" << "\n";
    o << "    movq %rsp, %rbp" << "\n";
    
    // Allouer de l'espace pour les variables locales
    int frameSize = ((-bb->cfg->stv.offsetTable[bb->cfg->functionName] + 15) & ~15);  // Alignement sur 16 octets uniquement
    o << "    subq $" << frameSize << ", %rsp" << "\n";
}

// implémentation de Epilogue
Epilogue::Epilogue(BasicBlock* p_bb)
    : Operation(), bb(p_bb)
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
LdConst::LdConst(BasicBlock* bb, const std::string& dest_reg, int val) 
    : Operation(), dest(dest_reg), value(val), bb(bb) {}


std::string LdConst::get_operation_name() const {
    return "ldconst";
}

void LdConst::gen_x86(std::ostream& o) {
    o << "    movl $" << value << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Copy
Copy::Copy(BasicBlock* bb, const std::string& dest_reg, const std::string& src_reg) 
    : Operation(), dest(dest_reg), src(src_reg), bb(bb) {}


std::string Copy::get_operation_name() const {
    return "copy";
}

void Copy::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Add
Add::Add(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string Add::get_operation_name() const {
    return "add";
}

void Add::gen_x86(std::ostream& o) {
    o << "    addl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Sub
Sub::Sub(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string Sub::get_operation_name() const {
    return "sub";
}

// sub %ebx, %eax ==> %eax = %eax - %ebx, donc on doit inverser op2 et op1
//  b-a s'écrit subl a, b
void Sub::gen_x86(std::ostream& o) {
    o << "    subl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// UnaryMinus
UnaryMinus::UnaryMinus(BasicBlock* bb, const std::string& dest_reg) 
    : Operation(), dest(dest_reg), bb(bb) {}


std::string UnaryMinus::get_operation_name() const {
    return "unaryMinus";
}

void UnaryMinus::gen_x86(std::ostream& o) {
    o << "    neg " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}


Not::Not(BasicBlock* bb, const std::string& dest_reg) 
    : Operation(), dest(dest_reg), bb(bb) {}


std::string Not::get_operation_name() const {
    return "not";
}

void Not::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    sete " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de Mul
Mul::Mul(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string Mul::get_operation_name() const {
    return "mul";
}

void Mul::gen_x86(std::ostream& o) {
    o << "    imull " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Div
Div::Div(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


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
Mod::Mod(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


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
Rmem::Rmem(BasicBlock* bb, const std::string& dest_reg, const std::string& address) 
    : Operation(), dest(dest_reg), addr(address), bb(bb) {}


std::string Rmem::get_operation_name() const {
    return "rmem";
}

void Rmem::gen_x86(std::ostream& o) {
    // o << " # Rmem addr " << addr << "\n";
    o << "    movl " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Wmem
Wmem::Wmem(BasicBlock* bb, const std::string& address, const std::string& src_reg) 
    : Operation(), addr(address), src(src_reg), bb(bb) {}


std::string Wmem::get_operation_name() const {
    return "wmem";
}

void Wmem::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << "\n";
}

// // Implémentation de Call
// Call::Call(BasicBlock* bb, const std::string& function) : func_name(function), bb(bb) {}



// std::string Call::get_operation_name() const {
//     return "call";
// }

// void Call::gen_x86(std::ostream& o) {
//     o << "    CALL " << func_name << "\n";
// }


// Implémentation de CmpEq
CmpEq::CmpEq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpEq::get_operation_name() const {
    return "cmp_eq";
}

void CmpEq::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    sete " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de CmpLt
CmpNeq::CmpNeq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpNeq::get_operation_name() const {
    return "cmp_neq";
}

void CmpNeq::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setne " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

// Implémentation de CmpLe
CmpLe::CmpLe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2)
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpLe::get_operation_name() const {
    return "cmp_le";
}

void CmpLe::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setle " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpLt::CmpLt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpLt::get_operation_name() const {
    return "cmp_lt";
}

void CmpLt::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setl " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpGe::CmpGe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpGe::get_operation_name() const {
    return "cmp_ge";
}

void CmpGe::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setge " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

CmpGt::CmpGt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}

std::string CmpGt::get_operation_name() const {
    return "cmp_gt";
}

void CmpGt::gen_x86(std::ostream& o) {
    o << "    cmp " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    o << "    setg " << bb->cfg->IR_reg_to_x86(dest+"8") << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(dest+"8") << ", " << bb->cfg->IR_reg_to_x86(dest+"32") << "\n";
}

And::And(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string And::get_operation_name() const {
    return "and";
}

void And::gen_x86(std::ostream& o) {
    o << "    andl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

Or::Or(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string Or::get_operation_name() const {
    return "or";
}

void Or::gen_x86(std::ostream& o) {
    o << "    orl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

Xor::Xor(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2) 
    : Operation(), dest(dest_reg), op2(operand2), bb(bb) {}


std::string Xor::get_operation_name() const {
    return "xor";
}

void Xor::gen_x86(std::ostream& o) {
    o << "    xorl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}