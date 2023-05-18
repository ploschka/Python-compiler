#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <functional>
#include <unordered_set>

#define impl(name)                                                                                                                                                                              \
    name::name(LexerInterface *lex) : BaseLexerState(lex) \
    {                                                                                                                                                                                           \
    }                                                                                                                                                                                           \
    bool name::recognize(char c)
#define newstate(name) lexer->setState(new name(lexer))
#define tablestate                                                              \
    if (symbols.find(c) != symbols.end())                                       \
        lexer->setState(table[c](lexer)); \
    else if (c == '\0')                                                         \
    {                                                                           \
        End *endstate = new End(lexer);   \
        endstate->setState(lexer->getState());                                  \
        lexer->setState(endstate);                                              \
        return 0;                                                               \
    }                                                                           \
    else                                                                        \
        newstate(Skip)
#define fac(state)                                                                                                                                      \
    inline LexerStateInterface *state##Factory(LexerInterface *a) \
    {                                                                                                                                                   \
        return new state(a);                                                                                                          \
    }
#define tab(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

typedef std::function<LexerStateInterface *(LexerInterface *)> stateFactory;

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
    tab('\n', Newline)};

static const std::unordered_set<char> symbols = {
    '+', '-', '*', '/', '@', '%', '&', '|', '^', '!', '<', '>', '=', '~', '.', ',', '(', ')', '[', ']', '{', '}', '#', ':', '"', '\n'};

inline bool isSuitableForIdBeginning(char c)
{
    return std::isalpha(c) || c == '_';
}

inline bool isSuitableForId(char c)
{
    return std::isalnum(c) || c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *lex) : lexer(lex) {}

typedef std::stack<unsigned int> instack_t;

static unsigned int pos = 0;
static unsigned int row = 0;
static instack_t stack;
static std::string accum;
static IndentType intype;

void eraseStack(instack_t& s)
{
    while(!s.empty())
    {
        s.pop();
    }
}

impl(Start)
{
    pos = 1;
    row = 1;
    eraseStack(stack);
    stack.push(0);
    accum.assign("");
    intype = IndentType::null;

    if (isSuitableForIdBeginning(c))
    {
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else if (c != '\n')
    {
        tablestate;
    }
    return 0;
}

impl(Skip)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else
    {
        tablestate;
    }
    return 0;
}

impl(Id)
{
    pos++;
    if (isSuitableForId(c))
    {
        accum.push_back(c);
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
    if (c == '"')
    {
        type = Type::string;
        newstate(Skip);
        return initpos;
    }
    else
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else
        tablestate;
    type = Type::dot;
    return initpos;
}

impl(FirstNumPart)
{
    pos++;
    if (std::isdigit(c))
    {
        accum.push_back(c);
    }
    else if (c == '.')
    {
        accum.push_back(c);
        newstate(SecondNumPart);
    }
    else
    {
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else
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
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
    else if (c == '>')
    {
        newstate(Skip);
        type = Type::arrow;
    }
    else
    {
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
            tablestate;
        type = Type::assign;
    }
    return initpos;
}

impl(Inv)
{
    pos++;
    if (isSuitableForIdBeginning(c))
    {
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
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
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else if (std::isdigit(c))
        {
            accum.push_back(c);
            newstate(FirstNumPart);
        }
        else
            tablestate;
        type = Type::unexpected;
    }
    return initpos;
}

impl(Newline)
{
    pos = 1;
    row++;
    if (c == '\0')
    {
        newstate(End);
        return 0;
    }
    if (c == '\n')
    {
        return 0;
    }
    lexer->setState(new Indent(lexer, c));
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
    if (isSuitableForIdBeginning(c))
    {
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else
        tablestate;
    type = Type::comma;
    return initpos;
}

Indent::Indent(LexerInterface *lex, char c) : BaseLexerState(lex), prevchar(c) {}
bool Indent::recognize(char c)
{
    pos++;
    if (intype == IndentType::null)
    {
        switch (prevchar)
        {
        case ' ':
            intype = IndentType::space;
            break;
        case '\t':
            intype = IndentType::tab;
            break;
        }
    }
    switch (intype)
    {
    case (IndentType::space):
        switch (prevchar)
        {
        case ' ':
            intcount++;
            break;
        case '\t':
            type = Type::tabspacemix;
            return initpos;
            break;
        }
        switch (c)
        {
        case ' ':
            intcount++;
            return 0;
            break;
        case '\t':
            type = Type::tabspacemix;
            return initpos;
            break;
        }
        break;

    case (IndentType::tab):
        switch (prevchar)
        {
        case '\t':
            intcount++;
            break;
        case ' ':
            type = Type::tabspacemix;
            return initpos;
            break;
        }
        switch (c)
        {
        case '\t':
            intcount++;
            return 0;
            break;
        case ' ':
            type = Type::tabspacemix;
            return initpos;
            break;
        }
        break;
    default:
        break;
    }
    prevchar = 0;
    if (isSuitableForIdBeginning(c))
    {
        accum.push_back(c);
        newstate(Id);
    }
    else if (std::isdigit(c))
    {
        accum.push_back(c);
        newstate(FirstNumPart);
    }
    else
        tablestate;

    if (intcount > stack.top())
    {
        stack.push(intcount);
        type = Type::indent;
        return initpos;
    }
    if (intcount < stack.top())
    {
        stack.pop();
        if (intcount == stack.top())
            type = Type::dedent;
        else
            type = Type::indenterror;
        return initpos;
    }
    return 0;
}

impl(End)
{
    pos++;
    type = Type::eof;
    lexer->setState(state.release());
    return initpos;
}

void End::setState(LexerStateInterface *state)
{
    this->state.reset(state);
}
