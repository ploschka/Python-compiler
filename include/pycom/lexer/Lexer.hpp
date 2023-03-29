#pragma once
#include "pycom/interface/LexerInterface.hpp"
#include "pycom/lexer/Lexeme.hpp"
#include <memory>

class Lexer: public LexerInterface
{
private:
    std::unique_ptr<char> buffer1;
    std::unique_ptr<char> buffer2;
public:
    int openFile(std::string filename);
    Lexeme getLexeme() const;
};
