#include <pycom/token/Token.hpp>

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

void Token::setValue(std::string _value)
{
    this->value = _value;
}

void Token::setType(Type _type)
{
    this->type = _type;
}

Token::Token() : value(""), type(Type::eof), row(0), pos(0) {}
Token::Token(std::string _value, Type _type, unsigned int _row, unsigned int _pos) : value(_value), type(_type), row(_row), pos(_pos) {}

bool Token::operator==(const Token &_other) const
{
    bool eq_types = this->type == _other.type;
    bool eq_values = this->value == _other.value;
    bool eq_pos = this->row == _other.row && this->pos == _other.pos;

    return this->type == Type::id ? eq_types && eq_values && eq_pos : eq_types && eq_pos;
}

bool Token::operator!=(const Token &_other) const
{
    bool neq_types = this->type != _other.type;
    bool neq_values = this->value != _other.value;
    bool neq_pos = this->pos != _other.pos || this->pos != _other.pos;

    return this->type == Type::id ? neq_types || neq_values || neq_pos : neq_types || neq_pos;
}
