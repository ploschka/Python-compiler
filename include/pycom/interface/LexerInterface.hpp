#pragma once
#include <pycom/lexeme/Lexeme.hpp>

class LexerInterface
{
public:
    virtual bool openFile(std::string filename) = 0;
    virtual Lexeme getLexeme() const = 0;
    virtual ~LexerInterface() = default;
};
