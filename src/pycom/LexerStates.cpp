#include <pycom/lexer/LexerStates.hpp>

#define impl(name) name::name(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos):BaseLexerState(lex, acc, t, row, pos){} \
                   bool name::recognize(char c)
#define newstate(name) lexer->setState(new name(lexer, accum, type, row, pos))
#define toacc accum.push_back(c)
#define argtoacc(arg) accum.push_back(arg);
#define singlesymbol(sym, state, t) case sym: \
                                    newstate(state); \
                                    type = t; \
                                    return true; \
                                    break

inline bool isSuitableForId(char c) {
    return std::isalnum(c) || c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos):
    lexer(lex), accum(acc), type(t), row(row), pos(pos) {}

impl(Start) {
    pos++;
    if (std::isalpha(c)) {
        toacc;
        newstate(Id);
    } else if (std::isdigit(c)) {
        toacc;
        newstate(FirstNumPart);
    } else {
        switch (c) {
        case ' ':
            break;

        case '\n':
            row++;
            pos = 0;
            break;

        case '#':
            newstate(Comment);
            break;

        case '(':
            newstate(Lpr);
            break;

        case '{':
            newstate(Lbr);
            break;

        case '[':
            newstate(Lsbr);
            break;

        case '+':
            newstate(Plus);
            break;

        case '-':
            newstate(Minus);
            break;

        case '~':
            newstate(Inv);
            break;

        case '"':
            newstate(String);
            break;

        case '\'':
            newstate(String);
            break;

        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    return false;
}

impl(Space) {
    pos++;
    if (std::isalpha(c)) {
        toacc;
        newstate(Id);
    } else if (std::isdigit(c)) {
        toacc;
        newstate(FirstNumPart);
    } else {
        switch (c) {
        case ' ':
            break;

        case '+':
            newstate(Plus);
            break;

        case '-':
            newstate(Minus);
            break;

        case '*':
            newstate(Star);
            break;

        case '/':
            newstate(Div);
            break;

        case '@':
            newstate(Matmul);
            break;

        case '<':
            newstate(Less);
            break;

        case '>':
            newstate(Greater);
            break;

        case '=':
            newstate(Assign);
            break;

        case '%':
            newstate(Mod);
            break;

        case '&':
            newstate(Band);
            break;

        case '|':
            newstate(Bor);
            break;

        case '^':
            newstate(Xor);
            break;

        case '(':
            newstate(Lpr);
            break;

        case ')':
            newstate(Rpr);
            break;

        case '[':
            newstate(Lsbr);
            break;

        case ']':
            newstate(Rsbr);
            break;

        case '{':
            newstate(Lbr);
            break;

        case '}':
            newstate(Rbr);
            break;

        case ',':
            newstate(Comma);
            break;

        case '.':
            newstate(Dot);
            break;

        case '\n':
            newstate(Newline);
            break;
        }
    }
    return false;
}

impl(Id) {
    #define t Type::id;
    pos++;
    if (isSuitableForId(c)) {
        toacc;
    } else {
        switch (c)
        {
        singlesymbol(' ', Space, t);
        singlesymbol('+', Plus, t);
        singlesymbol('-', Minus, t);
        singlesymbol('*', Star, t);
        singlesymbol('/', Div, t);
        singlesymbol('@', Matmul, t);
        singlesymbol('<', Less, t);
        singlesymbol('>', Greater, t);
        singlesymbol('=', Assign, t);
        singlesymbol('%', Mod, t);
        singlesymbol('&', Band, t);
        singlesymbol('|', Bor, t);
        singlesymbol('^', Xor, t);
        singlesymbol('(', Lpr, t);
        singlesymbol(')', Rpr, t);
        singlesymbol('[', Lsbr, t);
        singlesymbol(']', Rsbr, t);
        singlesymbol('{', Lbr, t);
        singlesymbol('}', Rbr, t);
        singlesymbol(',', Comma, t);
        singlesymbol('.', Dot, t);
        singlesymbol('\n', Newline, t);

        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    return false;
    #undef t
}

impl(String) {
    pos++;
    if (c == '\'' || c == '"') {
        type = Type::string;
        newstate(Space);
        return true;
    } else {
        toacc;
        return false;
    }
}

impl(Colon) {}
impl(Dot) {}
impl(FirstNumPart) {
    #define t Type::number
    pos++;
    if (std::isdigit(c)) {
        toacc;
    } else {
        switch (c) {
        singlesymbol(' ', Space, t);
        singlesymbol('+', Plus, t);
        singlesymbol('-', Minus, t);
        singlesymbol('*', Star, t);
        singlesymbol('/', Div, t);
        singlesymbol('@', Matmul, t);
        singlesymbol('<', Less, t);
        singlesymbol('>', Greater, t);
        singlesymbol('=', Assign, t);
        singlesymbol('%', Mod, t);
        singlesymbol('&', Band, t);
        singlesymbol('|', Bor, t);
        singlesymbol('^', Xor, t);
        singlesymbol('(', Lpr, t);
        singlesymbol(')', Rpr, t);
        singlesymbol('[', Lsbr, t);
        singlesymbol(']', Rsbr, t);
        singlesymbol('{', Lbr, t);
        singlesymbol('}', Rbr, t);
        singlesymbol(',', Comma, t);
        singlesymbol('\n', Newline, t);

        case '.':
            toacc;
            newstate(SecondNumPart);
            break;

        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    return false;
    #undef t
}

impl(SecondNumPart) {
    #define t Type::number
    pos++;
    if (std::isdigit(c)) {
        toacc;
    } else {
        switch (c) {
        singlesymbol(' ', Space, t);
        singlesymbol('+', Plus, t);
        singlesymbol('-', Minus, t);
        singlesymbol('*', Star, t);
        singlesymbol('/', Div, t);
        singlesymbol('@', Matmul, t);
        singlesymbol('<', Less, t);
        singlesymbol('>', Greater, t);
        singlesymbol('=', Assign, t);
        singlesymbol('%', Mod, t);
        singlesymbol('&', Band, t);
        singlesymbol('|', Bor, t);
        singlesymbol('^', Xor, t);
        singlesymbol('(', Lpr, t);
        singlesymbol(')', Rpr, t);
        singlesymbol('[', Lsbr, t);
        singlesymbol(']', Rsbr, t);
        singlesymbol('{', Lbr, t);
        singlesymbol('}', Rbr, t);
        singlesymbol(',', Comma, t);
        singlesymbol('\n', Newline, t);

        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    return false;
    #undef t
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
impl(Equal) {}
impl(Noteq) {}
impl(Grequal) {}
impl(Lequal) {}
impl(Lshift) {}
impl(Rshift) {}
impl(Plusass) {}
impl(Minass) {}
impl(Mulass) {}
impl(Divass) {}
impl(Modass) {}
impl(Idivass) {}
impl(Matmulass) {}
impl(Lshiftass) {}
impl(Rshiftass) {}
impl(Bandass) {}
impl(Borass) {}
impl(Xorass) {}
impl(Newline) {}
impl(Comment) {}
impl(Comma) {}
