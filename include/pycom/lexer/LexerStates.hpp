#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <memory>

class BaseLexerState: public LexerStateInterface
{
protected:
    LexerInterface* lexer;
};

class SomeLexerState: public BaseLexerState
{
public:
    void recognize();
};

class OtherLexerState: public BaseLexerState
{
public:
    void recognize();
};
