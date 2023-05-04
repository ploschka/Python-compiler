#pragma once
#include <pycom/token/Token.hpp>
#include <pycom/interface/LexerStateInterface.hpp>

class LexerInterface
{
public:
    virtual bool openFile(std::string filename) = 0;
    virtual Token getToken() = 0;
    virtual void setState(LexerStateInterface *state) = 0;
    virtual LexerStateInterface *getState() = 0;
    virtual unsigned int& getRow() = 0;
    virtual unsigned int& getPos() = 0;
    virtual void pushToQueue(Token token) = 0;
    virtual Token popFromQueue(Token token) = 0;
    virtual ~LexerInterface() = default;
};
