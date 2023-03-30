#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/lexeme/Lexeme.hpp>
#include <fstream>

class Lexer: public LexerInterface
{
private:
    std::ifstream file;
    std::string buffer1;
    std::string buffer2;
public:
    bool openFile(std::string filename);
    Lexeme getLexeme() const;
};
