#include <pycom/factory/LexerFactory.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

// Переводит enum Type в строку. Лучше функции не придумаешь.
static inline std::string type_to_str(Type _type)
{
    switch (_type)
    {
    case Type::id:
        return "id";
    case Type::number:
        return "number";
    case Type::string:
        return "string";
    case Type::lpr:
        return "lpr";
    case Type::rpr:
        return "rpr";
    case Type::lsbr:
        return "lsbr";
    case Type::rsbr:
        return "rsbr";
    case Type::lbr:
        return "lbr";
    case Type::rbr:
        return "rbr";
    case Type::colon:
        return "colon";
    case Type::plus:
        return "plus";
    case Type::minus:
        return "minus";
    case Type::star:
        return "star";
    case Type::div:
        return "div";
    case Type::mod:
        return "mod";
    case Type::idiv:
        return "idiv";
    case Type::matmul:
        return "matmul";
    case Type::dot:
        return "dot";
    case Type::greater:
        return "greater";
    case Type::less:
        return "less";
    case Type::equal:
        return "equal";
    case Type::noteq:
        return "noteq";
    case Type::grequal:
        return "grequal";
    case Type::lequal:
        return "lequal";
    case Type::notop:
        return "notop";
    case Type::in:
        return "in";
    case Type::is:
        return "is";
    case Type::andop:
        return "andop";
    case Type::orop:
        return "orop";
    case Type::inv:
        return "inv";
    case Type::lshift:
        return "lshift";
    case Type::rshift:
        return "rshift";
    case Type::band:
        return "band";
    case Type::bor:
        return "bor";
    case Type::xorop:
        return "xorop";
    case Type::assign:
        return "assign";
    case Type::plusass:
        return "plusass";
    case Type::minass:
        return "minass";
    case Type::mulass:
        return "mulass";
    case Type::divass:
        return "divass";
    case Type::modass:
        return "modass";
    case Type::idivass:
        return "idivass";
    case Type::matmulass:
        return "matmulass";
    case Type::lshiftass:
        return "lshiftass";
    case Type::rshiftass:
        return "rshiftass";
    case Type::bandass:
        return "bandass";
    case Type::borass:
        return "borass";
    case Type::xorass:
        return "xorass";
    case Type::arrow:
        return "arrow";
    case Type::defkw:
        return "defkw";
    case Type::ifkw:
        return "ifkw";
    case Type::elifkw:
        return "elifkw";
    case Type::elsekw:
        return "elsekw";
    case Type::forkw:
        return "forkw";
    case Type::whilekw:
        return "whilekw";
    case Type::classkw:
        return "classkw";
    case Type::yieldkw:
        return "yieldkw";
    case Type::returnkw:
        return "returnkw";
    case Type::continuekw:
        return "continuekw";
    case Type::passkw:
        return "passkw";
    case Type::breakkw:
        return "breakkw";
    case Type::indent:
        return "indent";
    case Type::dedent:
        return "dedent";
    case Type::newline:
        return "newline";
    case Type::eof:
        return "eof";
    case Type::comma:
        return "comma";
    case Type::unexpected:
        return "unexpected";
    case Type::indenterror:
        return "indenterror";
    case Type::tabspacemix:
        return "tabspacemix";
    default:
        return "␜⌾ⴶⶪ⤡₀⍣⁰";
    }
}

int main()
{
    auto lexfac = LexerFactory();
    auto lexer = lexfac.create();
    std::ifstream file("scripts/lexer.py");

    lexer->open(file);

    Token token("", Type::id);
    while ((token = lexer->getToken()).getType() != Type::eof)
    {
        std::cout << type_to_str(token.getType()) << ' ' << token.getValue() << ' ' << token.getRow() << ' ' << token.getPos() << '\n';
    }
    std::cout << type_to_str(token.getType()) << ' ' << token.getValue() << '\n';
}
