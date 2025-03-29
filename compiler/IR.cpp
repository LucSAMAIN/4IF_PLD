#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <map>

#include "IR.h"
#include "IRInstr.h"

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

void BasicBlock::pop_IRInstr() {
    if (!instructions.empty()) {
        delete instructions.back();
        instructions.pop_back();
    }
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

    // add_bb(start_block); // on les a mis dans gen_x86 pour que ce soit "dans le bon ordre" (hein luc)
    // add_bb(end_block);
    add_bb(current_bb);
}

CFG::~CFG()
{
    for (auto block : bbs)
        delete block;
    delete start_block;
    delete end_block;
}

void CFG::add_bb(BasicBlock* bb) {
    bbs.push_back(bb);
}

std::string CFG::IR_reg_to_x86(const VirtualRegister& vr) {
    // Mapping of virtual register names to x86 physical registers
    static const std::map<std::tuple<RegisterFunction, RegisterType, RegisterSize>, std::string> regMap = {
        {{RegisterFunction::REG, RegisterType::GPR, RegisterSize::SIZE_64}, "rax"},
        {{RegisterFunction::REG, RegisterType::GPR, RegisterSize::SIZE_32}, "eax"},
        {{RegisterFunction::REG, RegisterType::GPR, RegisterSize::SIZE_16}, "ax"},
        {{RegisterFunction::REG, RegisterType::GPR, RegisterSize::SIZE_8}, "al"},

        {{RegisterFunction::REG_LEFT, RegisterType::GPR, RegisterSize::SIZE_64}, "rbx"},
        {{RegisterFunction::REG_LEFT, RegisterType::GPR, RegisterSize::SIZE_32}, "ebx"},
        {{RegisterFunction::REG_LEFT, RegisterType::GPR, RegisterSize::SIZE_16}, "bx"},
        {{RegisterFunction::REG_LEFT, RegisterType::GPR, RegisterSize::SIZE_8}, "bl"},

        {{RegisterFunction::REG_RIGHT, RegisterType::GPR, RegisterSize::SIZE_64}, "rcx"},
        {{RegisterFunction::REG_RIGHT, RegisterType::GPR, RegisterSize::SIZE_32}, "ecx"},
        {{RegisterFunction::REG_RIGHT, RegisterType::GPR, RegisterSize::SIZE_16}, "cx"},
        {{RegisterFunction::REG_RIGHT, RegisterType::GPR, RegisterSize::SIZE_8}, "cl"},

        {{RegisterFunction::ARG0, RegisterType::GPR, RegisterSize::SIZE_64}, "rdi"},
        {{RegisterFunction::ARG0, RegisterType::GPR, RegisterSize::SIZE_32}, "edi"},
        {{RegisterFunction::ARG0, RegisterType::GPR, RegisterSize::SIZE_16}, "di"},
        {{RegisterFunction::ARG0, RegisterType::GPR, RegisterSize::SIZE_8}, "dil"},

        {{RegisterFunction::ARG1, RegisterType::GPR, RegisterSize::SIZE_64}, "rsi"},
        {{RegisterFunction::ARG1, RegisterType::GPR, RegisterSize::SIZE_32}, "esi"},
        {{RegisterFunction::ARG1, RegisterType::GPR, RegisterSize::SIZE_16}, "si"},
        {{RegisterFunction::ARG1, RegisterType::GPR, RegisterSize::SIZE_8}, "sil"},

        {{RegisterFunction::ARG2, RegisterType::GPR, RegisterSize::SIZE_64}, "rdx"},
        {{RegisterFunction::ARG2, RegisterType::GPR, RegisterSize::SIZE_32}, "edx"},
        {{RegisterFunction::ARG2, RegisterType::GPR, RegisterSize::SIZE_16}, "dx"},
        {{RegisterFunction::ARG2, RegisterType::GPR, RegisterSize::SIZE_8}, "dl"},

        {{RegisterFunction::ARG3, RegisterType::GPR, RegisterSize::SIZE_64}, "rcx"},
        {{RegisterFunction::ARG3, RegisterType::GPR, RegisterSize::SIZE_32}, "ecx"},
        {{RegisterFunction::ARG3, RegisterType::GPR, RegisterSize::SIZE_16}, "cx"},
        {{RegisterFunction::ARG3, RegisterType::GPR, RegisterSize::SIZE_8}, "cl"},

        {{RegisterFunction::ARG4, RegisterType::GPR, RegisterSize::SIZE_64}, "r8"},
        {{RegisterFunction::ARG4, RegisterType::GPR, RegisterSize::SIZE_32}, "r8d"},
        {{RegisterFunction::ARG4, RegisterType::GPR, RegisterSize::SIZE_16}, "r8w"},
        {{RegisterFunction::ARG4, RegisterType::GPR, RegisterSize::SIZE_8}, "r8b"},

        {{RegisterFunction::ARG5, RegisterType::GPR, RegisterSize::SIZE_64}, "r9"},
        {{RegisterFunction::ARG5, RegisterType::GPR, RegisterSize::SIZE_32}, "r9d"},
        {{RegisterFunction::ARG5, RegisterType::GPR, RegisterSize::SIZE_16}, "r9w"},
        {{RegisterFunction::ARG5, RegisterType::GPR, RegisterSize::SIZE_8}, "r9b"},

        {{RegisterFunction::REG, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm0"},
        {{RegisterFunction::REG_LEFT, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm1"},
        {{RegisterFunction::REG_RIGHT, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm2"},
        {{RegisterFunction::ARG0, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm0"},
        {{RegisterFunction::ARG1, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm1"},
        {{RegisterFunction::ARG2, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm2"},
        {{RegisterFunction::ARG3, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm3"},
        {{RegisterFunction::ARG4, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm4"},
        {{RegisterFunction::ARG5, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm5"}
    };

    auto it = regMap.find({vr.regFunc, vr.regType, vr.regSize});
    if (it == regMap.end()) {
        std::cerr << "Unknown combinaison of virtual register name, type and size\n";
        std::cerr << "RegFunc: " << static_cast<int>(vr.regFunc) << ", RegType: " << static_cast<int>(vr.regType) << ", RegSize: " << static_cast<int>(vr.regSize) << "\n";
        return "";
    }

    return "%" + it->second;
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
    start_block->gen_x86(o);
    for (BasicBlock* bb : bbs) {
        bb->gen_x86(o);
    }
    end_block->gen_x86(o);
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
    
    // Définir le bloc principal de la fonction qui sert d'épilogue
    string funcEpilogueBlockName = "$" + functionName + "_body_block";
    o << "    (block " << funcEpilogueBlockName << "\n";
    
    // Générer le prologue d'abord (toujours le bloc start_block)
    for (IRInstr* instr : start_block->instructions) {
        instr->gen_wat(o);
    }
    
    // Identifier les structures de while
    struct WhileInfo {
        BasicBlock* testBlock;   // Bloc de test de la condition
        BasicBlock* bodyBlock;   // Bloc du corps du while
        BasicBlock* endBlock;    // Bloc après le while
    };
    std::vector<WhileInfo> whileStructures;
    
    // Trouver les blocs de test de while
    for (BasicBlock* bb : bbs) {
        if (bb->label.find("_test_while") != std::string::npos) {
            // Trouver le JumpFalse qui définit les cibles de saut
            for (IRInstr* instr : bb->instructions) {
                if (instr->get_operation_name() == "jumpfalse") {
                    JumpFalse* jumpf = dynamic_cast<JumpFalse*>(instr);
                    if (jumpf) {
                        WhileInfo info;
                        info.testBlock = bb;
                        
                        // Chercher le bloc de corps correspondant
                        for (BasicBlock* bodyBB : bbs) {
                            if (bodyBB->label == jumpf->dest_true) {
                                info.bodyBlock = bodyBB;
                                break;
                            }
                        }
                        
                        // Chercher le bloc de fin correspondant
                        for (BasicBlock* endBB : bbs) {
                            if (endBB->label == jumpf->dest_false) {
                                info.endBlock = endBB;
                                break;
                            }
                        }
                        
                        if (info.bodyBlock && info.endBlock) {
                            whileStructures.push_back(info);
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Identifier les blocs faisant partie des structures if-else
    std::set<std::string> ifElseBlocks;
    for (BasicBlock* bb : bbs) {
        for (IRInstr* instr : bb->instructions) {
            if (instr->get_operation_name() == "jumpfalse" && bb->label.find("_test_while") == std::string::npos) {
                JumpFalse* jumpf = dynamic_cast<JumpFalse*>(instr);
                if (jumpf) {
                    // Ajouter les blocs destination au set des blocs if-else
                    ifElseBlocks.insert(jumpf->dest_true);
                    ifElseBlocks.insert(jumpf->dest_false);
                }
            }
        }
    }
    
    // Générer tous les blocs qui ne font pas partie des structures spéciales
    for (BasicBlock* bb : bbs) {
        if (bb != start_block && bb != end_block) {
            // Vérifier si ce bloc fait partie d'un while
            bool isPartOfWhile = false;
            for (const WhileInfo& info : whileStructures) {
                if (bb == info.testBlock || bb == info.bodyBlock) {
                    isPartOfWhile = true;
                    break;
                }
            }
            
            // Vérifier si ce bloc fait partie d'un if-else
            bool isPartOfIfElse = ifElseBlocks.find(bb->label) != ifElseBlocks.end();
            
            // Si le bloc n'est pas une partie spéciale, le générer normalement
            if (!isPartOfWhile && !isPartOfIfElse) {
                // Générer le bloc
                o << "      ;; Bloc " << bb->label << "\n";
                
                // Générer les instructions du bloc
                for (IRInstr* instr : bb->instructions) {
                    std::stringstream ss;
                    instr->gen_wat(ss);
                    std::string instr_str = ss.str();
                    // Indentation
                    size_t pos = 0;
                    while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                        instr_str.insert(pos + 1, "      ");
                        pos += 7;
                    }
                    o << instr_str;
                }
            }
        }
    }
    
    // Générer les structures if-else
    for (BasicBlock* bb : bbs) {
        // Ne pas traiter les blocs des while comme des if-else
        if (bb->label.find("_test_while") == std::string::npos) {
            for (IRInstr* instr : bb->instructions) {
                if (instr->get_operation_name() == "jumpfalse") {
                    o << "      ;; Structure if-else dans le bloc " << bb->label << "\n";
                    std::stringstream ss;
                    instr->gen_wat(ss);
                    std::string instr_str = ss.str();
                    // Indentation
                    size_t pos = 0;
                    while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                        instr_str.insert(pos + 1, "      ");
                        pos += 7;
                    }
                    o << instr_str;
                }
            }
        }
    }
    
    // Générer les structures while
    for (const WhileInfo& info : whileStructures) {
        o << "      ;; Structure while: test=" << info.testBlock->label 
          << ", corps=" << info.bodyBlock->label 
          << ", fin=" << info.endBlock->label << "\n";
        
        // Structure WAT d'un while:
        // block $end
        //   loop $continue
        //     <condition>
        //     br_if $end (i32.eqz <condition>)
        //     <body>
        //     br $continue
        //   end
        // end
        
        o << "      (block $" << info.endBlock->label << "\n";
        o << "        (loop $" << info.testBlock->label << "\n";
        
        // Générer le code de test (sans le JumpFalse)
        for (IRInstr* instr : info.testBlock->instructions) {
            if (instr->get_operation_name() != "jumpfalse") {
                std::stringstream ss;
                instr->gen_wat(ss);
                std::string instr_str = ss.str();
                // Indentation
                size_t pos = 0;
                while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                    instr_str.insert(pos + 1, "          ");
                    pos += 11;
                }
                o << instr_str;
            }
        }
        
        // Condition de sortie
        o << "          (br_if $" << info.endBlock->label << " (i32.eqz (local.get $reg)))\n";
        
        // Générer le corps du while (sans le Jump de retour)
        for (IRInstr* instr : info.bodyBlock->instructions) {
            if (instr->get_operation_name() != "jump") {
                std::stringstream ss;
                instr->gen_wat(ss);
                std::string instr_str = ss.str();
                // Indentation
                size_t pos = 0;
                while ((pos = instr_str.find('\n', pos)) != std::string::npos) {
                    instr_str.insert(pos + 1, "          ");
                    pos += 11;
                }
                o << instr_str;
            }
        }
        
        // Retour au début du while
        o << "          (br $" << info.testBlock->label << ")\n";
        o << "        )\n";
        o << "      )\n";
    }
    
    // Fermer le bloc d'épilogue principal
    o << "    )\n"; // Fin du bloc epilogue
    
    // Générer l'épilogue
    for (IRInstr* instr : end_block->instructions) {
        instr->gen_wat(o);
    }
    
    // Valeur de retour par défaut (ne devrait jamais être atteinte)
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
    stv.funcTable[functionName].offset -= typeSize(t);
    offset = stv.funcTable[functionName].offset;
    
    // Créer le vrai nom avec l'offset dans la pile
    string temp_name("!tmp"+offset);

    // Mettre à jour la table des symboles avec le nouveau nom
    stv.varTable[temp_name] = {.type = t, .name = temp_name, .offset = offset, .declared = true, .used = false};
    
    return temp_name;
}

int CFG::get_var_offset(string name) {
    return stv.varTable[name].offset;
}

Type CFG::get_var_type(string name) {
    return stv.varTable[name].type;
}

string CFG::new_BB_name() {
    stringstream ss;
    ss << functionName << "_" << bbs.size(); // je pense ça passe car on ajoute le block direct dans la liste après
    return ss.str();
}

