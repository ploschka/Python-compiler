#include <pycom/lexer/LexerStates.hpp>
#include <pycom/lexer/FileData.hpp>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <string>

#define impl(name)                                                                         \
    name::name(LexerInterface *lex, FileData *filedata) : BaseLexerState(lex, filedata) {} \
    bool name::recognize(char _c)
#define newstate(name) lexer->setState(new name(lexer, filedata))
#define fac(state) \
    static inline LexerStateInterface *state##Factory(LexerInterface *a, FileData *b) { return new state(a, b); }
#define tab(symbol, state)     \
    {                          \
        symbol, state##Factory \
    }

typedef std::function<LexerStateInterface *(LexerInterface *, FileData *)> stateFactory_t;

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
fac(Skip)

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

static const std::unordered_set<char> symbols = {
    '+', '-', '*', '/', '@', '%', '&', '|', '^', '!', '<', '>', '=', '~', '.', ',', '(', ')', '[', ']', '{', '}', '#', ':', '"', '\n'};

static inline bool isSuitableForIdBeginning(char _c)
{
    return std::isalpha(_c) || _c == '_';
}

static inline bool isSuitableForId(char _c)
{
    return std::isalnum(_c) || _c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface *_lex, FileData *_filedata) : lexer(_lex), filedata(_filedata), initpos(_filedata->pos) {}

static stateFactory_t tablestate(char _c)
{
    if (symbols.find(_c) != symbols.end())
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
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else if (_c != '\n')
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Skip)
{
    filedata->pos++;
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Id)
{
    filedata->pos++;
    if (isSuitableForId(_c))
    {
        filedata->accum.push_back(_c);
    }
    else
    {
        filedata->put(Type::id, filedata->row, initpos);
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(String)
{
    filedata->pos++;
    if (_c == '"')
    {
        filedata->put(Type::string, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->accum.push_back(_c);
    }
    return false;
}

impl(Colon)
{
    filedata->pos++;
    filedata->put(Type::colon, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Dot)
{
    filedata->pos++;
    filedata->put(Type::dot, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(FirstNumPart)
{
    filedata->pos++;
    if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
    }
    else if (_c == '.')
    {
        filedata->accum.push_back(_c);
        newstate(SecondNumPart);
    }
    else
    {
        filedata->put(Type::number, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(SecondNumPart)
{
    filedata->pos++;
    if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
    }
    else
    {
        filedata->put(Type::number, filedata->row, initpos);
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Plus)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::plusass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::plus, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Minus)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::minass, filedata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '>')
    {
        filedata->put(Type::arrow, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::minus, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Star)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::mulass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::star, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Div)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::divass, filedata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '/')
    {
        newstate(Idiv);
    }
    else
    {
        filedata->put(Type::div, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}
impl(Mod)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::modass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::mod, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Matmul)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::matmulass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::matmul, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Greater)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::grequal, filedata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '>')
    {
        newstate(Rshift);
    }
    else
    {
        filedata->put(Type::greater, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Less)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::lequal, filedata->row, initpos);
        newstate(Skip);
    }
    else if (_c == '>')
    {
        newstate(Rshift);
    }
    else
    {
        filedata->put(Type::less, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Assign)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::equal, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::assign, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Inv)
{
    filedata->pos++;
    filedata->put(Type::inv, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Band)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::bandass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::band, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Bor)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::borass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::bor, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Xor)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::xorass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::xorop, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }

    return false;
}

impl(Lpr)
{
    filedata->pos++;
    filedata->put(Type::lpr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Rpr)
{
    filedata->pos++;
    filedata->put(Type::rpr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Lsbr)
{
    filedata->pos++;
    filedata->put(Type::lsbr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Rsbr)
{
    filedata->pos++;
    filedata->put(Type::rsbr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Lbr)
{
    filedata->pos++;
    filedata->put(Type::lbr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Rbr)
{
    filedata->pos++;
    filedata->put(Type::rbr, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Idiv)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::idivass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::idiv, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Lshift)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::lshiftass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::lshift, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }

    return false;
}

impl(Rshift)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::rshiftass, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::rshift, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

impl(Exclamation)
{
    filedata->pos++;
    if (_c == '=')
    {
        filedata->put(Type::noteq, filedata->row, initpos);
        newstate(Skip);
    }
    else
    {
        filedata->put(Type::unexpected, filedata->row, initpos);
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }

    return false;
}

impl(Comment)
{
    if (_c == '\n')
    {
        newstate(Newline);
    }
    return false;
}

impl(Comma)
{
    filedata->pos++;
    filedata->put(Type::comma, filedata->row, initpos);
    if (isSuitableForIdBeginning(_c))
    {
        filedata->accum.push_back(_c);
        newstate(Id);
    }
    else if (std::isdigit(_c))
    {
        filedata->accum.push_back(_c);
        newstate(FirstNumPart);
    }
    else
    {
        auto p = tablestate(_c);
        ;
        if (p)
        {
            lexer->setState(p(lexer, filedata));
        }
        else
        {
            filedata->put(Type::eof, filedata->row, initpos);
        }
    }
    return false;
}

impl(Newline)
{
    if (_c == '\n')
    {
        return false;
    }
    if (_c == '\0')
    {
        filedata->put(Type::eof, filedata->row, initpos);
        return false;
    }
    filedata->put(Type::newline, filedata->row, initpos);
    filedata->pos = 1;
    filedata->row++;
    if (_c == ' ' || _c == '\t')
    {
        lexer->setState(new Indent(lexer, filedata, _c));
    }
    else
    {
        if (filedata->stack.top() > 0)
        {
            while (filedata->stack.top() != 0)
            {
                filedata->stack.pop();
                filedata->put(Type::dedent, filedata->row, initpos);
            }
        }
        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }
    return false;
}

Indent::Indent(LexerInterface *_lex, FileData *_filedata, char _c) : BaseLexerState(_lex, _filedata), prevchar(_c) {}

bool Indent::recognize(char _c)
{
    // bool indent = false;
    filedata->pos++;
    if (prevchar > 0)
    {
        if (filedata->intype == IndentType::null)
        {
            switch (prevchar)
            {
            case ' ':
                filedata->intype = IndentType::space;
                break;
            case '\t':
                filedata->intype = IndentType::tab;
                break;
            }
        }

        switch (filedata->intype)
        {
        case (IndentType::space):
            switch (prevchar)
            {
            case ' ':
                intcount++;
                break;
            case '\t':
                filedata->put(Type::tabspacemix, filedata->row, initpos);
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
                filedata->put(Type::tabspacemix, filedata->row, initpos);
                return false;
                break;
            }
            break;
        default:
            break;
        }
        prevchar = 0;
    }

    if (_c == ' ' || _c == ' ')
    {
        switch (filedata->intype)
        {
        case (IndentType::space):
            switch (_c)
            {
            case ' ':
                intcount++;
                return false;
                break;
            case '\t':
                filedata->put(Type::tabspacemix, filedata->row, initpos);
                return false;
                break;
            }
            break;

        case (IndentType::tab):
            switch (_c)
            {
            case '\t':
                intcount++;
                return false;
                break;
            case ' ':
                filedata->put(Type::tabspacemix, filedata->row, initpos);
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
        if (intcount > filedata->stack.top())
        {
            filedata->stack.push(intcount);
            filedata->put(Type::indent, filedata->row, initpos);
        }
        else if (intcount < filedata->stack.top())
        {
            while (!filedata->stack.empty() && intcount != filedata->stack.top())
            {
                filedata->stack.pop();
                filedata->put(Type::dedent, filedata->row, initpos);
            }
            if (filedata->stack.empty())
            {
                filedata->put(Type::indenterror, filedata->row, initpos);
            }
        }

        if (isSuitableForIdBeginning(_c))
        {
            filedata->accum.push_back(_c);
            newstate(Id);
        }
        else if (std::isdigit(_c))
        {
            filedata->accum.push_back(_c);
            newstate(FirstNumPart);
        }
        else
        {
            auto p = tablestate(_c);
            ;
            if (p)
            {
                lexer->setState(p(lexer, filedata));
            }
            else
            {
                filedata->put(Type::eof, filedata->row, initpos);
            }
        }
    }

    return false;
}
