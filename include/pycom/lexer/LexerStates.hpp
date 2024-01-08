#pragma once

#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/lexer/LexerData.hpp>

#define ST(name)                                           \
    class name : public BaseLexerState                     \
    {                                                      \
    public:                                                \
        name(LexerInterface *_lex, LexerData *_lexerdata); \
        bool recognize(char _c);                           \
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

ST(Start)
ST(Skip)
ST(Id)
ST(FirstNumPart)
ST(String)
ST(Colon)
ST(Dot)
ST(Plus)
ST(Minus)
ST(Star)
ST(Div)
ST(Mod)
ST(Matmul)
ST(Greater)
ST(Less)
ST(Assign)
ST(Inv)
ST(Band)
ST(Bor)
ST(Xor)
ST(Lpr)
ST(Rpr)
ST(Lsbr)
ST(Rsbr)
ST(Lbr)
ST(Rbr)
ST(Idiv)
ST(Exclamation)
ST(Lshift)
ST(Rshift)
ST(Newline)
ST(Comment)
ST(Comma)

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

#undef ST
