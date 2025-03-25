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

// Génère une représentation textuelle du bloc de base
void BasicBlock::gen_wat(ostream& o) {
    o << "  ;; Bloc " << label << "\n";
    
    for (IRInstr* instr : instructions) {
        instr->gen_wat(o);
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

std::string CFG::IR_reg_to_wat(const std::string &reg) {
    // Gérer tous les registres d'arguments (!arg0, !arg1, etc.) indépendamment de leur taille
    if (reg.substr(0, 5) == "!arg0" || 
        reg.substr(0, 5) == "!arg1" ||
        reg.substr(0, 5) == "!arg2" ||
        reg.substr(0, 5) == "!arg3" ||
        reg.substr(0, 5) == "!arg4" ||
        reg.substr(0, 5) == "!arg5") {
        // Extraire le numéro de l'argument (0-5)
        std::string argNum = reg.substr(4, 1);
        return "$!arg" + argNum;
    } else if (reg.substr(0, 8) == "!regLeft") {
        return "$regLeft";
    } else if (reg.substr(0, 9) == "!regRight") {
        return "$regRight";
    } else if (reg.substr(0, 4) == "!reg") {
        return "$reg";
    }
    return "$" + reg;  // Ajouter le préfixe $ pour les variables WebAssembly
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

std::string CFG::IR_addr_to_wat(const std::string &addr) {
    if (addr.substr(0, 3) == "RBP") {
        std::string offset = addr.substr(3);
        return "(i32.add (local.get $bp) (i32.const " + offset + "))";
    }
    std::cerr << "# Erreur conversion adresse IR to wat\n"; 
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
    // Générer une seule fonction WebAssembly par fonction C
    o << "(func $" << functionName;

    if (functionName == "main") {
        o << " (export \"main\")";
    }

    stv.printSymbolTable();
    // Déclaration des arguments
    if (stv.offsetTable.find(functionName) != stv.offsetTable.end()) {
        // Pour chaque argument de la fonction (jusqu'à 6 maximum)
        int numArgs = std::min(6, stv.symbolTable[functionName].index_arg);
        for (int i = 0; i < numArgs; i++) {
            // Déclarer chaque argument comme un paramètre i32
            o << " (param $!arg" << i << " i32)";
        }
    }
    
    o << " (result i32)\n";
    o << "    (local $reg i32)\n";      // Registre pour les résultats
    o << "    (local $regLeft i32)\n";  // Registre pour l'opérande gauche
    o << "    (local $regRight i32)\n"; // Registre pour l'opérande droite
    o << "    (local $bp i32)\n";       // Base pointer
    
    // Si c'est la fonction main, déclarer les variables locales pour les arguments
    if (functionName == "main") {
        for (int i = 0; i < 6; i++) {
            o << "    (local $!arg" << i << " i32)\n";
        }
    }
    
    // Définir le nom du bloc d'épilogue
    string funcEpilogueBlockName = "$" + functionName + "_body_block";
    
    // Définir uniquement le bloc d'épilogue (le seul qui est utilisé pour les sauts)
    o << "    (block " << funcEpilogueBlockName << "\n";
    
    // Générer le prologue d'abord (toujours le bloc start_block)
    for (IRInstr* instr : start_block->instructions) {
        instr->gen_wat(o);
    }
    
    // Générer tous les blocs intermédiaires (ni prologue ni épilogue)
    for (BasicBlock* bb : bbs) {
        if (bb != start_block && bb != end_block) {
            o << "      ;; Bloc " << bb->label << "\n";
            for (IRInstr* instr : bb->instructions) {
                instr->gen_wat(o);
            }
        }
    }
    
    // Si on arrive ici sans sauter à l'épilogue, on continue naturellement
    
    // Fermer le bloc d'épilogue
    o << "    )\n"; // Fin du bloc epilogue
    
    // Générer l'épilogue (toujours le bloc end_block)
    for (IRInstr* instr : end_block->instructions) {
        instr->gen_wat(o);
    }
    
    // Pas besoin d'autre bloc
    
    // Valeur de retour par défaut - ne devrait jamais être atteinte car l'épilogue a son propre return
    o << "    (return (i32.const 0))\n";
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

