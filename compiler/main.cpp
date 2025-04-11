#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"

#include "IRGenVisitor.h"
#include "TypeCheckVisitor.h"
#include "ContinueBreakCheckVisitor.h"

using namespace antlr4;

void wat_init(std::ostream& o) {
    o << "(module\n ;; Import de putchar depuis l'environnement hôte\n (import \"env\" \"putchar\" (func $putchar (param i32) (result i32)))\n";
    o << "  ;; Déclaration de la mémoire\n";
    o << "  (memory 1)\n";  
    o << "  (export \"memory\" (memory 0))\n";  // Exporter la mémoire pour pouvoir l'accéder depuis JS
    o << "  (global $sp (mut i32) (i32.const 1024))\n";  // Commencer avec un stack pointer non nul
}

void wat_end(std::ostream& o) {
    o << ")\n";
}

int main(int argc, const char **argv)
{
    std::stringstream in;
    bool wasm = false;
    std::string input_file;
    std::string output_file;
    
    // Parse command line arguments
    for(int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if(arg == "-w" || arg == "--wat") {
            wasm = true;
        } else if(arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else {
            input_file = arg;
        }
    }
    
    if(input_file.empty()) {
        std::cerr << "usage: ifcc [-w|--wat] [-o output_file] path/to/file.c\n";
        return 1;
    }

    // Lecture du fichier d'entrée
    std::ifstream lecture(input_file);
    if (!lecture.good()) {
        std::cerr << "error: cannot read file: " << input_file << "\n";
        return 1;
    }
    in << lecture.rdbuf();

    ANTLRInputStream input(in.str());
    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();

    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0) {
        exit(1);
    }

    ContinueBreakCheckVisitor cbv(input);
    cbv.visit(tree);
    if (cbv.getNumberError() != 0) {
        exit(1);
    }

    SymbolTableGenVisitor stv(input);
    stv.visit(tree);

    if (stv.getNumberError() != 0) {
        exit(1);
    }

    // Print the content of the variable table
    if (verbose) {
        out << "# Variable Table:\n";
        for (const auto &entry : stv.varTable)
        {
            out << "# Name: " << entry.first << ", Type: " << typeToString(entry.second.type)
                    << ", offset: " << entry.second.offset << ", declared: " << entry.second.declared << ", used: " << entry.second.used << "\n";
        }

        // Print the content of the function table
        out << "# Function Table:\n";
        for (const auto &entry : stv.funcTable)
        {
            out << "# Name: " << entry.first << ", Return Type: " << typeToString(entry.second.type)
                    << ", Parameters: ";
            for (const auto &param : entry.second.args)
            {
                out << typeToString(param->type) << " " << param->name << ", ";
            }
            out << "\n";
        }
    }

    TypeCheckVisitor tcv(input, stv);
    tcv.visit(tree);
    if (tcv.getNumberError() != 0) {
        exit(1);
    }

    IRGenVisitor cgv(stv);
    cgv.visit(tree);

    // Récupération du CFG généré
    std::vector<CFG*> cfgs = cgv.getCFGs();

    if (cfgs.size() > 0) {        
        if (wasm) {
            if(has_output_file) {
                wat_init(output_file);  
                // Génère le code pour tous les CFG
                for (auto cfg : cfgs) {
                    cfg->gen_wat(output_file);
                }
                wat_end(output_file);
            } else {
                wat_init(std::cout);
                for (auto cfg : cfgs) {
                    cfg->gen_wat(std::cout);    
                }
                wat_end(std::cout);
            }
        } else {
            if(has_output_file) {
                output_file << ".text\n";
                output_file << ".globl main\n";
                // Génère le code pour tous les CFG
                for (auto cfg : cfgs) {
                    cfg->gen_x86(output_file);
                }
            } else {
                std::cout << ".text\n";
                std::cout << ".globl main\n";
                for (auto cfg : cfgs) {
                    cfg->gen_x86(std::cout);
                }
            }
        }
        
        // Libération de la mémoire
        for (auto cfg : cfgs) {
            delete cfg;
        }
    }

    return 0;
}
