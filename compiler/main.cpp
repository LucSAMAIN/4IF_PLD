#include <getopt.h>
#include <fstream>

#include "generated/ifccLexer.h"

#include "IRGenVisitor.h"
#include "TypeCheckVisitor.h"
#include "ContinueBreakCheckVisitor.h"

using namespace antlr4;

int main(int argc, char* argv[])
{
    std::stringstream in;
    if (argc < 2) {
        std::cerr << "usage: ifcc path/to/file.c\n";
        exit(1);
    }
    

    int option;
    bool has_output_file = false, verbose = false, wat = false;
    std::ofstream output_file;
    while ((option = getopt(argc, argv, "ho:vw")) != -1) {
        switch (option) {
            case 'h':
                std::cout << "usage: ifcc [-h] [-o output_file] [-v] path/to/file.c\n";
                std::cout << "options:\n";
                std::cout << "\t-h\t\tDisplay this help message\n";
                std::cout << "\t-o <file>\tOutput the backend code to the specified file\n";
                std::cout << "\t-v\t\tVerbose, print the symbol table in the output\n";
                std::cout << "\t-w\t\tGenerate WebAssembly code instead of x86_64\n";
                exit(0);
            case 'o':
                output_file.open(optarg);
                if (!output_file.good())
                {
                    std::cerr << "error: cannot open file: " << optarg << "\n";
                    exit(1);
                }
                has_output_file = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'w':
                wat = true;
                break;
            default:
                std::cerr << "bad option: -" << (char)option << "\n";
                exit(1);
        }
    }
    if (optind < argc) {
        std::ifstream lecture(argv[optind]);
        if (!lecture.good()) {
            std::cerr << "error: cannot read file: " << argv[optind] << "\n";
            exit(1);
        }
        in << lecture.rdbuf();
    }
    else {
        std::cerr << "error: no input file\n";
        exit(1);
    }
    std::ostream& out = has_output_file ? output_file : std::cout;

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
    if (wat) {
        out << "(module\n ;; Import de putchar depuis l'environnement hôte\n (import \"env\" \"putchar\" (func $putchar (param i32) (result i32)))\n";
        out << "  ;; Déclaration de la mémoire\n";
        out << "  (memory 1)\n";  
        out << "  (export \"memory\" (memory 0))\n";  // Exporter la mémoire pour pouvoir l'accéder depuis JS
        out << "  (global $sp (mut i32) (i32.const 1024))\n";  // Commencer avec un stack pointer non nul
        for (CFG* cfg : cfgs) {
            cfg->gen_wat(out);
            delete cfg;
        }
        out << ")\n";
    }
    else{
        out << ".text\n";
        out << ".globl main\n";
        for (CFG* cfg : cfgs) {
            cfg->gen_x86(out);
            delete cfg;
        }
    }    


    return 0;
}
