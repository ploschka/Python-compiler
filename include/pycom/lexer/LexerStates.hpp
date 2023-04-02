#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <memory>

class BaseLexerState: public LexerStateInterface
{
protected:
    LexerInterface* lexer;
    std::string accum;
};

class SomeLexerState: public BaseLexerState
{
public:
    bool recognize(char ch);
};

class OtherLexerState: public BaseLexerState
{
public:
    bool recognize(char ch);
};
