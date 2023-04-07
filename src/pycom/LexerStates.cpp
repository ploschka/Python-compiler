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
#define justswitch(symbol, state) case symbol: \
                                    newstate(state); \
                                    break

inline bool isSuitableForIdBeginning(char c) {
    return std::isalpha(c) || c == '_';
}

inline bool isSuitableForId(char c) {
    return std::isalnum(c) || c == '_';
}

BaseLexerState::BaseLexerState(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos):
    lexer(lex), accum(acc), type(t), row(row), pos(pos) {}

impl(Start) {
    pos++;
    if (isSuitableForIdBeginning(c)) {
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

        justswitch('#', Comment);
        justswitch('(', Lpr);
        justswitch('{', Lbr);
        justswitch('[', Lsbr);
        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('~', Inv);
        justswitch('"', String);
        justswitch('\'', String);

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
    if (isSuitableForIdBeginning(c)) {
        toacc;
        newstate(Id);
    } else if (std::isdigit(c)) {
        toacc;
        newstate(FirstNumPart);
    } else {
        switch (c) {
        case ' ':
            break;

        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('*', Star);
        justswitch('/', Div);
        justswitch('@', Matmul);
        justswitch('<', Less);
        justswitch('>', Greater);
        justswitch('=', Assign);
        justswitch('%', Mod);
        justswitch('&', Band);
        justswitch('|', Bor);
        justswitch('^', Xor);
        justswitch('(', Lpr);
        justswitch(')', Rpr);
        justswitch('[', Lsbr);
        justswitch(']', Rsbr);
        justswitch('{', Lbr);
        justswitch('}', Rbr);
        justswitch(',', Comma);
        justswitch('.', Dot);
        justswitch('\n', Newline);
        }
    }
    return false;
}

impl(Id) {
    pos++;
    if (isSuitableForId(c)) {
        toacc;
    } else {
        type = Type::id;
        switch (c)
        {
        justswitch(' ', Space);
        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('*', Star);
        justswitch('/', Div);
        justswitch('@', Matmul);
        justswitch('<', Less);
        justswitch('>', Greater);
        justswitch('=', Assign);
        justswitch('%', Mod);
        justswitch('&', Band);
        justswitch('|', Bor);
        justswitch('^', Xor);
        justswitch('(', Lpr);
        justswitch(')', Rpr);
        justswitch('[', Lsbr);
        justswitch(']', Rsbr);
        justswitch('{', Lbr);
        justswitch('}', Rbr);
        justswitch(',', Comma);
        justswitch('.', Dot);
        justswitch('\n', Newline);

        default:
            type = Type::unexpected;
            break;
        }
        return true;
    }
    return false;
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

impl(Colon) {
    pos++;
    if (isSuitableForIdBeginning(c)) {
        newstate(Id);
    } else if (std::isdigit(c)) {
        newstate(FirstNumPart);
    } else {
        switch (c) {
        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('~', Inv);
        justswitch('"', String);
        justswitch('\'', String);
        justswitch('\n', Newline);
        justswitch(' ', Space);            
        justswitch('#', Comment);

        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    type = Type::colon;
    return true;
}

impl(Dot) {
    pos++;
    if (isSuitableForIdBeginning(c)) {
        type = Type::dot;
        newstate(Id);
        return true;
    } else {
        type = Type::unexpected;
        return true;
    }
}

impl(FirstNumPart) {
    pos++;
    if (std::isdigit(c)) {
        toacc;
    } else if (c == '.') {
        toacc;
        newstate(SecondNumPart);
    } else {
        type = Type::number;
        switch (c) {
        justswitch(' ', Space);
        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('*', Star);
        justswitch('/', Div);
        justswitch('@', Matmul);
        justswitch('<', Less);
        justswitch('>', Greater);
        justswitch('=', Assign);
        justswitch('%', Mod);
        justswitch('&', Band);
        justswitch('|', Bor);
        justswitch('^', Xor);
        justswitch('(', Lpr);
        justswitch(')', Rpr);
        justswitch('[', Lsbr);
        justswitch(']', Rsbr);
        justswitch('{', Lbr);
        justswitch('}', Rbr);
        justswitch(',', Comma);
        justswitch('\n', Newline);

        default:
            type = Type::unexpected;
            break;
        }
        return true;
    }
    return false;
}

impl(SecondNumPart) {
    pos++;
    if (std::isdigit(c)) {
        toacc;
    } else {
        type = Type::number;
        switch (c) {
        justswitch(' ', Space);
        justswitch('+', Plus);
        justswitch('-', Minus);
        justswitch('*', Star);
        justswitch('/', Div);
        justswitch('@', Matmul);
        justswitch('<', Less);
        justswitch('>', Greater);
        justswitch('=', Assign);
        justswitch('%', Mod);
        justswitch('&', Band);
        justswitch('|', Bor);
        justswitch('^', Xor);
        justswitch('(', Lpr);
        justswitch(')', Rpr);
        justswitch('[', Lsbr);
        justswitch(']', Rsbr);
        justswitch('{', Lbr);
        justswitch('}', Rbr);
        justswitch(',', Comma);
        justswitch('\n', Newline);

        default:
            type = Type::unexpected;
            break;
        }
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
