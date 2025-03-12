#include "IR.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Implémentation de IRInstr
IRInstr::IRInstr(BasicBlock* bb_, Operation op, Type t, vector<string> params) :
    bb(bb_), op(op), t(t), params(params) {}

// Génère une représentation textuelle de l'instruction IR
void IRInstr::gen_asm(ostream &o) {
    string reg_dst = params[0];
    
    switch(op) {
        case ldconst:
            o << "    LDCONST " << params[1] << " -> " << reg_dst << endl;
            break;
            
        case copy:
            o << "    COPY " << params[1] << " -> " << reg_dst << endl;
            break;
            
        case add:
            o << "    ADD " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        case sub:
            o << "    SUB " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        case mul:
            o << "    MUL " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        case cmp_eq:
            o << "    CMP_EQ " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        case cmp_lt:
            o << "    CMP_LT " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        case cmp_le:
            o << "    CMP_LE " << params[1] << ", " << params[2] << " -> " << reg_dst << endl;
            break;
            
        default:
            o << "    # Opération non implémentée: " << op << endl;
    }
}

// Génère du code assembleur x86 pour cette instruction IR
void IRInstr::gen_x86(ostream &o) {
    // Ne génère plus de code assembleur - seule la nouvelle architecture est utilisée
    // Un commentaire est ajouté pour la notification
    o << "    # Architecture obsolète : utilisez les nouvelles classes d'opérations" << endl;
}

// Implémentation de BasicBlock
BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg), operations() {}

// Génère une représentation textuelle du bloc de base
void BasicBlock::gen_asm(ostream& o) {
    o << "BASIC_BLOCK " << label << ":" << endl;
    
    // Générer uniquement le code pour les nouvelles opérations
    for (Operation* op : operations) {
        op->gen_asm(o);
    }
    
    // Générer les sauts
    if (exit_true == nullptr && exit_false == nullptr) {
        o << "    RETURN" << endl;
    } else if (exit_false == nullptr) {
        // Saut inconditionnel
        o << "    JUMP " << exit_true->label << endl;
    } else {
        // Saut conditionnel
        o << "    JUMP_IF_ZERO " << test_var_name << " -> " << exit_false->label << endl;
        o << "    JUMP " << exit_true->label << endl;
    }
}

// Génère du code assembleur x86 pour ce bloc de base
void BasicBlock::gen_x86(ostream& o) {
    o << "." << label << ":" << endl;
    
    // Générer uniquement le code pour les nouvelles opérations
    for (Operation* op : operations) {
        op->generate_assembly(o);
    }
    
    // Générer les sauts
    if (exit_true == nullptr && exit_false == nullptr) {
        // Épilogue de la fonction
        o << "    movq %rbp, %rsp" << endl;
        o << "    popq %rbp" << endl;
        o << "    ret" << endl;
    } else if (exit_false == nullptr) {
        // Saut inconditionnel
        o << "    jmp ." << exit_true->label << endl;
    } else {
        // Saut conditionnel
        o << "    cmpl $0, " << cfg->IR_reg_to_asm(test_var_name) << endl;
        o << "    je ." << exit_false->label << endl;
        o << "    jmp ." << exit_true->label << endl;
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, vector<string> params) {
    // Ne plus ajouter à l'ancienne architecture
    // Créer uniquement avec la nouvelle architecture orientée objet
    try {
        switch (op) {
            case IRInstr::ldconst:
                add_ldconst(params[0], stoi(params[1]));
                break;
            case IRInstr::copy:
                add_copy(params[0], params[1]);
                break;
            case IRInstr::add:
                add_add(params[0], params[1], params[2]);
                break;
            case IRInstr::sub:
                add_sub(params[0], params[1], params[2]);
                break;
            case IRInstr::mul:
                add_mul(params[0], params[1], params[2]);
                break;
            case IRInstr::cmp_eq:
                add_cmp_eq(params[0], params[1], params[2]);
                break;
            case IRInstr::cmp_lt:
                add_cmp_lt(params[0], params[1], params[2]);
                break;
            case IRInstr::cmp_le:
                add_cmp_le(params[0], params[1], params[2]);
                break;
            default:
                // Opération non implémentée dans la nouvelle architecture
                cerr << "Opération non supportée dans la nouvelle architecture: " << op << endl;
                break;
        }
    } catch (const exception& e) {
        cerr << "Erreur lors de la conversion: " << e.what() << endl;
    }
}

// Méthode générique pour ajouter n'importe quelle opération
void BasicBlock::add_operation(Operation* op) {
    operations.push_back(op);
}

// Nouvelles méthodes pour ajouter des opérations spécifiques
void BasicBlock::add_ldconst(const string& dest, int val) {
    LdConst* op = new LdConst(this, dest, val);
    add_operation(op);
}

void BasicBlock::add_copy(const string& dest, const string& src) {
    Copy* op = new Copy(this, dest, src);
    add_operation(op);
}

void BasicBlock::add_add(const string& dest, const string& op1, const string& op2) {
    Add* op = new Add(this, dest, op1, op2);
    add_operation(op);
}

void BasicBlock::add_sub(const string& dest, const string& op1, const string& op2) {
    Sub* op = new Sub(this, dest, op1, op2);
    add_operation(op);
}

void BasicBlock::add_mul(const string& dest, const string& op1, const string& op2) {
    Mul* op = new Mul(this, dest, op1, op2);
    add_operation(op);
}

void BasicBlock::add_rmem(const string& dest, const string& addr) {
    Rmem* op = new Rmem(this, dest, addr);
    add_operation(op);
}

void BasicBlock::add_wmem(const string& addr, const string& src) {
    Wmem* op = new Wmem(this, addr, src);
    add_operation(op);
}

void BasicBlock::add_call(const string& func_name) {
    Call* op = new Call(this, func_name);
    add_operation(op);
}

void BasicBlock::add_cmp_eq(const string& dest, const string& op1, const string& op2) {
    CmpEq* op = new CmpEq(this, dest, op1, op2);
    add_operation(op);
}

void BasicBlock::add_cmp_lt(const string& dest, const string& op1, const string& op2) {
    CmpLt* op = new CmpLt(this, dest, op1, op2);
    add_operation(op);
}

void BasicBlock::add_cmp_le(const string& dest, const string& op1, const string& op2) {
    CmpLe* op = new CmpLe(this, dest, op1, op2);
    add_operation(op);
}

// Implémentation de CFG
CFG::CFG(DefFonction* ast) : ast(ast), current_bb(nullptr), nextFreeSymbolIndex(0), nextBBnumber(0) {}

void CFG::add_bb(BasicBlock* bb) {
    bbs.push_back(bb);
}

string CFG::IR_reg_to_asm(string reg) {
    if (reg[0] == '%') {
        // C'est un registre
        return reg;
    } else if (isdigit(reg[0]) || (reg[0] == '-' && reg.size() > 1 && isdigit(reg[1]))) {
        // C'est une constante numérique
        return "$" + reg;
    } else {
        // C'est une variable
        return to_string(SymbolIndex[reg]) + "(%rbp)";
    }
}

// Génère une représentation textuelle du CFG
void CFG::gen_asm(ostream& o) {
    // En-tête
    o << "CFG pour la fonction " << ast->getName() << ":" << endl;
    
    // Informations sur la table des symboles
    o << "Table des symboles:" << endl;
    for (auto const& [name, index] : SymbolIndex) {
        o << "  " << name << " à l'offset " << index << " (type: ";
        switch(SymbolType[name]) {
            case Type::INT: o << "INT"; break;
            case Type::CHAR: o << "CHAR"; break;
            case Type::VOID: o << "VOID"; break;
            default: o << "UNKNOWN"; break;
        }
        o << ")" << endl;
    }
    
    // Générer le code pour tous les blocs de base
    o << "Blocs de base:" << endl;
    for (BasicBlock* bb : bbs) {
        bb->gen_asm(o);
    }
}

// Génère du code assembleur x86 à partir du CFG
void CFG::gen_x86(ostream& o) {
    // Prologue
    gen_asm_prologue(o);
    
    // Générer le code pour tous les blocs de base
    for (BasicBlock* bb : bbs) {
        bb->gen_x86(o);
    }
}

void CFG::gen_asm_prologue(ostream& o) {
    o << ".text" << endl;
    o << ".globl " << ast->getName() << endl;
    o << ast->getName() << ":" << endl;
    o << "    pushq %rbp" << endl;
    o << "    movq %rsp, %rbp" << endl;
    
    // Allouer de l'espace pour les variables locales
    int frameSize = ((-nextFreeSymbolIndex + 15) & ~15);  // Alignement sur 16 octets uniquement
    o << "    subq $" << frameSize << ", %rsp" << endl;
}

void CFG::gen_asm_epilogue(ostream& o) {
    o << "    movq %rbp, %rsp" << endl;
    o << "    popq %rbp" << endl;
    o << "    ret" << endl;
}

void CFG::add_to_symbol_table(string name, Type t) {
    if (SymbolIndex.find(name) == SymbolIndex.end()) {
        // Suivant le type, allocate un certain nombre d'octets
        if (t == Type::INT || t == Type::INT32_T) {
            nextFreeSymbolIndex -= 4;  // 4 octets pour un int
        } else if (t == Type::CHAR || t == Type::INT8_T) {
            nextFreeSymbolIndex -= 1;  // 1 octet pour un char
        } else if (t == Type::INT64_T) {
            nextFreeSymbolIndex -= 8;  // 8 octets pour un int64_t
        }
        
        SymbolIndex[name] = nextFreeSymbolIndex;
        SymbolType[name] = t;
    }
}

string CFG::create_new_tempvar(Type t) {
    string temp_name;
    
    // Créer d'abord un nom temporaire avec un préfixe spécial
    stringstream ss;
    ss << "!tmp_placeholder_" << nextBBnumber++;
    temp_name = ss.str();
    
    // Ajouter à la table des symboles pour obtenir l'index dans la pile
    add_to_symbol_table(temp_name, t);
    
    // Récupérer l'index attribué
    int stack_index = SymbolIndex[temp_name];
    
    // Créer le vrai nom avec l'index dans la pile
    stringstream final_name;
    final_name << "!tmp" << -stack_index;
    
    // Mettre à jour la table des symboles avec le nouveau nom
    SymbolIndex[final_name.str()] = stack_index;
    SymbolType[final_name.str()] = t;
    
    // Supprimer l'entrée temporaire
    SymbolIndex.erase(temp_name);
    SymbolType.erase(temp_name);
    
    return final_name.str();
}

int CFG::get_var_index(string name) {
    if (SymbolIndex.find(name) != SymbolIndex.end()) {
        return SymbolIndex[name];
    }
    return 0;  // Erreur, la variable n'existe pas
}

Type CFG::get_var_type(string name) {
    if (SymbolType.find(name) != SymbolType.end()) {
        return SymbolType[name];
    }
    return Type::INT;  // Par défaut
}

string CFG::new_BB_name() {
    stringstream ss;
    ss << "BB" << nextBBnumber++;
    return ss.str();
} 