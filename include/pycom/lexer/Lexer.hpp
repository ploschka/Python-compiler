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
    Type token;
    std::unique_ptr<LexerStateInterface> state;
    std::string::const_iterator iter;
    bool eof;
    Type recognize(const std::string& id) const;
public:
    bool openFile(std::string filename);
    void setState(LexerStateInterface* state);
    Lexeme getLexeme();
};
