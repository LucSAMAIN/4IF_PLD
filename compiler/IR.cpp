#include "IR.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU BLOCK ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





// Implémentation de BasicBlock
BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg), instructions(), test_var_name()
{
}

BasicBlock::~BasicBlock() {
    for (IRInstr* instr : instructions) {
        delete instr;
    }
}



void BasicBlock::add_IRInstr(IRInstr *instruction) {
    instructions.push_back(instruction);
    
}




// Génère une représentation textuelle du bloc de base
void BasicBlock::gen_x86(ostream& o) {
    o << label << ":\n";
    
    for (IRInstr* instr : instructions) {
        instr->gen_x86(o);
        // if return_true vers epilogue, on arrête parce qu'on vient de voir un return et on ne veut pas générer de code après
    }
}

// La méthode gen_wat n'est plus utilisée, elle est remplacée par la logique dans CFG::gen_wat



////////////////////////////////////////////////////////////////////////////////
////////////////////////////// NIVEAU CFG //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// Implémentation de CFG

CFG::CFG(SymbolTableGenVisitor& p_stv, const std::string& p_funcName) : stv(p_stv), current_bb(nullptr), start_block(nullptr), end_block(nullptr), bbs(), functionName(p_funcName) {
    // Ajouter le bloc de base initial
    start_block = new BasicBlock(this, functionName);
    IRInstr* instr_start = new Prologue(start_block);
    start_block->add_IRInstr(instr_start);

    current_bb = new BasicBlock(this, new_BB_name());
    start_block->exit_true = current_bb;

    end_block = new BasicBlock(this, functionName + "_epilogue");
    IRInstr* instr_end = new Epilogue(end_block);
    end_block->add_IRInstr(instr_end);
    current_bb->exit_true = end_block;

    add_bb(start_block);
    add_bb(end_block);
    add_bb(current_bb);
}

CFG::~CFG()
{
    for (auto block : bbs)
        delete block;
}

void CFG::add_bb(BasicBlock* bb) {
    bbs.push_back(bb);
}

string CFG::IR_reg_to_x86(const string &reg) {
    if (reg.substr(0, 5) == "!arg0") {
        if (reg.substr(5) == "64") {
            return "%rdi";
        }
        else if (reg.substr(5) == "32") {
            return "%edi";
        }
        else if (reg.substr(5) == "16") {
            return "%di";
        }
        else if (reg.substr(5) == "8") {
            return "%dil";
        }
        else {
            return "%edi";
        }
    } else if (reg.substr(0, 5) == "!arg1") {
        if (reg.substr(5) == "64") {
            return "%rsi";
        }
        else if (reg.substr(5) == "32") {
            return "%esi";
        }
        else if (reg.substr(5) == "16") {
            return "%si";
        }
        else if (reg.substr(5) == "8") {
            return "%sil";
        }
        else {
            return "%esi";
        }
    } else if (reg.substr(0, 5) == "!arg2") {
        if (reg.substr(5) == "64") {
            return "%rdx";
        }
        else if (reg.substr(5) == "32") {
            return "%edx";
        }
        else if (reg.substr(5) == "16") {
            return "%dx";
        }
        else if (reg.substr(5) == "8") {
            return "%dl";
        }
        else {
            return "%edx";
        }
    } else if (reg.substr(0, 5) == "!arg3") {
        if (reg.substr(5) == "64") {
            return "%rcx";
        }
        else if (reg.substr(5) == "32") {
            return "%ecx";
        }
        else if (reg.substr(5) == "16") {
            return "%cx";
        }
        else if (reg.substr(5) == "8") {
            return "%cl";
        }
        else {
            return "%ecx";
        }
    } else if (reg.substr(0, 5) == "!arg4") {
        if (reg.substr(5) == "64") {
            return "%r8";
        }
        else if (reg.substr(5) == "32") {
            return "%r8d";
        }
        else if (reg.substr(5) == "16") {
            return "%r8w";
        }
        else if (reg.substr(5) == "8") {
            return "%r8b";
        }
        else {
            return "%r8d";
        }
    } else if (reg.substr(0, 5) == "!arg5") {
        if (reg.substr(5) == "64") {
            return "%r9";
        }
        else if (reg.substr(5) == "32") {
            return "%r9d";
        }
        else if (reg.substr(5) == "16") {
            return "%r9w";
        }
        else if (reg.substr(5) == "8") {
            return "%r9b";
        }
        else {
            return "%r9d";
        }
    } else if (reg.substr(0, 8) == "!regLeft") {
        if (reg.substr(8) == "64") {
            return "%rbx";
        }
        else if (reg.substr(8) == "32") {
            return "%ebx";
        }
        else if (reg.substr(8) == "16") {
            return "%bx";
        }
        else if (reg.substr(8) == "8") {
            return "%bl";
        }
        else {
            return "%ebx";
        }
    } else if (reg.substr(0, 9) == "!regRight") {
        if (reg.substr(9) == "64") {
            return "%rcx";
        }
        else if (reg.substr(9) == "32") {
            return "%ecx";
        }
        else if (reg.substr(9) == "16") {
            return "%cx";
        }
        else if (reg.substr(9) == "8") {
            return "%cl";
        }
        else {
            return "%ecx";
        }
    } else if (reg.substr(0, 4) == "!reg") {
        if (reg.substr(4) == "64") {
            return "%rax";
        }
        else if (reg.substr(4) == "32") {
            return "%eax";
        }
        else if (reg.substr(4) == "16") {
            return "%ax";
        }
        else if (reg.substr(4) == "8") {
            return "%al";
        }
        else {
            return "%eax";
        }
    }
    std::cerr << "Erreur conversion registre IR to x86, le registre renseigné n'existe pas\n"; 
    return "";
}

std::string CFG::IR_addr_to_x86(const std::string &addr)
{
    // std::cout << "# addr IR_addr_to_x86 " << addr << "\n";
    if (addr.substr(0, 3) == "RBP") {
        std::string offset = addr.substr(3);
        // std::cout << "# offset IR_addr_to_x86 " << offset << "\n";
        return offset + "(%rbp)";
    
    }
    std::cerr << "# Erreur conversion adresse IR to x86\n"; 
    return "";
}

// Génère une représentation textuelle du CFG
void CFG::gen_x86(ostream& o) {
    // Générer le code pour tous les blocs de base
    o << "\n";
    for (BasicBlock* bb : bbs) {
        bb->gen_x86(o);
    }
}

void CFG::gen_wat(ostream& o) {
    // Début du module WebAssembly
    o << "(module\n";
    o << "  ;; Déclaration de la mémoire\n";
    o << "  (memory 1)\n";  
    o << "  (export \"memory\" (memory 0))\n";  // Exporter la mémoire pour pouvoir l'accéder depuis JS
    
    // Déclarer la mémoire globale
    o << "  ;; Import de la mémoire et des fonctions système si nécessaire\n";
    o << "  (global $sp (mut i32) (i32.const 1024))\n";  // Commencer avec un stack pointer non nul
    
    // Commencer la fonction principale
    o << "  (func $main (export \"main\") (result i32)\n";
    o << "    (local $reg i32)\n";      // Déclarer un registre local pour stocker les résultats
    o << "    (local $regLeft i32)\n";  // Déclarer le registre pour l'opérande gauche
    o << "    (local $regRight i32)\n"; // Déclarer le registre pour l'opérande droite
    o << "    (local $bp i32)\n";       // Déclarer le base pointer
    
    // Ici, nous allons utiliser un unique block englobant tous les autres
    o << "    (block $main_block\n";
    
    // Nous déclarons l'épilogue comme un bloc à l'intérieur du bloc principal
    o << "      (block $main_epilogue\n";
    
    // Prologue et code principal
    // Générer le code pour le bloc principal
    for (BasicBlock* bb : bbs) {
        if (bb->label == functionName) {
            // Générer le prologue
            for (IRInstr* instr : bb->instructions) {
                instr->gen_wat(o);
            }
        }
    }
    
    // Générer le code pour les blocs intermédiaires
    for (BasicBlock* bb : bbs) {
        if (bb->label != functionName && bb->label != functionName + "_epilogue") {
            // Générer le code intermédiaire
            for (IRInstr* instr : bb->instructions) {
                instr->gen_wat(o);
            }
        }
    }
    
    // Si on arrive ici, c'est qu'on a dépassé tous les blocs intermédiaires sans sauter à l'épilogue
    // On saute directement à l'épilogue pour éviter d'exécuter son code qui suit
    o << "      (br $main_epilogue)\n";
    
    // Ici on définit le code de l'épilogue qui sera exécuté après tous les sauts à l'épilogue
    o << "      )\n"; // Fin du bloc $main_epilogue
    
    // Générer le code de l'épilogue
    for (BasicBlock* bb : bbs) {
        if (bb->label == functionName + "_epilogue") {
            for (IRInstr* instr : bb->instructions) {
                instr->gen_wat(o);
            }
        }
    }
    
    o << "    )\n"; // Fin du bloc $main_block
    
    // S'assurer que la fonction retourne bien une valeur par défaut au cas où
    o << "    ;; Default return value (should not be reached)\n";
    o << "    (return (i32.const 0))\n";
    
    // Fermer la fonction et le module
    o << "  )\n";
    o << ")\n";
}

// void CFG::add_to_symbol_table(string name, Type t) {
//     if (stv.symbolTable.find(name) == stv.symbolTable.end()) {
//         // Suivant le type, allocate un certain nombre d'octets
//         if (t == Type::INT || t == Type::INT32_T) {
//             nextFreeSymbolIndex -= 4;  // 4 octets pour un int
//         } else if (t == Type::CHAR || t == Type::INT8_T) {
//             nextFreeSymbolIndex -= 1;  // 1 octet pour un char
//         } else if (t == Type::INT64_T) {
//             nextFreeSymbolIndex -= 8;  // 8 octets pour un int64_t
//         }
        
//         SymbolIndex[name] = nextFreeSymbolIndex;
//         SymbolType[name] = t;
//     }
// }

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
    ss << functionName << "_" << bbs.size(); // je pense ça passe car on ajoute le block direct dans la liste après
    return ss.str();
}

std::string CFG::IR_reg_to_wat(const std::string &reg) {
    if (reg.substr(0, 8) == "!regLeft") {
        return "$regLeft";
    } else if (reg.substr(0, 9) == "!regRight") {
        return "$regRight";
    } else if (reg.substr(0, 4) == "!reg") {
        return "$reg";
    }
    return "$" + reg;  // Ajouter le préfixe $ pour les variables WebAssembly
} 