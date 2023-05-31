#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <string>

#define impl(name)                                        \
    name::name(LexerInterface *lex) : BaseLexerState(lex) \
    {                                                     \
    }                                                     \
    bool name::recognize(char c)
#define newstate(name) lexer->setState(new name(lexer))
#define tablestate(c)                                       \
    if (symbols.find(c) != symbols.end())                   \
        lexer->setState(table[c](lexer));                   \
    else if (c == '\0')                                     \
    {                                                       \
        lexer->pushToQueue(Token(Type::eof, row, initpos)); \
        return false;                                       \
    }                                                       \
    else                                                    \
    {                                                       \
        newstate(Skip);                                     \
    }
#define fac(state) \
    static inline LexerStateInterface *state##Factory(LexerInterface *a) { return new state(a); }
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

static inline bool isSuitableForIdBeginning(char c)
{
    return std::isalpha(c) || c == '_';
}

static inline bool isSuitableForId(char c)
{
    return std::isalnum(c) || c == '_';
}

enum class IndentType
{
    null,
    space,
    tab
};

typedef std::stack<unsigned int> instack_t;

static unsigned int pos = 0;
static unsigned int row = 0;
static instack_t stack;
static std::string accum;
static IndentType intype;

void unwind(LexerInterface *lexer)
{
    while (stack.top() != 0)
    {
        lexer->pushToQueue(Token(Type::dedent, row, pos));
        stack.pop();
    }
}

static inline std::string getAccum()
{
    auto ret = accum;
    accum.clear();
    return ret;
}

BaseLexerState::BaseLexerState(LexerInterface *lex) : lexer(lex), initpos(pos) {}

static void eraseStack(instack_t &s)
{
    while (!s.empty())
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
        tablestate(c);
    }
    return false;
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
        tablestate(c);
    }
    return false;
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
        lexer->pushToQueue(Token(getAccum(), Type::id, row, initpos));
        tablestate(c);
    }
    return false;
}

impl(String)
{
    pos++;
    if (c == '"')
    {
        lexer->pushToQueue(Token(getAccum(), Type::string, row, initpos));
        newstate(Skip);
    }
    else
    {
        accum.push_back(c);
    }
    return false;
}

impl(Colon)
{
    pos++;
    lexer->pushToQueue(Token(Type::colon, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Dot)
{
    pos++;
    lexer->pushToQueue(Token(Type::dot, row, initpos));
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
        tablestate(c);
    }
    return false;
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
        lexer->pushToQueue(Token(getAccum(), Type::number, row, initpos));
        if (isSuitableForIdBeginning(c))
        {
            accum.push_back(c);
            newstate(Id);
        }
        else
        {
            tablestate(c);
        }
    }
    return false;
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
        lexer->pushToQueue(Token(getAccum(), Type::number, row, initpos));
        tablestate(c);
    }
    return false;
}

impl(Plus)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::plusass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::plus, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Minus)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::minass, row, initpos));
        newstate(Skip);
    }
    else if (c == '>')
    {
        lexer->pushToQueue(Token(Type::arrow, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::minus, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Star)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::mulass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::star, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Div)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::divass, row, initpos));
        newstate(Skip);
    }
    else if (c == '/')
    {
        newstate(Idiv);
    }
    else
    {
        lexer->pushToQueue(Token(Type::div, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}
impl(Mod)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::modass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::mod, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Matmul)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::matmulass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::matmul, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Greater)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::grequal, row, initpos));
        newstate(Skip);
    }
    else if (c == '>')
    {
        newstate(Rshift);
    }
    else
    {
        lexer->pushToQueue(Token(Type::greater, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Less)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::lequal, row, initpos));
        newstate(Skip);
    }
    else if (c == '>')
    {
        newstate(Rshift);
    }
    else
    {
        lexer->pushToQueue(Token(Type::less, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Assign)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::equal, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::assign, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Inv)
{
    pos++;
    lexer->pushToQueue(Token(Type::inv, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Band)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::bandass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::band, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Bor)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::borass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::bor, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Xor)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::xorass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::xorop, row, initpos));
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
            tablestate(c);
        }
    }

    return false;
}

impl(Lpr)
{
    pos++;
    lexer->pushToQueue(Token(Type::lpr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Rpr)
{
    pos++;
    lexer->pushToQueue(Token(Type::rpr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Lsbr)
{
    pos++;
    lexer->pushToQueue(Token(Type::lsbr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Rsbr)
{
    pos++;
    lexer->pushToQueue(Token(Type::rsbr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Lbr)
{
    pos++;
    lexer->pushToQueue(Token(Type::lbr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Rbr)
{
    pos++;
    lexer->pushToQueue(Token(Type::rbr, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Idiv)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::idivass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::idiv, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Lshift)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::lshiftass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::lshift, row, initpos));
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
            tablestate(c);
        }
    }

    return false;
}

impl(Rshift)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::rshiftass, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::rshift, row, initpos));
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
            tablestate(c);
        }
    }
    return false;
}

impl(Exclamation)
{
    pos++;
    if (c == '=')
    {
        lexer->pushToQueue(Token(Type::noteq, row, initpos));
        newstate(Skip);
    }
    else
    {
        lexer->pushToQueue(Token(Type::unexpected, row, initpos));
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
            tablestate(c);
        }
    }

    return false;
}

impl(Comment)
{
    if (c == '\n')
    {
        newstate(Newline);
    }
    return false;
}

impl(Comma)
{
    pos++;
    lexer->pushToQueue(Token(Type::comma, row, initpos));
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
        tablestate(c);
    }
    return false;
}

impl(Newline)
{
    if (c == '\n')
    {
        return false;
    }
    if (c == '\0')
    {
        lexer->pushToQueue(Token(Type::eof, row, initpos));
        return false;
    }
    lexer->pushToQueue(Token(Type::newline, row, initpos));
    pos = 1;
    row++;
    if (c == ' ' || c == '\t')
    {
        lexer->setState(new Indent(lexer, c));
    }
    else
    {
        if (stack.top() > 0)
        {
            while (stack.top() != 0)
            {
                stack.pop();
                lexer->pushToQueue(Token(Type::dedent, row, initpos));
            }
        }
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
            tablestate(c);
        }
    }
    return false;
}

Indent::Indent(LexerInterface *lex, char c) : BaseLexerState(lex), prevchar(c) {}

bool Indent::recognize(char c)
{
    // bool indent = false;
    pos++;
    if (prevchar > 0)
    {
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
                lexer->pushToQueue(Token(Type::tabspacemix, row, initpos));
                return false;
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
                lexer->pushToQueue(Token(Type::tabspacemix, row, initpos));
                return false;
                break;
            }
            break;
        default:
            break;
        }
        prevchar = 0;
    }

    if (c == ' ' || c == ' ')
    {
        switch (intype)
        {
        case (IndentType::space):
            switch (c)
            {
            case ' ':
                intcount++;
                return false;
                break;
            case '\t':
                lexer->pushToQueue(Token(Type::tabspacemix, row, initpos));
                return false;
                break;
            }
            break;

        case (IndentType::tab):
            switch (c)
            {
            case '\t':
                intcount++;
                return false;
                break;
            case ' ':
                lexer->pushToQueue(Token(Type::tabspacemix, row, initpos));
                return false;
                break;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        if (intcount > stack.top())
        {
            stack.push(intcount);
            lexer->pushToQueue(Token(Type::indent, row, initpos));
        }
        else if (intcount < stack.top())
        {
            while (!stack.empty() && intcount != stack.top())
            {
                stack.pop();
                lexer->pushToQueue(Token(Type::dedent, row, initpos));
            }
            if (stack.empty())
            {
                lexer->pushToQueue(Token(Type::indenterror, row, initpos));
            }
        }

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
            tablestate(c);
        }
    }

    return false;
}
