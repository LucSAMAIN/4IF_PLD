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

        {{RegisterFunction::REG, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm6"},
        {{RegisterFunction::REG_LEFT, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm7"},
        {{RegisterFunction::REG_RIGHT, RegisterType::XMM, RegisterSize::SIZE_64}, "xmm8"},
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

std::string CFG::IR_reg_to_wat(const VirtualRegister& reg) {
    // Mapping of virtual register functions to base WAT variable names
    static const std::map<std::tuple<RegisterFunction, RegisterType>, std::string> baseNameMap = {
        {{RegisterFunction::REG, RegisterType::GPR}, "reg"},
        {{RegisterFunction::REG, RegisterType::XMM}, "reg"},
        {{RegisterFunction::REG_LEFT, RegisterType::GPR}, "regLeft"},
        {{RegisterFunction::REG_LEFT, RegisterType::XMM}, "regLeft"},
        {{RegisterFunction::REG_RIGHT, RegisterType::GPR}, "regRight"},
        {{RegisterFunction::REG_RIGHT, RegisterType::XMM}, "regRight"},
        {{RegisterFunction::ARG0, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG0, RegisterType::XMM}, "!arg"},
        {{RegisterFunction::ARG1, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG1, RegisterType::XMM}, "!arg"},
        {{RegisterFunction::ARG2, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG2, RegisterType::XMM}, "!arg"},
        {{RegisterFunction::ARG3, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG3, RegisterType::XMM}, "!arg"},
        {{RegisterFunction::ARG4, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG4, RegisterType::XMM}, "!arg"},
        {{RegisterFunction::ARG5, RegisterType::GPR}, "!arg"},
        {{RegisterFunction::ARG5, RegisterType::XMM}, "!arg"}
    };

    auto it = baseNameMap.find({reg.regFunc, reg.regType});
    if (it == baseNameMap.end()) {
        std::cerr << "Unknown combination of virtual register function and type\n";
        std::cerr << "RegFunc: " << static_cast<int>(reg.regFunc) << ", RegType: " << static_cast<int>(reg.regType) << "\n";
        return ""; // Or throw an exception
    }

    std::string baseName = it->second;
    std::string sizeSuffix;
    std::string argNumStr;

    // Determine size suffix based on RegisterSize or RegisterType for XMM
    if (reg.regType == RegisterType::XMM || reg.regSize == RegisterSize::SIZE_64) {
        sizeSuffix = "_64";
    } else if (reg.regSize == RegisterSize::SIZE_32 || reg.regSize == RegisterSize::SIZE_16 || reg.regSize == RegisterSize::SIZE_8) {
         // Treat 8, 16, 32 bits GPR as i32 in WAT for simplicity currently
        sizeSuffix = "_32";
    } else {
        std::cerr << "Unknown register size: " << static_cast<int>(reg.regSize) << "\n";
        return ""; // Or throw an exception
    }

    // Append argument number if it's an argument register
    if (reg.regFunc >= RegisterFunction::ARG0 && reg.regFunc <= RegisterFunction::ARG5) {
        argNumStr = std::to_string(static_cast<int>(reg.regFunc) - static_cast<int>(RegisterFunction::ARG0));
    }

    // Construct the final WAT variable name
    return "$" + baseName + sizeSuffix + argNumStr;
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

    // Déclaration des arguments
    if (stv.funcTable.find(functionName) != stv.funcTable.end()) {
        // Pour chaque argument de la fonction (jusqu'à 6 maximum)
        int numArgs = std::min(6, static_cast<int>(stv.funcTable[functionName].args.size()));
        for (int i = 0; i < numArgs; i++) {
            // Déclarer chaque argument comme un paramètre i32
            if (stv.funcTable[functionName].args[i]->type == Type::INT32_T) {
                o << " (param $!arg_32" << i << " i32)";   
            } else if (stv.funcTable[functionName].args[i]->type == Type::FLOAT64_T) {
                o << " (param $!arg_64" << i << " f64)";
            }
        }
    }
    // Gérer le type de retour de la fonction
    Type returnType = stv.funcTable[functionName].type;
    if (returnType == Type::FLOAT64_T) {
        o << " (result f64)";
    } else if (returnType == Type::INT32_T || returnType == Type::INT8_T || returnType == Type::INT64_T) { // Supposons que char/int retournent i32
        o << " (result i32)";
    } // Ne rien générer pour Type::VOID_T ou autres types non gérés explicitement ici

    o << "\n"; // Nouvelle ligne après les paramètres/resultat

    o << "    (local $reg_32 i32)\n";      // Registre pour les résultats
    o << "    (local $regLeft_32 i32)\n";  // Registre pour l'opérande gauche
    o << "    (local $regRight_32 i32)\n"; // Registre pour l'opérande droite
    o << "    (local $bp i32)\n";       // Base pointer
    o << "    (local $reg_64 f64)\n";      // Registre pour les résultats
    o << "    (local $regLeft_64 f64)\n";  // Registre pour l'opérande gauche
    o << "    (local $regRight_64 f64)\n"; // Registre pour l'opérande droite
    
    // Si c'est la fonction main, déclarer les variables locales pour les arguments
    if (functionName == "main") {
        for (int i = 0; i < 6; i++) {
            o << "    (local $!arg_32" << i << " i32)\n";
            o << "    (local $!arg_64" << i << " f64)\n";
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
        string whileLabel;       // Étiquette unique pour cette boucle while
    };
    std::map<string, WhileInfo> whileStructures;
    std::map<string, bool> blockProcessed;
    
    // Trouver les blocs de test de while
    for (BasicBlock* bb : bbs) {
        blockProcessed[bb->label] = false;
        if (bb->label.find("_test_while") != std::string::npos) {
            // Trouver le JumpFalse qui définit les cibles de saut
            for (IRInstr* instr : bb->instructions) {
                if (instr->get_operation_name() == "jumpfalse") {
                    JumpFalse* jumpf = dynamic_cast<JumpFalse*>(instr);
                    if (jumpf) {
                        WhileInfo info;
                        info.testBlock = bb;
                        info.whileLabel = "while_" + bb->label;
                        
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
                            whileStructures[info.endBlock->label] = info;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Parcourir les blocs dans l'ordre et générer le code
    for (BasicBlock* bb : bbs) {
        if (bb != start_block && bb != end_block && !blockProcessed[bb->label]) {
            // Si ce bloc est un bloc de fin de while, générer toute la structure while
            if (whileStructures.find(bb->label) != whileStructures.end()) {
                std::cerr << "# Processing while block: " << bb->label << "\n";
                WhileInfo& info = whileStructures[bb->label];
                
                // Marquer les blocs comme traités
                blockProcessed[info.testBlock->label] = true;
                blockProcessed[info.bodyBlock->label] = true;
                blockProcessed[info.endBlock->label] = true;
                
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
                o << "          (br_if $" << info.endBlock->label << " (i32.eqz (local.get $reg_32)))\n";
                
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
                
                // Maintenant générer le bloc après le while
                o << "      ;; Bloc " << info.endBlock->label << " (après le while)\n";
                for (IRInstr* instr : info.endBlock->instructions) {
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
            // Traiter les structures if/else
            else if (bb->label.find("_if_") != std::string::npos && bb->label.find("_endif") == std::string::npos) {
                std::cerr << "# Processing if block: " << bb->label << "\n";
                // Identifie les blocs associés à cette structure if/else
                std::string prefix = bb->label.substr(0, bb->label.find_last_of('_'));
                std::string trueBlockLabel, falseBlockLabel, endifBlockLabel;
                
                // Trouver le JumpFalse dans ce bloc pour déterminer les destinations
                JumpFalse* jumpf = nullptr;
                for (IRInstr* instr : bb->instructions) {
                    if (instr->get_operation_name() == "jumpfalse") {
                        jumpf = dynamic_cast<JumpFalse*>(instr);
                        break;
                    }
                }
                
                if (jumpf) {
                    trueBlockLabel = jumpf->dest_true;
                    falseBlockLabel = jumpf->dest_false;
                    
                    // Trouver le bloc endif correspondant (habituellement la cible d'un saut depuis true ou false)
                    for (BasicBlock* endifBB : bbs) {
                        if (endifBB->label.find(prefix) != std::string::npos && 
                            endifBB->label.find("_endif") != std::string::npos) {
                            endifBlockLabel = endifBB->label;
                            break;
                        }
                    }
                    
                    // Trouver les pointeurs vers les blocs correspondants
                    BasicBlock* trueBlock = nullptr;
                    BasicBlock* falseBlock = nullptr;
                    BasicBlock* endifBlock = nullptr;
                    
                    for (BasicBlock* searchBB : bbs) {
                        if (searchBB->label == trueBlockLabel) trueBlock = searchBB;
                        if (searchBB->label == falseBlockLabel) falseBlock = searchBB;
                        if (searchBB->label == endifBlockLabel) endifBlock = searchBB;
                    }
                    
                    // Si on a trouvé tous les blocs, générer le code if/else
                    if (trueBlock && falseBlock && endifBlock) {
                        // Marquer tous les blocs comme traités
                        blockProcessed[bb->label] = true;
                        blockProcessed[trueBlock->label] = true;
                        blockProcessed[falseBlock->label] = true;
                        blockProcessed[endifBlock->label] = true;
                        
                        o << "      ;; Structure if-else: test=" << bb->label 
                          << ", true=" << trueBlock->label 
                          << ", false=" << falseBlock->label 
                          << ", endif=" << endifBlock->label << "\n";
                        
                        // Générer le code de test (sans le JumpFalse)
                        for (IRInstr* instr : bb->instructions) {
                            if (instr->get_operation_name() != "jumpfalse") {
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
                        
                        // Vérifier si le bloc false est un bloc endif ou un vrai bloc else
                        bool hasElse = (falseBlock->label.find("_if_false") != std::string::npos);
                        
                        // Si le bloc false est un bloc endif, alors il n'y a pas de branche else dans le code C
                        if (!hasElse) {
                            // Générer seulement la partie then
                            o << "      (if (local.get $reg_32)\n";
                            o << "        (then\n";
                            o << "          ;; Exécution du bloc " << trueBlock->label << "\n";
                            
                            // Générer le code du bloc true
                            for (IRInstr* instr : trueBlock->instructions) {
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
                            
                            o << "        )\n";
                            o << "      )\n";
                        } else {
                            // Générer la structure if-else
                            o << "      (if (local.get $reg_32)\n";
                            o << "        (then\n";
                            o << "          ;; Exécution du bloc " << trueBlock->label << "\n";
                            
                            // Générer le code du bloc true
                            for (IRInstr* instr : trueBlock->instructions) {
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
                            
                            o << "        )\n";
                            o << "        (else\n";
                            o << "          ;; Exécution du bloc " << falseBlock->label << "\n";
                            
                            // Générer le code du bloc false
                            for (IRInstr* instr : falseBlock->instructions) {
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
                            
                            o << "        )\n";
                            o << "      )\n";
                        }
                        
                        // Générer le bloc endif (après la structure if/else)
                        std::cerr << "# Processing endif block: " << endifBlock->label << " with " << endifBlock->instructions.size() << " instructions\n";
                        o << "      ;; Bloc " << endifBlock->label << " (après le if-else)\n";
                        for (IRInstr* instr : endifBlock->instructions) {
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
            // Sinon, si c'est un bloc de test/corps de while, il sera traité avec sa structure while
            else if (bb->label.find("_test_while") != std::string::npos || 
                    bb->label.find("_while_true") != std::string::npos ||
                    // Éviter de traiter séparément les blocs if/else/endif qui ont déjà été traités
                    bb->label.find("_if_true") != std::string::npos ||
                    bb->label.find("_if_false") != std::string::npos) {
                std::cerr << "# Skipping block (already processed): " << bb->label << "\n";
                continue;
            }
            // Sinon, génération normale du bloc
            else {
                std::cerr << "# Processing normal block: " << bb->label << " with " << bb->instructions.size() << " instructions\n";
                blockProcessed[bb->label] = true;
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
    
    // Fermer le bloc d'épilogue principal
    o << "    )\n"; // Fin du bloc epilogue
    
    // Générer l'épilogue
    for (IRInstr* instr : end_block->instructions) {
        instr->gen_wat(o);
    }
    
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

