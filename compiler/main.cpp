#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "CodeGenVisitor.h"
#include "SymbolTableGenVisitor.h"

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
    //     cout << var.first << " : " << var.second.type << " at " << var.second.offset;
    //     if (var.second.used)
    //     {
    //         cout << " (used)";
    //     }
    //     if (var.second.declared)
    //     {
    //         cout << " (declared)";
    //     }
    //     cout << "\n";
    // }

    CodeGenVisitor cgv(stv.getSymbolTable());
    cgv.visit(tree);

    return 0;
}
