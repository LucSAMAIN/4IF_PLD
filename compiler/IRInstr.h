#ifndef IRINSTR_H
#define IRINSTR_H

#include <iostream>
#include <vector>
#include <string>

class BasicBlock;

//! The class for one 3-address instruction
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
class LdConst : public IRInstr
{
private:
    // Registre destination
    std::string dest;
    // Valeur constante à charger
    int value;

public:
    LdConst(BasicBlock* bb, const std::string& dest_reg, int val);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de copie
class Copy : public IRInstr
{
private:
    std::string dest;
    std::string src;

public:
    Copy(BasicBlock* bb, const std::string& dest_reg, const std::string& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération d'addition
class Add : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Add(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de soustraction
class Sub : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Sub(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class UnaryMinus : public IRInstr
{
private:
    std::string dest;

public:
    UnaryMinus(BasicBlock* bb, const std::string& dest_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Not : public IRInstr
{
private:
    std::string dest;

public:
    Not(BasicBlock* bb, const std::string& dest_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'opération de multiplication
class Mul : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Mul(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Div : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Div(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Mod : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Mod(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour la lecture mémoire
class Rmem : public IRInstr
{
private:
    std::string dest;
    std::string addr;

public:
    Rmem(BasicBlock* bb, const std::string& dest_reg, const std::string& address);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour l'écriture mémoire
class Wmem : public IRInstr
{
private:
    std::string addr;
    std::string src;

public:
    Wmem(BasicBlock* bb, const std::string& address, const std::string& src_reg);
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
class CmpEq : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpEq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class CmpNeq : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpNeq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

// Sous-classe pour la comparaison "plus petit ou égal à"
class CmpLe : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpLe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class CmpLt : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpLt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class CmpGe : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpGe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class CmpGt : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    CmpGt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class And : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    And(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Or : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Or(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Xor : public IRInstr
{
private:
    std::string dest;
    std::string op2;

public:
    Xor(BasicBlock* bb, const std::string& dest_reg, const std::string& operand2);
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
    std::string dest_true;
    std::string op;
    
    JumpFalse(BasicBlock* bb, const std::string& dest_false, const std::string& dest_true, const std::string& operand);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Push : public IRInstr
{
private:
    std::string op;

public:
    Push(BasicBlock* bb, const std::string& operand);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

class Pop : public IRInstr
{
private:
    std::string dest;

public:
    Pop(BasicBlock* bb, const std::string& dest);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
    void gen_wat(std::ostream& o) override;
};

#endif // IRINSTR_H