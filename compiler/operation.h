#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <iostream>

// Classe de base pour toutes les opérations
class Operation
{
protected:
    // Constructeur protégé pour que seules les sous-classes puissent être instanciées
    Operation();

public:
    // Destructeur virtuel pour permettre la destruction propre des objets de sous-classes
    virtual ~Operation();
    
    // Méthode virtuelle pure à implémenter par chaque sous-classe
    virtual void generate_assembly(std::ostream& o) = 0;
    
    // Méthode pour obtenir le nom de l'opération (pour le débogage)
    virtual std::string get_operation_name() const = 0;
};

// Sous-classe pour l'opération de chargement d'une constante
class LdConst : public Operation
{
private:
    // Registre destination
    std::string dest;
    // Valeur constante à charger
    int value;

public:
    LdConst(const std::string& dest_reg, int val);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'opération de copie
class Copy : public Operation
{
private:
    std::string dest;
    std::string src;

public:
    Copy(const std::string& dest_reg, const std::string& src_reg);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'opération d'addition
class Add : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    Add(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'opération de soustraction
class Sub : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    Sub(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'opération de multiplication
class Mul : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    Mul(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour la lecture mémoire
class Rmem : public Operation
{
private:
    std::string dest;
    std::string addr;

public:
    Rmem(const std::string& dest_reg, const std::string& address);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'écriture mémoire
class Wmem : public Operation
{
private:
    std::string addr;
    std::string src;

public:
    Wmem(const std::string& address, const std::string& src_reg);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour l'appel de fonction
class Call : public Operation
{
private:
    std::string func_name;

public:
    Call(const std::string& function);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour la comparaison d'égalité
class CmpEq : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    CmpEq(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour la comparaison "plus petit que"
class CmpLt : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    CmpLt(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

// Sous-classe pour la comparaison "plus petit ou égal à"
class CmpLe : public Operation
{
private:
    std::string dest;
    std::string op1;
    std::string op2;

public:
    CmpLe(const std::string& dest_reg, const std::string& operand1, const std::string& operand2);
    void generate_assembly(std::ostream& o) override;
    std::string get_operation_name() const override;
};

#endif // OPERATION_H