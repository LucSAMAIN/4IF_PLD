#include "IR.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU INSTRUCTION //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Constructeur de IRInstr
IRInstr::IRInstr(BasicBlock* bb_, Operation *op) :
    bb(bb_), op(op) {}

// Génère une représentation textuelle de l'instruction IR
void IRInstr::gen_x86(ostream &o) {    
    /*
        Remplacez le switch case qu'il y avait avant.
    */
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU BLOCK ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





// Implémentation de BasicBlock
BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg), instructions() {}








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


void BasicBlock::add_IRInstr(IRInstr *instruction) {
    this->instructions.push_back(instruction);
}





// Méthode générique pour ajouter n'importe quelle opération
void BasicBlock::add_operation(Operation* op) {
    operations.push_back(op);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU CFG //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


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
    if (stv.symbolTable.find(name) == stv.symbolTable.end()) {
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
    
    // Récupérer l'offset:
    int offset = stv.offsetTable[current_bb->label];
    
    // Récupérer l'index attribué
    int stack_index = stv.SymbolTable[temp_name].offset;
    
    // Créer le vrai nom avec l'index dans la pile
    stringstream final_name;
    final_name << "!tmp" << -stack_index;
    final_name = final_name.str();

    // Mettre à jour la table des symboles avec le nouveau nom
    stv.SymbolTable[final_name].offset = stack_index;
    stv.SymbolTable[final_name].type = t;
    stv.SymbolTable[final_name].declared = true;
    stv.SymbolTable[final_name].used = false;
    
    // Supprimer l'entrée temporaire
    stv.SymbolTable.erase(temp_name);
    
    return final_name.str();
}

int CFG::get_var_index(string name) {
    if (stv.SymbolTable.find(name) != stv.SymbolTable.end()) {
        return stv.SymbolTable[name];
    }
    return 0;  // Erreur, la variable n'existe pas
}

Type CFG::get_var_type(string name) {
    if (stv.symbolTable.find(name) != stv.symbolTable.end()) {
        return stv.symbolTable[name].type;
    }
    return Type::INT;  // Par défaut
}

string CFG::new_BB_name() {
    stringstream ss;
    ss << "BB" << nextBBnumber++;
    return ss.str();
} 