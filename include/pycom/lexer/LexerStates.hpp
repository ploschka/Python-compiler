#pragma once
#include <pycom/interface/LexerStateInterface.hpp>
#include <pycom/interface/LexerInterface.hpp>
#include <memory>

#define st(name) class name: public BaseLexerState \
                 { \
                 public: \
                     name(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos); \
                     bool recognize(char c); \
                 };

class BaseLexerState: public LexerStateInterface
{
protected:
    LexerInterface* lexer;
    std::string& accum;
    Type& type;
    unsigned int& row;
    unsigned int& pos;
    BaseLexerState(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos);
};

st(Start)
st(Space)
st(Id)
st(String)
st(Colon)
st(Dot)
st(FirstNumPart)
st(SecondNumPart)
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
st(Equal)
st(Noteq)
st(Grequal)
st(Lequal)
st(Lshift)
st(Rshift)
st(Plusass)
st(Minass)
st(Mulass)
st(Divass)
st(Modass)
st(Idivass)
st(Matmulass)
st(Lshiftass)
st(Rshiftass)
st(Bandass)
st(Borass)
st(Xorass)
st(Newline)
st(Comment)
st(Comma)

#undef st
