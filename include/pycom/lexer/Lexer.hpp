#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/lexeme/Lexeme.hpp>
#include <fstream>
#include <memory>

class Lexer: public LexerInterface
{
private:
    std::ifstream file;
    std::string buffer1;
    std::string buffer2;
    std::string accum;
    std::unique_ptr<LexerStateInterface> state;
    char currChar;
    bool eof;
    Type recognize() const;
public:
    bool openFile(std::string filename);
    void setState(LexerStateInterface* state);
    Lexeme getLexeme();
};
