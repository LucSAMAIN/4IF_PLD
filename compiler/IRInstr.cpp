#include "IRInstr.h"


// implémentation de Prologue
Prologue::Prologue(BasicBlock* p_bb)
    : IRInstr(p_bb)
{
}
std::string Prologue::get_operation_name() const {
    return "prologue";
}
void Prologue::gen_x86(std::ostream& o) {
    o << "    pushq %rbp # prologue\n";
    o << "    movq %rsp, %rbp" << "\n";
    
    // Allouer de l'espace pour les variables locales
    int frameSize = ((-bb->cfg->stv.funcTable[bb->cfg->functionName].offset + 15) & ~15);  // Alignement sur 16 octets uniquement
    if (frameSize > 0) {
        o << "    subq $" << frameSize << ", %rsp" << "\n";
    }

    // on sauvegarde les registres
    static const RegisterFunction index_to_reg[6] = { RegisterFunction::ARG0, RegisterFunction::ARG1, RegisterFunction::ARG2, RegisterFunction::ARG3, RegisterFunction::ARG4, RegisterFunction::ARG5 };
    int index_int = 0;
    int index_double = 0;
    for (VarInfos* p : bb->cfg->stv.funcTable[bb->cfg->functionName].args) {
        if (p->type == Type::INT32_T) {
            o << "    movl " << bb->cfg->IR_reg_to_x86(VirtualRegister(index_to_reg[index_int], RegisterSize::SIZE_32, RegisterType::GPR)) << ", " << bb->cfg->IR_addr_to_x86("RBP" + std::to_string(bb->cfg->stv.varTable[bb->cfg->stv.funcTable[bb->cfg->functionName].args[index_int+index_double]->name].offset)) << "\n";
            index_int++;
        }
        else if (p->type == Type::FLOAT64_T) {
            o << "    movsd " << bb->cfg->IR_reg_to_x86(VirtualRegister(index_to_reg[index_double], RegisterSize::SIZE_64, RegisterType::XMM)) << ", " << bb->cfg->IR_addr_to_x86("RBP" + std::to_string(bb->cfg->stv.varTable[bb->cfg->stv.funcTable[bb->cfg->functionName].args[index_int+index_double]->name].offset)) << "\n";
            index_double++;
        }
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
    o << "    movq %rbp, %rsp # epilogue\n";
    o << "    popq %rbp\n";
    o << "    ret\n";
}

// Implémentation de LdConst
LdConstInt::LdConstInt(BasicBlock* p_bb, const VirtualRegister& dest_reg, int val) 
    : IRInstr(p_bb), dest(dest_reg), value(val) {}
std::string LdConstInt::get_operation_name() const {
    return "ldconstint";
}
void LdConstInt::gen_x86(std::ostream& o) {
    o << "    movl $" << value << ", " << bb->cfg->IR_reg_to_x86(dest) << " # ldconstint\n";
}

// Implémentation de Copy
Copy::Copy(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {}
std::string Copy::get_operation_name() const {
    return "copy";
}
void Copy::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # copy\n";
}

// Implémentation de Add
Add::Add(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Add::get_operation_name() const {
    return "add";
}
void Add::gen_x86(std::ostream& o) {
    o << "    addl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # add\n";
}

// Implémentation de Sub
Sub::Sub(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Sub::get_operation_name() const {
    return "sub";
}
// sub %ebx, %eax ==> %eax = %eax - %ebx, donc on doit inverser op2 et op1
//  b-a s'écrit subl a, b
void Sub::gen_x86(std::ostream& o) {
    o << "    subl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # sub\n";
}

// UnaryMinus
UnaryMinus::UnaryMinus(BasicBlock* p_bb, const VirtualRegister& dest_reg) 
    : IRInstr(p_bb), dest(dest_reg) {}
std::string UnaryMinus::get_operation_name() const {
    return "unaryMinus";
}
void UnaryMinus::gen_x86(std::ostream& o) {
    o << "    neg " << bb->cfg->IR_reg_to_x86(dest) << " # unary minus\n";
}


Not::Not(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), op(src_reg) {}
std::string Not::get_operation_name() const {
    return "not";
}
void Not::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(op) << " # not\n";
    VirtualRegister byte_dest(dest.regFunc, RegisterSize::SIZE_8, dest.regType);
    o << "    sete " << bb->cfg->IR_reg_to_x86(byte_dest) << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(byte_dest) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

// Implémentation de Mul
Mul::Mul(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Mul::get_operation_name() const {
    return "mul";
}
void Mul::gen_x86(std::ostream& o) {
    o << "    imull " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # mul\n";
}

// Implémentation de Div
Div::Div(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Div::get_operation_name() const {
    return "div";
}
void Div::gen_x86(std::ostream& o) {
    o << "    cqo # div\n";
    o << "    idivl " << bb->cfg->IR_reg_to_x86(op2) << "\n";
    if (bb->cfg->IR_reg_to_x86(dest) != "%eax") {
        o << "    movl " << "%eax" << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    }
}

// Implémentation de Mod
Mod::Mod(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Mod::get_operation_name() const {
    return "mod";
}
void Mod::gen_x86(std::ostream& o) {
    o << "    cqo # mod\n";
    o << "    idivl " << bb->cfg->IR_reg_to_x86(op2) << "\n";
    if (bb->cfg->IR_reg_to_x86(dest) != "%edx") {
        o << "    movl " << "%edx, " << bb->cfg->IR_reg_to_x86(dest) << "\n";
    }
}

// Implémentation de Rmem
Rmem::Rmem(BasicBlock* p_bb, const VirtualRegister& dest_reg, const std::string& address) 
    : IRInstr(p_bb), dest(dest_reg), addr(address) {}
std::string Rmem::get_operation_name() const {
    return "rmem";
}
void Rmem::gen_x86(std::ostream& o) {
    // o << " # Rmem addr " << addr << "\n";
    o << "    movl " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # rmem\n";
}

// Implémentation de Wmem
Wmem::Wmem(BasicBlock* p_bb, const std::string& address, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), addr(address), src(src_reg) {}
std::string Wmem::get_operation_name() const {
    return "wmem";
}
void Wmem::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << " # wmem\n";
}

// Implémentation de Call
Call::Call(BasicBlock* p_bb, const std::string& function, const std::vector<std::string>& arguments) : IRInstr(p_bb), func_name(function), args(arguments) {}
std::string Call::get_operation_name() const {
    return "call";
}
void Call::gen_x86(std::ostream& o) {
    // on place les variables dans les registres
    static const RegisterFunction index_to_reg[6] = { RegisterFunction::ARG0, RegisterFunction::ARG1, RegisterFunction::ARG2, RegisterFunction::ARG3, RegisterFunction::ARG4, RegisterFunction::ARG5 };
    int index_int = 0;
    int index_double = 0;

    o << "    # call " << func_name << "\n";
    for (VarInfos* p : bb->cfg->stv.funcTable[func_name].args) {
        if (p->type == Type::INT32_T) {
            o << "    movl " << bb->cfg->IR_addr_to_x86(args[index_int+index_double]) << ", " << bb->cfg->IR_reg_to_x86(VirtualRegister(index_to_reg[index_int], RegisterSize::SIZE_32, RegisterType::GPR)) << "\n";
            index_int++;
        }
        else if (p->type == Type::FLOAT64_T) {
            o << "    movsd " << bb->cfg->IR_addr_to_x86(args[index_int+index_double]) << ", " << bb->cfg->IR_reg_to_x86(VirtualRegister(index_to_reg[index_double], RegisterSize::SIZE_64, RegisterType::XMM)) << "\n";
            index_double++;
        }
    }

    o << "    call " << func_name << "\n";
}

CompareInt::CompareInt(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp) 
    : IRInstr(p_bb), dest(dest_reg), left(p_left), right(p_right), comp(comp) {}

std::string CompareInt::get_operation_name() const {
    return "compare_int";
}
void CompareInt::gen_x86(std::ostream& o) {
    std::string leftName = bb->cfg->IR_reg_to_x86(left);
    std::string rightName = bb->cfg->IR_reg_to_x86(right);
    std::string destName = bb->cfg->IR_reg_to_x86(dest);

    o << "    cmp " << rightName << ", " << leftName << " # compare int\n";

    VirtualRegister byte_dest(dest.regFunc, RegisterSize::SIZE_8, dest.regType);
    std::string byte_dest_name = bb->cfg->IR_reg_to_x86(byte_dest);
    // Map operator to set instruction
    if (comp == ">") {
        o << "    setg " << byte_dest_name << "\n";   // Above: CF=0, ZF=0
    } else if (comp == "<") {
        o << "    setl " << byte_dest_name << "\n";   // Below: CF=1
    } else if (comp == "==") {
        o << "    sete " << byte_dest_name << "\n";   // Equal: ZF=1
    } else if (comp == "!=") {
        o << "    setne " << byte_dest_name << "\n";  // Not equal: ZF=0
    } else if (comp == ">=") {
        o << "    setge " << byte_dest_name << "\n";  // Above or equal: CF=0
    } else if (comp == "<=") {
        o << "    setle " << byte_dest_name << "\n";  // Below or equal: CF=1 or ZF=1
    }
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(byte_dest) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

And::And(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string And::get_operation_name() const {
    return "and";
}
void And::gen_x86(std::ostream& o) {
    o << "    andl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # and\n";
}

Or::Or(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Or::get_operation_name() const {
    return "or";
}
void Or::gen_x86(std::ostream& o) {
    o << "    orl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # or\n";
}

Xor::Xor(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string Xor::get_operation_name() const {
    return "xor";
}
void Xor::gen_x86(std::ostream& o) {
    o << "    xorl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # xor\n";
}

Jump::Jump(BasicBlock* p_bb, const std::string& p_dest_label) 
    : IRInstr(p_bb), dest_label(p_dest_label) {}
std::string Jump::get_operation_name() const {
    return "jump";
}
void Jump::gen_x86(std::ostream& o) {
    auto it = std::find(bb->cfg->bbs.begin(), bb->cfg->bbs.end(), bb);
    if (it+1 != bb->cfg->bbs.end() && (*(it+1))->label == dest_label) {
        // le prochain bloc est celui vers lequel on saute
        // donc pas besoin de jump
        o << "    # jmp " << dest_label << " # jump\n";
    }
    else {
        o << "    jmp " << dest_label << " # jump\n";
    }
}

JumpFalse::JumpFalse(BasicBlock* p_bb, const std::string& p_dest_false, const VirtualRegister& p_op) 
    : IRInstr(p_bb), dest_false(p_dest_false), op(p_op) {}
std::string JumpFalse::get_operation_name() const {
    return "jumpfalse";
}
void JumpFalse::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(op) << " # jump false\n";
    o << "    je " << dest_false << "\n";
}

Push::Push(BasicBlock* p_bb, const VirtualRegister& p_op) 
    : IRInstr(p_bb), op(p_op) {}
std::string Push::get_operation_name() const {
    return "push";
}
void Push::gen_x86(std::ostream& o) {
    o << "    push " << bb->cfg->IR_reg_to_x86(op) << " # push\n";
}

Pop::Pop(BasicBlock* p_bb, const VirtualRegister& p_dest)
    : IRInstr(p_bb), dest(p_dest) {}
std::string Pop::get_operation_name() const {
    return "pop";
}
void Pop::gen_x86(std::ostream& o) {
    o << "    pop " << bb->cfg->IR_reg_to_x86(dest) << " # pop\n";
}


// DOUBLE

LdConstDouble::LdConstDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, double val) 
    : IRInstr(p_bb), dest(dest_reg), value(val) {}
std::string LdConstDouble::get_operation_name() const {
    return "ldconstdouble";
}
void LdConstDouble::gen_x86(std::ostream& o) {
    uint64_t bits = *reinterpret_cast<uint64_t*>(&value);
    o << "    movq $0x" << std::hex << bits << std::dec << ", %rax # ldconstdouble " << value << "\n";
    o << "    movq %rax, " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

DAdd::DAdd(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string DAdd::get_operation_name() const {
    return "dadd";
}
void DAdd::gen_x86(std::ostream& o) {
    o << "    addsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dadd\n";
}

DSub::DSub(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string DSub::get_operation_name() const {
    return "dsub";
}
void DSub::gen_x86(std::ostream& o) {
    o << "    subsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dsub\n";
}

DMul::DMul(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string DMul::get_operation_name() const {
    return "dmul";
}
void DMul::gen_x86(std::ostream& o) {
    o << "    mulsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dmul\n";
}

DDiv::DDiv(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}
std::string DDiv::get_operation_name() const {
    return "ddiv";
}
void DDiv::gen_x86(std::ostream& o) {
    o << "    divsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # ddiv\n";
}

CompareDouble::CompareDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp) 
    : IRInstr(p_bb), dest(dest_reg), left(p_left), right(p_right), comp(comp) {}
std::string CompareDouble::get_operation_name() const {
    return "compare_double";
}
void CompareDouble::gen_x86(std::ostream& o) {
    std::string leftName = bb->cfg->IR_reg_to_x86(left);
    std::string rightName = bb->cfg->IR_reg_to_x86(right);
    std::string destName = bb->cfg->IR_reg_to_x86(dest);

    o << "    ucomisd " << rightName << ", " << leftName << " # compare double\n";

    VirtualRegister byte_dest(dest.regFunc, RegisterSize::SIZE_8, dest.regType);
    std::string byte_dest_name = bb->cfg->IR_reg_to_x86(byte_dest);
    // Map operator to set instruction
    if (comp == ">") {
        o << "    seta " << byte_dest_name << "\n";   // Above: CF=0, ZF=0
    } else if (comp == "<") {
        o << "    setb " << byte_dest_name << "\n";   // Below: CF=1
    } else if (comp == "==") {
        o << "    sete " << byte_dest_name << "\n";   // Equal: ZF=1
    } else if (comp == "!=") {
        o << "    setne " << byte_dest_name << "\n";  // Not equal: ZF=0
    } else if (comp == ">=") {
        o << "    setae " << byte_dest_name << "\n";  // Above or equal: CF=0
    } else if (comp == "<=") {
        o << "    setbe " << byte_dest_name << "\n";  // Below or equal: CF=1 or ZF=1
    }
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(byte_dest) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}


DWmem::DWmem(BasicBlock* p_bb, const std::string& address, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), addr(address), src(src_reg) {}
std::string DWmem::get_operation_name() const {
    return "dwmem";
}
void DWmem::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << " # dwmem\n";
}

DRmem::DRmem(BasicBlock* p_bb, const VirtualRegister& dest_reg, const std::string& address) 
    : IRInstr(p_bb), dest(dest_reg), addr(address) {}
std::string DRmem::get_operation_name() const {
    return "drmem";
}
void DRmem::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # drmem\n";
}

DCopy::DCopy(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {}
std::string DCopy::get_operation_name() const {
    return "copy";
}
void DCopy::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dcopy\n";
}

IntToDouble::IntToDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {}
std::string IntToDouble::get_operation_name() const {
    return "int_to_double";
}
void IntToDouble::gen_x86(std::ostream& o) {
    o << "    cvtsi2sd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # int2double\n";
}

DoubleToInt::DoubleToInt(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {}
std::string DoubleToInt::get_operation_name() const {
    return "double_to_int";
}
void DoubleToInt::gen_x86(std::ostream& o) {
    // cvtt pour truncation
    o << "    cvttsd2si " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # double2int\n";
}

DUnaryMinus::DUnaryMinus(BasicBlock* p_bb, const VirtualRegister& dest_reg) 
    : IRInstr(p_bb), dest(dest_reg) {}
std::string DUnaryMinus::get_operation_name() const {
    return "dunaryMinus";
}
void DUnaryMinus::gen_x86(std::ostream& o) {
    VirtualRegister mask(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM);
    if (dest.regFunc == RegisterFunction::REG_RIGHT) {
        mask.regFunc = RegisterFunction::REG_LEFT;
    }
    o << "    movq $0x8000000000000000, %rax # dunaryminus\n"; // on crée un masque et on fait un xor avec
    o << "    movq %rax, " << bb->cfg->IR_reg_to_x86(mask) << "\n";
    o << "    xorpd " << bb->cfg->IR_reg_to_x86(mask) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}


DNot::DNot(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), op(src_reg) {}
std::string DNot::get_operation_name() const {
    return "dnot";
}
void DNot::gen_x86(std::ostream& o) {
    VirtualRegister zero(RegisterFunction::REG_RIGHT, RegisterSize::SIZE_64, RegisterType::XMM);
    if (dest.regFunc == RegisterFunction::REG_RIGHT || op.regFunc == RegisterFunction::REG_RIGHT) {
        zero.regFunc = RegisterFunction::REG_LEFT;
        if (dest.regFunc == RegisterFunction::REG_LEFT || op.regFunc == RegisterFunction::REG_LEFT) {
            zero.regFunc = RegisterFunction::REG;
        }
    }
    o << "    xorpd " << bb->cfg->IR_reg_to_x86(zero) << ", " << bb->cfg->IR_reg_to_x86(zero) << " # dnot\n";
    o << "    ucomisd " << bb->cfg->IR_reg_to_x86(zero) << ", " << bb->cfg->IR_reg_to_x86(op) << "\n";
    VirtualRegister byte_dest(dest.regFunc, RegisterSize::SIZE_8, dest.regType);
    std::string byte_dest_name = bb->cfg->IR_reg_to_x86(byte_dest);
    o << "    sete " << byte_dest_name << "\n";
    o << "    movzbl " << bb->cfg->IR_reg_to_x86(byte_dest) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}