#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <functional>

#define impl(name)                                                                                                                           \
    name::name(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos) : BaseLexerState(lex, acc, t, row, pos) \
    {                                                                                                                                        \
    }                                                                                                                                        \
    bool name::recognize(char c)
#define newstate(name) lexer->setState(new name(lexer, accum, type, row, pos))
#define toacc accum.push_back(c)
#define tablestate lexer->setState(table[c](lexer, accum, type, row, pos))
#define fac(state)                                                                                                           \
    inline LexerStateInterface *state##Factory(LexerInterface *a, std::string &b, Type &c, unsigned int &d, unsigned int &e) \
    {                                                                                                                        \
        return new state(a, b, c, d, e);                                                                                     \
    }

fac(String)
fac(Colon)
fac(Dot)
fac(Plus)
fac(Minus)
fac(Star)
fac(Div)
fac(Mod)
fac(Matmul)
fac(Greater)
fac(Less)
fac(Assign)
fac(Inv)
fac(Band)
fac(Bor)
fac(Xor)
fac(Lpr)
fac(Rpr)
fac(Lsbr)
fac(Rsbr)
fac(Lbr)
fac(Rbr)
fac(Exclamation)
fac(Newline)
fac(Comment)
fac(Comma)

#define tab(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

static std::unordered_map<char, std::function<LexerStateInterface *(LexerInterface *,
                                                                    std::string &,
                                                                    Type &,
                                                                    unsigned int &,
                                                                    unsigned int &)>> table = {
    tab('+', Plus),
    tab('-', Minus),
    tab('*', Star),
    tab('/', Div),
    tab('@', Matmul),
    tab('%', Mod),
    tab('&', Band),
    tab('|', Bor),
    tab('^', Xor),
    tab('!', Exclamation),
    tab('<', Less),
    tab('>', Greater),
    tab('=', Assign),
    tab('~', Inv),
    tab('.', Dot),
    tab(',', Comma),
    tab('(', Lpr),
    tab(')', Rpr),
    tab('[', Lsbr),
    tab(']', Rsbr),
    tab('{', Lbr),
    tab('}', Rbr),
    tab('#', Comment),
    tab(':', Colon),
    tab('"', String),
    tab('\'', String),
    tab('\n', Newline),
};

inline bool isSuitableForIdBeginning(char c)
{
    return std::isalpha(c) || c == '_';
}

inline bool isSuitableForId(char c)
{
    return std::isalnum(c) || c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos) : lexer(lex), accum(acc), type(t), row(row), pos(pos) {}

impl(Start)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        toacc;
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        toacc;
        newstate(FirstNumPart);
    }
    else
    {
        tablestate;
    }
    return false;
}

impl(Skip)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        toacc;
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        toacc;
        newstate(FirstNumPart);
    }
    else
    {
        tablestate;
    }
    return false;
}

impl(Id)
{
    pos++;
    if (isSuitableForId(c))
    {
        toacc;
    }
    else
    {
        tablestate;
        type = Type::id;
        return true;
    }
    return false;
}

impl(String)
{
    pos++;
    if (c == '\'' || c == '"')
    {
        type = Type::string;
        newstate(Skip);
        return true;
    }
    else
    {
        toacc;
        return false;
    }
}

impl(Colon)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        newstate(FirstNumPart);
    }
    else
    {
        tablestate;
    }
    type = Type::colon;
    return true;
}

impl(Dot)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        newstate(Id);
    }
    else
    {
        tablestate;
    }
    type = Type::dot;
    return true;
}

impl(FirstNumPart)
{
    pos++;
    if (std::isdigit(c))
    {
        toacc;
    }
    else if (c == '.')
    {
        toacc;
        newstate(SecondNumPart);
    }
    else
    {
        tablestate;
        type = Type::number;
        return true;
    }
    return false;
}

impl(SecondNumPart)
{
    pos++;
    if (std::isdigit(c))
    {
        toacc;
    }
    else
    {
        tablestate;
        type = Type::number;
        return true;
    }
    return false;
}

impl(Plus) {}
impl(Minus) {}
impl(Star) {}
impl(Div) {}
impl(Mod) {}
impl(Matmul) {}
impl(Greater) {}
impl(Less) {}
impl(Assign) {}
impl(Inv) {}
impl(Band) {}
impl(Bor) {}
impl(Xor) {}
impl(Lpr) {}
impl(Rpr) {}
impl(Lsbr) {}
impl(Rsbr) {}
impl(Lbr) {}
impl(Rbr) {}
impl(Idiv) {}
impl(Lshift) {}
impl(Rshift) {}
impl(Exclamation) {}
impl(Newline) {}
impl(Comment) {}
impl(Comma) {}
