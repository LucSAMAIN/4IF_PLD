#include "operation.h"
#include "IR.h" // Pour accéder à BasicBlock et CFG

// Implémentation de la classe Operation
Operation::Operation() {}

Operation::~Operation() {}

// Implémentation de LdConst
LdConst::LdConst(BasicBlock* bb, const std::string& dest_reg, int val) 
    : dest(dest_reg), value(val), bb(bb) {}

void LdConst::generate_assembly(std::ostream& o) {
    o << "    movl $" << value << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
}

std::string LdConst::get_operation_name() const {
    return "ldconst";
}

void LdConst::gen_asm(std::ostream& o) {
    o << "    LDCONST " << value << " -> " << dest << std::endl;
}

// Implémentation de Copy
Copy::Copy(BasicBlock* bb, const std::string& dest_reg, const std::string& src_reg) 
    : dest(dest_reg), src(src_reg), bb(bb) {}

void Copy::generate_assembly(std::ostream& o) {
    if (src[0] == '%') {
        // Copie depuis un registre
        o << "    movl " << src << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else {
        // Copie depuis une variable
        o << "    movl " << bb->cfg->IR_reg_to_asm(src) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    }
}

std::string Copy::get_operation_name() const {
    return "copy";
}

void Copy::gen_asm(std::ostream& o) {
    o << "    COPY " << src << " -> " << dest << std::endl;
}

// Implémentation de Add
Add::Add(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void Add::generate_assembly(std::ostream& o) {
    if (op1[0] == '%' && op2[0] == '%') {
        // Les deux opérandes sont des registres
        o << "    movl " << op1 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    addl " << op2 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else if (op1[0] == '%') {
        // Le premier est un registre, le second est une variable
        o << "    movl " << op1 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    addl " << bb->cfg->IR_reg_to_asm(op2) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else if (op2[0] == '%') {
        // Le premier est une variable, le second est un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    addl " << op2 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else {
        // Les deux sont des variables
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    addl " << bb->cfg->IR_reg_to_asm(op2) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    }
}

std::string Add::get_operation_name() const {
    return "add";
}

void Add::gen_asm(std::ostream& o) {
    o << "    ADD " << op1 << ", " << op2 << " -> " << dest << std::endl;
}

// Implémentation de Sub
Sub::Sub(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void Sub::generate_assembly(std::ostream& o) {
    if (op1[0] == '%' && op2[0] == '%') {
        // Les deux opérandes sont des registres
        o << "    movl " << op1 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    subl " << op2 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else if (op1[0] == '%') {
        // Le premier est un registre, le second est une variable
        o << "    movl " << op1 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    subl " << bb->cfg->IR_reg_to_asm(op2) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else if (op2[0] == '%') {
        // Le premier est une variable, le second est un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    subl " << op2 << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else {
        // Les deux sont des variables
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
        o << "    subl " << bb->cfg->IR_reg_to_asm(op2) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    }
}

std::string Sub::get_operation_name() const {
    return "sub";
}

void Sub::gen_asm(std::ostream& o) {
    o << "    SUB " << op1 << ", " << op2 << " -> " << dest << std::endl;
}

// Implémentation de Mul
Mul::Mul(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void Mul::generate_assembly(std::ostream& o) {
    if (op2[0] != '%') {
        // Si le deuxième opérande n'est pas un registre, le charger dans %ebx
        o << "    movl " << bb->cfg->IR_reg_to_asm(op2) << ", %ebx" << std::endl;
        if (op1[0] == '%') {
            o << "    movl " << op1 << ", %eax" << std::endl;
        } else {
            o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", %eax" << std::endl;
        }
        o << "    imul %ebx, %eax" << std::endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    } else {
        // Sinon, opération directe
        if (op1[0] == '%') {
            o << "    movl " << op1 << ", %eax" << std::endl;
        } else {
            o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", %eax" << std::endl;
        }
        o << "    imul " << op2 << ", %eax" << std::endl;
        o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
    }
}

std::string Mul::get_operation_name() const {
    return "mul";
}

void Mul::gen_asm(std::ostream& o) {
    o << "    MUL " << op1 << ", " << op2 << " -> " << dest << std::endl;
}

// Implémentation de Rmem
Rmem::Rmem(BasicBlock* bb, const std::string& dest_reg, const std::string& address) 
    : dest(dest_reg), addr(address), bb(bb) {}

void Rmem::generate_assembly(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_asm(addr) << ", " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
}

std::string Rmem::get_operation_name() const {
    return "rmem";
}

void Rmem::gen_asm(std::ostream& o) {
    o << "    RMEM " << addr << " -> " << dest << std::endl;
}

// Implémentation de Wmem
Wmem::Wmem(BasicBlock* bb, const std::string& address, const std::string& src_reg) 
    : addr(address), src(src_reg), bb(bb) {}

void Wmem::generate_assembly(std::ostream& o) {
    o << "    movl " << bb->cfg->IR_reg_to_asm(src) << ", " << bb->cfg->IR_reg_to_asm(addr) << std::endl;
}

std::string Wmem::get_operation_name() const {
    return "wmem";
}

void Wmem::gen_asm(std::ostream& o) {
    o << "    WMEM " << src << " -> " << addr << std::endl;
}

// Implémentation de Call
Call::Call(BasicBlock* bb, const std::string& function) : func_name(function), bb(bb) {}

void Call::generate_assembly(std::ostream& o) {
    o << "    call " << func_name << std::endl;
}

std::string Call::get_operation_name() const {
    return "call";
}

void Call::gen_asm(std::ostream& o) {
    o << "    CALL " << func_name << std::endl;
}

// Implémentation de CmpEq
CmpEq::CmpEq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void CmpEq::generate_assembly(std::ostream& o) {
    if (op1[0] != '%') {
        // Charger le premier opérande dans %eax si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", %eax" << std::endl;
    } else {
        o << "    movl " << op1 << ", %eax" << std::endl;
    }
    
    if (op2[0] != '%') {
        // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op2) << ", %ebx" << std::endl;
        o << "    cmpl %ebx, %eax" << std::endl;
    } else {
        o << "    cmpl " << op2 << ", %eax" << std::endl;
    }
    
    o << "    sete %al" << std::endl;
    o << "    movzbl %al, %eax" << std::endl;
    o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
}

std::string CmpEq::get_operation_name() const {
    return "cmp_eq";
}

void CmpEq::gen_asm(std::ostream& o) {
    o << "    CMP_EQ " << op1 << ", " << op2 << " -> " << dest << std::endl;
}

// Implémentation de CmpLt
CmpLt::CmpLt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void CmpLt::generate_assembly(std::ostream& o) {
    if (op1[0] != '%') {
        // Charger le premier opérande dans %eax si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", %eax" << std::endl;
    } else {
        o << "    movl " << op1 << ", %eax" << std::endl;
    }
    
    if (op2[0] != '%') {
        // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op2) << ", %ebx" << std::endl;
        o << "    cmpl %ebx, %eax" << std::endl;
    } else {
        o << "    cmpl " << op2 << ", %eax" << std::endl;
    }
    
    o << "    setl %al" << std::endl;
    o << "    movzbl %al, %eax" << std::endl;
    o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
}

std::string CmpLt::get_operation_name() const {
    return "cmp_lt";
}

void CmpLt::gen_asm(std::ostream& o) {
    o << "    CMP_LT " << op1 << ", " << op2 << " -> " << dest << std::endl;
}

// Implémentation de CmpLe
CmpLe::CmpLe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2), bb(bb) {}

void CmpLe::generate_assembly(std::ostream& o) {
    if (op1[0] != '%') {
        // Charger le premier opérande dans %eax si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op1) << ", %eax" << std::endl;
    } else {
        o << "    movl " << op1 << ", %eax" << std::endl;
    }
    
    if (op2[0] != '%') {
        // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
        o << "    movl " << bb->cfg->IR_reg_to_asm(op2) << ", %ebx" << std::endl;
        o << "    cmpl %ebx, %eax" << std::endl;
    } else {
        o << "    cmpl " << op2 << ", %eax" << std::endl;
    }
    
    o << "    setle %al" << std::endl;
    o << "    movzbl %al, %eax" << std::endl;
    o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(dest) << std::endl;
}

std::string CmpLe::get_operation_name() const {
    return "cmp_le";
}

void CmpLe::gen_asm(std::ostream& o) {
    o << "    CMP_LE " << op1 << ", " << op2 << " -> " << dest << std::endl;
}