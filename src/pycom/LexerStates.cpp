#include <pycom/lexer/LexerStates.hpp>
#include <pycom/lexer/LexerData.hpp>

#include <unordered_map>
#include <functional>
#include <string>

#define IMPL(name)                                                                            \
    name::name(LexerInterface *lex, LexerData *lexerdata) : BaseLexerState(lex, lexerdata) {} \
    bool name::recognize(char _c)
#define FAC(state) \
    static inline std::unique_ptr<LexerStateInterface> state##Factory(LexerInterface *a, LexerData *b) { return std::make_unique<state>(a, b); }
#define TAB(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

typedef std::function<std::unique_ptr<LexerStateInterface>(LexerInterface *, LexerData *)> stateFactory_t;

FAC(String)
FAC(Colon)
FAC(Dot)
FAC(Plus)
FAC(Minus)
FAC(Star)
FAC(Div)
FAC(Mod)
FAC(Matmul)
FAC(Greater)
FAC(Less)
FAC(Assign)
FAC(Inv)
FAC(Band)
FAC(Bor)
FAC(Xor)
FAC(Lpr)
FAC(Rpr)
FAC(Lsbr)
FAC(Rsbr)
FAC(Lbr)
FAC(Rbr)
FAC(Exclamation)
FAC(Comma)
FAC(Skip)
static inline std::unique_ptr<LexerStateInterface> CommentFactory(LexerInterface *a, LexerData *b)
{
    b->put(Type::newline, b->row, b->pos);
    return std::make_unique<Comment>(a, b);
}
static inline std::unique_ptr<LexerStateInterface> NewlineFactory(LexerInterface *a, LexerData *b)
{
    b->put(Type::newline, b->row, b->pos);
    return std::make_unique<Newline>(a, b);
}

static std::unordered_map<char, stateFactory_t> table = {
    TAB('+', Plus),
    TAB('-', Minus),
    TAB('*', Star),
    TAB('/', Div),
    TAB('@', Matmul),
    TAB('%', Mod),
    TAB('&', Band),
    TAB('|', Bor),
    TAB('^', Xor),
    TAB('!', Exclamation),
    TAB('<', Less),
    TAB('>', Greater),
    TAB('=', Assign),
    TAB('~', Inv),
    TAB('.', Dot),
    TAB(',', Comma),
    TAB('(', Lpr),
    TAB(')', Rpr),
    TAB('[', Lsbr),
    TAB(']', Rsbr),
    TAB('{', Lbr),
    TAB('}', Rbr),
    TAB('#', Comment),
    TAB(':', Colon),
    TAB('"', String),
    TAB('\n', Newline)};

static inline bool isSuitableForIdBeginning(char _c)
{
    return std::isalpha(_c) || _c == '_';
}

static inline bool isSuitableForId(char _c)
{
    return std::isalnum(_c) || _c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *_lex, LexerData *_lexerdata) : lexer(_lex), lexerdata(_lexerdata), initpos(_lexerdata->pos) {}

static stateFactory_t tablestate(char c)
{
    if (table.find(c) != table.end())
        return table[c];
    else if (c == '\0')
    {
        return nullptr;
    }
    else
    {
        return SkipFactory;
    }
}

template <class T>
static void newstate(LexerInterface *lexer, LexerData *lexerdata)
{
    lexer->setState(std::make_unique<T>(lexer, lexerdata));
}

IMPL(Start)
{
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else if (_c == '#')
    {
        newstate<Comment>(lexer, lexerdata);
    }
    else if (_c == ' ' || _c == '\t')
    {
        lexer->setState(std::make_unique<Indent>(lexer, lexerdata, 0));
    }
    else if (_c != '\n')
    {
        auto p = tablestate(_c);
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

IMPL(Skip)
{
    lexerdata->pos++;
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Id)
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

IMPL(String)
{
    lexerdata->pos++;
    if (_c == '"')
    {
        lexerdata->put(Type::string, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->accum.push_back(_c);
    }
    return false;
}

IMPL(Colon)
{
    lexerdata->pos++;
    lexerdata->put(Type::colon, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Dot)
{
    lexerdata->pos++;
    lexerdata->put(Type::dot, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(FirstNumPart)
{
    lexerdata->pos++;
    if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
    }
    else if (_c == '.')
    {
        lexerdata->accum.push_back(_c);
        lexer->setState(std::make_unique<SecondNumPart>(lexer, lexerdata, initpos));
    }
    else
    {
        lexerdata->put(Type::number, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

SecondNumPart::SecondNumPart(LexerInterface *_lex, LexerData *_lexerdata, unsigned int _initpos) : BaseLexerState(_lex, _lexerdata)
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

IMPL(Plus)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::plusass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::plus, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Minus)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::minass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else if (_c == '>')
    {
        lexerdata->put(Type::arrow, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::minus, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Star)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::mulass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::star, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Div)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::divass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else if (_c == '/')
    {
        newstate<Idiv>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::div, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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
IMPL(Mod)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::modass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::mod, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Matmul)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::matmulass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::matmul, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Greater)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::grequal, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else if (_c == '>')
    {
        newstate<Rshift>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::greater, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Less)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::lequal, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else if (_c == '<')
    {
        newstate<Lshift>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::less, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Assign)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::equal, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::assign, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Inv)
{
    lexerdata->pos++;
    lexerdata->put(Type::inv, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Band)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::bandass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::band, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Bor)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::borass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::bor, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Xor)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::xorass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::xorop, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Lpr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lpr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Rpr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rpr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Lsbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lsbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Rsbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rsbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Lbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::lbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Rbr)
{
    lexerdata->pos++;
    lexerdata->put(Type::rbr, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Idiv)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::idivass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::idiv, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Lshift)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::lshiftass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::lshift, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Rshift)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::rshiftass, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::rshift, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Exclamation)
{
    lexerdata->pos++;
    if (_c == '=')
    {
        lexerdata->put(Type::noteq, lexerdata->row, initpos);
        newstate<Skip>(lexer, lexerdata);
    }
    else
    {
        lexerdata->put(Type::unexpected, lexerdata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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

IMPL(Comment)
{
    if (_c == '\n')
    {
        lexerdata->row++;
        lexerdata->pos = 0;
        lexer->setState(std::make_unique<Indent>(lexer, lexerdata, 0));
    }
    return false;
}

IMPL(Comma)
{
    lexerdata->pos++;
    lexerdata->put(Type::comma, lexerdata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<Id>(lexer, lexerdata);
    }
    else if (std::isdigit(_c))
    {
        lexerdata->accum.push_back(_c);
        newstate<FirstNumPart>(lexer, lexerdata);
    }
    else
    {
        auto p = tablestate(_c);
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

IMPL(Newline)
{
    lexerdata->pos = 1;
    lexerdata->row++;
    if (_c == '\n')
    {
        return false;
    }
    if (_c == '#')
    {
        newstate<Comment>(lexer, lexerdata);
        return false;
    }
    if (_c == '\0')
    {
        lexerdata->put(Type::eof, lexerdata->row, initpos);
        return false;
    }
    if (_c == ' ' || _c == '\t')
    {
        lexer->setState(std::make_unique<Indent>(lexer, lexerdata, _c));
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
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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
        newstate<Comment>(lexer, lexerdata);
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
        if (lexerdata->intype == IndentType::null)
        {
            switch (_c)
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
            newstate<Id>(lexer, lexerdata);
        }
        else if (std::isdigit(_c))
        {
            lexerdata->accum.push_back(_c);
            newstate<FirstNumPart>(lexer, lexerdata);
        }
        else
        {
            auto p = tablestate(_c);
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
