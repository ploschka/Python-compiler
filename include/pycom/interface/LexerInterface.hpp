#pragma once
#include <pycom/token/Token.hpp>
#include <pycom/interface/LexerStateInterface.hpp>

class LexerInterface
{
public:
    virtual void open(std::istream& stream) = 0;
    virtual Token getToken() = 0;
    virtual void setState(LexerStateInterface *state) = 0;
    virtual LexerStateInterface *getState() = 0;
    virtual void pushToQueue(Token token) = 0;
    virtual Token popFromQueue(Token token) = 0;
    virtual ~LexerInterface() = default;
};
