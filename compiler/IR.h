#ifndef IR_H
#define IR_H

#include <vector>
#include <string>
#include <iostream>

#include "generated/ifccParser.h"
#include "SymbolTableGenVisitor.h"

class IRInstr;
class CFG;

enum class RegisterFunction {
	REG,
	REG_LEFT,
	REG_RIGHT,
	ARG0,
	ARG1,
	ARG2,
	ARG3,
	ARG4,
	ARG5,
	ARG6
};
enum class RegisterSize {
	SIZE_64 = 64,
	SIZE_32 = 32,
	SIZE_16 = 16,
	SIZE_8 = 8
};
enum class RegisterType {
	GPR,
	XMM
};
typedef struct VirtualRegister {
	RegisterFunction regFunc;
	RegisterSize regSize;
	RegisterType regType;

	VirtualRegister(RegisterFunction func, RegisterSize size, RegisterType type)
		: regFunc(func), regSize(size), regType(type) {}
	VirtualRegister(const VirtualRegister& other)
		: regFunc(other.regFunc), regSize(other.regSize), regType(other.regType) {}
	VirtualRegister& operator=(const VirtualRegister& other) {
		if (this != &other) {
			regFunc = other.regFunc;
			regSize = other.regSize;
			regType = other.regType;
		}
		return *this;
	}
} VirtualRegister;


/**  The class for a basic block */

/* A few important comments.
	IRInstr has no jump instructions.
	cmp_* instructions behaves as an arithmetic two-operand instruction (add or mult),
	returning a boolean value (as an int)

	Assembly jumps are generated as follows:
	BasicBlock::gen_asm() first calls IRInstr::gen_asm() on all its instructions, and then 
		if  exit_true  is a  nullptr, 
		the epilogue is generated
	else if exit_false is a nullptr, 
		an unconditional jmp to the exit_true branch is generated
			else (we have two successors, hence a branch)
		an instruction comparing the value of test_var_name to true is generated,
				followed by a conditional branch to the exit_false branch,
				followed by an unconditional branch to the exit_true branch
	The attribute test_var_name itself is defined when converting 
	the if, while, etc of the AST  to IR.

Possible optimization:
	a cmp_* comparison instructions, if it is the last instruction of its block, 
	generates an actual assembly comparison 
	followed by a conditional jump to the exit_false branch
*/

class BasicBlock {
public:
	BasicBlock(CFG* cfg, std::string entry_label);
	virtual ~BasicBlock();	
	void gen_x86(std::ostream &o); /**< x86 assembly code generation for this basic block */
	void gen_wat(std::ostream &o); /**< wat code generation for this basic block */

	// Méthode originale pour ajouter une instruction IRInstr (pour compatibilité)
	void add_IRInstr(IRInstr *instruction);

	// No encapsulation whatsoever here. Feel free to do better.
	BasicBlock* exit_true;  /**< pointer to the next basic block, true branch. If nullptr, return from procedure */ 
	BasicBlock* exit_false; /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */
	std::string label; /**< label of the BB, also will be the label in the generated code */
	CFG* cfg; /** < the CFG where this block belongs */
	std::vector<IRInstr*> instructions; /** < the operations with the new design. */
	std::string test_var_name;  /** < when generating IR code for an if(expr) or while(expr) etc,
								store here the name of the variable that holds the value of expr */

};




/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
	The entry block is the one with the same label as the AST function name.
		(it could be the first of bbs, or it could be defined by an attribute value)
	The exit block is the one with both exit pointers equal to nullptr.
	(again it could be identified in a more explicit way)

 */
class CFG {
public:
	CFG(SymbolTableGenVisitor& p_stv, const std::string& p_funcName);
	~CFG();

	void add_bb(BasicBlock* bb);

	// x86 code generation: could be encapsulated in a processor class in a retargetable compiler
	void gen_x86(std::ostream& o); /**< x86 assembly code generation for the whole CFG */
	void gen_wat(std::ostream& o); /**< wat code generation for the whole CFG */
	std::string IR_reg_to_x86(const std::string &reg);
	std::string IR_addr_to_x86 (const std::string &Iraddr);

	std::string IR_reg_to_x86(const VirtualRegister& reg);
	std::string IR_reg_to_wat(const std::string &reg); /**< helper method: inputs a IR reg, returns WebAssembly local variable name */
	std::string IR_addr_to_wat(const std::string &Iraddr);

	// symbol table methods
	// void add_to_symbol_table(std::string name, Type t); pas besoin car visiteur symbol table
	std::string create_new_tempvar(Type t);

	// Helper functions
	int get_var_offset(std::string name);
	Type get_var_type(std::string name);

	// basic block management
	std::string new_BB_name();
	BasicBlock* current_bb;
	BasicBlock* start_block;
	BasicBlock* end_block;
	SymbolTableGenVisitor& stv; /**< the visitor for the symbol table */
	std::vector<BasicBlock*> bbs; /**< all the basic blocks of this CFG*/
	std::string functionName;
};


#endif