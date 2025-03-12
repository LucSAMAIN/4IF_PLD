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
    string reg_dst = params[0];
    
    switch(op) {
        case ldconst:
            o << "    movl $" << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            break;
            
        case copy:
            if (params[1][0] == '%') {
                // Copie depuis un registre
                o << "    movl " << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else {
                // Copie depuis une variable
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            }
            break;
            
        case add:
            if (params[1][0] == '%' && params[2][0] == '%') {
                // Les deux opérandes sont des registres
                o << "    movl " << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    addl " << params[2] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else if (params[1][0] == '%') {
                // Le premier est un registre, le second est une variable
                o << "    movl " << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else if (params[2][0] == '%') {
                // Le premier est une variable, le second est un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    addl " << params[2] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else {
                // Les deux sont des variables
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    addl " << bb->cfg->IR_reg_to_asm(params[2]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            }
            break;
            
        case sub:
            if (params[1][0] == '%' && params[2][0] == '%') {
                // Les deux opérandes sont des registres
                o << "    movl " << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    subl " << params[2] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else if (params[1][0] == '%') {
                // Le premier est un registre, le second est une variable
                o << "    movl " << params[1] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    subl " << bb->cfg->IR_reg_to_asm(params[2]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else if (params[2][0] == '%') {
                // Le premier est une variable, le second est un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    subl " << params[2] << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else {
                // Les deux sont des variables
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
                o << "    subl " << bb->cfg->IR_reg_to_asm(params[2]) << ", " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            }
            break;
            
        case mul:
            if (params[2][0] != '%') {
                // Si le deuxième opérande n'est pas un registre, le charger dans %ebx
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
                if (params[1][0] == '%') {
                    o << "    movl " << params[1] << ", %eax" << endl;
                } else {
                    o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
                }
                o << "    imul %ebx, %eax" << endl;
                o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            } else {
                // Sinon, opération directe
                if (params[1][0] == '%') {
                    o << "    movl " << params[1] << ", %eax" << endl;
                } else {
                    o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
                }
                o << "    imul " << params[2] << ", %eax" << endl;
                o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            }
            break;
            
        case cmp_eq:
            if (params[1][0] != '%') {
                // Charger le premier opérande dans %eax si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            } else {
                o << "    movl " << params[1] << ", %eax" << endl;
            }
            
            if (params[2][0] != '%') {
                // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
                o << "    cmpl %ebx, %eax" << endl;
            } else {
                o << "    cmpl " << params[2] << ", %eax" << endl;
            }
            
            o << "    sete %al" << endl;
            o << "    movzbl %al, %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            break;
            
        case cmp_lt:
            if (params[1][0] != '%') {
                // Charger le premier opérande dans %eax si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            } else {
                o << "    movl " << params[1] << ", %eax" << endl;
            }
            
            if (params[2][0] != '%') {
                // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
                o << "    cmpl %ebx, %eax" << endl;
            } else {
                o << "    cmpl " << params[2] << ", %eax" << endl;
            }
            
            o << "    setl %al" << endl;
            o << "    movzbl %al, %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            break;
            
        case cmp_le:
            if (params[1][0] != '%') {
                // Charger le premier opérande dans %eax si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[1]) << ", %eax" << endl;
            } else {
                o << "    movl " << params[1] << ", %eax" << endl;
            }
            
            if (params[2][0] != '%') {
                // Charger le deuxième opérande dans %ebx si ce n'est pas un registre
                o << "    movl " << bb->cfg->IR_reg_to_asm(params[2]) << ", %ebx" << endl;
                o << "    cmpl %ebx, %eax" << endl;
            } else {
                o << "    cmpl " << params[2] << ", %eax" << endl;
            }
            
            o << "    setle %al" << endl;
            o << "    movzbl %al, %eax" << endl;
            o << "    movl %eax, " << bb->cfg->IR_reg_to_asm(reg_dst) << endl;
            break;
            
        default:
            o << "    # Opération non implémentée: " << op << endl;
    }
}

// Implémentation de BasicBlock
BasicBlock::BasicBlock(CFG* cfg, string entry_label) :
    exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg), instrs() {}

// Génère une représentation textuelle du bloc de base
void BasicBlock::gen_asm(ostream& o) {
    o << "BASIC_BLOCK " << label << ":" << endl;
    
    // Générer le code pour toutes les instructions
    for (IRInstr* instr : instrs) {
        instr->gen_asm(o);
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
    
    // Générer le code pour toutes les instructions
    for (IRInstr* instr : instrs) {
        instr->gen_x86(o);
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
    IRInstr* instr = new IRInstr(this, op, t, params);
    instrs.push_back(instr);
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
    int frameSize = 16 + ((-nextFreeSymbolIndex + 15) & ~15);  // Alignement sur 16 octets
    if (frameSize > 16) {
        o << "    subq $" << frameSize << ", %rsp" << endl;
    }
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
    stringstream ss;
    ss << "!tmp" << nextBBnumber++;
    add_to_symbol_table(ss.str(), t);
    return ss.str();
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