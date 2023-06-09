#pragma once
#include <pycom/interface/ParserInterface.hpp>

class Parser : public ParserInterface
{
public:
    void setLexer(LexerInterface*);
    AST* getAST();
};
