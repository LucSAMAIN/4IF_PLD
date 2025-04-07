#ifndef IRINSTR_H
#define IRINSTR_H

#include <iostream>
#include <vector>
#include <string>

#include "IR.h"

class IRInstr {
public:

    /**  constructor */
    IRInstr(BasicBlock* bb_) : bb(bb_) { };
    ~IRInstr() {  }

    virtual std::string get_operation_name() const = 0; /**< Return the name of the operation */
    /** Actual code generation */
    virtual void gen_x86(std::ostream &o) = 0; /**< Representation textuelle de l'instruction IR */
    virtual void gen_wat(std::ostream &o) = 0; /**< Representation textuelle de l'instruction IR */
protected:
    BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
};

// Sous-classe pour le prologue d'une fonction
class Prologue : public IRInstr
{
public:
    Prologue(BasicBlock* bb);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'épilogue d'une fonction
class Epilogue : public IRInstr
{

public:
    Epilogue(BasicBlock* bb);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de chargement d'une constante
class LdConstInt : public IRInstr
{
private:
    // Registre destination
    VirtualRegister dest;
    // Valeur constante à charger
    int value;

public:
    LdConstInt(BasicBlock* bb, const VirtualRegister& dest_reg, int val);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de copie
class Copy : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister src;

public:
    Copy(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération d'addition
class Add : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Add(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de soustraction
class Sub : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Sub(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class UnaryMinus : public IRInstr
{
private:
    VirtualRegister dest;

public:
    UnaryMinus(BasicBlock* bb, const VirtualRegister& dest_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Not : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op;

public:
    Not(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de multiplication
class Mul : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Mul(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Div : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Div(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Mod : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Mod(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour la lecture mémoire
class Rmem : public IRInstr
{
private:
    VirtualRegister dest;
    std::string addr;

public:
    Rmem(BasicBlock* bb, const VirtualRegister& dest_reg, const std::string& address);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'écriture mémoire
class Wmem : public IRInstr
{
private:
    std::string addr;
    VirtualRegister src;

public:
    Wmem(BasicBlock* bb, const std::string& address, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'appel de fonction
class Call : public IRInstr
{
private:
    std::string func_name;
    std::vector<std::string> args;

public:
    Call(BasicBlock* bb, const std::string& function, const std::vector<std::string>& arguments);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour la comparaison d'égalité
class CompareInt : public IRInstr
{
private:
    VirtualRegister left;
    VirtualRegister right;
    VirtualRegister dest;
    std::string comp;
public:
    CompareInt(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class And : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    And(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Or : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Or(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Xor : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    Xor(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Jump : public IRInstr
{
private:
    std::string dest_label;

public:
    Jump(BasicBlock* bb, const std::string& dest_label);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
    
    // Getter pour dest_label
    std::string get_dest_label() const { return dest_label; }
};

class JumpFalse : public IRInstr
{


public:
    std::string dest_false;
    VirtualRegister op;

public:
    JumpFalse(BasicBlock* bb, const std::string& dest_false, const VirtualRegister& operand);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Push : public IRInstr
{
private:
    VirtualRegister op;

public:
    Push(BasicBlock* bb, const VirtualRegister& operand);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Pop : public IRInstr
{
private:
    VirtualRegister dest;

public:
    Pop(BasicBlock* bb, const VirtualRegister& dest);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// double operations
class LdConstDouble : public IRInstr
{
private:
    // Registre destination
    VirtualRegister dest;
    // Valeur constante à charger
    double value;

public:
    LdConstDouble(BasicBlock* bb, const VirtualRegister& dest_reg, double val);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DAdd : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;
public:
    DAdd(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};
class DSub : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;
public:
    DSub(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};
class DMul : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;
public:
    DMul(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};
class DDiv : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op2;

public:
    DDiv(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class CompareDouble : public IRInstr
{
private:
    VirtualRegister left;
    VirtualRegister right;
    VirtualRegister dest;
    std::string comp;
public:
    CompareDouble(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& p_left, const VirtualRegister& p_right, const std::string& comp);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DRmem : public IRInstr
{
private:
    VirtualRegister dest;
    std::string addr;

public:
    DRmem(BasicBlock* bb, const VirtualRegister& dest_reg, const std::string& address);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DWmem : public IRInstr
{
private:
    std::string addr;
    VirtualRegister src;

public:
    DWmem(BasicBlock* bb, const std::string& address, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DCopy : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister src;

public:
    DCopy(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class IntToDouble : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister src;

public:
    IntToDouble(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DoubleToInt : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister src;

public:
    DoubleToInt(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DUnaryMinus : public IRInstr
{
private:
    VirtualRegister dest;

public:
    DUnaryMinus(BasicBlock* bb, const VirtualRegister& dest_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class DNot : public IRInstr
{
private:
    VirtualRegister dest;
    VirtualRegister op;

public:
    DNot(BasicBlock* bb, const VirtualRegister& dest_reg, const VirtualRegister& operand);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

#endif // IRINSTR_H