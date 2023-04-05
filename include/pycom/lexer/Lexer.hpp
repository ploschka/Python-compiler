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
    std::string* currBuff;
    std::string* otherBuff;
    std::string accum;
    Type type;
    std::unique_ptr<LexerStateInterface> state;
    std::string::const_iterator iter;
    unsigned int row;
    unsigned int pos;
    Type recognize(const std::string& id) const;
public:
    bool openFile(std::string filename);
    void setState(LexerStateInterface* state);
    Lexeme getLexeme();
};
