#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <memory>
#include <stack>

#define st(name)                                                                                                                        \
    class name : public BaseLexerState                                                                                                  \
    {                                                                                                                                   \
    public:                                                                                                                             \
        name(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos, instack &stack, IndentType &intype); \
        unsigned int recognize(char c);                                                                                                 \
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
    std::string &accum;
    Type &type;
    unsigned int &row;
    unsigned int &pos;
    unsigned int initpos;
    instack &stack;
    IndentType &intype;
    BaseLexerState(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos, instack &stack, IndentType &intype);
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
    Indent(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos, instack &stack, IndentType &intype, char c);
    unsigned int recognize(char c);
};

class End : public BaseLexerState
{
private:
    std::unique_ptr<LexerStateInterface> state;

public:
    End(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos, instack &stack, IndentType &intype);
    unsigned int recognize(char c);
    void setState(LexerStateInterface *state);
};

#undef st
