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

void Prologue::gen_wat(std::ostream& o) {
    o << "    ;; Prologue\n";
    o << "    (local.set $bp (global.get $sp))\n";
    int frameSize = ((-bb->cfg->stv.offsetTable[bb->cfg->functionName] + 15) & ~15);
    frameSize = std::max(16, frameSize); // Au moins 16 octets pour le cadre de pile
    o << "    (global.set $sp (i32.sub (global.get $sp) (i32.const " << frameSize << ")))\n";
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

void Epilogue::gen_wat(std::ostream& o) {
    o << "    ;; Epilogue\n";
    o << "    (global.set $sp (local.get $bp))\n";
    o << "    (return (local.get $reg))\n"; // Retourne explicitement la valeur
}

// Implémentation de LdConst
LdConstInt::LdConstInt(BasicBlock* p_bb, const std::string& dest_reg, int val) 
    : IRInstr(p_bb), dest(dest_reg), value(val) {}


std::string LdConstInt::get_operation_name() const {
    return "ldconstint";
}

void LdConstInt::gen_x86(std::ostream& o) {
    o << "    movl $" << value << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}



LdConstDouble::LdConstDouble(BasicBlock* p_bb, const std::string& dest_reg, double val) 
    : IRInstr(p_bb), dest(dest_reg), value(val) {}


std::string LdConstDouble::get_operation_name() const {
    return "ldconstdouble";
}

void LdConstDouble::gen_x86(std::ostream& o) {
    uint64_t bits = *reinterpret_cast<uint64_t*>(&value);
    o << "mov rax, 0x" << std::hex << bits << std::dec << "\n";
    o << "movq " << bb->cfg->IR_reg_to_x86(dest) << ", rax\n";
}

void LdConst::gen_wat(std::ostream& o) {
    o << "    ;; Load constant\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.const " << value << "))\n";
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

void Copy::gen_wat(std::ostream& o) {
    o << "    ;; Copy\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
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

void Add::gen_wat(std::ostream& o) {
    o << "    ;; Addition\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.add (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Sub::gen_wat(std::ostream& o) {
    o << "    ;; Subtraction\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.sub (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void UnaryMinus::gen_wat(std::ostream& o) {
    o << "    ;; Unary minus\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.sub (i32.const 0) (local.get " << bb->cfg->IR_reg_to_wat(dest) << ")))\n";
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

void Not::gen_wat(std::ostream& o) {
    o << "    ;; Logical not\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(dest) << ")))\n";
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

void Mul::gen_wat(std::ostream& o) {
    o << "    ;; Multiplication\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.mul (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Div::gen_wat(std::ostream& o) {
    o << "    ;; Division\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.div_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Mod::gen_wat(std::ostream& o) {
    o << "    ;; Modulo\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.rem_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void Rmem::gen_wat(std::ostream& o) {
    o << "    ;; Read memory\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.load " << bb->cfg->IR_addr_to_wat(addr) << "))\n";
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

void Wmem::gen_wat(std::ostream& o) {
    o << "    ;; Write memory\n";
    o << "    (i32.store " << bb->cfg->IR_addr_to_wat(addr) << " (local.get " << bb->cfg->IR_reg_to_wat(src) << "))\n";
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

void Call::gen_wat(std::ostream& o) {
    o << "    ;; Function call\n";
    // Passer les arguments et stocker la valeur de retour
    o << "    (local.set $reg (call $" << func_name;
    
    // Récupérer le nombre d'arguments attendus par la fonction appelée (max 6)
    int numArgs = std::min(6, bb->cfg->stv.symbolTable[func_name].index_arg);
    
    // Passer les arguments depuis les registres d'arguments
    for (int i = 0; i < numArgs; i++) {
        o << "\n        (i32.load " << bb->cfg->IR_addr_to_wat(args[i]) << ")";
    }
    
    o << "))\n";
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

void CmpEq::gen_wat(std::ostream& o) {
    o << "    ;; Compare equal\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.eq (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void CmpNeq::gen_wat(std::ostream& o) {
    o << "    ;; Compare not equal\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.ne (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void CmpLe::gen_wat(std::ostream& o) {
    o << "    ;; Compare less or equal\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.le_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void CmpLt::gen_wat(std::ostream& o) {
    o << "    ;; Compare less than\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.lt_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void CmpGe::gen_wat(std::ostream& o) {
    o << "    ;; Compare greater or equal\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.ge_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
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

void CmpGt::gen_wat(std::ostream& o) {
    o << "    ;; Compare greater than\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.gt_s (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

And::And(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string And::get_operation_name() const {
    return "and";
}

void And::gen_x86(std::ostream& o) {
    o << "    andl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

void And::gen_wat(std::ostream& o) {
    o << "    ;; Logical and\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.and (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Or::Or(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Or::get_operation_name() const {
    return "or";
}

void Or::gen_x86(std::ostream& o) {
    o << "    orl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

void Or::gen_wat(std::ostream& o) {
    o << "    ;; Logical or\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.or (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Xor::Xor(BasicBlock* p_bb, const std::string& dest_reg, const std::string& operand2) 
    : IRInstr(p_bb), dest(dest_reg), op2(operand2) {}


std::string Xor::get_operation_name() const {
    return "xor";
}

void Xor::gen_x86(std::ostream& o) {
    o << "    xorl " << bb->cfg->IR_reg_to_x86(op2) << ", " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

void Xor::gen_wat(std::ostream& o) {
    o << "    ;; Logical xor\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.xor (local.get " << bb->cfg->IR_reg_to_wat(dest) << ") (local.get " << bb->cfg->IR_reg_to_wat(op2) << ")))\n";
}

Jump::Jump(BasicBlock* p_bb, const std::string& p_dest_label) 
    : IRInstr(p_bb), dest_label(p_dest_label) {}


std::string Jump::get_operation_name() const {
    return "jump";
}

void Jump::gen_x86(std::ostream& o) {
    o << "    jmp " << dest_label << "\n";
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

void JumpFalse::gen_wat(std::ostream& o) {
    o << "    ;; Conditional jump\n";
    
    // Détecter si c'est un bloc de test de while
    bool isWhileTest = bb->label.find("_test_while") != std::string::npos;
    
    // Traitement spécial pour les while
    if (isWhileTest) {
        // Pour les while, la structure est différente:
        // - dest_true est le corps du while
        // - dest_false est le bloc après le while
        
        // Pas besoin de génération spéciale, car le while est traité dans la 
        // méthode CFG::gen_wat avec les structures loop/block
        o << "    ;; Condition de while - saut vers " << dest_false << " si faux, " << dest_true << " si vrai\n";
        return;
    }
    
    // Dans le cas d'un saut conditionnel simple (sans bloc true/false séparé)
    if (dest_false == bb->cfg->functionName + "_epilogue") {
        // Saut vers l'épilogue si la condition est fausse
        std::string epilogueBlockName = "$" + bb->cfg->functionName + "_body_block";
        o << "      (br_if " << epilogueBlockName << " (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(op) << ")))\n";
    } else if (dest_true == bb->cfg->functionName + "_epilogue") {
        // Saut vers l'épilogue si la condition est vraie
        std::string epilogueBlockName = "$" + bb->cfg->functionName + "_body_block";
        o << "      (br_if " << epilogueBlockName << " (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
    } else {
        // Pour les structures if-else complètes, générer les blocs de code
        o << "      (if (i32.eqz (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
        o << "        (then\n";
        
        // Bloc else part (dest_false)
        o << "          ;; Else part (" << dest_false << ")\n";
        BasicBlock* false_block = nullptr;
        for (BasicBlock* block : bb->cfg->bbs) {
            if (block->label == dest_false) {
                false_block = block;
                break;
            }
        }
        
        if (false_block) {
            for (IRInstr* instr : false_block->instructions) {
                // Ne pas générer les sauts vers endif (ils seront implicites)
                if (instr->get_operation_name() != "jump") {
                    std::stringstream ss;
                    instr->gen_wat(ss);
                    std::string instr_str = ss.str();
                    // Indentation
                    size_t pos = 0;
                    while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                        instr_str.insert(pos + 1, "          ");
                        pos += 11;
                    }
                    o << instr_str;
                }
            }
        }
        
        o << "        )\n";
        o << "        (else\n";
        
        // Bloc then part (dest_true)
        o << "          ;; Then part (" << dest_true << ")\n";
        BasicBlock* true_block = nullptr;
        for (BasicBlock* block : bb->cfg->bbs) {
            if (block->label == dest_true) {
                true_block = block;
                break;
            }
        }
        
        if (true_block) {
            for (IRInstr* instr : true_block->instructions) {
                // Ne pas générer les sauts vers endif (ils seront implicites)
                if (instr->get_operation_name() != "jump") {
                    std::stringstream ss;
                    instr->gen_wat(ss);
                    std::string instr_str = ss.str();
                    // Indentation
                    size_t pos = 0;
                    while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                        instr_str.insert(pos + 1, "          ");
                        pos += 11;
                    }
                    o << instr_str;
                }
            }
        }
        
        o << "        )\n";
        o << "      )\n";
    }
}

Push::Push(BasicBlock* p_bb, const std::string& p_op) 
    : IRInstr(p_bb), op(p_op) {}


std::string Push::get_operation_name() const {
    return "push";
}

void Push::gen_x86(std::ostream& o) {
    o << "    push " << bb->cfg->IR_reg_to_x86(op) << "\n";
}

void Push::gen_wat(std::ostream& o) {
    o << "    ;; Push\n";
    o << "    (i32.store (global.get $sp) (local.get " << bb->cfg->IR_reg_to_wat(op) << "))\n";
    o << "    (global.set $sp (i32.add (global.get $sp) (i32.const 4)))\n";
}

Pop::Pop(BasicBlock* p_bb, const std::string& p_dest) 
    : IRInstr(p_bb), dest(p_dest) {}


std::string Pop::get_operation_name() const {
    return "pop";
}

void Pop::gen_x86(std::ostream& o) {
    o << "    pop " << bb->cfg->IR_reg_to_x86(dest) << "\n";
}

void Pop::gen_wat(std::ostream& o) {
    o << "    ;; Pop\n";
    o << "    (global.set $sp (i32.sub (global.get $sp) (i32.const 4)))\n";
    o << "    (local.set " << bb->cfg->IR_reg_to_wat(dest) << " (i32.load (global.get $sp)))\n";
}