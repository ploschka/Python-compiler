#include <pycom/lexeme/Lexeme.hpp>

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

Lexeme::Lexeme(std::string value, Type type): value(value), type(type) {}
