#include <pycom/lexer/LexerStates.hpp>

#define impl(name) name::name(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos):BaseLexerState(lex, acc, t, row, pos){} \
                   bool name::recognize(char c)

#define newstate(name) lexer->setState(new name(lexer, accum, type, row, pos))
#define toacc accum.push_back(c)

BaseLexerState::BaseLexerState(LexerInterface* lex, std::string& acc, Type& t, unsigned int& row, unsigned int& pos): 
    lexer(lex), accum(acc), type(t), row(row), pos(pos) {}

impl(Start) {
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
            type = Type::lpr;
            return true;
            break;
        
        case '{':
            type = Type::lbr;
            return true;
            break;
        
        case '[':
            type = Type::lsbr;
            return true;
            break;
        
        case '+':
            type = Type::unarplus;
            return true;
            break;
        
        case '-':
            type = Type::unarminus;
            return true;
            break;

        case '~':
            type = Type::inv;
            return true;
            break;
        
        default:
            type = Type::unexpected;
            return true;
            break;
        }
    }
    
    pos++;
    return false;
}

impl(Id) {}
impl(String) {}
impl(UnaryPlus) {}
impl(UnaryMinus) {}
impl(Colon) {}
impl(Dot) {}
impl(FirstNumPart) {}
impl(SecondNumPart) {}
impl(BinaryPlus) {}
impl(BinaryMinus) {}
impl(Star) {}
impl(Div) {}
impl(Mod) {}
impl(Matmul) {}
impl(Greater) {}
impl(Less) {}
impl(Assign) {}
impl(Inv) {}
impl(Land) {}
impl(Lor) {}
impl(Xor) {}
// impl(Lpr) {}
// impl(Rpr) {}
// impl(Lsbr) {}
// impl(Rsbr) {}
// impl(Lbr) {}
// impl(Rbr) {}
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
