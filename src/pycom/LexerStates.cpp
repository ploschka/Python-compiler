#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <functional>
#include <unordered_set>

#define impl(name)                                                                                                                                                           \
    name::name(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos) : BaseLexerState(lex, acc, t, row, pos) \
    {                                                                                                                                                                        \
    }                                                                                                                                                                        \
    unsigned int name::recognize(char c)
#define newstate(name) lexer->setState(new name(lexer, accum, type, row, pos))
#define toacc accum.push_back(c)
#define tablestate                                                        \
    if (symbols.find(c) != symbols.end())                                 \
        lexer->setState(table[c](lexer, accum, type, row, pos)); \
    else                                                                  \
        newstate(Skip)
#define fac(state)                                                                                                                            \
    inline LexerStateInterface *state##Factory(LexerInterface *a, std::string &b, Type &c, unsigned int &d, unsigned int &e) \
    {                                                                                                                                         \
        return new state(a, b, c, d, e);                                                                                                   \
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
fac(End)

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
    tab('\0', End)};

static const std::unordered_set<char> symbols = {
    '+', '-', '*', '/', '@', '%', '&', '|', '^', '!', '<', '>', '=', '~', '.', ',', '(', ')', '[', ']', '{', '}', '#', ':', '"', '\'', '\n', '\0'};

inline bool isSuitableForIdBeginning(char c)
{
    return std::isalpha(c) || c == '_';
}

inline bool isSuitableForId(char c)
{
    return std::isalnum(c) || c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *lex, std::string &acc, Type &t, unsigned int &row, unsigned int &pos) : lexer(lex), accum(acc), type(t), row(row), pos(pos), initpos(pos) {}

impl(Start)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        toacc;
        newstate(Id);
        initpos = pos;
    }
    else if (std::isdigit(c))
    {
        toacc;
        newstate(FirstNumPart);
        initpos = pos;

    }
    else if (c != '\n')
    {
        tablestate;
        initpos = pos;
    }
    return 0;
}

impl(Skip)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        toacc;
        newstate(Id);
        initpos = pos;
    }
    else if (std::isdigit(c))
    {
        toacc;
        newstate(FirstNumPart);
        initpos = pos;
    }
    else {
        tablestate;
        initpos = pos;
    }
    return 0;
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
        return initpos;
    }
    return 0;
}

impl(String)
{
    pos++;
    if (c == '\'' || c == '"')
    {
        type = Type::string;
        newstate(Skip);
        return initpos;
    }
    else
    {
        toacc;
        return 0;
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
    return initpos;
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
    return initpos;
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
        return initpos;
    }
    return 0;
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
        return initpos;
    }
    return 0;
}

impl(Plus)
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
    else if (c == '=')
    {
        newstate(Skip);
        type = Type::plusass;
        return initpos;
    }
    else
    {
        tablestate;
    }
    type = Type::plus;
    return initpos;
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
    return initpos;
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
    return initpos;
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
        return 0;
    }
    else
    {
        tablestate;
        type = Type::div;
    }
    return initpos;
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
    return initpos;
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
    return initpos;
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
        return 0;
    }
    else
    {
        tablestate;
        type = Type::greater;
    }
    return initpos;
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
        return 0;
    }
    else
    {
        tablestate;
        type = Type::less;
    }
    return initpos;
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
    return initpos;
}

impl(Inv)
{
    pos++;
    tablestate;
    type = Type::inv;
    return initpos;
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
    return initpos;
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
    return initpos;
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
    return initpos;
}

impl(Lpr)
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
    type = Type::lpr;
    return initpos;
}

impl(Rpr)
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
    type = Type::rpr;
    return initpos;
}

impl(Lsbr)
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
    type = Type::lsbr;
    return initpos;
}

impl(Rsbr)
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
    type = Type::rsbr;
    return initpos;
}

impl(Lbr)
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
    type = Type::lbr;
    return initpos;
}

impl(Rbr)
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
    type = Type::rbr;
    return initpos;
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
    return initpos;
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
    return initpos;
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
    return initpos;
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
    return initpos;
}

impl(Newline)
{
    pos = 1;
    row++;
    newstate(Indent);
    type = Type::newline;
    return initpos;
}

impl(Comment)
{
    if (c == '\n')
    {
        newstate(Newline);
    }
    return 0;
}

impl(Comma)
{
    pos++;
    newstate(Skip);
    type = Type::comma;
    return initpos;
}

impl(Indent)
{
    pos++;
    tablestate;
    type = Type::indent;
    return initpos;
}

impl(End)
{
    pos++;
    type = Type::eof;
    return initpos;
}
