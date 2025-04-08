#ifndef ERRORVISITOR_H
#define ERRORVISITOR_H

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class ErrorVisitor : public ifccBaseVisitor
{
public:
    ErrorVisitor(antlr4::ANTLRInputStream& input);
    virtual ~ErrorVisitor() {}

    int getNumberError() { return error; }
    void reportError(const std::string& message, antlr4::ParserRuleContext* ctx);
    std::string getSourceLine(int lineNumber);

protected:
    int error;
    antlr4::ANTLRInputStream& inputStream;
};

#endif // ERRORVISITOR_H
