#include <iostream>
#include <sstream>
#include <string>

#include "ErrorVisitor.h"


ErrorVisitor::ErrorVisitor(antlr4::ANTLRInputStream& input) : ifccBaseVisitor(), error(0), inputStream(input)
{

}

void ErrorVisitor::reportError(const std::string& message, antlr4::ParserRuleContext* ctx) {
    antlr4::Token* token = ctx->getStart();
    int line = token->getLine();
    int column = token->getCharPositionInLine();

    std::string sourceLine = getSourceLine(line);

    std::cerr << message << "\n";
    std::cerr << "  at line " << line << ":" << column << "\n";
    std::cerr << "    " << sourceLine << "\n";
    std::cerr << "    " << std::string(column, ' ') << "^\n";
}

std::string ErrorVisitor::getSourceLine(int lineNumber) {
    std::string inputText = inputStream.toString();
    std::istringstream iss(inputText);
    std::string line;
    int currentLine = 1;

    // Read until the desired line
    while (std::getline(iss, line) && currentLine < lineNumber) {
        currentLine++;
    }
    return currentLine == lineNumber ? line : "<unknown line>";
}
