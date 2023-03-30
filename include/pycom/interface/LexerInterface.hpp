#pragma once
#include <pycom/lexer/Lexeme.hpp>

class LexerInterface
{
public:
    virtual bool openFile(std::string filename) = 0;
    virtual Lexeme getLexeme() const = 0;
};
