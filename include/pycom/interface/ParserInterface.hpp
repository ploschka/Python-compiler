#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/AST/AST.hpp>

class ParserInterface
{
public:
    virtual void setLexer(LexerInterface *) = 0;
    virtual AST *getAST() = 0;
    virtual ~ParserInterface() = default;
};
