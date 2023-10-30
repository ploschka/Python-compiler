#pragma once

#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/lexer/LexerData.hpp>

#define st(name)                                         \
    class name : public BaseLexerState                   \
    {                                                    \
    public:                                              \
        name(LexerInterface *_lex, LexerData *_lexerdata); \
        bool recognize(char _c);                         \
    };

class BaseLexerState : public LexerStateInterface
{
protected:
    LexerInterface *lexer;
    LexerData *lexerdata;
    unsigned int initpos;
    Type type;
    BaseLexerState(LexerInterface *_lex, LexerData *_lexerdata);
};

st(Start)
st(Skip)
st(Id)
st(FirstNumPart)
st(String)
st(Colon)
st(Dot)
st(Plus)
st(Minus)
st(Star)
st(Div)
st(Mod)
st(Matmul)
st(Greater)
st(Less)
st(Assign)
st(Inv)
st(Band)
st(Bor)
st(Xor)
st(Lpr)
st(Rpr)
st(Lsbr)
st(Rsbr)
st(Lbr)
st(Rbr)
st(Idiv)
st(Exclamation)
st(Lshift)
st(Rshift)
st(Newline)
st(Comment)
st(Comma)

class SecondNumPart : public BaseLexerState
{
public:
    SecondNumPart(LexerInterface *_lex, LexerData *_lexerdata, unsigned int _initpos);
    bool recognize(char _c);
};

class Indent : public BaseLexerState
{
private:
    unsigned int intcount = 0;
    char prevchar;

public:
    Indent(LexerInterface *_lex, LexerData *_lexerdata, char _c);
    bool recognize(char _c);
};

#undef st
