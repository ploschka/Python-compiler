#pragma once
#include <pycom/token/Token.hpp>
#include <pycom/interface/LexerStateInterface.hpp>

class LexerInterface
{
public:
    virtual void open(std::istream &_stream) = 0;
    virtual Token getToken() = 0;
    virtual void setState(LexerStateInterface *_state) = 0;
    virtual ~LexerInterface() = default;
};
