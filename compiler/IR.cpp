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
    start_block->gen_x86(o);
    for (BasicBlock* bb : bbs) {
        bb->gen_x86(o);
    }
    end_block->gen_x86(o);
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