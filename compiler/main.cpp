
#include "generated/ifccLexer.h"

#include "IRGenVisitor.h"
#include "TypeCheckVisitor.h"
#include "ContinueBreakCheckVisitor.h"

using namespace antlr4;

int main(int argn, const char **argv)
{
    std::stringstream in;
    if (argn == 2)
    {
        std::ifstream lecture(argv[1]);
        if (!lecture.good())
        {
            std::cerr << "error: cannot read file: " << argv[1] << "\n";
            exit(1);
        }
        in << lecture.rdbuf();
    }
    else
    {
        std::cerr << "usage: ifcc path/to/file.c\n";
        exit(1);
    }

    ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0)
    {
        std::cerr << "error: syntax error during parsing\n";
        exit(1);
    }

    ContinueBreakCheckVisitor cbv;
    cbv.visit(tree);
    if (cbv.getNumberError() != 0)
    {
        std::cerr << "error: continue/break error\n";
        exit(1);
    }

    SymbolTableGenVisitor stv;
    stv.visit(tree);

    if (stv.getErrorCount() != 0)
    {
        std::cerr << "error: symbol table generation error\n";
        exit(1);
    }

    // Print the content of the variable table
    std::cout << "# Variable Table:\n";
    for (const auto &entry : stv.varTable)
    {
        std::cout << "# Name: " << entry.first << ", Type: " << typeToString(entry.second.type)
                << ", offset: " << entry.second.offset << ", declared: " << entry.second.declared << ", used: " << entry.second.used << "\n";
    }

    // Print the content of the function table
    std::cout << "# Function Table:\n";
    for (const auto &entry : stv.funcTable)
    {
        std::cout << "# Name: " << entry.first << ", Return Type: " << typeToString(entry.second.type)
                << ", Parameters: ";
        for (const auto &param : entry.second.args)
        {
            std::cout << typeToString(param->type) << " " << param->name << ", ";
        }
        std::cout << "\n";
    }

    TypeCheckVisitor tcv(stv);
    tcv.visit(tree);
    if (tcv.getNumberTypeError() != 0)
    {
        std::cerr << "error: type error during type checking\n";
        exit(1);
    }

    IRGenVisitor cgv(stv);
    cgv.visit(tree);

    // Récupération du CFG généré
    std::vector<CFG*> cfgs = cgv.getCFGs();
    
    // gen_x86
    std::cout << ".text\n";
    std::cout << ".globl main\n";
    for (CFG* cfg : cfgs) {
        cfg->gen_x86(std::cout);
        delete cfg;
    }

    return 0;
}
