#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/lexer/FileData.hpp>
#include <memory>
#include <stack>

#define st(name)                                         \
    class name : public BaseLexerState                   \
    {                                                    \
    public:                                              \
        name(LexerInterface *_lex, FileData *_filedata); \
        bool recognize(char _c);                         \
    };

typedef std::stack<unsigned int> instack;

class BaseLexerState : public LexerStateInterface
{
protected:
    LexerInterface *lexer;
    FileData *filedata;
    unsigned int initpos;
    Type type;
    BaseLexerState(LexerInterface *_lex, FileData *_filedata);
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
    Indent(LexerInterface *_lex, FileData *_filedata, char _c);
    bool recognize(char _c);
};

#undef st
