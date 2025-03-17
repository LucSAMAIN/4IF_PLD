#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <iostream>

/*
    LISTE DE TOUTES LES CLASSES IMPLEMENTÉES CI-DESSOUS
    - Operation

    - LdConst
    - Copy
    - Add
    - Sub
    - Mul
    - Rmem
    - Wmem
    - Call
    - CmpEq
    - CmpLt
    - CmpLe
    

*/

// Déclarations anticipées
class BasicBlock;
class CFG;

// Classe de base pour toutes les opérations
class Operation
{
protected:
    // Constructeur protégé pour que seules les sous-classes puissent être instanciées
    Operation();

public:
    // Destructeur virtuel pour permettre la destruction propre des objets de sous-classes
    virtual ~Operation();
    
    
    // Méthode pour obtenir le nom de l'opération (pour le débogage)
    virtual std::string get_operation_name() const = 0;

    // Méthode pour générer une représentation textuelle de l'instruction IR
    virtual void gen_x86(std::ostream& o) = 0;
};

// Sous-classe pour le prologue d'une fonction
class Prologue : public Operation
{
private:
    // Bloc de base parent
    BasicBlock* bb;

public:
    Prologue(BasicBlock* bb);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'épilogue d'une fonction
class Epilogue : public Operation
{
private:
    // Bloc de base parent
    BasicBlock* bb;

public:
    Epilogue(BasicBlock* bb);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'opération de chargement d'une constante
class LdConst : public Operation
{
private:
    // Registre destination
    std::string dest;
    // Valeur constante à charger
    int value;
    // Bloc de base parent
    BasicBlock* bb;

public:
    LdConst(BasicBlock* bb, const std::string& dest_reg, int val);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'opération de copie
class Copy : public Operation
{
private:
    std::string dest;
    std::string src;
    BasicBlock* bb;

public:
    Copy(BasicBlock* bb, const std::string& dest_reg, const std::string& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'opération d'addition
class Add : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    Add(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'opération de soustraction
class Sub : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    Sub(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'opération de multiplication
class Mul : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    Mul(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour la lecture mémoire
class Rmem : public Operation
{
private:
    std::string dest;
    std::string addr;
    BasicBlock* bb;

public:
    Rmem(BasicBlock* bb, const std::string& dest_reg, const std::string& address);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'écriture mémoire
class Wmem : public Operation
{
private:
    std::string addr;
    std::string src;
    BasicBlock* bb;

public:
    Wmem(BasicBlock* bb, const std::string& address, const std::string& src_reg);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour l'appel de fonction
class Call : public Operation
{
private:
    std::string func_name;
    BasicBlock* bb;

public:
    Call(BasicBlock* bb, const std::string& function);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour la comparaison d'égalité
class CmpEq : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    CmpEq(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour la comparaison "plus petit que"
class CmpLt : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    CmpLt(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

// Sous-classe pour la comparaison "plus petit ou égal à"
class CmpLe : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;
    BasicBlock* bb;

public:
    CmpLe(BasicBlock* bb, const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    std::string get_operation_name() const override;
    void gen_x86(std::ostream& o) override;
};

#endif // OPERATION_H