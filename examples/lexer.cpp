#include <pycom/factory/LexerFactory.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

static std::unordered_map<Type, std::string> map = {
    {Type::id, "id"},
    {Type::number, "number"},
    {Type::string, "string"},
    {Type::lpr, "lpr"},
    {Type::rpr, "rpr"},
    {Type::lsbr, "lsbr"},
    {Type::rsbr, "rsbr"},
    {Type::lbr, "lbr"},
    {Type::rbr, "rbr"},
    {Type::colon, "colon"},
    {Type::plus, "plus"},
    {Type::minus, "minus"},
    {Type::star, "star"},
    {Type::div, "div"},
    {Type::mod, "mod"},
    {Type::idiv, "idiv"},
    {Type::matmul, "matmul"},
    {Type::dot, "dot"},
    {Type::greater, "greater"},
    {Type::less, "less"},
    {Type::equal, "equal"},
    {Type::noteq, "noteq"},
    {Type::grequal, "grequal"},
    {Type::lequal, "lequal"},
    {Type::notop, "notop"},
    {Type::in, "in"},
    {Type::is, "is"},
    {Type::andop, "andop"},
    {Type::orop, "orop"},
    {Type::inv, "inv"},
    {Type::lshift, "lshift"},
    {Type::rshift, "rshift"},
    {Type::band, "band"},
    {Type::bor, "bor"},
    {Type::xorop, "xorop"},
    {Type::assign, "assign"},
    {Type::plusass, "plusass"},
    {Type::minass, "minass"},
    {Type::mulass, "mulass"},
    {Type::divass, "divass"},
    {Type::modass, "modass"},
    {Type::idivass, "idivass"},
    {Type::matmulass, "matmulass"},
    {Type::lshiftass, "lshiftass"},
    {Type::rshiftass, "rshiftass"},
    {Type::bandass, "bandass"},
    {Type::borass, "borass"},
    {Type::xorass, "xorass"},
    {Type::defkw, "defkw"},
    {Type::ifkw, "ifkw"},
    {Type::elifkw, "elifkw"},
    {Type::elsekw, "elsekw"},
    {Type::forkw, "forkw"},
    {Type::whilekw, "whilekw"},
    {Type::classkw, "classkw"},
    {Type::returnkw, "returnkw"},
    {Type::yieldkw, "yieldkw"},
    {Type::continuekw, "continuekw"},
    {Type::breakkw, "breakkw"},
    {Type::indent, "indent"},
    {Type::dedent, "dedent"},
    {Type::newline, "newline"},
    {Type::eof, "eof"},
    {Type::comma, "comma"},
    {Type::unexpected, "unexpected"},
    {Type::arrow, "arrow"},
    {Type::indenterror, "indenterror"}
};

int main()
{
    auto lexfac = LexerFactory();
    auto lexer = lexfac.create();

    lexer->openFile("scripts/lexer.py");
    bool keepGoing = true;
    while (keepGoing)
    {
        auto lexeme = lexer->getLexeme();
        keepGoing = lexeme.getType() != Type::eof;
        std::cout << '"' << lexeme.getValue() << "\"  " << map.at(lexeme.getType()) << " row: " << lexeme.getRow() << " pos: " << lexeme.getPos() << std::endl;
    }
}
