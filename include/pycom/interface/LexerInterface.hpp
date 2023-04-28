#pragma once
#include <pycom/lexeme/Lexeme.hpp>
#include <pycom/interface/LexerStateInterface.hpp>

class LexerInterface
{
public:
    virtual bool openFile(std::string filename) = 0;
    virtual Lexeme getLexeme() = 0;
    virtual void setState(LexerStateInterface *state) = 0;
    virtual LexerStateInterface *getState() = 0;
    virtual ~LexerInterface() = default;
};
