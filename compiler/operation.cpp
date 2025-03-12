#include "Operation.h"

// Implémentation de la classe Operation
Operation::Operation() {}

Operation::~Operation() {}

// Implémentation de LdConst
LdConst::LdConst(const std::string& dest_reg, int val) 
    : dest(dest_reg), value(val) {}

void LdConst::generate_assembly(std::ostream& o) {
    o << "    movl $" << value << ", " << dest << std::endl;
}

std::string LdConst::get_operation_name() const {
    return "ldconst";
}

// Implémentation de Copy
Copy::Copy(const std::string& dest_reg, const std::string& src_reg) 
    : dest(dest_reg), src(src_reg) {}

void Copy::generate_assembly(std::ostream& o) {
    o << "    movl " << src << ", " << dest << std::endl;
}

std::string Copy::get_operation_name() const {
    return "copy";
}

// Implémentation de Add
Add::Add(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void Add::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", " << dest << std::endl;
    o << "    addl " << op2 << ", " << dest << std::endl;
}

std::string Add::get_operation_name() const {
    return "add";
}

// Implémentation de Sub
Sub::Sub(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void Sub::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", " << dest << std::endl;
    o << "    subl " << op2 << ", " << dest << std::endl;
}

std::string Sub::get_operation_name() const {
    return "sub";
}

// Implémentation de Mul
Mul::Mul(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void Mul::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", " << dest << std::endl;
    o << "    imull " << op2 << ", " << dest << std::endl;
}

std::string Mul::get_operation_name() const {
    return "mul";
}

// Implémentation de Rmem
Rmem::Rmem(const std::string& dest_reg, const std::string& address) 
    : dest(dest_reg), addr(address) {}

void Rmem::generate_assembly(std::ostream& o) {
    o << "    movl " << addr << ", " << dest << std::endl;
}

std::string Rmem::get_operation_name() const {
    return "rmem";
}

// Implémentation de Wmem
Wmem::Wmem(const std::string& address, const std::string& src_reg) 
    : addr(address), src(src_reg) {}

void Wmem::generate_assembly(std::ostream& o) {
    o << "    movl " << src << ", " << addr << std::endl;
}

std::string Wmem::get_operation_name() const {
    return "wmem";
}

// Implémentation de Call
Call::Call(const std::string& function) : func_name(function) {}

void Call::generate_assembly(std::ostream& o) {
    o << "    call " << func_name << std::endl;
}

std::string Call::get_operation_name() const {
    return "call";
}

// Implémentation de CmpEq
CmpEq::CmpEq(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void CmpEq::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", %eax" << std::endl;
    o << "    cmpl " << op2 << ", %eax" << std::endl;
    o << "    sete %al" << std::endl;
    o << "    movzbl %al, " << dest << std::endl;
}

std::string CmpEq::get_operation_name() const {
    return "cmp_eq";
}

// Implémentation de CmpLt
CmpLt::CmpLt(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void CmpLt::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", %eax" << std::endl;
    o << "    cmpl " << op2 << ", %eax" << std::endl;
    o << "    setl %al" << std::endl;
    o << "    movzbl %al, " << dest << std::endl;
}

std::string CmpLt::get_operation_name() const {
    return "cmp_lt";
}

// Implémentation de CmpLe
CmpLe::CmpLe(const std::string& dest_reg, const std::string& operand1, const std::string& operand2) 
    : dest(dest_reg), op1(operand1), op2(operand2) {}

void CmpLe::generate_assembly(std::ostream& o) {
    o << "    movl " << op1 << ", %eax" << std::endl;
    o << "    cmpl " << op2 << ", %eax" << std::endl;
    o << "    setle %al" << std::endl;
    o << "    movzbl %al, " << dest << std::endl;
}

std::string CmpLe::get_operation_name() const {
    return "cmp_le";
}