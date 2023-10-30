#include <pycom/lexer/LexerStates.hpp>
#include <pycom/lexer/LexerData.hpp>

#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <string>

#define impl(name)                                                                         \
    name::name(LexerInterface *lex, LexerData *lexerdata) : BaseLexerState(lex, lexerdata) {} \
    bool name::recognize(char _c)
#define newstate(name) lexer->setState(new name(lexer, lexerdata))
#define fac(state) \
    static inline LexerStateInterface *state##Factory(LexerInterface *a, LexerData *b) { return new state(a, b); }
#define tab(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

typedef std::function<LexerStateInterface *(LexerInterface *, LexerData *)> stateFactory_t;

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
fac(Comma)
fac(Skip)
static inline LexerStateInterface *CommentFactory(LexerInterface *a, LexerData *b)
{
    b->put(Type::newline, b->row, b->pos);
    return new Comment(a, b);
}
static inline LexerStateInterface *NewlineFactory(LexerInterface *a, LexerData *b)
{
    b->put(Type::newline, b->row, b->pos);
    return new Newline(a, b);
}

static std::unordered_map<char, stateFactory_t> table = {
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

static inline bool isSuitableForIdBeginning(char _c)
{
    return std::isalpha(_c) || _c == '_';
}

static inline bool isSuitableForId(char _c)
{
    return std::isalnum(_c) || _c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *_lex, LexerData *_lexerdata) : lexer(_lex), lexerdata(_lexerdata), initpos(_lexerdata->pos) {}

static stateFactory_t tablestate(char _c)
{
    if (table.find(_c) != table.end())
        return table[_c];
    else if (_c == '\0')
    {
        return nullptr;
    }
    else
    {
        return SkipFactory;
    }
}

impl(Start)
{
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else if (_c != '\n')
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Skip)
{
    lexerdata->pos++;
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Id)
{
    lexerdata->pos++;
    if (isSuitableForId(_c))
    {
        lexerdata->accum.push_back(_c);
    }
    else
    {
        lexerdata->put(Type::id, lexerdata->row, initpos);
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(String)
{
    lexerdata->pos++;
    if (_c == '"')
    {
        lexerdata->put(Type::string, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->accum.push_back(_c);
    }
    return false;
}

impl(Colon)
{
    lexerdata->pos++;
    lexerdata->put(Type::colon, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Dot)
{
    lexerdata->pos++;
    lexerdata->put(Type::dot, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(FirstNumPart)
{
    lexerdata->pos++;
    if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
    }
    else if (_c == '.')
    {
        lexerdata->accum.push_back(_c);
        lexer->setState(new SecondNumPart(lexer, lexerdata, initpos));
    }
    else
    {
        lexerdata->put(Type::number, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

SecondNumPart::SecondNumPart(LexerInterface* _lex, LexerData* _lexerdata, unsigned int _initpos) : BaseLexerState(_lex, _lexerdata)
{
    initpos = _initpos;
}
bool SecondNumPart::recognize(char _c)
{
    lexerdata->pos++;
    if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
    }
    else
    {
        lexerdata->put(Type::number, lexerdata->row, initpos);
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Plus)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::plusass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::plus, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Minus)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::minass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '>')
    {
        lexerdata->put(Type::arrow, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::minus, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Star)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::mulass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::star, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Div)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::divass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '/')
    {
        newstate(Idiv);
    }
    else
    {
        lexerdata->put(Type::div, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}
impl(Mod)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::modass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::mod, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Matmul)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::matmulass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::matmul, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Greater)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::grequal, lexerdata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '>')
    {
        newstate(Rshift);
    }
    else
    {
        lexerdata->put(Type::greater, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Less)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::lequal, lexerdata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '<')
    {
        newstate(Lshift);
    }
    else
    {
        lexerdata->put(Type::less, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Assign)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::equal, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::assign, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Inv)
{
    lexerdata->pos++;
    lexerdata->put(Type::inv, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Band)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::bandass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::band, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Bor)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::borass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::bor, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Xor)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::xorass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::xorop, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }

    return false;
}

impl(Lpr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lpr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Rpr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rpr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Lsbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lsbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Rsbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rsbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Lbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Rbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Idiv)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::idivass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::idiv, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Lshift)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::lshiftass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::lshift, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }

    return false;
}

impl(Rshift)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::rshiftass, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::rshift, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

impl(Exclamation)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::noteq, lexerdata->row, initpos);
        newstate(Skip);
    }
    else
    {
        lexerdata->put(Type::unexpected, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }

    return false;
}

impl(Comment)
{
    if (_c == '\n')
    {
        lexerdata->row++;
        lexerdata->pos = 0;
        lexer->setState(new Indent(lexer, lexerdata, 0));
    }
    return false;
}

impl(Comma)
{
    lexerdata->pos++;
    lexerdata->put(Type::comma, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, lexerdata));
        }
        else
        {
            lexerdata->put(Type::eof, lexerdata->row, initpos);
        }
    }
    return false;
}

impl(Newline)
{
    lexerdata->pos = 1;
    lexerdata->row++;
    if (_c == '\n')
    {
        return false;
    }
    if (_c == '#')
    {
        newstate(Comment);
        return false;
    }
    if (_c == '\0')
    {
        lexerdata->put(Type::eof, lexerdata->row, initpos);
        return false;
    }
    if (_c == ' ' || _c == '\t')
    {
        lexer->setState(new Indent(lexer, lexerdata, _c));
    }
    else
    {
        if (lexerdata->stack.top() > 0)
        {
            while (lexerdata->stack.top() != 0)
            {
                lexerdata->stack.pop();
                lexerdata->put(Type::dedent, lexerdata->row, initpos);
            }
        }
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }
    return false;
}

Indent::Indent(LexerInterface *_lex, LexerData *_lexerdata, char _c) : BaseLexerState(_lex, _lexerdata), prevchar(_c) {}

bool Indent::recognize(char _c)
{
    lexerdata->pos++;
    if (_c == '\n')
    {
        prevchar = 0;
        intcount = 0;
        lexerdata->row++;
        lexerdata->pos = 0;
        return false;
    }
    if (_c == '#')
    {
        newstate(Comment);
        return false;
    }
    if (prevchar > 0)
    {
        if (lexerdata->intype == IndentType::null)
        {
            switch (prevchar)
            {
            case ' ':
                lexerdata->intype = IndentType::space;
                break;
            case '\t':
                lexerdata->intype = IndentType::tab;
                break;
            }
        }

        switch (lexerdata->intype)
        {
        case (IndentType::space):
            switch (prevchar)
            {
            case ' ':
                intcount++;
                break;
            case '\t':
                lexerdata->put(Type::tabspacemix, lexerdata->row, initpos);
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
                lexerdata->put(Type::tabspacemix, lexerdata->row, initpos);
                return false;
                break;
            }
            break;
        default:
            break;
        }
        prevchar = 0;
    }

    if (_c == ' ' || _c == '\t')
    {
        switch (lexerdata->intype)
        {
        case (IndentType::space):
            switch (_c)
            {
            case ' ':
                intcount++;
                break;
            case '\t':
                lexerdata->put(Type::tabspacemix, lexerdata->row, initpos);
                break;
            }
            break;

        case (IndentType::tab):
            switch (_c)
            {
            case '\t':
                intcount++;
                break;
            case ' ':
                lexerdata->put(Type::tabspacemix, lexerdata->row, initpos);
                break;
            }
            break;
        default:
            break;
        }
        return false;
    }
    else
    {
        if (intcount > lexerdata->stack.top())
        {
            lexerdata->stack.push(intcount);
            lexerdata->put(Type::indent, lexerdata->row, initpos);
        }
        else if (intcount < lexerdata->stack.top())
        {
            while (!lexerdata->stack.empty() && intcount != lexerdata->stack.top())
            {
                lexerdata->stack.pop();
                lexerdata->put(Type::dedent, lexerdata->row, initpos);
            }
            if (lexerdata->stack.empty())
            {
                lexerdata->put(Type::indenterror, lexerdata->row, initpos);
            }
        }

        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, lexerdata));
            }
            else
            {
                lexerdata->put(Type::eof, lexerdata->row, initpos);
            }
        }
    }

    return false;
}
