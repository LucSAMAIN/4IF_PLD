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
    op->gen_x86(o);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU BLOCK ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





// Implémentation de BasicBlock
BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg), instructions()
{

}



void BasicBlock::add_IRInstr(IRInstr *instruction) {
    instructions.push_back(instruction);
}




// Génère une représentation textuelle du bloc de base
void BasicBlock::gen_x86(ostream& o) {
    o << label << ":\n";
    
    for (IRInstr* instr : instructions) {
        instr->gen_x86(o);
    }
    
    // Générer les sauts
    if (exit_true == nullptr && exit_false == nullptr) {
        o << "    RETURN" << "\n";
    } else if (exit_false == nullptr) {
        // Saut inconditionnel
        o << "    JUMP " << exit_true->label << "\n";
    } else {
        // Saut conditionnel
        o << "    JUMP_IF_ZERO " << test_var_name << " -> " << exit_false->label << "\n";
        o << "    JUMP " << exit_true->label << "\n";
    }
}


void BasicBlock::add_IRInstr(IRInstr *instruction) {
    this->instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU CFG //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// Implémentation de CFG

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
        return stv.symbolTable[reg].offset + "(%rbp)";
    }
}

// Génère une représentation textuelle du CFG
void CFG::gen_x86(ostream& o) {
    // Générer le code pour tous les blocs de base
    for (BasicBlock* bb : bbs) {
        bb->gen_x86(o);
    }
}


void CFG::gen_x86_prologue(ostream& o) {
    o << ".text" << "\n";
    if (functionName == "main") {
        o << ".globl main\n";
    }
    o << ast->getName() << ":" << "\n";
    o << "    pushq %rbp" << "\n";
    o << "    movq %rsp, %rbp" << "\n";
    
    // Allouer de l'espace pour les variables locales
    int frameSize = ((-nextFreeSymbolIndex + 15) & ~15);  // Alignement sur 16 octets uniquement
    o << "    subq $" << frameSize << ", %rsp" << "\n";
}

void CFG::gen_asm_epilogue(ostream& o) {
    o << "    movq %rbp, %rsp" << "\n";
    o << "    popq %rbp" << "\n";
    o << "    ret" << "\n";
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
    // Récupérer l'offset:
    int offset = 0;
    if (t == Type::INT) {
        stv.offsetTable[functionName] -= 4;
        offset = stv.offsetTable[functionName];
    }
    
    // Créer le vrai nom avec l'offset dans la pile
    string temp_name("!tmp"+offset);

    // Mettre à jour la table des symboles avec le nouveau nom
    stv.symbolTable[temp_name].offset = offset;
    stv.symbolTable[temp_name].type = t;
    stv.symbolTable[temp_name].declared = true;
    stv.symbolTable[temp_name].used = false;
    
    return temp_name;
}

int CFG::get_var_offset(string name) {
    if (stv.symbolTable.find(name) != stv.symbolTable.end()) {
        return stv.symbolTable[name].offset;
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
    ss << functionName << bbs.size(); // je pense ça passe car on ajoute le block direct dans la liste après
    return ss.str();
} 