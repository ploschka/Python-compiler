#include <pycom/lexer/Lexeme.hpp>

std::string Lexeme::getValue() const {
    return value;
}

Type Lexeme::getType() const {
    return type;
}

void Lexeme::setValue(std::string value) {
    this->value = value;
}

void Lexeme::setType(Type type) {
    this->type = type;
}
