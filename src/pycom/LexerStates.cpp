#include <pycom/lexer/LexerStates.hpp>

#define impl(name) name::name(LexerInterface* lex, std::string& acc, Type& t):BaseLexerState(lex, acc, t){} \
                   bool name::recognize(char c)

BaseLexerState::BaseLexerState(LexerInterface* lex, std::string& acc, Type& t): lexer(lex), accum(acc), type(t) {}

impl(Start) {}
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
