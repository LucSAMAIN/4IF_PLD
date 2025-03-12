#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CodeGenVisitor.h"
#include "IRGenVisitor.h"
#include "SymbolTableGenVisitor.h"
#include "IR.h"

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

    SymbolTableGenVisitor stv;
    stv.visit(tree);

    // for (auto const &var : stv.getSymbolTable())
    // {
    //     std::cout << var.first << " : " << var.second.type << " offset: " << var.second.offset;
    //     if (var.second.used)
    //     {
    //         std::cout << " (used)";
    //     }
    //     if (var.second.declared)
    //     {
    //         std::cout << " (declared)";
    //     }
    //     std::cout << "\n";
    // }

    IRGenVisitor cgv(stv.symbolTable);
    cgv.visit(tree);

    // Récupération du CFG généré
    CFG* cfg = cgv.getCFG();
    
    if (cfg) {
        // Affichage de la représentation intermédiaire
        std::cout << "// Programme en représentation intermédiaire (IR) véritable :\n";
        cfg->gen_asm(std::cout);
        
        // Génération du code x86 à partir de l'IR
        std::cout << "\n// Génération du code assembleur x86 à partir de l'IR :\n";
        cfg->gen_x86(std::cout);
    }

    return 0;
}
