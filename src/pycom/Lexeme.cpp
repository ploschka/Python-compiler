#include <pycom/lexeme/Lexeme.hpp>

std::string Lexeme::getValue() const
{
    return value;
}

Type Lexeme::getType() const
{
    return type;
}

unsigned int Lexeme::getPos() const
{
    return pos;
}

unsigned int Lexeme::getRow() const
{
    return row;
}

void Lexeme::setValue(std::string value)
{
    this->value = value;
}

void Lexeme::setType(Type type)
{
    this->type = type;
}

Lexeme::Lexeme(Type type, unsigned int row, unsigned int pos) : value(""), type(type), row(row), pos(pos) {}

Lexeme::Lexeme(std::string value, Type type, unsigned int row, unsigned int pos) : value(value), type(type), row(row), pos(pos) {}
