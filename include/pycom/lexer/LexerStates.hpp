#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <memory>
#include <stack>

#define st(name)                                                                                                                        \
    class name : public BaseLexerState                                                                                                  \
    {                                                                                                                                   \
    public:                                                                                                                             \
        name(LexerInterface *lex); \
        bool recognize(char c);                                                                                                 \
    };

enum class IndentType
{
    null,
    space,
    tab
};

typedef std::stack<unsigned int> instack;

class BaseLexerState : public LexerStateInterface
{
protected:
    LexerInterface *lexer;
    unsigned int initpos;
    Type type;
    BaseLexerState(LexerInterface *lex);
};

st(Start)
st(Skip)
st(Id)
st(FirstNumPart)
st(SecondNumPart)
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

class Indent : public BaseLexerState
{
private:
    unsigned int intcount = 0;
    char prevchar;

public:
    Indent(LexerInterface *lex, char c);
    bool recognize(char c);
};

class End : public BaseLexerState
{
private:
    std::unique_ptr<LexerStateInterface> state;

public:
    End(LexerInterface *lex);
    bool recognize(char c);
    void setState(LexerStateInterface *state);
};

#undef st
