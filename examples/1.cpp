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
    {Type::def, "def"},
    {Type::ifop, "ifop"},
    {Type::elifop, "elifop"},
    {Type::elseop, "elseop"},
    {Type::forop, "forop"},
    {Type::whileop, "whileop"},
    {Type::classop, "classop"},
    {Type::indent, "indent"},
    {Type::dedent, "dedent"},
    {Type::newline, "newline"},
    {Type::eof, "eof"},
    {Type::comma, "comma"},
    {Type::unexpected, "unexpected"}
};

int main()
{
    auto lexfac = LexerFactory();
    auto lexer = lexfac.create();

    lexer->openFile("scripts/1.py");
    for (int i = 0; i < 20; i++)
    {
        auto lexeme = lexer->getLexeme();
        std::cout << lexeme.getValue() << ((lexeme.getValue() == "") ? "" : " ") << map.at(lexeme.getType()) << " row: " << lexeme.getRow() <<  " pos: " << lexeme.getPos() << std::endl;
    }
}
