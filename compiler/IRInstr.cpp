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

void Prologue::gen_wat(std::ostream& o) {
    o << "    ;; Prologue\n";
    o << "    (local.set $bp (global.get $sp))\n";
    int frameSize = ((-bb->cfg->stv.funcTable[bb->cfg->functionName].offset + 15) & ~15);
    frameSize = std::max(16, frameSize); // Au moins 16 octets pour le cadre de pile
    o << "    (global.set $sp (i32.sub (global.get $sp) (i32.const " << frameSize << ")))\n";

    // Copier les arguments des paramètres locaux vers la pile
    o << "    ;; Copie des arguments vers la pile\n";
    static const RegisterFunction index_to_regFunc[6] = { 
        RegisterFunction::ARG0, RegisterFunction::ARG1, RegisterFunction::ARG2, 
        RegisterFunction::ARG3, RegisterFunction::ARG4, RegisterFunction::ARG5 
    };
    int arg_idx = 0; // Index global pour les arguments
    for (VarInfos* argInfo : bb->cfg->stv.funcTable[bb->cfg->functionName].args) {
        if (arg_idx >= 6) break; // Limité aux 6 premiers arguments pour l'instant

        std::string stack_addr_wat = bb->cfg->IR_addr_to_wat("RBP" + std::to_string(argInfo->offset));
        
        if (argInfo->type == Type::INT32_T || argInfo->type == Type::INT8_T) {
             // Les types GPR plus petits sont traités comme i32 pour les params WAT
            VirtualRegister argReg(index_to_regFunc[arg_idx], RegisterSize::SIZE_32, RegisterType::GPR);
            std::string param_name = bb->cfg->IR_reg_to_wat(argReg);
            o << "    (i32.store " << stack_addr_wat << " (local.get " << param_name << ")) ;; Store arg " << arg_idx << " (" << argInfo->name << ")\n";
        } else if (argInfo->type == Type::FLOAT64_T) {
             // Les types XMM sont f64
            VirtualRegister argReg(index_to_regFunc[arg_idx], RegisterSize::SIZE_64, RegisterType::XMM);
             std::string param_name = bb->cfg->IR_reg_to_wat(argReg);
            o << "    (f64.store " << stack_addr_wat << " (local.get " << param_name << ")) ;; Store arg " << arg_idx << " (" << argInfo->name << ")\n";
        }
        // Ajoutez d'autres types si nécessaire (ex: INT64_T -> i64.store)
        
        arg_idx++;
    }
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

void Epilogue::gen_wat(std::ostream& o) {
    o << "    ;; Epilogue\n";
    o << "    (global.set $sp (local.get $bp))\n";
    if (bb->cfg->stv.funcTable[bb->cfg->functionName].type == Type::INT32_T) {
        o << "    (return (local.get $reg_32))\n"; // Retourne explicitement la valeur
    } else if (bb->cfg->stv.funcTable[bb->cfg->functionName].type == Type::FLOAT64_T) {
        o << "    (return (local.get $reg_64))\n"; // Retourne explicitement la valeur
    }
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

void LdConstInt::gen_wat(std::ostream& o) {
    o << "    ;; Load integer constant\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.const " << value << "))\n";
}

LdConstDouble::LdConstDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, double val)
    : IRInstr(p_bb), dest(dest_reg), value(val) {}


std::string LdConstDouble::get_operation_name() const {
    return "ldconstdouble";
}

void LdConstDouble::gen_x86(std::ostream& o) {
    uint64_t bits = *reinterpret_cast<uint64_t*>(&value);
    o << "    movq $0x" << std::hex << bits << std::dec << ", %rax # ldconstdouble " << value << "\n";
    o << "    movq %rax, " << bb->cfg->IR_reg_to_x86(dest) << " # ldconstint\n";
}

void LdConstDouble::gen_wat(std::ostream& o) {
    o << "    ;; Load double constant\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.const " << value << "))\n";
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

void Copy::gen_wat(std::ostream& o) {
    o << "    ;; Copy\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
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

void Add::gen_wat(std::ostream& o) {
    o << "    ;; Addition\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.add (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Sub::gen_wat(std::ostream& o) {
    o << "    ;; Subtraction\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.sub (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void UnaryMinus::gen_wat(std::ostream& o) {
    o << "    ;; Unary minus\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.sub (i32.const 0) (local.get " << bb->cfg->IR_reg_to_wat(dest) << ")))\n";
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

void Not::gen_wat(std::ostream& o) {
    o << "    ;; Logical not\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(dest) << ")))\n";
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

void Mul::gen_wat(std::ostream& o) {
    o << "    ;; Multiplication\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.mul (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Div::gen_wat(std::ostream& o) {
    o << "    ;; Division\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.div_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Mod::gen_wat(std::ostream& o) {
    o << "    ;; Modulo\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.rem_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

// Implémentation de Rmem
Rmem::Rmem(BasicBlock* p_bb, const VirtualRegister& dest_reg, const std::string& address) 
    : IRInstr(p_bb), dest(dest_reg), addr(address) {};

std::string Rmem::get_operation_name() const {
    return "rmem";
}
void Rmem::gen_x86(std::ostream& o) {
    // o << " # Rmem addr " << addr << "\n";
    o << "    movl " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # rmem\n";
}

void Rmem::gen_wat(std::ostream& o) {
    o << "    ;; Read memory\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.load " << bb->cfg->IR_addr_to_wat(addr) << "))\n";
}

// Implémentation de Wmem
Wmem::Wmem(BasicBlock* p_bb, const std::string& address, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), addr(address), src(src_reg) {};

std::string Wmem::get_operation_name() const {
    return "wmem";
}
void Wmem::gen_x86(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << " # wmem\n";
}

void Wmem::gen_wat(std::ostream& o) {
    o << "    ;; Write memory\n";
    o << "    (i32.store " << bb->cfg->IR_addr_to_wat(addr) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
}

// Implémentation de Call
Call::Call(BasicBlock* p_bb, const std::string& function, const std::vector<std::string>& arguments) 
    : IRInstr(p_bb), func_name(function), args(arguments) {};
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

void Call::gen_wat(std::ostream& o) {
    o << "    ;; Function call\n";
    // Passer les arguments et stocker la valeur de retour
    if (bb->cfg->stv.funcTable[func_name].type == Type::INT32_T) {
        o << "    (local.set $reg_32 (call $" << func_name;
    } else if (bb->cfg->stv.funcTable[func_name].type == Type::FLOAT64_T) {
        o << "    (local.set $reg_64 (call $" << func_name;
    } else if (bb->cfg->stv.funcTable[func_name].type == Type::VOID) {
        o << "    (call $" << func_name;
    }
    
    // Récupérer le nombre d'arguments attendus par la fonction appelée (max 6)
    int numArgs = std::min(6, static_cast<int>(bb->cfg->stv.funcTable[func_name].args.size()));
    
    // Passer les arguments depuis les registres d'arguments
    for (int i = 0; i < numArgs; i++) {
        if (bb->cfg->stv.funcTable[func_name].args[i]->type == Type::INT32_T) {
            o << "\n        (i32.load " << bb->cfg->IR_addr_to_wat(args[i]) << ")";
        } else if (bb->cfg->stv.funcTable[func_name].args[i]->type == Type::FLOAT64_T) {
            o << "\n        (f64.load " << bb->cfg->IR_addr_to_wat(args[i]) << ")";
        }
    }
    if (bb->cfg->stv.funcTable[func_name].type == Type::INT32_T) {
        o << "))\n";
    } else if (bb->cfg->stv.funcTable[func_name].type == Type::FLOAT64_T) {
        o << "))\n";
    } else if (bb->cfg->stv.funcTable[func_name].type == Type::VOID) {
        o << ")\n";
    }
}

CompareInt::CompareInt(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp) 
    : IRInstr(p_bb), dest(dest_reg), left(p_left), right(p_right), comp(comp) {};

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

void CompareInt::gen_wat(std::ostream& o) {
    std::string leftWat = bb->cfg->IR_reg_to_wat(left);
    std::string rightWat = bb->cfg->IR_reg_to_wat(right);
    std::string destWat = bb->cfg->IR_reg_to_wat(dest);

    o << "    ;; Compare int (" << comp << ")\n";
    // Charger les opérandes sur la pile
    o << "    (local.get " << leftWat << ")\n";
    o << "    (local.get " << rightWat << ")\n";

    // Choisir l'instruction de comparaison WAT appropriée
    // (en supposant une comparaison signée pour >, <, >=, <= comme dans gen_x86)
    if (comp == ">") {
        o << "    (i32.gt_s)\n";
    } else if (comp == "<") {
        o << "    (i32.lt_s)\n";
    } else if (comp == "==") {
        o << "    (i32.eq)\n";
    } else if (comp == "!=") {
        o << "    (i32.ne)\n";
    } else if (comp == ">=") {
        o << "    (i32.ge_s)\n";
    } else if (comp == "<=") {
        o << "    (i32.le_s)\n";
    } else {
        // Gérer une erreur si l'opérateur n'est pas reconnu
        o << "    (unreachable) ;; Error: Unsupported comparison operator: " << comp << "\n";
    }

    // Stocker le résultat (0 ou 1) de la pile dans la destination
    o << "    (local.set " << destWat << ")\n";
}

And::And(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string And::get_operation_name() const {
    return "and";
}
void And::gen_x86(std::ostream& o) {
    o << "    andl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # and\n";
}

void And::gen_wat(std::ostream& o) {
    o << "    ;; Logical and\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.and (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Or::Or(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string Or::get_operation_name() const {
    return "or";
}
void Or::gen_x86(std::ostream& o) {
    o << "    orl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # or\n";
}

void Or::gen_wat(std::ostream& o) {
    o << "    ;; Logical or\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.or (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Xor::Xor(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string Xor::get_operation_name() const {
    return "xor";
}
void Xor::gen_x86(std::ostream& o) {
    o << "    xorl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # xor\n";
}

void Xor::gen_wat(std::ostream& o) {
    o << "    ;; Logical xor\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.xor (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Jump::Jump(BasicBlock* p_bb, const std::string& p_dest_label) 
    : IRInstr(p_bb), dest_label(p_dest_label) {};
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

void Jump::gen_wat(std::ostream& o) {
    o << "    ;; Jump\n";
    if (dest_label == bb->cfg->functionName + "_epilogue") {
        // Nom du bloc d'épilogue spécifique à la fonction
        std::string epilogueBlockName = "$" + bb->cfg->functionName + "_body_block";
        o << "      (br " << epilogueBlockName << ")\n";
    } else if (dest_label.find(bb->cfg->functionName + "_") == 0) {
        // Le saut est vers un autre bloc de la même fonction
        // En WebAssembly, l'exécution est linéaire par défaut
        o << "      ;; Jump to " << dest_label << " (handled by linear execution)\n";
    } else {
        // Saut vers une autre fonction ou un bloc inconnu
        o << "      ;; Jump to unknown destination " << dest_label << " (not implemented)\n";
    }
}

JumpFalse::JumpFalse(BasicBlock* p_bb, const std::string& p_dest_false, const std::string& p_dest_true, const VirtualRegister& p_op) 
    : IRInstr(p_bb), dest_false(p_dest_false), dest_true(p_dest_true), op(p_op) {};
std::string JumpFalse::get_operation_name() const {
    return "jumpfalse";
}
void JumpFalse::gen_x86(std::ostream& o) {
    o << "    cmpl $0, " << bb->cfg->IR_reg_to_x86(op) << " # jump false\n";
    o << "    je " << dest_false << "\n";
    o << "    jmp " << dest_true << "\n";
}
void JumpFalse::gen_wat(std::ostream& o) {
    o << "    ;; Conditional jump\n";
    std::string epilogueBlockName = "$" + bb->cfg->functionName + "_body_block";
    
    if (dest_false == bb->cfg->functionName + "_epilogue") {
        // Saut conditionnel vers l'épilogue si la condition est fausse
        o << "      (if (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
        o << "        (then (br " << epilogueBlockName << "))\n";
        o << "      )\n";
    } else if (dest_true == bb->cfg->functionName + "_epilogue") {
        // Saut conditionnel vers l'épilogue si la condition est vraie
        o << "      (if (i32.ne (local.get " << bb->cfg->IR_reg_to_wat(op) << ") (i32.const 0))\n";
        o << "        (then (br " << epilogueBlockName << "))\n";
        o << "      )\n";
    } else {
        // Implémentation correcte des sauts conditionnels vers d'autres blocs
        o << "      (if (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
        o << "        (then\n";
        o << "          ;; Exécution du bloc " << dest_false << "\n";
        
        // Recherche du bloc de destination false dans le CFG
        BasicBlock* false_block = nullptr;
        for (BasicBlock* block : bb->cfg->bbs) {
            if (block->label == dest_false) {
                false_block = block;
                break;
            }
        }
        
        // Instructions du bloc false
        if (false_block) {
            for (IRInstr* instr : false_block->instructions) {
                std::stringstream ss;
                instr->gen_wat(ss);
                std::string instr_str = ss.str();
                // Ajouter une indentation pour chaque ligne
                size_t pos = 0;
                while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                    instr_str.insert(pos + 1, "          ");
                    pos += 11; // 10 espaces + 1 pour le \n
                }
                o << instr_str;
            }
        }
        
        o << "        )\n";
        o << "        (else\n";
        o << "          ;; Exécution du bloc " << dest_true << "\n";
        
        // Recherche du bloc de destination true dans le CFG
        BasicBlock* true_block = nullptr;
        for (BasicBlock* block : bb->cfg->bbs) {
            if (block->label == dest_true) {
                true_block = block;
                break;
            }
        }
        
        // Instructions du bloc true
        if (true_block) {
            for (IRInstr* instr : true_block->instructions) {
                std::stringstream ss;
                instr->gen_wat(ss);
                std::string instr_str = ss.str();
                // Ajouter une indentation pour chaque ligne
                size_t pos = 0;
                while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                    instr_str.insert(pos + 1, "          ");
                    pos += 11; // 10 espaces + 1 pour le \n
                }
                o << instr_str;
            }
        }
        
        o << "        )\n";
        o << "      )\n";
    }
}

Push::Push(BasicBlock* p_bb, const VirtualRegister& p_op) 
    : IRInstr(p_bb), op(p_op) {};
std::string Push::get_operation_name() const {
    return "push";
}
void Push::gen_x86(std::ostream& o) {
    o << "    push " << bb->cfg->IR_reg_to_x86(op) << " # push\n";
}
void Push::gen_wat(std::ostream& o) {
    o << "    ;; Push\n";
    o << "    (i32.store (global.get $sp) (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
    o << "    (global.set $sp (i32.add (global.get $sp) (i32.const 4)))\n";
}

Pop::Pop(BasicBlock* p_bb, const VirtualRegister& p_dest)
    : IRInstr(p_bb), dest(p_dest) {};
std::string Pop::get_operation_name() const {
    return "pop";
}
void Pop::gen_x86(std::ostream& o) {
    o << "    pop " << bb->cfg->IR_reg_to_x86(dest) << " # pop\n";
}

void Pop::gen_wat(std::ostream& o) {
    o << "    ;; Pop\n";
    o << "    (global.set $sp (i32.sub (global.get $sp) (i32.const 4)))\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.load (global.get $sp)))\n";
}


// DOUBLE

DAdd::DAdd(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string DAdd::get_operation_name() const {
    return "dadd";
}
void DAdd::gen_x86(std::ostream& o) {
    o << "    addsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dadd\n";
}

void DAdd::gen_wat(std::ostream& o) {
    o << "    ;; Double addition\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.add (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}


DSub::DSub(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string DSub::get_operation_name() const {
    return "dsub";
}
void DSub::gen_x86(std::ostream& o) {
    o << "    subsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dsub\n";
}

void DSub::gen_wat(std::ostream& o) {
    o << "    ;; Double subtraction\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.sub (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}


DMul::DMul(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string DMul::get_operation_name() const {
    return "dmul";
}
void DMul::gen_x86(std::ostream& o) {
    o << "    mulsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dmul\n";
}

void DMul::gen_wat(std::ostream& o) {
    o << "    ;; Double multiplication\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.mul (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}


DDiv::DDiv(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {};
std::string DDiv::get_operation_name() const {
    return "ddiv";
}
void DDiv::gen_x86(std::ostream& o) {
    o << "    divsd " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # ddiv\n";
}

void DDiv::gen_wat(std::ostream& o) {
    o << "    ;; Double division\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.div (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}


CompareDouble::CompareDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp) 
    : IRInstr(p_bb), dest(dest_reg), left(p_left), right(p_right), comp(comp) {};
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

void CompareDouble::gen_wat(std::ostream& o) {
    o << "    ;; Compare double\n";
    std::string wat_comp;
    if (comp == ">") {
        wat_comp = "f64.gt";
    } else if (comp == "<") {
        wat_comp = "f64.lt";
    } else if (comp == "==") {
        wat_comp = "f64.eq";
    } else if (comp == "!=") {
        wat_comp = "f64.ne";
    } else if (comp == ">=") {
        wat_comp = "f64.ge";
    } else if (comp == "<=") {
        wat_comp = "f64.le";
    }
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (" << wat_comp << " (local.get " << bb->cfg->IR_reg_to_wat(left) << ") (local.get " << bb->cfg->IR_reg_to_wat(right) << ")))\n";
}



DWmem::DWmem(BasicBlock* p_bb, const std::string& address, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), addr(address), src(src_reg) {};
std::string DWmem::get_operation_name() const {
    return "dwmem";
}
void DWmem::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_addr_to_x86(addr) << " # dwmem\n";
}

void DWmem::gen_wat(std::ostream& o) {
    o << "    ;; Write double memory\n";
    o << "    (f64.store " << bb->cfg->IR_addr_to_wat(addr) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
}


DRmem::DRmem(BasicBlock* p_bb, const VirtualRegister& dest_reg, const std::string& address) 
    : IRInstr(p_bb), dest(dest_reg), addr(address) {};
std::string DRmem::get_operation_name() const {
    return "drmem";
}
void DRmem::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_addr_to_x86(addr) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # drmem\n";
}

void DRmem::gen_wat(std::ostream& o) {
    o << "    ;; Read double memory\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.load " << bb->cfg->IR_addr_to_wat(addr) << "))\n";
}


DCopy::DCopy(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {};
std::string DCopy::get_operation_name() const {
    return "copy";
}
void DCopy::gen_x86(std::ostream& o) {
    o << "    movsd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # dcopy\n";
}

void DCopy::gen_wat(std::ostream& o) {
    o << "    ;; Double copy\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
}


IntToDouble::IntToDouble(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {};
std::string IntToDouble::get_operation_name() const {
    return "int_to_double";
}
void IntToDouble::gen_x86(std::ostream& o) {
    o << "    cvtsi2sd " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # int2double\n";
}

void IntToDouble::gen_wat(std::ostream& o) {
    o << "    ;; Int to double conversion\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.convert_i32_s (local.get " << bb->cfg->IR_reg_to_wat(src) << ")))\n";
}


DoubleToInt::DoubleToInt(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), src(src_reg) {};
std::string DoubleToInt::get_operation_name() const {
    return "double_to_int";
}
void DoubleToInt::gen_x86(std::ostream& o) {
    // cvtt pour truncation
    o << "    cvttsd2si " << bb->cfg->IR_reg_to_x86(src) << ", " << bb->cfg->IR_reg_to_x86(dest) << " # double2int\n";
}

void DoubleToInt::gen_wat(std::ostream& o) {
    o << "    ;; Double to int conversion (truncate)\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.trunc_f64_s (local.get " << bb->cfg->IR_reg_to_wat(src) << ")))\n";
}


DUnaryMinus::DUnaryMinus(BasicBlock* p_bb, const VirtualRegister& dest_reg) 
    : IRInstr(p_bb), dest(dest_reg) {};
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

void DUnaryMinus::gen_wat(std::ostream& o) {
    o << "    ;; Double unary minus\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.neg (local.get " << bb->cfg->IR_reg_to_wat(dest) << ")))\n";
}



DNot::DNot(BasicBlock* p_bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg) 
    : IRInstr(p_bb), dest(dest_reg), op(src_reg) {};
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

void DNot::gen_wat(std::ostream& o) {
    o << "    ;; Double logical not\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (f64.eq (local.get " << bb->cfg->IR_reg_to_wat(op) << ") (f64.const 0.0)))\n";
}