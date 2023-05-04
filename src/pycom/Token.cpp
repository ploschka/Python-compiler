#include <pycom/token/Token.hpp>
#include <unordered_set>

std::string Token::getValue() const
{
    return value;
}

Type Token::getType() const
{
    return type;
}

unsigned int Token::getPos() const
{
    return pos;
}

unsigned int Token::getRow() const
{
    return row;
}

void Token::setValue(std::string value)
{
    this->value = value;
}

void Token::setType(Type type)
{
    this->type = type;
}

Token::Token(Type type, unsigned int row, unsigned int pos) : value(""), type(type), row(row), pos(pos) {}

Token::Token(std::string value, Type type, unsigned int row, unsigned int pos) : value(value), type(type), row(row), pos(pos) {}

bool Token::operator==(const Token& l)
{
    return this->type == l.getType() && this->value == l.getValue();
}
