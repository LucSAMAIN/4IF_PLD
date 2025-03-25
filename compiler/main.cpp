#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "IRGenVisitor.h"
#include "SymbolTableGenVisitor.h"
#include "IR.h"

using namespace antlr4;

void wat_init(std::ostream& o) {
    o << "(module\n";
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
        std::cerr << "error: syntax error during parsing\n";
        return 1;
    }

    SymbolTableGenVisitor stv;
    stv.visit(tree);

    for (auto const &var : stv.symbolTable)
    {
        std::cout << "# " << var.first << " : type " << fromTypeToString(var.second.type) << " offset: " << var.second.offset << " index_arg: " << var.second.index_arg;
        if (var.second.used)
        {
            std::cout << " (used)";
        }
        if (var.second.declared)
        {
            std::cout << " (declared)";
        }
        std::cout << "\n";
    }
    for (auto const &var : stv.offsetTable)
    {
        std::cout << "# " << var.first << " : " << var.second << "\n";
    }

    IRGenVisitor cgv(stv);
    cgv.visit(tree);

    // Récupération du CFG généré
    std::vector<CFG*> cfgs = cgv.getCFGs();

    if (cfgs.size() > 0) {        
        if (wasm) {
            std::ofstream outFile;
            if(!output_file.empty()) {
                outFile.open(output_file);
                if(!outFile.is_open()) {
                    std::cerr << "error: cannot write to file: " << output_file << std::endl;
                    return 1;
                }
                wat_init(outFile);  
                // Génère le code pour tous les CFG
                for (auto cfg : cfgs) {
                    cfg->gen_wat(outFile);
                }
                wat_end(outFile);
                outFile.close();
            } else {
                wat_init(std::cout);
                for (auto cfg : cfgs) {
                    cfg->gen_wat(std::cout);    
                }
                wat_end(std::cout);
            }
        } else {
            std::ofstream outFile;
            if(!output_file.empty()) {
                outFile.open(output_file);
                if(!outFile.is_open()) {
                    std::cerr << "error: cannot write to file: " << output_file << std::endl;
                    return 1;
                }
                outFile << ".text\n";
                outFile << ".globl main\n";
                // Génère le code pour tous les CFG
                for (auto cfg : cfgs) {
                    cfg->gen_x86(outFile);
                }
                outFile.close();
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
