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
#define tab(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

typedef std::function<LexerStateInterface *(LexerInterface *, std::string &, Type &, unsigned int &, unsigned int &)> stateFactory;

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

static std::unordered_map<char, stateFactory> table = {
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

impl(Plus)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::plusass;
    }
    else
    {
        tablestate;
        type = Type::plus;
    }
    return true;
}

impl(Minus)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::minass;
    }
    else
    {
        tablestate;
        type = Type::minus;
    }
    return true;
}

impl(Star)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::mulass;
    }
    else
    {
        tablestate;
        type = Type::star;
    }
    return true;
}

impl(Div)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::plusass;
    }
    else if (c == '/')
    {
        newstate(Idiv);
        return false;
    }
    else
    {
        tablestate;
        type = Type::div;
    }
    return true;
}
impl(Mod)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::modass;
    }
    else
    {
        tablestate;
        type = Type::mod;
    }
    return true;
}

impl(Matmul)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::matmulass;
    }
    else
    {
        tablestate;
        type = Type::matmul;
    }
    return true;
}

impl(Greater)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::grequal;
    }
    else if (c == '>')
    {
        newstate(Rshift);
        return false;
    }
    else
    {
        tablestate;
        type = Type::greater;
    }
    return true;
}

impl(Less)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::lequal;
    }
    else if (c == '<')
    {
        newstate(Lshift);
        return false;
    }
    else
    {
        tablestate;
        type = Type::less;
    }
    return true;
}

impl(Assign)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::equal;
    }
    else
    {
        tablestate;
        type = Type::assign;
    }
    return true;
}

impl(Inv)
{
    pos++;
    tablestate;
    type = Type::inv;
    return true;
}

impl(Band)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::bandass;
    }
    else
    {
        tablestate;
        type = Type::band;
    }
    return true;
}

impl(Bor)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::borass;
    }
    else
    {
        tablestate;
        type = Type::bor;
    }
    return true;
}

impl(Xor)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::xorass;
    }
    else
    {
        tablestate;
        type = Type::xorop;
    }
    return true;
}

impl(Lpr)
{
    pos++;
    newstate(Skip);
    type = Type::lpr;
    return true;
}

impl(Rpr)
{
    pos++;
    newstate(Skip);
    type = Type::rpr;
    return true;
}

impl(Lsbr)
{
    pos++;
    newstate(Skip);
    type = Type::lsbr;
    return true;
}

impl(Rsbr)
{
    pos++;
    newstate(Skip);
    type = Type::rsbr;
    return true;
}

impl(Lbr)
{
    pos++;
    newstate(Skip);
    type = Type::lbr;
    return true;
}

impl(Rbr)
{
    pos++;
    newstate(Skip);
    type = Type::rbr;
    return true;
}

impl(Idiv)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::idivass;
    }
    else
    {
        tablestate;
        type = Type::idiv;
    }
    return true;
}

impl(Lshift)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::lshiftass;
    }
    else
    {
        tablestate;
        type = Type::lshift;
    }
    return true;
}

impl(Rshift)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::rshiftass;
    }
    else
    {
        tablestate;
        type = Type::rshift;
    }
    return true;
}

impl(Exclamation)
{
    pos++;
    if (c == '=')
    {
        newstate(Skip);
        type = Type::noteq;
    }
    else
    {
        tablestate;
        type = Type::unexpected;
    }
    return true;
}

impl(Newline) {}
impl(Comment) {}
impl(Comma)
{
    pos++;
    newstate(Skip);
    type = Type::comma;
    return true;
}
